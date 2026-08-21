#include "bitboard.h"

#include <stdio.h>

#include "formatting.h"
#include "inttypes.h"
#include "types.h"

void print_bitboard(Bitboard bitboard, FILE *stream) {
  for (Rank rank = RANK_8; rank > NO_RANK; rank--) {
    fputc(format_rank(rank), stream);
    fputc(' ', stream);
    for (File file = FILE_A; file < N_FILES; file++) {
      Bitboard bb_square = 1ULL<<(rank*8 + file);

      if (bb_square & bitboard) {
        fputc('*', stream);
      } else {
        fputc('.', stream);
      }
      fputc(' ', stream);
    }

    fputc('\n', stream);
  }

  fputs("  a b c d e f g h\n", stream);

#ifndef NDEBUG
  char lsb_str[3];
  format_square(lsb(bitboard), lsb_str);

  fprintf(stream, "\nvalue 0x%" PRIX64 " count %" PRIu8 " lsb %s\n",
      bitboard, popcount(bitboard), lsb_str);
#endif
}
