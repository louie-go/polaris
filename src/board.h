#pragma once

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

void init_zobrist(uint64_t seed);

void make_move(Board *board, Move move);
void undo_move(Board *board);

void print_board(const Board *board, FILE *stream);
