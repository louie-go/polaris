// movegen.h
#pragma once

#include <stdint.h>

#include "board.h"
#include "types.h"

#ifndef __BMI2__
#error "requires BMI2"
#endif

uint8_t legal_moves(const Board *, Move *);
uint8_t pseudo_legal_moves(const Board *, Move *);
