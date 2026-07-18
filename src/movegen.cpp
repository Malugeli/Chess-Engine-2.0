#include "movegen.hpp"

uint64_t rook_attacks(Square square, uint64_t occupied){
  uint64_t attacks = 0ULL;

  // Anders als beim Knight/King ist das KEIN Sprungziel, sondern eine Richtung in der wir Schritt für Schritt weiterlaufen.
  constexpr std::array<int8_t, 4> rank_directions{+1, -1, 0, 0};
  constexpr std::array<int8_t, 4> file_directions{0, 0, +1, -1};

  for(uint8_t direction = 0; direction < 4; ++direction){
    int rank = std::to_underlying(square) / 8;
    int file = std::to_underlying(square) % 8;

    while(true){
      rank += rank_directions[direction];
      file += file_directions[direction];

      if(rank < 0 || rank > 7 || file < 0 || file > 7){
        break; // Brettrand erreicht, Richtung fertig
      }

      uint64_t target = square_bb(static_cast<Square>(rank * 8 + file));
      attacks |= target; // Feld aufnehmen — auch wenn dort ein Blocker steht (gedeckt/angegriffen)

      if(occupied & target){
        break; // Blocker gefunden: Ray endet hier, Feld wurde aber noch mitgenommen
      }
    }
  }
  return attacks;
}

uint64_t bishop_attacks(Square square, uint64_t occupied) {
  uint64_t attacks = 0ULL;

  constexpr std::array<int8_t, 4> rank_direction{+1, +1, -1, -1};
  constexpr std::array<int8_t, 4> file_direction{-1, +1, -1, +1};
  
  for (uint8_t direction = 0; direction < 4; ++direction) {
    int rank = std::to_underlying(square) / 8;
    int file = std::to_underlying(square) % 8;
    while (true) {
      rank += rank_direction[direction];
      file += file_direction[direction];

      if (file > 7 || file < 0 || rank < 0 || rank > 7) {
        break;
      }
      uint64_t target = square_bb(static_cast<Square>((rank * 8) + file));
      attacks |= target;
      if (occupied & target) {
        break;
      }
    }
  }
  return attacks;
}

uint64_t queen_attacks(Square square, uint64_t occupied){
  return bishop_attacks(square, occupied) | rook_attacks(square, occupied);
};