// movegen.h
#pragma once

#include <stdint.h>

#include "board.h"
#include "types.h"

#ifndef __BMI2__
#error "requires BMI2"
#endif

static const uint8_t MAX_MOVES = 218;

uint8_t legal_moves(const Board *board, Move *moves);
uint8_t pseudo_legal_moves(const Board *board, Move *moves);
