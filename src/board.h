#pragma once

#include <stdio.h>

#include "types.h"
#include "bitboard.h"

typedef struct {
  Move         history[1024];
  Bitboard     bitboards[N_COLORS][N_PIECES];
  Piece        pieces[64];
  // +1 for `ALL` (check `types.h`)
  Bitboard     occupancies[N_COLORS+1];
  // TODO: implement zobrist hashing
  // uint64_t     hash;
  uint16_t     fullmove_no;
  uint8_t      halfmove_clk;
  Color        turn;
  CastleRights rights;
  Square       ep_square;
} Board;

void make_move(Board *, Move);
void undo_move(Board *);
void print_board(const Board *, FILE *);
