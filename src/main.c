#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bitboard.h"
#include "board.h"
#include "formatting.h"
#include "movegen.h"
#include "parsing.h"
#include "types.h"

#define DEPTH 6

uint64_t perft(uint8_t depth, Board *board) {
  if (depth == 0) return 1;
  uint64_t nodes = 0;

  Move moves[MAX_MOVES];
  Move *moves_end = legal_moves(board, moves);

  char move_str[6];
  for (Move *move = moves; move < moves_end; move++) {
    make_move(board, *move);
    uint64_t move_nodes = perft(depth-1, board);
    nodes += move_nodes;

    if (depth == DEPTH) {
      format_move(*move, move_str);
      printf("move %s nodes %" PRIu64 "\n",
        move_str, move_nodes);
    }

    undo_move(board);
  }

  return nodes;
}

int main(void) {
  Board *board = malloc(sizeof(*board));
  parse_fen(STARTING_POSITION, board);

  print_board(board, stdout);

  printf("nodes %" PRIu64 "\n", perft(DEPTH, board));

  free(board);
  return 0;
}
