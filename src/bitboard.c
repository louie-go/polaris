#include "bitboard.h"

#include <inttypes.h>
#include <stdio.h>

#include "formatting.h"
#include "types.h"

void print_bitboard(bitboard_t bitboard, FILE *stream) {
  for (rank_t rank = RANK_LENGTH-1; rank > RANK_NONE; rank--) {
    fputc(format_rank(rank), stream);
    fputc(' ', stream);
    for (file_t file = FILE_NONE+1; file < FILE_LENGTH; file++) {
      bitboard_t sq_bb = new_bitboard(new_square(file, rank));
      if (sq_bb & bitboard) fputc('*', stream);
      else fputc('.', stream);
      fputc(' ', stream);
    }

    fputc('\n', stream);
  }

  fputs("  a b c d e f g h\n", stream);

#ifndef NDEBUG
  char lsb_str[3];
  format_square(
    bitboard!=0 ? lsb(bitboard) : SQUARE_NONE,
    lsb_str);

  fprintf(stream, "\nvalue 0x%016" PRIX64 " count %" PRIu8 " lsb %s\n",
    bitboard, popcount(bitboard), lsb_str);
#endif
}
