#include <print>
#include "board.hpp"
#include "perft.hpp"

int main() {
  // std::print("e4:\n{}\n", pretty_bitboard(kKnightAttack[std::to_underlying(Square::e4)]));
  // std::print("a1:\n{}\n", pretty_bitboard(kKnightAttack[std::to_underlying(Square::a1)]));
  // std::print("h8:\n{}\n", pretty_bitboard(kKnightAttack[std::to_underlying(Square::h8)]));
  // std::print("h8:\n{}\n", pretty_bitboard(kKingAttack[std::to_underlying(Square::h8)]));
  // std::print("e4:\n{}\n", pretty_bitboard(kKingAttack[std::to_underlying(Square::e4)]));

  Board board;
  auto test = board.set_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
  if (test) {
    print_perft_divide(board, 2);
  }
}
