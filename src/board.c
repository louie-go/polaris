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
  /************************
   *UPDATING CURRENT STATE*
   ************************/
  board->states[board->ply++] = new_state(board, move);

  /************************
   *         MOVE         *
   ************************/
  Square src = move_src(move), dst = move_dst(move);
  Bitboard src_bb = new_bitboard(src), dst_bb = new_bitboard(dst);
  Piece piece = board->pieces[src];
  Color src_color = piece_color(piece);
  PieceType src_type = piece_type(piece);
  MoveType type = move_type(move);
  Piece captured = board->pieces[dst];

  Bitboard move_bb = src_bb | dst_bb;
  board->occupancies[src_color] ^= move_bb;
  board->occupancies[ALL] ^= move_bb;
  board->pieces[src] = PIECE_NONE;

  if (type < MOVE_PROMO_N || type > MOVE_PROMO_Q) {
    board->bitboards[src_color][src_type] ^= move_bb;
    board->pieces[dst] = piece;
  }

  /************************
   *        CAPTURE       *
   ************************/
  Color opposing = opposite(board->turn);
  if (captured != PIECE_NONE) {
    PieceType captured_type = piece_type(captured);
    board->bitboards[opposing][captured_type] ^= dst_bb;
    board->occupancies[opposing] ^= dst_bb;
    board->occupancies[ALL] ^= dst_bb;
  }

  /************************
   *      EN PASSANT      *
   ************************/
  if (type == MOVE_EP) {
    Square ep_piece = new_square(board->ep, square_rank(src));
    Square ep_bb = new_bitboard(ep_piece);
    board->bitboards[opposing][PAWN] ^= ep_bb;
    board->occupancies[opposing] ^= ep_bb;
    board->occupancies[ALL] ^= ep_bb;
    board->pieces[ep_piece] = PIECE_NONE;
  }

  File src_file = square_file(src);
  Bitboard adj = dst_bb>>1 | dst_bb<<1;
  if (src_type == PAWN
      && (dst-src == 16 || src-dst == 16)
      && board->bitboards[opposing][PAWN] & adj)
    board->ep = src_file;
  else board->ep = FILE_NONE;

  /************************
   *       CASTLING       *
   ************************/
  CastleRights castle_turn = CASTLE_WHITE << (2*board->turn);
  if (type >= MOVE_CASTLE_WK && type <= MOVE_CASTLE_BQ) {
    uint8_t castle_idx = type-MOVE_CASTLE_WK;
    board->bitboards[board->turn][ROOK] ^= CASTLE_BB[castle_idx];
    board->occupancies[board->turn] ^= CASTLE_BB[castle_idx];
    board->occupancies[ALL] ^= CASTLE_BB[castle_idx];

    if (type == MOVE_CASTLE_WK || type == MOVE_CASTLE_BK) {
      board->pieces[src+1] = new_piece(board->turn, ROOK);
      board->pieces[dst+1] = PIECE_NONE;
    }

    if (type == MOVE_CASTLE_WQ || type == MOVE_CASTLE_BQ) {
      board->pieces[src-1] = new_piece(board->turn, ROOK);
      board->pieces[dst-2] = PIECE_NONE;
    }

    board->rights ^= castle_turn;
  } else if (src_type == KING)
    board->rights &= ~castle_turn;

  if (src_type == ROOK && (src <= H1 || src >= A8)) {
    if (src_file == FILE_A)
      board->rights &= ~(castle_turn & CASTLE_QUEEN);
    if (src_file == FILE_H)
      board->rights &= ~(castle_turn & CASTLE_KING);
  }

  if (captured != PIECE_NONE && piece_type(captured) == ROOK && (dst <= H1 || dst >= A8)) {
    File dst_file = square_file(dst);
    CastleRights castle_opposing = CASTLE_WHITE << (2*opposing);
    if (dst_file == FILE_A)
      board->rights &= ~(castle_opposing & CASTLE_QUEEN);
    if (dst_file == FILE_H)
      board->rights &= ~(castle_opposing & CASTLE_KING);
  }

  /************************
   *       PROMOTION      *
   ************************/
  if (type >= MOVE_PROMO_N && type <= MOVE_PROMO_Q) {
    // `Piece` and `MoveType` overlap (check `types.h`)
    board->bitboards[board->turn][type] |= dst_bb;
    board->pieces[dst] = new_piece(board->turn, type);
    board->bitboards[src_color][PAWN] ^= src_bb;
  }

  /************************
   *  UPDATING BOARD INFO *
   ************************/
  board->turn = opposite(board->turn);
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
