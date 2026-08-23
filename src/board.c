#include "board.h"

#include <stdio.h>
#include <inttypes.h>

#include "bitboard.h"
#include "formatting.h"
#include "parsing.h"
#include "types.h"

void print_board(const Board *board, FILE *stream) {
  for (Rank rank = RANK_LEN-1; rank > RANK_NONE; rank--) {
    fputc(format_rank(rank), stream);
    fputc(' ', stream);
    for (File file = FILE_NONE+1; file < FILE_LEN; file++) {
      Square square = (rank<<3) + file;
      Piece piece = board->pieces[square];
      if (piece != SQUARE_NONE) fputc(format_piece(piece), stream);
      else fputc('.', stream);
      fputc(' ', stream);
    }

    fputc('\n', stream);
  }

  fputs("  a b c d e f g h\n", stream);

#ifndef NDEBUG
  char fen[93];
  format_fen(board, fen);

  fputc('\n', stream);
  fputs(fen, stream);
  fputc('\n', stream);
#endif
}
