#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef int8_t Color;
enum { COLOR_NONE = -1, WHITE, BLACK, ALL, COLOR_LEN = 2 };

static inline Color opposite(Color color) {
  assert(color > COLOR_NONE && color < COLOR_LEN);

  return color ^ 1;
}

typedef int8_t PieceType;
enum { PIECETYPE_NONE = -1, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECETYPE_LEN };

typedef int8_t Piece;
enum { PIECE_NONE = -1,
  WHITE_PAWN,   BLACK_PAWN,
  WHITE_KNIGHT, BLACK_KNIGHT,
  WHITE_BISHOP, BLACK_BISHOP,
  WHITE_ROOK,   BLACK_ROOK,
  WHITE_QUEEN,  BLACK_QUEEN,
  WHITE_KING,   BLACK_KING,
  PIECE_LEN };

static inline Piece new_piece(Color color, PieceType piecetype) {
  assert(color > COLOR_NONE && color < COLOR_LEN);
  assert(piecetype > PIECETYPE_NONE && piecetype < PIECETYPE_LEN);

  return color | piecetype<<1;
}

static inline Color piece_color(Piece piece) {
  assert(piece > PIECE_NONE && piece < PIECE_LEN);

  return piece & 1;
}

static inline PieceType piece_type(Piece piece) {
  assert(piece > PIECE_NONE && piece < PIECE_LEN);

  return piece >> 1;
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

static inline Square new_square(File file, Rank rank) {
  return (rank<<3) + file;
}

static inline File square_file(Square square) {
  return square & 7;
}

static inline Rank square_rank(Square square) {
  return square >> 3;
}

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

// no reason to add a `MOVE_NONE` or `MOVE_LEN`
typedef uint8_t MoveType;
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
  MOVE_DOUBLE_PUSH,
};

// bits 1-6:   source square
// bits 7-12:  destination square
// bits 13-16: move type
typedef uint16_t Move;

static inline Move new_move(Square src, Square dst, MoveType type) {
  assert(src > SQUARE_NONE && src < SQUARE_LEN);
  assert(dst > SQUARE_NONE && dst < SQUARE_LEN);
  assert(type >= MOVE_NORMAL && type <= MOVE_EP);

  return src | dst<<6 | type<<12;
}

static inline Square move_src(Move move) {
  return move & 0x3F;
}

static inline Square move_dst(Move move) {
  return (move>>6) & 0x3F;
}

static inline MoveType move_type(Move move) {
  return move >> 12;
}

static inline Move set_move_src(Square src, Move move) {
  assert(src > SQUARE_NONE && src < SQUARE_LEN);

  return move | src;
}

static inline Move set_move_dst(Square dst, Move move) {
  assert(dst > SQUARE_NONE && dst < SQUARE_LEN);

  return move | dst<<6;
}

static inline Move set_move_type(MoveType type, Move move) {
  assert(type >= MOVE_NORMAL && type <= MOVE_EP);

  return move | type<<12;
}

static inline bool is_normal(Move move) {
  return move_type(move) == MOVE_NORMAL;
}

static inline bool is_promotion(Move move) {
  MoveType type = move_type(move);

  return type >= MOVE_PROMO_N && type <= MOVE_PROMO_Q;
}

static inline bool is_castle(Move move) {
  MoveType type = move_type(move);

  return type >= MOVE_CASTLE_WK && type <= MOVE_CASTLE_BQ;
}

static inline bool is_ep(Move move) {
  return move_type(move) == MOVE_EP;
}

static inline bool is_double_push(Move move) {
  return move_type(move) == MOVE_DOUBLE_PUSH;
}
