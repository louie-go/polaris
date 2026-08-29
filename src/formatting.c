#include "formatting.h"

#include <assert.h>
#include <stdio.h>

#include "board.h"
#include "types.h"

char *format_square(Square square, char *buffer) {
  if (square == SQUARE_NONE) {
    *buffer++ = '-';
    *buffer = '\0';
    return buffer;
  }

  assert(square > SQUARE_NONE && square < SQUARE_LEN);

  *buffer++ = format_file(square_file(square));
  *buffer++ = format_rank(square_rank(square));
  *buffer = '\0';

  return buffer;
}

char *format_rights(CastleRights rights, char *buffer) {
  assert(rights <= CASTLE_ALL);

  if (rights == CASTLE_NONE) {
    *buffer++ = '-';
    *buffer = '\0';
    return buffer;
  }

  for (CastleRights right = CASTLE_WK; right < 1<<CASTLE_LEN; right <<= 1)
    switch (right & rights) {
      case CASTLE_WK: *buffer++ = 'K'; break;
      case CASTLE_WQ: *buffer++ = 'Q'; break;
      case CASTLE_BK: *buffer++ = 'k'; break;
      case CASTLE_BQ: *buffer++ = 'q'; break;
    }

  *buffer = '\0';
  return buffer;
}

char *format_move(Move move, char *buffer) {
  buffer = format_square(move_src(move), buffer);
  buffer = format_square(move_dst(move), buffer);

  MoveType type = move_type(move);
  if (is_promotion(move))
    *buffer++ = format_piecetype(type-MOVE_PROMO_N + KNIGHT);

  return buffer;
}

char *format_fen(const Board *board, char *buffer) {
  /************************
   *        BOARD         *
   ************************/
  char *start = buffer;
  for (Rank rank = RANK_LEN-1; rank > RANK_NONE; rank--) {
    for (File file = FILE_NONE+1; file < FILE_LEN; file++) {
      Square square = new_square(file, rank);
      Piece piece = board->pieces[square];

      if (piece != PIECE_NONE) *buffer++ = format_piece(piece);
      else
        if (buffer != start
            && buffer[-1] >= '1'
            && buffer[-1] <= '8') buffer[-1]++;
        else *buffer++ = '1';
    }

    if (rank != RANK_1) *buffer++ = '/';
  }

  /************************
   *        TURN          *
   ************************/
  *buffer++ = ' ';
  *buffer++ = format_color(board->turn);

  /************************
   *     CASTLE RIGHTS    *
   ************************/
  *buffer++ = ' ';
  buffer = format_rights(board->rights, buffer);
  
  /************************
   *   EN PASSANT SQUARE  *
   ************************/
  *buffer++ = ' ';
  buffer = format_square(board->ep_square, buffer);

  /************************
   *    HALF MOVE CLOCK   *
   ************************/
  // TODO: make faster halfmove and fullmove formatter and not just `sprintf`.
  *buffer++ = ' ';
  buffer += sprintf(buffer, "%u", board->halfmove_clk);

  /************************
   *   FULL MOVE NUMBER   *
   ************************/
  *buffer++ = ' ';
  buffer += sprintf(buffer, "%u", board->fullmove_no);

  return buffer;
}
