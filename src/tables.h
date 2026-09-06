#if defined(__BMI2__) && !defined(USE_MAGICS)
#define USE_PEXT
#endif

#include "bitboard.h"
#include "types.h"

extern const Bitboard pawn_attacks[COLOR_LEN][SQUARE_LEN];
extern const Bitboard knight_attacks[SQUARE_LEN];
extern const Bitboard bishop_masks[SQUARE_LEN];
extern const Bitboard rook_masks[SQUARE_LEN];

#ifdef USE_PEXT
extern const Bitboard bishop_pext_attacks[SQUARE_LEN][512];
extern const Bitboard rook_pext_attacks[SQUARE_LEN][4096];
#else
extern const Bitboard bishop_magics[SQUARE_LEN];
extern const Bitboard rook_magics[SQUARE_LEN];
#endif

extern const Bitboard king_attacks[SQUARE_LEN];
extern const Bitboard between[SQUARE_LEN][SQUARE_LEN];
