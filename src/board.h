#pragma once

#include <stdint.h>

#include "types.h"
#include "bitboard.h"

static const uint16_t MAX_GAME_PLY = 17697;

// bits 1-16:  move
// bits 17-24: halfmove clock
// bits 25-28: castling rights
// bits 29-32: captured piece
typedef uint32_t State;

static inline Move state_move(State state) {
  return state & 0xFFFF;
}

static inline uint8_t state_halfmove_clk(State state) {
  return state>>16 & 0xFF;
}

static inline CastleRights state_rights(State state) {
  return state>>24 & 0xF;
}

static inline Piece state_captured(State state) {
  return (state>>28 & 0xF) - 1;
}

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
  File         ep;
} Board;

static inline State new_state(const Board *board, Move move) {
  return (State)move
    | (State)board->halfmove_clk<<16
    | (State)board->rights<<24
    | (State)(board->pieces[move_dst(move)]+1)<<28;
}

static inline Square board_ep_square(const Board *board) {
  if (board->ep == FILE_NONE) return SQUARE_NONE;

  return new_square(
    board->ep,
    board->turn == WHITE ? RANK_6 : RANK_3);
}

void make_move(Board *board, Move move);
void undo_move(Board *board);
void print_board(const Board *board, FILE *stream);
