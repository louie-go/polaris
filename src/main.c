#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitboard.h"
#include "board.h"
#include "formatting.h"
#include "movegen.h"
#include "parsing.h"
#include "types.h"

#define PERFT_TESTS_LEN 23

typedef struct {
  char     fen[MAX_FEN_BUFFER];
  uint64_t nodes;
  uint8_t  depth;
} PerftTest;

static const PerftTest perft_tests[PERFT_TESTS_LEN] = {
  {"r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2",8,1},
  {"8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3",8,1},
  {"r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2",19,1},
  {"r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2",5,1},
  {"2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3 2",44,1},
  {"rnb2k1r/pp1Pbppp/2p5/q7/2B5/8/PPPQNnPP/RNB1K2R w KQ - 3 9",39,1},
  {"2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5 4",9,1},
  {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",62379,3},
  {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",89890,3},
  {"3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1",1134888,6},
  {"8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1",1015133,6},
  {"8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1",1440467,6},
  {"5k2/8/8/8/8/8/8/4K2R w K - 0 1",661072,6},
  {"3k4/8/8/8/8/8/8/R3K3 w Q - 0 1",803711,6},
  {"r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1",1274206,4},
  {"r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1",1720476,4},
  {"2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1",3821001,6},
  {"8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1",1004658,5},
  {"4k3/1P6/8/8/8/8/K7/8 w - - 0 1",217342,6},
  {"8/P1k5/K7/8/8/8/8/8 w - - 0 1",92683,6},
  {"K1k5/8/P7/8/8/8/8/8 w - - 0 1",2217,6},
  {"8/k1P5/8/1K6/8/8/8/8 w - - 0 1",567584,7},
  {"8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1",23527,4},
};

uint64_t perft(uint8_t depth, Board *board) {
  if (depth == 0) return 1;
  uint64_t nodes = 0;

  Move moves[MAX_MOVES];
  Move *moves_end = legal_moves(board, moves);

  for (Move *move = moves; move < moves_end; move++) {
    make_move(board, *move);
    uint64_t move_nodes = perft(depth-1, board);
    nodes += move_nodes;
    undo_move(board);
  }

  return nodes;
}

int main(void) {
  Board *board = malloc(sizeof(*board));

  for (uint8_t test = 0; test < PERFT_TESTS_LEN; test++) {
    PerftTest perft_test = perft_tests[test];

    parse_fen(perft_test.fen, board);
    assert(perft(perft_test.depth, board) == perft_test.nodes);
  }

  puts("all tests passed");
  free(board);
  return 0;
}
