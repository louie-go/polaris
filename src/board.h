#pragma once

#include <stdint.h>

#include "types.h"
#include "bitboard.h"

static const uint16_t MAX_GAME_PLY = 17697;

typedef struct {
  Bitboard     bitboards[COLOR_LEN][PIECETYPE_LEN];
  Piece        pieces[64];
  // +1 for `ALL` (check `types.h`)
  Bitboard     occupancies[COLOR_LEN+1];
  uint16_t     fullmove_no;
  uint8_t      halfmove_clk;
  Color        turn;
  CastleRights rights;
  File         ep;
} Board;

static inline Square board_ep_square(const Board *board) {
  if (board->ep == FILE_NONE) return SQUARE_NONE;

  return new_square(
    board->ep,
    board->turn == WHITE ? RANK_6 : RANK_3);
}

void make_move(Board *board, Move move);
void undo_move(Board *board);
void print_board(const Board *board, FILE *stream);
