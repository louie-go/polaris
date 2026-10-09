#include "parsing.h"

#include <stdio.h>
#include <string.h>

#include "bitboard.h"
#include "board.h"
#include "types.h"

square_t parse_square(const char *square_str) {
  if (*square_str == '-') return SQUARE_NONE;

  return new_square(parse_file(*square_str), parse_rank(square_str[1]));
}

castle_rights_t parse_rights(const char *rights_str) {
  if (*rights_str == '-') return CASTLE_NONE;

  for (castle_rights_t rights = 0;;rights_str++)
    switch (*rights_str) {
      case 'K':
        rights |= CASTLE_WK;
        break;
      case 'Q':
        rights |= CASTLE_WQ;
        break;
      case 'k':
        rights |= CASTLE_BK;
        break;
      case 'q':
        rights |= CASTLE_BQ;
        break;
      default: return rights;
    }
}

void parse_fen(const char *fen, board_t *board) {
  /************************
   *        BOARD         *
   ************************/
  memset(board->piece_bb, 0, sizeof(board->piece_bb));
  memset(board->color_bb, 0, sizeof(board->color_bb));
  memset(board->pieces, PIECE_NONE, sizeof(board->pieces));

  file_t file = FILE_NONE+1; rank_t rank = RANK_LENGTH-1;
  for (; *fen != ' '; fen++, file++) {
    char fen_char = *fen;
    if (fen_char >= '1' && fen_char <= '8') {
      file += fen_char-'1';
    } else if (fen_char == '/') {
      file = FILE_NONE; rank--;
    } else {
      square_t square = new_square(file, rank);
      bitboard_t sq_bb = new_bitboard(square);

      piece_t piece = parse_piece(fen_char);
      color_t color = piece_color(piece);

      board->piece_bb[color][piece_type(piece)] |= sq_bb;
      board->color_bb[color] |= sq_bb;
      board->color_bb[COLOR_ALL] |= sq_bb;
      board->pieces[square] = piece;
    }
  }

  assert(file == FILE_LENGTH);
  assert(rank == RANK_NONE+1);

  /************************
   *        TURN          *
   ************************/
  board->turn = parse_color(*++fen);

  /************************
   *     CASTLE RIGHTS    *
   ************************/
  fen++;
  assert(*fen == ' ');
  board->rights = parse_rights(++fen);

  /************************
   *   EN PASSANT SQUARE  *
   ************************/
  while (*fen != ' ') fen++;
  board->ep_square = parse_square(++fen);

  /************************
   *    HALF MOVE CLOCK   *
   ************************/
  if (*++fen != ' ') fen++;
  fen++;
  board->halfmove_clk = 0;
  while (*fen >= '0' && *fen <= '9') {
    board->halfmove_clk = board->halfmove_clk*10 + (*fen-'0');
    fen++;
  }

  /************************
   *   FULL MOVE NUMBER   *
   ************************/
  assert(*fen == ' ');
  fen++;
  board->fullmove_no = 0;
  while (*fen >= '0' && *fen <= '9') {
    board->fullmove_no = board->fullmove_no*10 + (*fen-'0');
    fen++;
  }

  board->ply = board->fullmove_no-1;
}
