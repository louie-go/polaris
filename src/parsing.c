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
  if (promo_char != 'n'
      && promo_char != 'b'
      && promo_char != 'r'
      && promo_char != 'q')
    return move;

  return set_move_type(parse_piecetype(promo_char)+MOVE_PROMO_N - KNIGHT, move);
}

void parse_fen(const char *fen, Board *board) {
  /************************
   *        BOARD         *
   ************************/
  Bitboard *all_bb = &board->color_bb[ALL];

  *all_bb = 0;
  for (Color color = COLOR_NONE+1; color < COLOR_LEN; color++)
    board->color_bb[color] = 0;
  for (PieceType type = PIECETYPE_NONE+1; type < PIECETYPE_LEN; type++)
    board->type_bb[type] = 0;

  File file = FILE_NONE+1; Rank rank = RANK_LEN-1;
  for (; *fen != ' '; fen++, file++) {
    char fen_char = *fen;
    Square square = new_square(file, rank);

    if (fen_char >= '1' && fen_char <= '8') {
      for (Square empty = square; empty < square+(fen_char-'0'); empty++)
        board->pieces[empty] = PIECE_NONE;
      file += fen_char-'1';
    } else if (fen_char == '/') {
      file = FILE_NONE; rank--;
    } else {
      Piece piece = parse_piece(fen_char);
      Bitboard sq_bb = new_bitboard(square);

      *all_bb |= sq_bb;
      board->color_bb[piece_color(piece)] |= sq_bb;
      board->type_bb[piece_type(piece)] |= sq_bb;
      board->pieces[square] = piece;
    }
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
