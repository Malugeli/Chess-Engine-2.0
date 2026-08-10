#include "movegen.hpp"
#include <bit>

void add_moves(Square from, uint64_t targets, MoveList& list){
  while(targets){
    Square to = static_cast<Square>(std::countr_zero(targets));
    list.add(Move( from, to ));
    targets &= targets - 1; // ich komm darauf immer noch nicht klar
  }
};

void generate_knight_moves(const Board& b, Color turn_player, MoveList& list){
  //Das habe ich am Ende des Tages verstanden. Versuch das zu verstehen und mach dann mal King!
  uint64_t knights = b.get_bitmap(turn_player, PieceType::Knight);
  uint64_t pieces_turn_player = b.get_color_board(turn_player);

  while(knights){
    Square from = static_cast<Square>(std::countr_zero(knights));
    add_moves(from, kKnightAttack[+from] & ~pieces_turn_player, list);
    knights &= knights - 1;
  }
}

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