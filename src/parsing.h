#pragma once

#include <assert.h>

#include "board.h"
#include "types.h"

static const uint8_t FEN_BUFFER_MAX = 93;

static inline Color parse_color(char color_char) {
  assert(color_char == 'w' || color_char == 'b');

  return color_char == 'w' ? WHITE : BLACK;
}

static inline PieceType parse_piecetype(char piecetype_char) {
  switch (piecetype_char) {
    case 'p': return PAWN;
    case 'n': return KNIGHT;
    case 'b': return BISHOP;
    case 'r': return ROOK;
    case 'q': return QUEEN;
    case 'k': return KING;
    default:
      assert(0);
      return PIECETYPE_NONE;
  }
}

static inline Piece parse_piece(char piece_char) {
  switch (piece_char) {
    case 'P': return WHITE_PAWN;
    case 'N': return WHITE_KNIGHT;
    case 'B': return WHITE_BISHOP;
    case 'R': return WHITE_ROOK;
    case 'Q': return WHITE_QUEEN;
    case 'K': return WHITE_KING;
    case 'p': return BLACK_PAWN;
    case 'n': return BLACK_KNIGHT;
    case 'b': return BLACK_BISHOP;
    case 'r': return BLACK_ROOK;
    case 'q': return BLACK_QUEEN;
    case 'k': return BLACK_KING;
    default:
      assert(0);
      return PIECE_NONE;
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
