#pragma once

#include <stdio.h>

#include "types.h"
#include "bitboard.h"

typedef struct {
  // TODO: implement PGN formatting
  // Move         history[1024];
  Bitboard     bitboards[COLOR_LEN][PIECETYPE_LEN];
  Piece        pieces[64];
  // +1 for `ALL` (check `types.h`)
  Bitboard     occupancies[COLOR_LEN+1];
  // TODO: implement zobrist hashing
  // uint64_t     hash;
  uint16_t     fullmove_no;
  uint8_t      halfmove_clk;
  Color        turn;
  CastleRights rights;
  Square       ep_square;
} Board;

void make_move(Board *, Move);
// TODO: implement states
// void undo_move(Board *);
void print_board(const Board *, FILE *);
