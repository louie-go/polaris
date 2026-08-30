#include "board.h"

#include <assert.h>
#include <stdio.h>
#include <inttypes.h>

#include "bitboard.h"
#include "formatting.h"
#include "parsing.h"
#include "types.h"
#include "zobrist.h"

static const Bitboard CASTLE_BB[CASTLE_LEN] = {
  0x00000000000000A0ULL,
  0x0000000000000009ULL,
  0xA000000000000000ULL,
  0x0900000000000000ULL,
};

static const Square ROOK_KINGSIDE_CASTLE_SQUARES[COLOR_LEN] = {H1, H8};
static const Square ROOK_QUEENSIDE_CASTLE_SQUARES[COLOR_LEN] = {A1, A8};

static const Move ROOK_CASTLE_MOVES[CASTLE_LEN] = {
  new_move(H1, F1, MOVE_NORMAL),
  new_move(A1, D1, MOVE_NORMAL),
  new_move(H8, F8, MOVE_NORMAL),
  new_move(A8, D8, MOVE_NORMAL),
};

void make_move(Board *board, Move move) {
  /************************
   *       VARIABLES      *
   ************************/
  Square src = move_src(move), dst = move_dst(move);
  Bitboard src_bb = new_bitboard(src), dst_bb = new_bitboard(dst);

  Piece piece = board->pieces[src];
  PieceType src_type = piece_type(piece);

  Piece captured = board->pieces[dst];
  Color opposing = opposite(board->turn);

  MoveType type = move_type(move);

  /************************
   *UPDATING CURRENT STATE*
   ************************/
  board->states[board->ply++] = (State){
    .move = move,
    .rights = board->rights,
    .captured = captured,
    .ep_square = board->ep_square,
    .halfmove_clk = board->halfmove_clk,
  };

  /************************
   *   MOVING THE PIECE   *
   ************************/
  Bitboard move_bb = src_bb | dst_bb;
  board->occupancies[board->turn] ^= move_bb;
  board->pieces[src] = PIECE_NONE;

  if (captured == PIECE_NONE)
    board->occupancies[ALL] ^= move_bb;

  if (!is_promotion(move)) {
    board->bitboards[board->turn][src_type] ^= move_bb;
    board->pieces[dst] = piece;
  }

  /************************
   *       CAPTURES       *
   ************************/
  if (captured != PIECE_NONE) {
    board->bitboards[opposing][piece_type(captured)] ^= dst_bb;
    board->occupancies[opposing] ^= dst_bb;
    board->occupancies[ALL] ^= src_bb;
  }

  /************************
   *       CASTLING       *
   ************************/
  CastleRights castle_turn = CASTLE_WHITE << (board->turn<<1);

  if (src_type == KING)
    board->rights &= ~castle_turn;

  if (src_type == ROOK) {
    if (src == ROOK_KINGSIDE_CASTLE_SQUARES[board->turn])
      board->rights &= ~(castle_turn & CASTLE_KING);
    if (src == ROOK_QUEENSIDE_CASTLE_SQUARES[board->turn])
      board->rights &= ~(castle_turn & CASTLE_QUEEN);
  }

  if (piece_type(captured) == ROOK) {
    CastleRights castle_opposing = CASTLE_WHITE << (opposing<<1);
    if (dst == ROOK_KINGSIDE_CASTLE_SQUARES[opposing])
      board->rights &= ~(castle_opposing & CASTLE_KING);
    if (dst == ROOK_QUEENSIDE_CASTLE_SQUARES[opposing])
      board->rights &= ~(castle_opposing & CASTLE_QUEEN);
  }

  if (is_castle(move)) {
    MoveType castle_idx = type - MOVE_CASTLE_WK;

    Bitboard rook_move_bb = CASTLE_BB[castle_idx];
    board->bitboards[board->turn][ROOK] ^= rook_move_bb;
    board->occupancies[board->turn] ^= rook_move_bb;
    board->occupancies[ALL] ^= rook_move_bb;

    Move rook_move = ROOK_CASTLE_MOVES[castle_idx];
    board->pieces[move_src(rook_move)] = PIECE_NONE;
    board->pieces[move_dst(rook_move)] = new_piece(board->turn, ROOK);
  }

  /************************
   *      EN PASSANT      *
   ************************/
  if (is_ep(move)) {
    Square ep_piece = board->ep_square + (board->turn == WHITE ?
      -8 : +8);
    Bitboard ep_bb = new_bitboard(ep_piece);
    board->bitboards[opposing][PAWN] ^= ep_bb;
    board->occupancies[opposing] ^= ep_bb;
    board->occupancies[ALL] ^= ep_bb;
    board->pieces[ep_piece] = PIECE_NONE;
  }

  /************************
   *       PROMOTION      *
   ************************/
  if (is_promotion(move)) {
    board->bitboards[board->turn][PAWN] ^= src_bb;
    board->bitboards[board->turn][type-MOVE_PROMO_N + KNIGHT] |= dst_bb;
    board->pieces[dst] = new_piece(board->turn, type-MOVE_PROMO_N + KNIGHT);
  }
  
  /************************
   *  UPDATING BOARD INFO *
   ************************/
  board->turn = opposing;

  if (src_type == PAWN && is_double_push(move))
    board->ep_square = src +
      (board->turn == WHITE ? -8 : +8);
  else board->ep_square = SQUARE_NONE;

  if (src_type != PAWN && captured == PIECE_NONE) board->halfmove_clk++;
  else board->halfmove_clk = 0;

  if (board->turn == WHITE) board->fullmove_no++;
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
