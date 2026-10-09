#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#if defined(__POPCNT__) || defined(__BMI__)
#include <immintrin.h>
#endif

#include "types.h"

typedef uint64_t bitboard_t;

static inline bitboard_t new_bitboard(square_t square) {
  return 1ULL << square;
}

static inline bitboard_t bitboard_file(file_t file) {
  return 0x101010101010101ULL << file;
}

static inline bitboard_t bitboard_rank(rank_t rank) {
  return 0xFFULL << rank*RANK_LENGTH;
}

static inline uint8_t popcount(bitboard_t bitboard) {
#ifdef __POPCNT__
  return _mm_popcnt_u64(bitboard);
#else
  return __builtin_popcountll(bitboard);
#endif
}

static inline square_t lsb(bitboard_t bitboard) {
  assert(bitboard);

#ifdef __BMI__
  return _tzcnt_u64(bitboard);
#else
  return __builtin_ctzll(bitboard);
#endif
}

static inline bitboard_t clear_lsb(bitboard_t bitboard) {
#ifdef __BMI__
  return _blsr_u64(bitboard);
#else
  return bitboard & bitboard-1;
#endif
}

static inline square_t pop_lsb(bitboard_t *bitboard) {
  square_t square = lsb(*bitboard);
  *bitboard = clear_lsb(*bitboard);
  return square;
}

void print_bitboard(bitboard_t bitboard, FILE *stream);
