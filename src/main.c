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

#define DEPTH 7

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

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  uint64_t nodes = perft(DEPTH, board);
  clock_gettime(CLOCK_MONOTONIC, &end);

  double elapsed =
    (end.tv_sec - start.tv_sec) +
    (end.tv_nsec - start.tv_nsec) / 1e9;

  double nps = nodes / elapsed;

  printf("depth %d nodes %" PRIu64 " time %.2fs nps %.2f\n",
    DEPTH, nodes, elapsed, nps);

  free(board);
  return 0;
}
