#include "formatting.h"

#include <assert.h>
#include <stdio.h>

#include "board.h"
#include "types.h"

const char piecetype_chars[PIECETYPE_LENGTH] = "pnbrqk";
const char piece_chars[PIECE_LENGTH] = "PpNnBbRrQqKk";

char *format_square(square_t square, char *buffer) {
  if (square == SQUARE_NONE) {
    *buffer++ = '-';
    *buffer = '\0';
    return buffer;
  }

  assert(square > SQUARE_NONE && square < SQUARE_LENGTH);

  *buffer++ = format_file(square_file(square));
  *buffer++ = format_rank(square_rank(square));
  *buffer = '\0';
  return buffer;
}

char *format_rights(castle_rights_t rights, char *buffer) {
  assert(rights <= CASTLE_MAX);

  if (rights == CASTLE_NONE) {
    *buffer++ = '-';
    *buffer = '\0';
    return buffer;
  }

  for (castle_rights_t right = CASTLE_WK; right < 1<<CASTLE_LENGTH; right <<= 1)
    switch (right & rights) {
      case CASTLE_WK: *buffer++ = 'K'; break;
      case CASTLE_WQ: *buffer++ = 'Q'; break;
      case CASTLE_BK: *buffer++ = 'k'; break;
      case CASTLE_BQ: *buffer++ = 'q'; break;
    }

  *buffer = '\0';
  return buffer;
}

char *format_move(move_t move, char *buffer) {
  buffer = format_square(move_src(move), buffer);
  buffer = format_square(move_dst(move), buffer);

  movetype_t type = move_type(move);
  if (is_promotion(type))
    *buffer++ = format_piecetype(move_promo_piece(type));

  *buffer = '\0';
  return buffer;
}

char *format_fen(const board_t *board, char *buffer) {
  /************************
   *        BOARD         *
   ************************/
  char *start = buffer;
  for (rank_t rank = RANK_LENGTH-1; rank > RANK_NONE; rank--) {
    for (file_t file = FILE_NONE+1; file < FILE_LENGTH; file++) {
      square_t square = new_square(file, rank);
      piece_t piece = board->pieces[square];

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
