#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "types.h"
#include "bitboard.h"

#define MAX_GAME_PLY 17697

typedef uint64_t Zobrist;

typedef struct {
  Move         move;
  CastleRights rights;
  Piece        captured;
  Square       ep_square;
  uint8_t      halfmove_clk;
} State;

typedef struct {
  State        states[MAX_GAME_PLY];
  Piece        pieces[SQUARE_LEN];
  Bitboard     type_bb[PIECETYPE_LEN];
  // +1 for `ALL` (check `types.h`)
  Bitboard     color_bb[COLOR_LEN+1];
  Zobrist      hash;
  uint16_t     ply;
  uint16_t     fullmove_no;
  uint8_t      halfmove_clk;
  Color        turn;
  CastleRights rights;
  Square       ep_square;
} Board;

static inline Bitboard pieces(const Board *board, Color color, PieceType type) {
  return board->color_bb[color] & board->type_bb[type];
}

static inline bool can_castle_kingside(const Board *board) {
  if (board->turn == WHITE) {
    return board->rights&CASTLE_WK > 0;
  } else {
    return board->rights&CASTLE_BK > 0;
  }
}

static inline bool can_castle_queenside(const Board *board) {
  if (board->turn == WHITE) {
    return board->rights&CASTLE_WQ > 0;
  } else {
    return board->rights&CASTLE_BQ > 0;
  }
}

void init_zobrist(uint64_t seed);

void make_move(Board *board, Move move);
void undo_move(Board *board);

void print_board(const Board *board, FILE *stream);
