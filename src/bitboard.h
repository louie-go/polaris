#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#if !defined(__POPCNT__) || !defined(__BMI__)
#error "requires POPCNT and BMI"
#endif

#include <immintrin.h>

#include "types.h"

typedef uint64_t Bitboard;

static inline Bitboard bitboard_file(File file) {
  return 0x101010101010101ULL << file;
}

static inline Bitboard bitboard_rank(Rank rank) {
  return 0xFFULL << rank*N_RANKS;
}

static inline uint8_t popcount(Bitboard bitboard) {
  return _mm_popcnt_u64(bitboard);
}

static inline Square lsb(Bitboard bitboard) {
  assert(bitboard != 0);

  return _tzcnt_u64(bitboard);
}

static inline Bitboard clear_lsb(Bitboard bitboard) {
  return _blsr_u64(bitboard);
}

void print_bitboard(Bitboard bitboard, FILE *);
