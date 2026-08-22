#include "formatting.h"

#include <assert.h>
#include <stdio.h>

#include "board.h"
#include "types.h"

void format_square(Square square, char *buffer) {
  if (square == NO_SQUARE) {
    *buffer++ = '-';
    *buffer = '\0';
    return;
  }

  assert(square > NO_SQUARE && square < N_SQUARES);

  *buffer++ = format_file(square%8);
  *buffer++ = format_rank(square/8);
  *buffer = '\0';
}

void format_rights(CastleRights rights, char *buffer) {
  assert(rights <= ALL_CASTLES);
  if (rights == NO_CASTLES) {
    *buffer++ = '-';
    *buffer = '\0';
    return;
  }

  for (CastleRights right = CASTLE_WK; right <= (1 << (N_CASTLES-1)); right <<= 1)
    switch (right&rights) {
      case CASTLE_WK:
        *buffer++ = 'K';
        break;
      case CASTLE_WQ:
        *buffer++ = 'Q';
        break;
      case CASTLE_BK:
        *buffer++ = 'k';
        break;
      case CASTLE_BQ:
        *buffer++ = 'q';
        break;
    }

  *buffer = '\0';
}

void format_move(Move move, char *buffer) {
  format_square(move_src(move), buffer);
  format_square(move_dst(move), buffer+2);

  MoveType type = move_type(move);
  if (type >= MOVE_PROMO_N && type <= MOVE_PROMO_Q) {
    // `Piece` and `MoveType` overlap (check `types.h`)
    buffer[4] = format_piece(type);
  }
}

void format_fen(const Board *board, char *buffer) {
  /************************
   *        BOARD         *
   ************************/
  for (Rank rank = RANK_8; rank >= RANK_1; rank--) {
    for (File file = FILE_A; file < N_FILES; file++) {
      Bitboard bb_square = 1ULL<<(rank*8 + file);
      for (Color color = WHITE; color < N_COLORS; color++)
        for (Piece piece = PAWN; piece < N_PIECES; piece++)
          if (bb_square & board->bitboards[color][piece]) {
            char piece_char = format_piece(piece);
            *buffer++ = color==WHITE ? piece_char&-33 : piece_char;
            goto next_square;
          }

      if (buffer[-1] >= '1' && buffer[-1] <= '8') buffer[-1]++;
      else *buffer++ = '1';

next_square:
      ;
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
  format_rights(board->rights, buffer);
  
  /************************
   *   EN PASSANT SQUARE  *
   ************************/
  while (*buffer != '\0') buffer++;
  *buffer++ = ' ';
  format_square(board->ep_square, buffer);

  /************************
   *    HALF MOVE CLOCK   *
   ************************/
  // TODO: make faster halfmove and fullmove formatter and not just `sprintf`.
  if (*++buffer != '\0') buffer++;
  *buffer++ = ' ';
  buffer += sprintf(buffer, "%u", board->halfmove_clk);

  /************************
   *   FULL MOVE NUMBER   *
   ************************/
  *buffer++ = ' ';
  sprintf(buffer, "%u", board->fullmove_no);
}
