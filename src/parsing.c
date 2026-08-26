#include "parsing.h"

#include <stdio.h>

#include "bitboard.h"
#include "board.h"
#include "types.h"

Square parse_square(const char *square_str) {
  if (*square_str == '-') return SQUARE_NONE;

  return new_square(parse_file(*square_str), parse_rank(square_str[1]));
}

CastleRights parse_rights(const char *rights_str) {
  if (*rights_str == '-') return CASTLE_NONE;

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

// TODO: better move parsing (more compliant to `MoveType`)
// * NOTE: would need `const Board *` as an argument
Move parse_move(const char *move_str) {
  Square src = parse_square(move_str);
  Square dst = parse_square(move_str + 2);
  Move move = new_move(src, dst, MOVE_NORMAL);

  char promo_char = move_str[4];
  if (promo_char == '\0')
    return move;

  // `Piece` and `MoveType` overlap (check `types.h`)
  return set_move_type(parse_piecetype(promo_char), move);
}

void parse_fen(const char *fen, Board *board) {
  board->ply = 0;

  /************************
   *        BOARD         *
   ************************/
  board->occupancies[ALL] = 0;
  for (Color color = COLOR_NONE+1; color < COLOR_LEN; color++) {
    board->occupancies[color] = 0;
    for (PieceType type = PIECETYPE_NONE+1; type < PIECETYPE_LEN; type++)
      board->bitboards[color][type] = 0;
  }
  File file = FILE_NONE+1; Rank rank = RANK_LEN-1;
  for (; *fen != ' '; fen++, file++) {
    Square square = new_square(file, rank);
    if (*fen == 'P' || *fen == 'N' || *fen == 'B' || *fen == 'R' || *fen == 'Q' || *fen == 'K' ||
        *fen == 'p' || *fen == 'n' || *fen == 'b' || *fen == 'r' || *fen == 'q' || *fen == 'k') { 
      Piece piece = parse_piece(*fen);
      Color color = piece_color(piece);
      Bitboard sq_bb = new_bitboard(square);

      board->bitboards[color][piece_type(piece)] |= sq_bb;
      board->occupancies[color] |= sq_bb;
      board->occupancies[ALL] |= sq_bb;
      board->pieces[square] = piece;
    } else if (*fen >= '1' && *fen <= '8') {
      for (Square empty = square; empty < square+(*fen-'0'); empty++)
        board->pieces[empty] = PIECE_NONE;
      file += *fen-'1';
    } else if (*fen == '/') {
      file = FILE_NONE; rank--;
    } else assert(0);
  }
  assert(file == FILE_LEN);
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
  if (*++fen == '-') board->ep = FILE_NONE;
  else board->ep = square_file(parse_square(fen));

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
}
