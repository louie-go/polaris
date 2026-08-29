#pragma once

#include <stdint.h>

#include "types.h"
#include "bitboard.h"

static const uint16_t MAX_GAME_PLY = 17697;

// bits 1-8:   halfmove clock      (8 bits)
// bits 9-14:  source square       (6 bits)
// bits 15-20: destination square  (6 bits)
// bits 21-24: castling rights     (4 bits)
// bits 25-27: captured piece type (3 bits)
// bits 38-30: en passant          (3 bits)
// bits 31-32: promotion           (2 bits)
typedef uint32_t State;

static inline uint8_t state_halfmove_clk(State state) {
  return state & 0xFF;
}

static inline Square state_src(State state) {
  return state>>8 & 0x3F;
}

static inline Square state_dst(State state) {
  return state>>14 & 0x3F;
}

static inline CastleRights state_rights(State state) {
  return state>>20 & 0xF;
}

static inline PieceType state_captured(State state) {
  return (state>>24 & 0x7) - 1;
}

static inline File state_ep(State state) {
  return state>>27 & 0x7;
}

static inline PieceType state_promotion(State state) {
  return (state>>30 & 0x3) + 1;
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
  Square       ep_square;
} Board;

static inline State new_state(const Board *board, Move move) {
  MoveType type = move_type(move);
  PieceType promotion = type >= MOVE_PROMO_N && type <= MOVE_PROMO_Q ?
    (type) : 1;

  return (State)board->halfmove_clk
    | (State)(move & 0xFFF) << 8
    | (State)board->rights << 20
    | (State)(piece_type(board->pieces[move_dst(move)])+1) << 24
    | (State)square_file(board->ep_square) << 27
    | (State)(promotion-1) << 30;
}

void make_move(Board *board, Move move);
void undo_move(Board *board);
void print_board(const Board *board, FILE *stream);
