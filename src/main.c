#include <stdio.h>
#include <stdlib.h>

#include "bitboard.h"
#include "board.h"
#include "formatting.h"
#include "movegen.h"
#include "parsing.h"
#include "types.h"

int main(void) {
  Board *board = malloc(sizeof(*board));
  parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board);
  print_board(board, stdout);

  return 0;
}
