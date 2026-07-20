#include <print>
#include "movegen.hpp"
#include "debug.hpp"
// #include "board.hpp"
int main() {
  // std::print("e4:\n{}\n", pretty_bitboard(kKnightAttack[std::to_underlying(Square::e4)]));
  // std::print("a1:\n{}\n", pretty_bitboard(kKnightAttack[std::to_underlying(Square::a1)]));
  // std::print("h8:\n{}\n", pretty_bitboard(kKnightAttack[std::to_underlying(Square::h8)]));

  // std::print("h8:\n{}\n", pretty_bitboard(kKingAttack[std::to_underlying(Square::h8)]));
  // std::print("e4:\n{}\n", pretty_bitboard(kKingAttack[std::to_underlying(Square::e4)]));
  // Board board;
  // std::print("e4:\n{}\n", pretty_bitboard(bishop_attacks(Square::e4, board.occupied()) ));
  std::print("a1:\n{}\n", pretty_bitboard(kPawnAttack[std::to_underlying(Color::Black)][std::to_underlying(Square::a2)]));
}
