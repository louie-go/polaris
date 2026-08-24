#pragma once

#include <stdint.h>

#include "board.h"

typedef uint64_t Zobrist;

void init_zobrist(uint64_t);
Zobrist hash_board(const Board *);
