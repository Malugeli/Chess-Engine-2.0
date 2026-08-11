#include "movegen.hpp"
#include <bit>

static void add_moves(Square from, uint64_t targets, MoveList& list) noexcept {
  while(targets){
    Square to = static_cast<Square>(std::countr_zero(targets));
    list.add(Move( from, to ));
    targets &= targets - 1; 
  }
}

static void generate_knight_moves(const Board& b, const Color turn_player, MoveList& list) noexcept {
  uint64_t knights = b.get_bitmap(turn_player, PieceType::Knight);
  const uint64_t pieces_turn_player = b.get_color_board(turn_player);

  while(knights){
    Square from = static_cast<Square>(std::countr_zero(knights));
    add_moves(from, kKnightAttack[+from] & ~pieces_turn_player, list);
    knights &= knights - 1;
  }
}

static void generate_king_moves(const Board& b, const Color turn_player, MoveList& list) noexcept {
  uint64_t king_bb = b.get_bitmap(turn_player, PieceType::King);
  assert(king_bb != 0);
  const uint64_t pieces_turn_player = b.get_color_board(turn_player);
  Square from = static_cast<Square>(std::countr_zero(king_bb));
  add_moves(from, kKingAttack[ +from ] & ~pieces_turn_player, list);
}

static void generate_bishop_moves(const Board& b, const Color turn_player, MoveList& list) noexcept {
  uint64_t bishops = b.get_bitmap(turn_player, PieceType::Bishop);
  const uint64_t pieces_turn_player = b.get_color_board(turn_player);
  const uint64_t occupied{b.occupied()};
  while(bishops){
    Square from = static_cast<Square>(std::countr_zero(bishops));
    add_moves(from, bishop_attacks(from, occupied) & ~pieces_turn_player, list);
    bishops &= bishops - 1;
  }
}

static void generate_rook_moves(const Board& b, const Color turn_player, MoveList& list) noexcept {
  uint64_t rooks = b.get_bitmap(turn_player, PieceType::Rook);
  const uint64_t pieces_turn_player = b.get_color_board(turn_player);
  const uint64_t occupied{b.occupied()};
  while(rooks){
    Square from = static_cast<Square>(std::countr_zero(rooks));
    add_moves(from, rook_attacks(from, occupied) & ~pieces_turn_player, list);
    rooks &= rooks - 1;
  }
}

static void generate_queen_moves(const Board& b, const Color turn_player, MoveList& list) noexcept {
  uint64_t queen = b.get_bitmap(turn_player, PieceType::Queen);
  const uint64_t pieces_turn_player = b.get_color_board(turn_player);
  const uint64_t occupied{b.occupied()};
  while(queen){ 
    Square from = static_cast<Square>(std::countr_zero(queen));
    add_moves(from, queen_attacks(from, occupied) & ~pieces_turn_player, list);
    queen &= queen - 1;
  }
}

static void generate_pawn_moves(const Board& b, const Color turn_player, MoveList& list) noexcept {
  
}

void generate_moves(const Board& b, MoveList& list) noexcept {
  list.clear();
  Color turn_player = b.get_gamestate().side_to_move;

  generate_pawn_moves(b, turn_player, list);
  generate_knight_moves(b, turn_player, list);
  generate_king_moves(b, turn_player, list);
  generate_rook_moves(b, turn_player, list);
  generate_bishop_moves(b, turn_player, list);
  generate_queen_moves(b, turn_player, list);

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
      attacks |= target; // Feld aufnehmen — auch wenn dort unsere Figur steht, Check passiert beim generaten.

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
    int rank = +square / 8;
    int file = +square % 8;
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
}