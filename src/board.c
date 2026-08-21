#include "board.h"

#include <stdio.h>
#include <inttypes.h>

#include "bitboard.h"
#include "formatting.h"
#include "parsing.h"
#include "types.h"

void print_board(const Board *board, FILE *stream) {
  for (Rank rank = RANK_8; rank >= RANK_1; rank--) {
    fputc(format_rank(rank), stream);
    fputc(' ', stream);
    for (File file = FILE_A; file < N_FILES; file++) {
      Bitboard bb_square = 1ULL<<((rank<<3) + file);

      if (bb_square & board->bitboards[WHITE][PAWN]) fputc('P', stream);
      else if (bb_square & board->bitboards[WHITE][KNIGHT]) fputc('N', stream);   
      else if (bb_square & board->bitboards[WHITE][BISHOP]) fputc('B', stream);   
      else if (bb_square & board->bitboards[WHITE][ROOK]) fputc('R', stream);   
      else if (bb_square & board->bitboards[WHITE][QUEEN]) fputc('Q', stream);   
      else if (bb_square & board->bitboards[WHITE][KING]) fputc('K', stream);   
      else if (bb_square & board->bitboards[BLACK][PAWN]) fputc('p', stream);   
      else if (bb_square & board->bitboards[BLACK][KNIGHT]) fputc('n', stream);   
      else if (bb_square & board->bitboards[BLACK][BISHOP]) fputc('b', stream);   
      else if (bb_square & board->bitboards[BLACK][ROOK]) fputc('r', stream);   
      else if (bb_square & board->bitboards[BLACK][QUEEN]) fputc('q', stream);   
      else if (bb_square & board->bitboards[BLACK][KING]) fputc('k', stream);
      else fputc('.', stream);
      fputc(' ', stream);
    }

    fputc('\n', stream);
  }

  fputs("  a b c d e f g h\n", stream);

#ifndef NDEBUG
  char fen[256];
  format_fen(board, fen);

  fputc('\n', stream);
  fputs(fen, stream);
  fputc('\n', stream);
#endif
}
