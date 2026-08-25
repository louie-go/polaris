#include "zobrist.h"

#include "bitboard.h"
#include "types.h"

static Zobrist board_hashes[COLOR_LEN][PIECETYPE_LEN][SQUARE_LEN];
static Zobrist turn_hash[COLOR_LEN];
static Zobrist castle_hashes[CASTLE_LEN];
static Zobrist ep_hash[FILE_LEN];

static uint64_t state;
static inline uint64_t rand64(void) {
  state ^= state >> 12;
  state ^= state << 25;
  state ^= state >> 27;
  return state * 0x2545F4914F6CDD1DULL;
}

void init_zobrist(uint64_t seed) {
  state = seed ? seed : 0x9E3779B97F4A7C15ULL;

  for (PieceType type = PIECETYPE_NONE+1; type < PIECETYPE_LEN; type++)
    for (Color color = COLOR_NONE+1; color < COLOR_LEN; color++)
      for (Square square = SQUARE_NONE+1; square < SQUARE_LEN; square++)
        board_hashes[color][type][square] = rand64();

  // "One number to indicate the side to move is black"
  // - https://chessprogramming.org/Zobrist_Hashing#initialization
  turn_hash[BLACK] = rand64();

  for (uint8_t right = 0; right < CASTLE_LEN; right++)
    castle_hashes[right] = rand64();

  for (File file = FILE_NONE+1; file < FILE_LEN; file++)
    ep_hash[file] = rand64();
}

Zobrist hash_board(const Board *board) {
  Zobrist hash = 0ULL;

  for (Square square = SQUARE_NONE+1; square < SQUARE_LEN; square++) {
    Piece piece = board->pieces[square];
    if (piece == PIECE_NONE) continue;
    hash ^= board_hashes[piece_color(piece)][piece_type(piece)][square];
  }

  hash ^= turn_hash[board->turn];
  
  for (CastleRights right = CASTLE_WK; right < 1<<CASTLE_LEN; right <<= 1)
    if (right & board->rights)
      hash ^= castle_hashes[lsb(right)];

  if (board->ep != FILE_NONE)
    hash ^= ep_hash[board->ep];

  return hash;
}
