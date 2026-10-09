#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "types.h"
#include "bitboard.h"

#define MAX_GAME_PLY 17697

typedef struct {
  move_t          move;
  castle_rights_t rights;
  piece_t         captured;
  square_t        ep_square;
  uint8_t         halfmove_clk;
} state_t;

typedef struct {
  state_t         states[MAX_GAME_PLY];
  bitboard_t      piece_bb[COLOR_LENGTH][PIECETYPE_LENGTH];
  piece_t         pieces[SQUARE_LENGTH];
  // +1 for `COLOR_ALL` (check `types.h`)
  bitboard_t      color_bb[COLOR_LENGTH+1];
  uint16_t        ply;
  uint16_t        fullmove_no;
  uint8_t         halfmove_clk;
  color_t         turn;
  castle_rights_t rights;
  square_t        ep_square;
} board_t;

extern const char STARTING_POSITION[];

void make_move(board_t *board, move_t move);
void undo_move(board_t *board);

void print_board(const board_t *board, FILE *stream);
