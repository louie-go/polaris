#include <stdio.h>

#include "bitboard.h"
#include "board.h"
#include "formatting.h"
#include "movegen.h"
#include "parsing.h"
#include "types.h"

int main(void) {
  Board *board = calloc(1, sizeof(*board));
  parse_fen("8/8/8/3k4/8/8/4K3/R7 w - - 0 1", board);
  print_board(board, stdout);

  return 0;
}
