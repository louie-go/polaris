#pragma once

#include <assert.h>

#include "board.h"
#include "types.h"

static const uint8_t MAX_FEN_BUFFER  = 93;
static const uint8_t MAX_MOVE_BUFFER = 6;

static inline char format_color(Color color) {
  assert(color > COLOR_NONE && color < COLOR_LEN);

  return color == WHITE ? 'w' : 'b';
}

static const char piecetype_chars[PIECETYPE_LEN] = "pnbrqk";
static inline char format_piecetype(PieceType piecetype) {
  assert(piecetype > PIECETYPE_NONE && piecetype < PIECETYPE_LEN);

  return piecetype_chars[piecetype];
}

static const char piece_chars[PIECE_LEN] = "PpNnBbRrQqKk";
static inline char format_piece(Piece piece) {
  assert(piece > PIECE_NONE && piece < PIECE_LEN);

  return piece_chars[piece];
}

static inline char format_file(File file) {
  assert(file > FILE_NONE && file < FILE_LEN);

  return file + 'a';
}

static inline char format_rank(Rank rank) {
  assert(rank > RANK_NONE && rank < RANK_LEN);

  return rank + '1';
}

char *format_square(Square square, char *buffer);
char *format_rights(CastleRights rights, char *buffer);
char *format_move(Move move, char *buffer);
char *format_fen(const Board *board, char *buffer);
