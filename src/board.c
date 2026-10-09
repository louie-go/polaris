#include "board.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <inttypes.h>

#include "bitboard.h"
#include "formatting.h"
#include "parsing.h"
#include "types.h"

const char STARTING_POSITION[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

static const castle_rights_t RIGHTS_LOST[SQUARE_LENGTH] = {
  [SQUARE_E1] = CASTLE_WHITE,
  [SQUARE_A1] = CASTLE_WQ,
  [SQUARE_H1] = CASTLE_WK,
  [SQUARE_E8] = CASTLE_BLACK,
  [SQUARE_A8] = CASTLE_BQ,
  [SQUARE_H8] = CASTLE_BK,
};

static const bitboard_t CASTLE_BB[CASTLE_LENGTH] = {
  0x00000000000000A0ULL,
  0x0000000000000009ULL,
  0xA000000000000000ULL,
  0x0900000000000000ULL,
};

static const struct rook_castle_move_t {
  Square src, dst;
} ROOK_CASTLE_MOVES[CASTLE_LENGTH] = {
  {SQUARE_H1, SQUARE_F1},
  {SQUARE_A1, SQUARE_D1},
  {SQUARE_H8, SQUARE_F8},
  {SQUARE_A8, SQUARE_D8},
};

// void make_move(Board *board, Move move) {
//   /************************
//    *       VARIABLES      *
//    ************************/
//   Square src = move_src(move), dst = move_dst(move);
//   Bitboard src_bb = new_bitboard(src), dst_bb = new_bitboard(dst);
//
//   Piece piece = board->pieces[src];
//   Color turn = board->turn;
//   PieceType src_type = piece_type(piece);
//
//   Piece captured = board->pieces[dst];
//   Color opposing = opposite(turn);
//
//   CastleRights *rights = &board->rights;
//   Bitboard *all_bb = &board->color_bb[ALL];
//   Bitboard *turn_bb = &board->color_bb[turn];
//   Bitboard *opposing_bb = &board->color_bb[opposing];
//   Bitboard *src_type_bb = &board->type_bb[src_type];
//
//   /************************
//    *UPDATING CURRENT STATE*
//    ************************/
//   board->states[board->ply++] = (State){
//     .move = move,
//     .rights = *rights,
//     .captured = captured,
//     .ep_square = board->ep_square,
//     .halfmove_clk = board->halfmove_clk,
//   };
//
//   /************************
//    *   MOVING THE PIECE   *
//    ************************/
//   Bitboard move_bb = src_bb | dst_bb;
//   *turn_bb ^= move_bb;
//   board->pieces[src] = PIECE_NONE;
//
//   MoveType type = move_type(move);
//
//   /************************
//    *       CAPTURES       *
//    ************************/
//   if (captured != PIECE_NONE) {
//     PieceType captured_type = piece_type(captured);
//
//     *all_bb ^= src_bb;
//     *opposing_bb ^= dst_bb;
//     board->type_bb[captured_type] ^= dst_bb;
//
//     if (captured_type == ROOK)
//       *rights &= ~RIGHTS_LOST[dst];
//
//     // update `board->halfmove_clk` here to reduce branches
//     board->halfmove_clk = 0;
//   } else {
//     if (src_type != PAWN) board->halfmove_clk++;
//     else board->halfmove_clk = 0;
//     *all_bb ^= move_bb;
//   }
//
//   /************************
//    *     SPECIAL MOVES    *
//    ************************/
//   // south in perspective (used for en passant)
//   Square south_turn = turn == WHITE ? -8 : +8;
//
//   *rights &= ~RIGHTS_LOST[src];
//
//   if (is_promotion(type)) {
//     PieceType promo_type = type-MOVE_PROMO_N + KNIGHT;
//     *src_type_bb ^= src_bb;
//     board->type_bb[promo_type] |= dst_bb;
//     board->pieces[dst] = new_piece(turn, promo_type);
//   } else {
//     *src_type_bb ^= move_bb;
//     board->pieces[dst] = piece;
//   }
//
//   if (is_castle(type)) {
//     uint8_t castle_idx = type-MOVE_CASTLE_WK;
//
//     Bitboard rook_move_bb = CASTLE_BB[castle_idx];
//     *all_bb  ^= rook_move_bb;
//     *turn_bb ^= rook_move_bb;
//     board->type_bb[ROOK] ^= rook_move_bb;
//
//     const struct RookCastleMove *rook_move = &ROOK_CASTLE_MOVES[castle_idx];
//     board->pieces[rook_move->src] = PIECE_NONE;
//     board->pieces[rook_move->dst] = new_piece(turn, ROOK);
//   } else if (is_ep(type)) {
//     Square ep_piece = board->ep_square + south_turn;
//     Bitboard ep_bb = new_bitboard(ep_piece);
//     *all_bb ^= ep_bb;
//     *opposing_bb ^= ep_bb;
//     *src_type_bb ^= ep_bb;
//     board->pieces[ep_piece] = PIECE_NONE;
//   }
//
//   /************************
//    *  UPDATING BOARD INFO *
//    ************************/
//   board->turn = opposing;
//
//   if (is_double_push(type))
//     board->ep_square = dst + south_turn;
//   else board->ep_square = SQUARE_NONE;
//
//   if (turn == BLACK) board->fullmove_no++;
// }
//
// void undo_move(Board *board) {
//   /************************
//    *       VARIABLES      *
//    ************************/
//   State state = board->states[--board->ply];
//
//   Move move = state.move;
//   MoveType type = move_type(move);
//
//   Square src = move_src(move), dst = move_dst(move);
//   Bitboard src_bb = new_bitboard(src), dst_bb = new_bitboard(dst);
//
//   Piece piece = board->pieces[dst];
//   Color turn = board->turn;
//   PieceType dst_type = piece_type(piece);
//
//   Piece captured = state.captured;
//   Color opposing = opposite(turn);
//
//   Bitboard *all_bb = &board->color_bb[ALL];
//   Bitboard *turn_bb = &board->color_bb[turn];
//   Bitboard *opposing_bb = &board->color_bb[opposing];
//   Bitboard *dst_type_bb = &board->type_bb[dst_type];
//
//   /************************
//    *   MOVING THE PIECE   *
//    ************************/
//   Bitboard move_bb = dst_bb | src_bb;
//   *opposing_bb ^= move_bb;
//
//   /************************
//    *       CAPTURES       *
//    ************************/
//   if (captured != PIECE_NONE) {
//     *all_bb |= src_bb;
//     *turn_bb |= dst_bb;
//     board->type_bb[piece_type(captured)] ^= dst_bb;
//     board->pieces[dst] = captured;
//   } else {
//     *all_bb ^= move_bb;
//     board->pieces[dst] = PIECE_NONE;
//   }
//
//   /************************
//    *     SPECIAL MOVES    *
//    ************************/
//   if (is_promotion(type)) {
//     *dst_type_bb ^= dst_bb;
//     board->type_bb[PAWN] |= src_bb;
//     board->pieces[src] = new_piece(opposing, PAWN);
//   } else {
//     *dst_type_bb ^= move_bb;
//     board->pieces[src] = piece;
//   }
//
//   if (is_castle(type)) {
//     uint8_t castle_idx = type-MOVE_CASTLE_WK;
//
//     Bitboard rook_move_bb = CASTLE_BB[castle_idx];
//     *all_bb ^= rook_move_bb;
//     *opposing_bb ^= rook_move_bb;
//     board->type_bb[ROOK] ^= rook_move_bb;
//
//     struct RookCastleMove rook_move = ROOK_CASTLE_MOVES[castle_idx];
//     board->pieces[rook_move.dst] = PIECE_NONE;
//     board->pieces[rook_move.src] = new_piece(opposing, ROOK);
//   } else if (is_ep(type)) {
//     Square ep_piece = dst + (turn == WHITE ? +8 : -8);
//     Bitboard ep_bb = new_bitboard(ep_piece);
//     *all_bb |= ep_bb;
//     *turn_bb |= ep_bb;
//     *dst_type_bb |= ep_bb;
//     board->pieces[ep_piece] = new_piece(turn, PAWN);
//   }
//
//   /************************
//    *  UPDATING BOARD INFO *
//    ************************/
//   board->turn = opposing;
//   board->rights = state.rights;
//   board->ep_square = state.ep_square;
//   board->halfmove_clk = state.halfmove_clk;
//   if (turn == WHITE) board->fullmove_no--;
// }

void print_board(const board_t *board, FILE *stream) {
  for (rank_t rank = RANK_LENGTH-1; rank > RANK_NONE; rank--) {
    fputc(format_rank(rank), stream);
    fputc(' ', stream);
    for (file_t file = FILE_NONE+1; file < FILE_LENGTH; file++) {
      square_t square = new_square(file, rank);
      piece_t piece = board->pieces[square];
      if (piece != PIECE_NONE) fputc(format_piece(piece), stream);
      else fputc('.', stream);
      fputc(' ', stream);
    }

    fputc('\n', stream);
  }

  fputs("  a b c d e f g h\n", stream);

#ifndef NDEBUG
  char fen[MAX_FEN_BUFFER];
  format_fen(board, fen);

  fputc('\n', stream);
  fputs(fen, stream);
  fputc('\n', stream);
#endif
}
