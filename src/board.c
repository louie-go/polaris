#include "board.h"

#include <assert.h>
#include <stdio.h>
#include <inttypes.h>

#include "bitboard.h"
#include "formatting.h"
#include "parsing.h"
#include "types.h"
#include "zobrist.h"

static const CastleRights RIGHTS_LOST[SQUARE_LEN] = {
  [E1] = CASTLE_WHITE,
  [A1] = CASTLE_WQ,
  [H1] = CASTLE_WK,
  [E8] = CASTLE_BLACK,
  [A8] = CASTLE_BQ,
  [H8] = CASTLE_BK,
};

static const Bitboard CASTLE_BB[CASTLE_LEN] = {
  0x00000000000000A0ULL,
  0x0000000000000009ULL,
  0xA000000000000000ULL,
  0x0900000000000000ULL,
};

typedef struct {
  Square src, dst;
} RookCastleMove;

static const RookCastleMove ROOK_CASTLE_MOVES[CASTLE_LEN] = {
  {H1, F1},
  {A1, D1},
  {H8, F8},
  {A8, D8},
};

void make_move(Board *board, Move move) {
  /************************
   *       VARIABLES      *
   ************************/

  Square src = move_src(move), dst = move_dst(move);
  Bitboard src_bb = new_bitboard(src), dst_bb = new_bitboard(dst);

  Piece piece = board->pieces[src];
  Color turn = board->turn;
  PieceType src_type = piece_type(piece);

  Piece captured = board->pieces[dst];
  Color opposing = opposite(turn);

  CastleRights *rights = &board->rights;
  Bitboard *occupancies_turn  = &board->occupancies[turn];
  Bitboard *occupancies_opposing   = &board->occupancies[opposing];
  Bitboard *occupancies_all   = &board->occupancies[ALL];

  /************************
   *UPDATING CURRENT STATE*
   ************************/
  board->states[board->ply++] = (State){
    .move = move,
    .rights = *rights,
    .captured = captured,
    .ep_square = board->ep_square,
    .halfmove_clk = board->halfmove_clk,
  };

  /************************
   *   MOVING THE PIECE   *
   ************************/
  Bitboard move_bb = src_bb | dst_bb;
  *occupancies_turn ^= move_bb;
  board->pieces[src] = PIECE_NONE;

  MoveType type = move_type(move);

  /************************
   *       CAPTURES       *
   ************************/
  if (captured != PIECE_NONE) {
    PieceType captured_type = piece_type(captured);

    board->bitboards[opposing][captured_type] ^= dst_bb;
    *occupancies_opposing ^= dst_bb;
    *occupancies_all ^= src_bb;

    if (captured_type == ROOK)
      *rights &= ~RIGHTS_LOST[dst];

    // update `board->halfmove_clk` here to reduce branches
    board->halfmove_clk = 0;
  } else {
    if (src_type != PAWN) board->halfmove_clk++;
    else board->halfmove_clk = 0;
    *occupancies_all ^= move_bb;
  }

  /************************
   *     SPECIAL MOVES    *
   ************************/
  // south in perspective (used for en passant)
  Square south_turn = turn == WHITE ? -8 : +8;

  *rights &= ~RIGHTS_LOST[src];

  if (is_promotion(type)) {
    PieceType promo_type = type-MOVE_PROMO_N + KNIGHT;
    board->bitboards[turn][PAWN] ^= src_bb;
    board->bitboards[turn][promo_type] |= dst_bb;
    board->pieces[dst] = new_piece(turn, promo_type);
  } else {
    board->bitboards[turn][src_type] ^= move_bb;
    board->pieces[dst] = piece;
  }

  if (is_castle(type)) {
    uint8_t castle_idx = type-MOVE_CASTLE_WK;

    Bitboard rook_move_bb = CASTLE_BB[castle_idx];
    board->bitboards[turn][ROOK] ^= rook_move_bb;
    *occupancies_turn ^= rook_move_bb;
    *occupancies_all ^= rook_move_bb;

    RookCastleMove rook_move = ROOK_CASTLE_MOVES[castle_idx];
    board->pieces[rook_move.src] = PIECE_NONE;
    board->pieces[rook_move.dst] = new_piece(turn, ROOK);
  } else if (is_ep(type)) {
    Square ep_piece = board->ep_square + south_turn;
    Bitboard ep_bb = new_bitboard(ep_piece);
    board->bitboards[opposing][PAWN] ^= ep_bb;
    *occupancies_opposing ^= ep_bb;
    *occupancies_all ^= ep_bb;
    board->pieces[ep_piece] = PIECE_NONE;
  }

  /************************
   *  UPDATING BOARD INFO *
   ************************/
  board->turn = opposing;

  if (type == MOVE_DOUBLE_PUSH)
    board->ep_square = src + south_turn;
  else board->ep_square = SQUARE_NONE;

  if (turn == BLACK) board->fullmove_no++;
}

void print_board(const Board *board, FILE *stream) {
  for (Rank rank = RANK_LEN-1; rank > RANK_NONE; rank--) {
    fputc(format_rank(rank), stream);
    fputc(' ', stream);
    for (File file = FILE_NONE+1; file < FILE_LEN; file++) {
      Square square = new_square(file, rank);
      Piece piece = board->pieces[square];
      if (piece != PIECE_NONE) fputc(format_piece(piece), stream);
      else fputc('.', stream);
      fputc(' ', stream);
    }

    fputc('\n', stream);
  }

  fputs("  a b c d e f g h\n", stream);

#ifndef NDEBUG
  char fen[FEN_BUFFER_MAX];
  format_fen(board, fen);

  fprintf(stream, "\nfen %s zobrist 0x%016" PRIX64 "\n", fen, hash_board(board));
#endif
}
