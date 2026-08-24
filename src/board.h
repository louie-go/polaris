#pragma once

#include "types.h"
#include "bitboard.h"

typedef struct State {
  struct State *previous;
  uint8_t      halfmove_clk;
} State;

typedef struct {
  Bitboard     bitboards[COLOR_LEN][PIECETYPE_LEN];
  Piece        pieces[64];
  // +1 for `ALL` (check `types.h`)
  Bitboard     occupancies[COLOR_LEN+1];
  State        *state;
  uint16_t     fullmove_no;
  uint8_t      halfmove_clk;
  Color        turn;
  CastleRights rights;
  Square       ep_square;
} Board;

void make_move(Board *, Move);
void undo_move(Board *);
void print_board(const Board *, FILE *);
