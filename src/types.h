#pragma once

#include <assert.h>
#include <stdint.h>

typedef int8_t Color;
enum { NO_COLOR = -1, WHITE, BLACK, ALL, N_COLORS = 2 };

static inline Color opposite(Color color) {
  assert(color > NO_COLOR && color < N_COLORS);

  return color == WHITE ? BLACK : WHITE;
}

typedef int8_t Piece;
enum { NO_PIECE = -1, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, N_PIECES };

typedef int8_t File;
enum { NO_FILE = -1, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, N_FILES };

typedef int8_t Rank;
enum { NO_RANK = -1, RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, N_RANKS };

typedef int8_t Square;
enum {
  NO_SQUARE = -1,
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,
  N_SQUARES,
};

typedef uint8_t CastleRights;
enum {
  NO_CASTLES  = 0,
  CASTLE_WK   = 1,
  CASTLE_WQ   = 2,
  CASTLE_BK   = 4,
  CASTLE_BQ   = 8,

  N_CASTLES   = 4,
  ALL_CASTLES = 15,
};

typedef int8_t MoveType;
enum {
  MOVE_NORMAL,
  MOVE_PROMO_N,
  MOVE_PROMO_B,
  MOVE_PROMO_R,
  MOVE_PROMO_Q,
  MOVE_CASTLE,
  MOVE_EP,
};

typedef uint16_t Move;
static inline Square move_src(Move move) {
  return move & 0x3F;
}

static inline Square move_dst(Move move) {
  return (move>>6) & 0x3F;
}

static inline MoveType move_type(Move move) {
  return move >> 12;
}
