#pragma once
#include "types.hpp"

inline constexpr uint64_t FileA = 0x0101010101010101ULL;
inline constexpr uint64_t FileH = 0x8080808080808080ULL;

inline constexpr uint64_t Rank1 = 0x00000000000000FFULL;
inline constexpr uint64_t Rank3 = 0x0000000000FF0000ULL;
inline constexpr uint64_t Rank6 = 0x0000FF0000000000ULL;
inline constexpr uint64_t Rank8 = 0xFF00000000000000ULL;

inline constexpr uint64_t square_bb(Square square) {
  assert(square != Square::None);
  return 1ULL << +square;
};
