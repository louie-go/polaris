#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef int8_t color_t;
enum { COLOR_NONE = -1, COLOR_WHITE, COLOR_BLACK, COLOR_ALL, COLOR_LENGTH = 2 };

static inline color_t opposite(color_t color) {
  assert(color > COLOR_NONE && color < COLOR_LENGTH);

  return color ^ 1;
}

typedef int8_t piecetype_t;
enum {
  PIECETYPE_NONE = -1,
  PIECETYPE_PAWN,
  PIECETYPE_KNIGHT,
  PIECETYPE_BISHOP,
  PIECETYPE_ROOK,
  PIECETYPE_QUEEN,
  PIECETYPE_KING,
  PIECETYPE_LENGTH,
};

static inline bool is_slider(piecetype_t type) {
  return type == PIECETYPE_BISHOP || type == PIECETYPE_ROOK || type == PIECETYPE_QUEEN;
}

typedef int8_t piece_t;
enum { PIECE_NONE = -1,
  PIECE_WP, PIECE_BP,
  PIECE_WN, PIECE_BN,
  PIECE_WB, PIECE_BB,
  PIECE_WR, PIECE_BR,
  PIECE_WQ, PIECE_BQ,
  PIECE_WK, PIECE_BK,
  PIECE_LENGTH,
};

static inline piece_t new_piece(color_t color, piecetype_t type) {
  assert(color > COLOR_NONE && color < COLOR_LENGTH);
  assert(type > PIECETYPE_NONE && type < PIECETYPE_LENGTH);

  return color | type<<1;
}

static inline color_t piece_color(piece_t piece) {
  assert(piece > PIECE_NONE && piece < PIECE_LENGTH);

  return piece & 1;
}

static inline piecetype_t piece_type(piece_t piece) {
  assert(piece > PIECE_NONE && piece < PIECE_LENGTH);

  return piece >> 1;
}

typedef int8_t file_t;
enum { FILE_NONE = -1, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_LENGTH };

typedef int8_t rank_t;
enum { RANK_NONE = -1, RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_LENGTH };

typedef int8_t square_t;
enum {
  SQUARE_NONE = -1,
  SQUARE_A1, SQUARE_B1, SQUARE_C1, SQUARE_D1, SQUARE_E1, SQUARE_F1, SQUARE_G1, SQUARE_H1,
  SQUARE_A2, SQUARE_B2, SQUARE_C2, SQUARE_D2, SQUARE_E2, SQUARE_F2, SQUARE_G2, SQUARE_H2,
  SQUARE_A3, SQUARE_B3, SQUARE_C3, SQUARE_D3, SQUARE_E3, SQUARE_F3, SQUARE_G3, SQUARE_H3,
  SQUARE_A4, SQUARE_B4, SQUARE_C4, SQUARE_D4, SQUARE_E4, SQUARE_F4, SQUARE_G4, SQUARE_H4,
  SQUARE_A5, SQUARE_B5, SQUARE_C5, SQUARE_D5, SQUARE_E5, SQUARE_F5, SQUARE_G5, SQUARE_H5,
  SQUARE_A6, SQUARE_B6, SQUARE_C6, SQUARE_D6, SQUARE_E6, SQUARE_F6, SQUARE_G6, SQUARE_H6,
  SQUARE_A7, SQUARE_B7, SQUARE_C7, SQUARE_D7, SQUARE_E7, SQUARE_F7, SQUARE_G7, SQUARE_H7,
  SQUARE_A8, SQUARE_B8, SQUARE_C8, SQUARE_D8, SQUARE_E8, SQUARE_F8, SQUARE_G8, SQUARE_H8,
  SQUARE_LENGTH,
};

static inline square_t new_square(file_t file, rank_t rank) {
  assert(file > FILE_NONE && file < FILE_LENGTH);
  assert(rank > RANK_NONE && rank < RANK_LENGTH);

  return (rank<<3) + file;
}

static inline file_t square_file(square_t square) {
  assert(square > SQUARE_NONE && square < SQUARE_LENGTH);

  return square & 7;
}

