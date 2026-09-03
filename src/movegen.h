#pragma once

#include <stdint.h>

#include "board.h"
#include "types.h"

#define MAX_MOVES 218

Move *legal_moves(const Board *board, Move *moves);
