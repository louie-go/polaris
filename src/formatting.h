#pragma once

#include <assert.h>

#include "board.h"
#include "types.h"

static inline char format_color(Color color) {
  assert(color > NO_COLOR && color < N_COLORS);

  return color == WHITE ? 'w' : 'b';
}

static inline char format_piece(Piece piece) {
  assert(piece > NO_PIECE && piece < N_PIECES);

  switch (piece) {
    case PAWN: return 'p';
    case KNIGHT: return 'n';
    case BISHOP: return 'b';
    case ROOK: return 'r';
    case QUEEN: return 'q';
    case KING: return 'k';
  }

  return 0;
}

static inline char format_file(File file) {
  assert(file > NO_FILE && file < N_FILES);

  return file + 'a';
}

static inline char format_rank(Rank rank) {
  assert(rank > NO_RANK && rank < N_RANKS);

  return rank + '1';
}

void format_square(Square, char *);
void format_rights(CastleRights, char *);
void format_move(Move, char *);
void format_fen(const Board *, char *);
