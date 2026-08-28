#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitboard.h"
#include "board.h"
#include "formatting.h"
#include "movegen.h"
#include "parsing.h"
#include "types.h"
#include "zobrist.h"

int main(void) {
  init_zobrist(0);
  Board *board = malloc(sizeof(*board));

  parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board);
  assert(hash_board(board) == 0xEE146256CCDE7759ULL);

  print_board(board, stdout);

  free(board);
  return 0;
}
