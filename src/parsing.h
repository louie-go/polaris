#pragma once

#include <assert.h>

#include "board.h"
#include "types.h"

static inline Color parse_color(char color_char) {
  assert(color_char == 'w' || color_char == 'b');

  return color_char == 'w' ? WHITE : BLACK;
}

static inline Piece parse_piece(char piece_char) {
  switch (piece_char) {
    case 'p': return PAWN;
    case 'n': return KNIGHT;
    case 'b': return BISHOP;
    case 'r': return ROOK;
    case 'q': return QUEEN;
    case 'k': return KING;
    default:
      assert(0);
      return NO_PIECE;
  }
}

static inline File parse_file(char file_char) {
  assert(file_char >= 'a' && file_char <= 'h');

  return file_char - 'a';
}

static inline Rank parse_rank(char rank_char) {
  assert(rank_char >= '1' && rank_char <= '8');

  return rank_char - '1';
}

Square parse_square(const char *);
CastleRights parse_rights(const char *);
Move parse_move(const char *);
void parse_fen(const char *, Board *);