static inline rank_t square_rank(square_t square) {
  assert(square > SQUARE_NONE && square < SQUARE_LENGTH);

  return square >> 3;
}

typedef uint8_t castle_rights_t;
enum {
  CASTLE_NONE  = 0,

  CASTLE_WHITE = 3<<0,
  CASTLE_BLACK = 3<<2,

  CASTLE_KING  = 5<<0,
  CASTLE_QUEEN = 5<<1,

  CASTLE_WK    = 1<<0,
  CASTLE_WQ    = 1<<1,
  CASTLE_BK    = 1<<2,
  CASTLE_BQ    = 1<<3,

  CASTLE_LENGTH   = 4,
  CASTLE_MAX   = 15,
};

static inline castle_rights_t castle_color(color_t color) {
  return CASTLE_WHITE << (color<<1);
}

typedef uint8_t movetype_t;
enum {
  MOVETYPE_MAX = 15,

  // masks
  MOVETYPE_PROMO_MASK         = 1<<2,
  MOVETYPE_CAPTURE_MASK       = 1<<3,
  MOVETYPE_PROMO_CAPTURE_MASK = MOVETYPE_PROMO_MASK | MOVETYPE_CAPTURE_MASK,
  MOVETYPE_PROMO_PIECE_MASK   = (1<<2) - 1,

  // quiet and special moves
  MOVETYPE_QUIET = 0,
  MOVETYPE_DOUBLE_PUSH,
  MOVETYPE_CASTLE_KING,
  MOVETYPE_CASTLE_QUEEN,

  // promotion moves
  MOVETYPE_PROMO_N = 0 | MOVETYPE_PROMO_MASK,
  MOVETYPE_PROMO_B = 1 | MOVETYPE_PROMO_MASK,
  MOVETYPE_PROMO_R = 2 | MOVETYPE_PROMO_MASK,
  MOVETYPE_PROMO_Q = 3 | MOVETYPE_PROMO_MASK,

  // capture moves
  MOVETYPE_CAPTURE = 0 | MOVETYPE_CAPTURE_MASK,
  MOVETYPE_EP      = 1 | MOVETYPE_CAPTURE_MASK,

  // promotion capture moves
  MOVETYPE_PROMO_CAPTURE_N = 0 | MOVETYPE_PROMO_MASK | MOVETYPE_CAPTURE_MASK,
  MOVETYPE_PROMO_CAPTURE_B = 1 | MOVETYPE_PROMO_MASK | MOVETYPE_CAPTURE_MASK,
  MOVETYPE_PROMO_CAPTURE_R = 2 | MOVETYPE_PROMO_MASK | MOVETYPE_CAPTURE_MASK,
  MOVETYPE_PROMO_CAPTURE_Q = 3 | MOVETYPE_PROMO_MASK | MOVETYPE_CAPTURE_MASK,
};

// bits 1-6:   source square
// bits 7-12:  destination square
// bits 13-16: move type
typedef uint16_t move_t;

static inline move_t new_move(square_t src, square_t dst, movetype_t type) {
  assert(src > SQUARE_NONE && src < SQUARE_LENGTH);
  assert(dst > SQUARE_NONE && dst < SQUARE_LENGTH);
  assert(type <= MOVETYPE_MAX);

  return src | dst<<6 | type<<12;
}

static inline square_t move_src(move_t move) {
  return move & 0x3F;
}

static inline square_t move_dst(move_t move) {
  return move>>6 & 0x3F;
}

static inline movetype_t move_type(move_t move) {
  return move>>12;
}

static inline bool is_promotion(movetype_t type) {
  return type & MOVETYPE_PROMO_MASK;
}

static inline bool is_capture(movetype_t type) {
  return type & MOVETYPE_CAPTURE_MASK;
}

static inline piecetype_t move_promo_piece(movetype_t type) {
  assert(is_promotion(type));

  return (type&MOVETYPE_PROMO_PIECE_MASK) + PIECETYPE_KNIGHT;
}
