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
void make_move(Board *board, Move move) {
  board->states[board->ply++] = new_state(board, move);

  Square src = move_src(move), dst = move_dst(move);
  Bitboard src_bb = new_bitboard(src), dst_bb = new_bitboard(dst);

  Piece piece = board->pieces[src];
  Color src_color = piece_color(piece);
  PieceType src_type = piece_type(piece);

  Piece captured = board->pieces[dst];
  Color opposing = opposite(board->turn);

  MoveType type = move_type(move);

  Bitboard move_bb = src_bb | dst_bb;
  board->occupancies[src_color] ^= move_bb;
  board->pieces[src] = PIECE_NONE;

  if (captured == PIECE_NONE)
    board->occupancies[ALL] ^= move_bb;

  if (!is_promotion(move)) {
    board->bitboards[src_color][src_type] ^= move_bb;
    board->pieces[dst] = piece;
  }

  if (captured != PIECE_NONE) {
    board->bitboards[opposing][piece_type(captured)] ^= dst_bb;
    board->occupancies[opposing] ^= dst_bb;
    board->occupancies[ALL] ^= src_bb;
  }

  CastleRights castle_turn = CASTLE_WHITE << (board->turn<<1);

  if (src_type == KING)
    board->rights &= ~castle_turn;

  if (src_type == ROOK && (src <= H1 || src >= A8)) {
    File src_file = square_file(src);
    if (src_file == FILE_A)
      board->rights &= ~(castle_turn & CASTLE_QUEEN);
    if (src_file == FILE_H)
      board->rights &= ~(castle_turn & CASTLE_KING);
  }

  if (piece_type(captured) == ROOK && (dst <= H1 || dst >= A8)) {
    File dst_file = square_file(dst);
    CastleRights castle_opposing = CASTLE_WHITE << (opposing<<1);
    if (dst_file == FILE_A)
      board->rights &= ~(castle_opposing & CASTLE_QUEEN);
    if (dst_file == FILE_H)
      board->rights &= ~(castle_opposing & CASTLE_KING);
  }

  if (is_ep(move)) {
    Square ep_piece = board->ep_square + (board->turn == WHITE ?
      +8 : -8);
    Bitboard ep_bb = new_bitboard(ep_piece);
    board->bitboards[opposing][PAWN] ^= ep_bb;
    board->occupancies[opposing] ^= ep_bb;
    board->occupancies[ALL] ^= ep_bb;
    board->pieces[ep_piece] = PIECE_NONE;
  } else if (is_castle(move)) {
    Bitboard rook_move = CASTLE_BB[type-MOVE_CASTLE_WK];
    board->bitboards[board->turn][ROOK] ^= rook_move;
    board->occupancies[board->turn] ^= rook_move;
    board->occupancies[ALL] ^= rook_move;

    if (type == MOVE_CASTLE_WK || type == MOVE_CASTLE_BK) {
      board->pieces[src+1] = new_piece(board->turn, ROOK);
      board->pieces[dst+1] = PIECE_NONE;
    }

    if (type == MOVE_CASTLE_WQ || type == MOVE_CASTLE_BQ) {
      board->pieces[src-1] = new_piece(board->turn, ROOK);
      board->pieces[dst-2] = PIECE_NONE;
    }

    board->rights &= ~castle_turn;
  } else if (is_promotion(move)) {
    // `Piece` and `MoveType` overlap (check `types.h`)
    board->bitboards[src_color][PAWN] ^= src_bb;
    board->bitboards[board->turn][type] |= dst_bb;
    board->pieces[dst] = new_piece(board->turn, type);
  }

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
