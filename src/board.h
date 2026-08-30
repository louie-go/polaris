#pragma once

#include <stdint.h>

#include "types.h"
#include "bitboard.h"

static const uint16_t MAX_GAME_PLY = 17697;

typedef struct {
  Move         move;
  CastleRights rights;
  Piece        captured;
  Square       ep_square;
  uint8_t      halfmove_clk;
} State;

typedef struct {
  State        states[MAX_GAME_PLY];
  Bitboard     bitboards[COLOR_LEN][PIECETYPE_LEN];
  Piece        pieces[SQUARE_LEN];
  // +1 for `ALL` (check `types.h`)
  Bitboard     occupancies[COLOR_LEN+1];
  uint16_t     ply;
  uint16_t     fullmove_no;
  uint8_t      halfmove_clk;
  Color        turn;
  CastleRights rights;
  Square       ep_square;
} Board;

void make_move(Board *board, Move move);
void undo_move(Board *board);
void print_board(const Board *board, FILE *stream);
