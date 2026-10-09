#pragma once

#include "bitboard.h"
#include "types.h"

extern const bitboard_t pawn_attacks[COLOR_LENGTH][SQUARE_LENGTH];
extern const bitboard_t knight_attacks[SQUARE_LENGTH];
extern const bitboard_t bishop_masks[SQUARE_LENGTH];
extern const bitboard_t rook_masks[SQUARE_LENGTH];

#if defined(__BMI2__) && !defined(USE_MAGICS)
extern const bitboard_t bishop_pext_attacks[SQUARE_LENGTH][512];
extern const bitboard_t rook_pext_attacks[SQUARE_LENGTH][4096];
#else
extern const bitboard_t bishop_magics[SQUARE_LENGTH];
extern const bitboard_t rook_magics[SQUARE_LENGTH];
#endif

extern const bitboard_t king_attacks[SQUARE_LENGTH];
extern const bitboard_t between[SQUARE_LENGTH][SQUARE_LENGTH];
