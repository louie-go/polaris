#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#if defined(__POPCNT__) || defined(__BMI__)
#include <immintrin.h>
#endif

#include "types.h"

typedef uint64_t Bitboard;

static inline Bitboard new_bitboard(Square square) {
  return 1ULL << square;
}

static inline Bitboard bitboard_file(File file) {
  return 0x101010101010101ULL << file;
}

static inline Bitboard bitboard_rank(Rank rank) {
  return 0xFFULL << rank*RANK_LEN;
}

static inline uint8_t popcount(Bitboard bitboard) {
#ifdef __POPCNT__
  return _mm_popcnt_u64(bitboard);
#else
  return __builtin_popcountll(bitboard);
#endif
}

static inline Square lsb(Bitboard bitboard) {
  assert(bitboard != 0);

#ifdef __BMI__
  return _tzcnt_u64(bitboard);
#else
  return __builtin_ctzll(bitboard);
#endif
}

static inline Bitboard clear_lsb(Bitboard bitboard) {
#ifdef __BMI__
  return _blsr_u64(bitboard);
#else
  return bitboard & bitboard-1;
#endif
}

static inline Square pop_lsb(Bitboard *bitboard) {
  Square square = lsb(*bitboard);
  *bitboard = clear_lsb(*bitboard);
  return square;
}

void print_bitboard(Bitboard bitboard, FILE *);
