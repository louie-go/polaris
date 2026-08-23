#pragma once

#include <assert.h>
#include <stdint.h>

typedef int8_t Color;
enum { COLOR_NONE = -1, WHITE, BLACK, ALL, COLOR_LEN = 2 };


typedef int8_t PieceType;
enum { PIECETYPE_NONE = -1, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECETYPE_LEN };

typedef int8_t Piece;
enum { PIECE_NONE = -1,
  WHITE_PAWN = PAWN,   WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
  BLACK_PAWN = PAWN+8, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
  PIECE_LEN };

static inline Piece make_piece(Color color, PieceType piecetype) {
  return (color<<3) | piecetype;
}

static inline Color piece_color(Piece piece) {
  return piece >> 3;
}

static inline PieceType piece_type(Piece piece) {
  return piece & 7;
}

typedef int8_t File;
enum { FILE_NONE = -1, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_LEN };

typedef int8_t Rank;
enum { RANK_NONE = -1, RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_LEN };

typedef int8_t Square;
enum {
  SQUARE_NONE = -1,
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,
  SQUARE_LEN,
};

typedef uint8_t CastleRights;
enum {
  CASTLE_NONE  = 0,

  CASTLE_WHITE = 3,
  CASTLE_BLACK = 12,

  CASTLE_KING  = 5,
  CASTLE_QUEEN = 10,

  CASTLE_WK    = 1,
  CASTLE_WQ    = 2,
  CASTLE_BK    = 4,
  CASTLE_BQ    = 8,

  CASTLE_LEN   = 4,
  CASTLE_ALL   = 15,
};

typedef int8_t MoveType;
enum {
  MOVE_NORMAL,
  MOVE_PROMO_N,
  MOVE_PROMO_B,
  MOVE_PROMO_R,
  MOVE_PROMO_Q,
  MOVE_CASTLE_WK,
  MOVE_CASTLE_WQ,
  MOVE_CASTLE_BK,
  MOVE_CASTLE_BQ,
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
