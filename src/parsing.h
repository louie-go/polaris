#pragma once

#include <assert.h>

#include "board.h"
#include "types.h"

static inline color_t parse_color(char color_char) {
  assert(color_char == 'w' || color_char == 'b');

  return color_char == 'w' ? COLOR_WHITE : COLOR_BLACK;
}

static inline piecetype_t parse_piecetype(char piecetype_char) {
  switch (piecetype_char) {
    case 'p': return PIECETYPE_PAWN;
    case 'n': return PIECETYPE_KNIGHT;
    case 'b': return PIECETYPE_BISHOP;
    case 'r': return PIECETYPE_ROOK;
    case 'q': return PIECETYPE_QUEEN;
    case 'k': return PIECETYPE_KING;
    default:
      assert(0);
      return PIECETYPE_NONE;
  }
}

static inline piece_t parse_piece(char piece_char) {
  switch (piece_char) {
    case 'P': return PIECE_WP;
    case 'N': return PIECE_WN;
    case 'B': return PIECE_WB;
    case 'R': return PIECE_WR;
    case 'Q': return PIECE_WQ;
    case 'K': return PIECE_WK;
    case 'p': return PIECE_BP;
    case 'n': return PIECE_BN;
    case 'b': return PIECE_BB;
    case 'r': return PIECE_BR;
    case 'q': return PIECE_BQ;
    case 'k': return PIECE_BK;
    default:
      assert(0);
      return PIECE_NONE;
  }
}

static inline file_t parse_file(char file_char) {
  assert(file_char >= 'a' && file_char <= 'h');

  return file_char - 'a';
}

static inline rank_t parse_rank(char rank_char) {
  assert(rank_char >= '1' && rank_char <= '8');

  return rank_char - '1';
}

square_t parse_square(const char *square_str);
castle_rights_t parse_rights(const char *rights_str);
// TODO: better move parsing (more compliant to `movetype_t`)
move_t parse_move(const board_t *board, const char *move_str);
void parse_fen(const char *fen, board_t *board);
