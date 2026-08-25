#include "movegen.hpp"
#include <bit>



static void add_moves(Square from, uint64_t targets, MoveList& list) noexcept {
  while(targets){
    Square to = static_cast<Square>(std::countr_zero(targets));
    list.add(Move( from, to ));
    targets &= targets - 1; 
  }
}

static void add_pawn_moves(uint64_t targets, int offset, MoveList& list) noexcept{
  while(targets){
    const int to = std::countr_zero(targets);
    list.add(Move(static_cast<Square>(to - offset), static_cast<Square>(to)));
    targets &= targets - 1;
  }
}

static void add_pawn_promo_moves(uint64_t targets, int offset, MoveList& list) noexcept{
  while(targets){
    const int to = std::countr_zero(targets);
    for(PieceType piece : {PieceType::Knight, PieceType::Bishop, PieceType::Rook, PieceType::Queen}){
      list.add(Move::make<MoveType::Promotion>(static_cast<Square>(to - offset), static_cast<Square>(to), piece));
    }
    targets &= targets - 1;
  }
}

static void add_pawn_ep_moves(uint64_t pawns, Square ep, Color enemy, MoveList& list){
      uint64_t ep_capture = kPawnAttack[+enemy][+ep] & pawns;
      while (ep_capture) {
        Square ep_attack = static_cast<Square>(std::countr_zero(ep_capture));
        list.add(Move::make<MoveType::En_Passant>(ep_attack, ep));
        ep_capture &= ep_capture - 1;
      }
    }

bool is_square_attacked(const Board &b, Square target, Color by) {
  const auto occupied = b.occupied();
  const auto other = static_cast<Color>(!+by);

  // L1 Zugriff - zusammen ist günstig, fasse es in einen Branch statt drei
  if ((kKnightAttack[+target] & b.get_bitmap(by, PieceType::Knight)) |
      (kKingAttack[+target] & b.get_bitmap(by, PieceType::King)) |
      (kPawnAttack[+other][+target] & b.get_bitmap(by, PieceType::Pawn))) {
    return true;
  }

  // Erst jetzt die teuren Zugriffe
  const auto queens = b.get_bitmap(by, PieceType::Queen);
  if (rook_attacks(target, occupied) &
      (b.get_bitmap(by, PieceType::Rook) | queens)) {
    return true;
  }
  if (bishop_attacks(target, occupied) &
      (b.get_bitmap(by, PieceType::Bishop) | queens)) {
    return true;
  }
  return false;
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

static void generate_pawn_moves(const Board &b, const Color turn_player,
                                MoveList &list) noexcept {
  uint64_t pawns = b.get_bitmap(turn_player, PieceType::Pawn);
  uint64_t occupied = b.occupied();
  Square ep = b.get_gamestate().ep_square;
  if(ep != Square::None){
    add_pawn_ep_moves(pawns, ep, static_cast<Color>(!+turn_player), list);
  }
  
  switch (turn_player) {
  case Color::White: {
    uint64_t enemies = b.get_color_board(Color::Black);
    uint64_t single_push = shift<white_single_push>(pawns) & ~occupied;
    uint64_t promo = single_push & Rank8;
    add_pawn_promo_moves(promo, white_single_push, list);
    uint64_t quiet = single_push & ~Rank8;
    add_pawn_moves(quiet, white_single_push, list);
    uint64_t double_push = shift<white_single_push>(single_push & Rank3) & ~occupied;
    add_pawn_moves(double_push, white_double_push, list);

    uint64_t left_capture = shift<white_left_capture>(pawns & ~FileA) & enemies;
    add_pawn_moves(left_capture & ~Rank8, white_left_capture, list);
    add_pawn_promo_moves(left_capture & Rank8, white_left_capture, list);

    uint64_t right_capture = shift<white_right_capture>(pawns & ~FileH) & enemies;
    add_pawn_moves(right_capture & ~Rank8, white_right_capture, list);
    add_pawn_promo_moves(right_capture & Rank8, white_right_capture, list);
    break;
  }

  case Color::Black: {
    uint64_t enemies = b.get_color_board(Color::White);
    uint64_t single_push = shift<black_single_push>(pawns) & ~occupied;
    uint64_t promo = single_push & Rank1;
    add_pawn_promo_moves(promo, black_single_push, list);
    uint64_t quiet = single_push & ~Rank1;
    add_pawn_moves(quiet, black_single_push, list);
    uint64_t double_push = shift<black_single_push>(single_push & Rank6) & ~occupied;
    add_pawn_moves(double_push, black_double_push, list);

    uint64_t left_capture = shift<black_left_capture>(pawns & ~FileA) & enemies;
    add_pawn_moves(left_capture & ~Rank1, black_left_capture, list);
    add_pawn_promo_moves(left_capture & Rank1, black_left_capture, list);

    uint64_t right_capture = shift<black_right_capture>(pawns & ~FileH) & enemies;
    add_pawn_moves(right_capture & ~Rank1, black_right_capture, list);
    add_pawn_promo_moves(right_capture & Rank1, black_right_capture, list);
    break;
  }
  }

}

static void generate_castling_moves(const Board &b, MoveList &list) {
  const Color us = b.get_gamestate().side_to_move;
  const Color enemy = static_cast<Color>(!+us);
  const uint8_t rights =
      static_cast<uint8_t>(+b.get_gamestate().castling_rights >> (2 * +us));

  if (!(rights & (CASTLING_SHORT | CASTLING_LONG))) {
    return;
  }

  const int offset = 56 * +us;
  uint64_t occupied = b.occupied();
  Square king = static_cast<Square>(+Square::e1 + offset);

  if (is_square_attacked(b, king, enemy)) {
    return;
  }

  // Short
  if (rights & CASTLING_SHORT) {
    Square f_square = static_cast<Square>(+Square::f1 + offset);
    Square g_square = static_cast<Square>(+Square::g1 + offset);

    uint64_t squares_between = square_bb(f_square) | square_bb(g_square);
    if (!(squares_between & occupied) &&
        !is_square_attacked(b, f_square, enemy) &&
        !is_square_attacked(b, g_square, enemy)) {
      list.add(Move::make<MoveType::Castling>(
          king, static_cast<Square>((+Square::h1) + offset)));
    }
  }

  // Long
  if (rights & CASTLING_LONG) {
    Square c_square = static_cast<Square>(+Square::c1 + offset);
    Square d_square = static_cast<Square>(+Square::d1 + offset);
    uint64_t b_square = square_bb(static_cast<Square>(+Square::b1 + offset));
    uint64_t square_between =
        square_bb(d_square) | square_bb(c_square) | b_square;

    if (!(square_between & occupied) &&
        !is_square_attacked(b, c_square, enemy) &&
        !is_square_attacked(b, d_square, enemy)) {
      list.add(Move::make<MoveType::Castling>(
          king, static_cast<Square>(+Square::a1 + offset)));
    }
  }
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
  generate_castling_moves(b, list);
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