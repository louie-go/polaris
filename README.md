# Polaris

A chess engine written in C.

## Todo

- [x] Implement `make_move` and `undo_move`.
- [x] Implement move generation.
- [ ] Implement evaluation via NNUE.
- [ ] Add fallback for functions with intrinsics.
- [x] Implement Zobrist hashing.
- [ ] Implement PGN formatting.
- [x] Implement states for `undo_move`.
- [ ] Implement move generation faster than stockfish (Stockfish is ~85MN/s, Polaris is ~41MN/s), the goal is about 100MN/s+ on my machine.
