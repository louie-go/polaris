#pragma once

#include <assert.h>

#include "board.h"
#include "types.h"

static inline char format_color(Color color) {
  assert(color > COLOR_NONE && color < COLOR_LEN);

  return color == WHITE ? 'w' : 'b';
}

static inline char format_piecetype(PieceType piecetype) {
  assert(piecetype > PIECETYPE_NONE && piecetype < PIECETYPE_LEN);

  switch (piecetype) {
    case PAWN: return 'p';
    case KNIGHT: return 'n';
    case BISHOP: return 'b';
    case ROOK: return 'r';
    case QUEEN: return 'q';
    case KING: return 'k';
  }

  return 0;
}

static inline char format_piece(Piece piece) {
  assert(piece > PIECE_NONE && piece < PIECE_LEN);

  switch (piece) {
    case WHITE_PAWN: return 'P';
    case WHITE_KNIGHT: return 'N';
    case WHITE_BISHOP: return 'B';
    case WHITE_ROOK: return 'R';
    case WHITE_QUEEN: return 'Q';
    case WHITE_KING: return 'K';
    case BLACK_PAWN: return 'p';
    case BLACK_KNIGHT: return 'n';
    case BLACK_BISHOP: return 'b';
    case BLACK_ROOK: return 'r';
    case BLACK_QUEEN: return 'q';
    case BLACK_KING: return 'k';
  }

  return 0;
}

static inline char format_file(File file) {
  assert(file > FILE_NONE && file < FILE_LEN);

  return file + 'a';
}

static inline char format_rank(Rank rank) {
  assert(rank > RANK_NONE && rank < RANK_LEN);

  return rank + '1';
}

char *format_square(Square, char *);
char *format_rights(CastleRights, char *);
char *format_move(Move, char *);
char *format_fen(const Board *, char *);
