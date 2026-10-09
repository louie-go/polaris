#pragma once

#include <assert.h>

#include "board.h"
#include "types.h"

#define MAX_FEN_BUFFER  93
#define MAX_MOVE_BUFFER 6

static inline char format_color(color_t color) {
  assert(color > COLOR_NONE && color < COLOR_LENGTH);

  return color == COLOR_WHITE ? 'w' : 'b';
}

extern const char piecetype_chars[PIECETYPE_LENGTH];
static inline char format_piecetype(piecetype_t piecetype) {
  assert(piecetype > PIECETYPE_NONE && piecetype < PIECETYPE_LENGTH);

  return piecetype_chars[piecetype];
}

extern const char piece_chars[PIECE_LENGTH];
static inline char format_piece(piece_t piece) {
  assert(piece > PIECE_NONE && piece < PIECE_LENGTH);

  return piece_chars[piece];
}

static inline char format_file(file_t file) {
  assert(file > FILE_NONE && file < FILE_LENGTH);

  return file + 'a';
}

static inline char format_rank(rank_t rank) {
  assert(rank > RANK_NONE && rank < RANK_LENGTH);

  return rank + '1';
}

char *format_square(square_t square, char *buffer);
char *format_rights(castle_rights_t rights, char *buffer);
char *format_move(move_t move, char *buffer);
char *format_fen(const board_t *board, char *buffer);
