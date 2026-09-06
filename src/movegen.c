#include "movegen.h"

#if defined(__BMI2__) && !defined(USE_MAGICS)
#define USE_PEXT
#include <immintrin.h>
#endif
#include <stdint.h>

#include "bitboard.h"
#include "board.h"
#include "tables.h"
#include "types.h"

static inline Bitboard bishop_attacks(Square src, Bitboard blockers) {
#ifdef USE_PEXT
  return bishop_pext_attacks[src][_pext_u64(blockers, bishop_masks[src])];
#else
#endif
}

static inline Bitboard rook_attacks(Square src, Bitboard blockers) {
#ifdef USE_PEXT
  return rook_pext_attacks[src][_pext_u64(blockers, rook_masks[src])];
#else
#endif
}

static const struct KingCastleMove {
  Bitboard empty, safe;
  Square src, dst;
} KING_CASTLE_MOVES[CASTLE_LEN] = {
  {0x0000000000000060, 0x0000000000000060, E1, G1},
  {0x000000000000000E, 0x000000000000000C, E1, C1},
  {0x6000000000000000, 0x6000000000000000, E8, G8},
  {0x0E00000000000000, 0x0C00000000000000, E8, C8},
};

Move *legal_moves(const Board *board, Move *moves) {
  Color turn     = board->turn;
  Color opposing = opposite(turn);

  Bitboard queens  = pieces(board, turn, QUEEN);
  Bitboard pawns   = pieces(board, turn, PAWN);
  Bitboard knights = pieces(board, turn, KNIGHT);
  Bitboard diags   = pieces(board, turn, BISHOP) | queens;
  Bitboard orthos  = pieces(board, turn, ROOK) | queens;
  Bitboard king_bb = pieces(board, turn, KING);
  Square   king    = lsb(king_bb);

  Bitboard all_bb      = board->color_bb[ALL];
  Bitboard empty       = ~all_bb;
  Bitboard turn_bb     = board->color_bb[turn];
  Bitboard not_turn_bb = ~turn_bb;

  /************************
   *  KING DANGER SQUARES *
   ************************/
  Bitboard opposing_queens  = pieces(board, opposing, QUEEN);
  Bitboard opposing_pawns   = pieces(board, opposing, PAWN);
  Bitboard opposing_knights = pieces(board, opposing, KNIGHT);
  Bitboard opposing_diags   = pieces(board, opposing, BISHOP) | opposing_queens;
  Bitboard opposing_orthos  = pieces(board, opposing, ROOK) | opposing_queens;
  Bitboard opposing_king_bb = pieces(board, opposing, KING);
  Square   opposing_king    = lsb(opposing_king_bb);
  Bitboard wo_king          = all_bb ^ king_bb;
  Bitboard attack_bb        = not_turn_bb & ~opposing_king_bb;

  Bitboard king_dangers = 0;

  while (opposing_pawns)
    king_dangers |= pawn_attacks[opposing][pop_lsb(&opposing_pawns)];
  while (opposing_knights)
    king_dangers |= knight_attacks[pop_lsb(&opposing_knights)];
  while (opposing_diags)
    king_dangers |= bishop_attacks(pop_lsb(&opposing_diags), wo_king);
  while (opposing_orthos)
    king_dangers |= rook_attacks(pop_lsb(&opposing_orthos), wo_king);
  king_dangers |= king_attacks[opposing_king];

  /************************
   *        CHECKS        *
   ************************/
  Bitboard checkers = 0;

  checkers |= pawn_attacks[turn][king]
    & pieces(board, opposing, PAWN);

  checkers |= knight_attacks[king]
    & pieces(board, opposing, KNIGHT);

  Bitboard diag_attacks = bishop_attacks(king, all_bb);
  opposing_diags = pieces(board, opposing, BISHOP) | opposing_queens;
  checkers |= diag_attacks & opposing_diags;

  Bitboard ortho_attacks = rook_attacks(king, all_bb);
  opposing_orthos = pieces(board, opposing, ROOK) | opposing_queens;
  checkers |= ortho_attacks & opposing_orthos;

  Bitboard king_attacks_bb = king_attacks[king] &
    ~(turn_bb | king_dangers);
  while (king_attacks_bb)
    *moves++ = new_move(king, pop_lsb(&king_attacks_bb), MOVE_NORMAL);

  uint8_t checkers_len = popcount(checkers);
  if (checkers_len > 1)
    return moves;

  Bitboard push_mask = ~0ULL, capture_mask = ~0ULL;
  if (checkers_len == 1) {
    capture_mask = checkers;

    Square checker_sq = lsb(checkers);
    
    // if checker is a slider then include blocks in `push_mask`
    if (is_slider(piece_type(board->pieces[checker_sq])))
      push_mask = between[king][checker_sq];
    else push_mask = 0;
  } else { // no checkers
    // 0 -> White O-O
    // 1 -> White O-O-O
    // 2 -> Black O-O
    // 3 -> Black O-O-O
    CastleRights rights = castle_color(turn);

    while (rights) {
      uint8_t right = __builtin_ctz(rights);
      rights &= rights-1;

      // skip if not allowed in castling rights
      if (!(1<<right & board->rights)) continue;

      const struct KingCastleMove *move = &KING_CASTLE_MOVES[right];

      // skip if squares are occupied
      if (move->empty & all_bb) continue;

      // skip if squares are attacked
      if (move->safe & king_dangers) continue;

      *moves++ = new_move(move->src, move->dst, MOVE_CASTLE_WK+right);
    }
  }

  Bitboard allowed_bb = push_mask | capture_mask;

  /************************
   *         PINS         *
   ************************/
#ifdef USE_PEXT
  Bitboard snipers =
    (bishop_pext_attacks[king][0] & opposing_diags)
    | (rook_pext_attacks[king][0] & opposing_orthos);
#else
#endif
  Bitboard pinned = 0, pin_rays[SQUARE_LEN];

  while (snipers) {
    Square sniper_sq = pop_lsb(&snipers);
    Bitboard ray = between[king][sniper_sq];
    Bitboard blockers = ray & all_bb;

    // skip if blockers does not contain only 1 bit
    if (!blockers || clear_lsb(blockers)) continue;

    // skip if not a friendly piece
    if (!(blockers & turn_bb)) continue;

    pin_rays[lsb(blockers)] = ray
      | new_bitboard(sniper_sq);
    pinned |= blockers;
  }

  Bitboard pinned_diags  = pinned & diags;
  Bitboard pinned_orthos = pinned & orthos;

  while (pinned_diags) {
    Square src = pop_lsb(&pinned_diags);
    Bitboard attacks = bishop_attacks(src, all_bb)
      & pin_rays[src]
      & attack_bb
      & allowed_bb;
    while (attacks)
      *moves++ = new_move(src, pop_lsb(&attacks), MOVE_NORMAL);
  }

  while (pinned_orthos) {
    Square src = pop_lsb(&pinned_orthos);
    Bitboard attacks = rook_attacks(src, all_bb)
      & pin_rays[src]
      & attack_bb
      & allowed_bb;
    while (attacks)
      *moves++ = new_move(src, pop_lsb(&attacks), MOVE_NORMAL);
  }

  /************************
   *      PIECE MOVES     *
   ************************/
  Bitboard not_pinned  = ~pinned;

  Bitboard not_pinned_knights = not_pinned & knights;
  Bitboard not_pinned_diags   = not_pinned & diags;
  Bitboard not_pinned_orthos  = not_pinned & orthos;

  while (not_pinned_knights) {
    Square src = pop_lsb(&not_pinned_knights);
    Bitboard attacks = knight_attacks[src]
      & attack_bb
      & allowed_bb;
    while (attacks)
      *moves++ = new_move(src, pop_lsb(&attacks), MOVE_NORMAL);
  }

  while (not_pinned_diags) {
    Square src = pop_lsb(&not_pinned_diags);
    Bitboard attacks = bishop_attacks(src, all_bb)
      & attack_bb
      & allowed_bb;
    while (attacks)
      *moves++ = new_move(src, pop_lsb(&attacks), MOVE_NORMAL);
  }

  while (not_pinned_orthos) {
    Square src = pop_lsb(&not_pinned_orthos);
    Bitboard attacks = rook_attacks(src, all_bb)
      & attack_bb
      & allowed_bb;
    while (attacks)
      *moves++ = new_move(src, pop_lsb(&attacks), MOVE_NORMAL);
  }

  /************************
   *      PAWN MOVES      *
   ************************/
  Bitboard not_pinned_pawns = not_pinned & pawns;

  Square promo_rank = turn == WHITE ? RANK_8 : RANK_1;

  // Pushes
  Bitboard single_push;
  if (turn == WHITE)
    single_push = (not_pinned_pawns << 8) & empty;
  else
    single_push = (not_pinned_pawns >> 8) & empty;

  single_push &= allowed_bb;

  Bitboard promotions = single_push & bitboard_rank(promo_rank);
  single_push &= ~promotions;

  while (single_push) {
    Square dst = pop_lsb(&single_push);
    Square src = dst + (turn == WHITE ? -8 : 8);
    *moves++ = new_move(src, dst, MOVE_NORMAL);
  }

  while (promotions) {
    Square dst = pop_lsb(&promotions);
    Square src = dst + (turn == WHITE ? -8 : 8);
    *moves++ = new_move(src, dst, MOVE_PROMO_N);
    *moves++ = new_move(src, dst, MOVE_PROMO_B);
    *moves++ = new_move(src, dst, MOVE_PROMO_R);
    *moves++ = new_move(src, dst, MOVE_PROMO_Q);
  }

  // Double push
  Bitboard double_push;
  if (turn == WHITE) {
    Bitboard mid = (not_pinned_pawns & bitboard_rank(RANK_2)) << 8;
    mid &= empty;
    double_push = (mid << 8) & empty & allowed_bb;
  } else {
    Bitboard mid = (not_pinned_pawns & bitboard_rank(RANK_7)) >> 8;
    mid &= empty;
    double_push = (mid >> 8) & empty & allowed_bb;
  }

  while (double_push) {
    Square dst = pop_lsb(&double_push);
    Square src = dst + (turn == WHITE ? -16 : 16);
    *moves++ = new_move(src, dst, MOVE_DOUBLE_PUSH);
  }

  // Captures
  Bitboard opponent_bb = all_bb & not_turn_bb & ~new_bitboard(opposing_king);
  Bitboard unpinned_captures = not_pinned_pawns;
  while (unpinned_captures) {
    Square src = pop_lsb(&unpinned_captures);
    Bitboard attacks = pawn_attacks[turn][src]
      & opponent_bb
      & capture_mask;
    while (attacks) {
      Square dst = pop_lsb(&attacks);
      if (new_bitboard(dst) & bitboard_rank(promo_rank)) {
        *moves++ = new_move(src, dst, MOVE_PROMO_N);
        *moves++ = new_move(src, dst, MOVE_PROMO_B);
        *moves++ = new_move(src, dst, MOVE_PROMO_R);
        *moves++ = new_move(src, dst, MOVE_PROMO_Q);
      } else {
        *moves++ = new_move(src, dst, MOVE_NORMAL);
      }
    }
  }

  // En passant
  if (board->ep_square != SQUARE_NONE) {
    Bitboard ep_attackers = pawns & pawn_attacks[opposing][board->ep_square];
    ep_attackers &= not_pinned_pawns;

    Square ep_captured = turn == WHITE ?
      board->ep_square - 8 : board->ep_square + 8;

    while (ep_attackers) {
      Square src = pop_lsb(&ep_attackers);

      Bitboard occ_after = all_bb
        ^ new_bitboard(src)
        ^ new_bitboard(ep_captured)
        ^ new_bitboard(board->ep_square);

      Bitboard ep_check = rook_attacks(king, occ_after) & opposing_orthos;
      if (ep_check) continue;

      if (checkers_len == 1) {
        bool captures_checker = new_bitboard(ep_captured) & checkers;
        bool blocks_check = new_bitboard(board->ep_square) & push_mask;
        if (!captures_checker && !blocks_check) continue;
      }

      *moves++ = new_move(src, board->ep_square, MOVE_EP);
    }
  }

  // Pinned pawns
  Bitboard pinned_pawns = pinned & pawns;
  while (pinned_pawns) {
    Square src = pop_lsb(&pinned_pawns);
    Bitboard pin_ray = pin_rays[src];

    // Push along pin ray
    Bitboard push;
    if (turn == WHITE)
      push = new_bitboard(src + 8) & empty & pin_ray & allowed_bb;
    else
      push = new_bitboard(src - 8) & empty & pin_ray & allowed_bb;

    while (push) {
      Square dst = pop_lsb(&push);
      if (new_bitboard(dst) & bitboard_rank(promo_rank)) {
        *moves++ = new_move(src, dst, MOVE_PROMO_N);
        *moves++ = new_move(src, dst, MOVE_PROMO_B);
        *moves++ = new_move(src, dst, MOVE_PROMO_R);
        *moves++ = new_move(src, dst, MOVE_PROMO_Q);
      } else {
        *moves++ = new_move(src, dst, MOVE_NORMAL);
      }
    }

    // Double push along pin ray
    Bitboard double_push_pin;
    if (turn == WHITE) {
      Bitboard one = new_bitboard(src + 8) & empty;
      double_push_pin = (one & bitboard_rank(RANK_3)) ?
        (new_bitboard(src + 16) & empty & pin_ray & allowed_bb) : 0;
    } else {
      Bitboard one = new_bitboard(src - 8) & empty;
      double_push_pin = (one & bitboard_rank(RANK_6)) ?
        (new_bitboard(src - 16) & empty & pin_ray & allowed_bb) : 0;
    }

    while (double_push_pin)
      *moves++ = new_move(src, pop_lsb(&double_push_pin), MOVE_DOUBLE_PUSH);

    // Captures along pin ray
    Bitboard captures = pawn_attacks[turn][src]
      & opponent_bb & pin_ray & capture_mask;

    while (captures) {
      Square dst = pop_lsb(&captures);
      if (new_bitboard(dst) & bitboard_rank(promo_rank)) {
        *moves++ = new_move(src, dst, MOVE_PROMO_N);
        *moves++ = new_move(src, dst, MOVE_PROMO_B);
        *moves++ = new_move(src, dst, MOVE_PROMO_R);
        *moves++ = new_move(src, dst, MOVE_PROMO_Q);
      } else {
        *moves++ = new_move(src, dst, MOVE_NORMAL);
      }
    }

    // En passant for pinned pawns
    if (board->ep_square != SQUARE_NONE
        && (new_bitboard(src) & pawn_attacks[opposing][board->ep_square])
        && (new_bitboard(board->ep_square) & pin_ray)) {
      Square ep_captured = turn == WHITE ?
        board->ep_square - 8 : board->ep_square + 8;

      Bitboard occ_after = all_bb
        ^ new_bitboard(src)
        ^ new_bitboard(ep_captured)
        ^ new_bitboard(board->ep_square);

      Bitboard ep_check = rook_attacks(king, occ_after) & opposing_orthos;
      if (!ep_check)
        *moves++ = new_move(src, board->ep_square, MOVE_EP);
    }
  }

  return moves;
}
