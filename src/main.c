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
  parse_fen(STARTING_POSITION, board);

  print_board(board, stdout);

  Move moves[MAX_MOVES];
  Move *moves_end = legal_moves(board, moves);

  char move_str[MAX_MOVE_BUFFER];
  Move *move = moves;
  while (move++ != moves_end) {
    format_move(*move, move_str);
    puts(move_str);
  }

  free(board);
  return 0;
}
