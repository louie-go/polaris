#pragma once

#include <stdint.h>

#include "types.h"
#include "bitboard.h"

static const uint16_t MAX_GAME_PLY = 17698;

typedef struct State {
  Move         move;
  Piece        captured;
  uint8_t      halfmove_clk;
} State;

typedef struct {
  State        state[MAX_GAME_PLY];
  Bitboard     bitboards[COLOR_LEN][PIECETYPE_LEN];
  Piece        pieces[64];
  // +1 for `ALL` (check `types.h`)
  Bitboard     occupancies[COLOR_LEN+1];
  uint16_t     ply;
  uint16_t     fullmove_no;
  uint8_t      halfmove_clk;
  Color        turn;
  CastleRights rights;
  Square       ep_square;
} Board;

void make_move(Board *, Move);
void undo_move(Board *);
void print_board(const Board *, FILE *);
