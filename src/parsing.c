#include "parsing.h"

#include <stdio.h>

#include "bitboard.h"
#include "board.h"
#include "types.h"

Square parse_square(const char *square_str) {
  if (*square_str == '-') return NO_SQUARE;

  return (parse_rank(square_str[1])<<3) + parse_file(*square_str);
}

CastleRights parse_rights(const char *rights_str) {
  if (*rights_str == '-') return NO_CASTLES;

  CastleRights rights = 0;

  for (;;rights_str++)
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

Move parse_move(const char *move_str) {
  Square src = parse_square(move_str);
  Square dst = parse_square(move_str + 2);

  char promo_char = move_str[4];
  if (promo_char == '\0')
    return src | dst<<6;

  Piece promo = parse_piece(promo_char);
  return src | dst<<6 | promo<<12;
}

void parse_fen(const char *fen, Board *board) {
  /************************
   *        BOARD         *
   ************************/
  board->occupancies[ALL] = 0;
  for (Color color = WHITE; color < N_COLORS; color++) {
    board->occupancies[color] = 0;
    for (Piece piece = PAWN; piece < N_PIECES; piece++)
      board->bitboards[color][piece] = 0;
  }
  File file = FILE_A; Rank rank = RANK_8;
  for (; *fen != ' '; fen++, file++) {
    if (*fen == 'P' || *fen == 'N' || *fen == 'B' || *fen == 'R' || *fen == 'Q' || *fen == 'K' ||
        *fen == 'p' || *fen == 'n' || *fen == 'b' || *fen == 'r' || *fen == 'q' || *fen == 'k') {
      Bitboard bb_square = 1ULL<<((rank<<3) + file);
      Color color = *fen&32 ? BLACK : WHITE;
      Piece piece = parse_piece(*fen | 32);

      board->bitboards[color][piece] |= bb_square;
      board->occupancies[color] |= bb_square;
      board->occupancies[ALL] |= bb_square;
    } else if (*fen >= '1' && *fen <= '8')
      file += *fen-'1';
    else if (*fen == '/') {
      file = FILE_A-1; rank--;
    } else assert(0);
  }

  /************************
   *        TURN          *
   ************************/
  board->turn = parse_color(*++fen);

  /************************
   *     CASTLE RIGHTS    *
   ************************/
  fen++;
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
  fen++;
  board->fullmove_no = 0;
  while (*fen >= '0' && *fen <= '9') {
    board->fullmove_no = board->fullmove_no*10 + (*fen-'0');
    fen++;
  }
}
