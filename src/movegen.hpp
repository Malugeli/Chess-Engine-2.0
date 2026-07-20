#pragma once
#include "types.hpp"
#include <array>

//Leaper Pieces:

//Knight-Attack-Array
inline constexpr std::array<uint64_t, 64> kKnightAttack = [](){
  std::array<uint64_t, 64> table{};

  // Die 8 gültigen Springer-Sprünge als feste (rank, file)-Paare.
  // Immer: einer der Werte ±1, der andere ±2 — niemals beide gleich.
  constexpr std::array<int8_t, 8> rank_offsets{+2, +2, -2, -2, +1, +1, -1, -1};
  constexpr std::array<int8_t, 8> file_offsets{+1, -1, +1, -1, +2, -2, +2, -2};

  for (uint8_t square = 0; square < 64; ++square) {
    //uint8_t: bei negative Overflow wird die Zahl > 7, wichtig für unseren Check später
    int8_t rank = static_cast<int8_t>(square / 8);
    int8_t file = square % 8;

    for (uint8_t i = 0; i < 8; ++i) {
      int8_t new_rank = rank + rank_offsets[i];
      int8_t new_file = file + file_offsets[i];

      // Bound-Check: nur wenn Ziel noch auf dem Brett liegt.
      if(new_rank <= 7 && new_rank >= 0 && new_file <= 7 && new_file >= 0) {
        int8_t target = static_cast<int8_t>((new_rank * 8) + new_file);
        table[square] |= square_bb(static_cast<Square>(target));
      }
    }
  }
  return table;
}();

static_assert(kKnightAttack[std::to_underlying(Square::e4)]
    == (square_bb(Square::c3) | square_bb(Square::g3)
      | square_bb(Square::c5) | square_bb(Square::g5)
      | square_bb(Square::d2) | square_bb(Square::f2)
      | square_bb(Square::d6) | square_bb(Square::f6)));

static_assert(kKnightAttack[std::to_underlying(Square::a1)]
    == (square_bb(Square::b3) | square_bb(Square::c2)));

static_assert(kKnightAttack[std::to_underlying(Square::h8)]
    == (square_bb(Square::f7) | square_bb(Square::g6)));


//King-Attack-Array
inline constexpr std::array<uint64_t, 64> kKingAttack = [](){
  std::array<uint64_t, 64> table{};

  std::array<int8_t, 8> rank_offsets{+1, +1, +1, 0, 0, -1, -1, -1};
  std::array<int8_t, 8> file_offsets{-1, 0, +1, -1, +1, -1, 0, +1};

  for(int square = 0; square < 64; ++square){
    int rank = square / 8;
    int file = square % 8; //durch integerpromotion und Compiletime unnötig int8 zu nutzen 
    for(uint8_t offset = 0; offset < 8; ++offset){
      int new_rank = rank + rank_offsets[offset];
      int new_file = file + file_offsets[offset];

      if(new_rank <= 7 && new_rank >= 0 && new_file <= 7 && new_file >= 0){
        table[static_cast<std::size_t>( square )] |= square_bb(static_cast<Square>((new_rank * 8) + new_file));
      }
    }
  }
  return table; 
}();

static_assert(kKingAttack[std::to_underlying(Square::a1)]
 == (square_bb(Square::a2) | square_bb(Square::b2) | square_bb(Square::b1)));

static_assert(kKingAttack[std::to_underlying(Square::h8)]
== (square_bb(Square::g7) | square_bb(Square::g8) |square_bb(Square::h7))); 

static_assert(kKingAttack[std::to_underlying(Square::e4)]
== (square_bb(Square::d3) | square_bb(Square::e3) | square_bb(Square::f3) | square_bb(Square::d4) 
| square_bb(Square::f4) | square_bb(Square::d5) | square_bb(Square::e5) | square_bb(Square::f5))); 

//Sliding Pieces:
//Rook Attack: Ergebnis hängt von occupied ab -> Laufzeitfunktion statt Tabelle
uint64_t rook_attacks(Square square, uint64_t occupied);

uint64_t bishop_attacks(Square square, uint64_t occupied);

uint64_t queen_attacks(Square square, uint64_t occupied);


/*
Pawns sind Leaper. Ich vergesse ständig das wir nur eine Karte mit den möglichen Zügen erstellen statt die möglichen Squares die wir gehen dürfen.
*/

inline constexpr std::array<std::array<uint64_t, 64>, 2> kPawnAttack = []() {
  std::array<std::array<uint64_t, 64>, 2> attack{};
  std::array<int, 2> pawn_direction{-1, +1};

  for(int square = 0; square < 64; ++square){ 
    int rank = square / 8;
    rank++; 
    if(rank > 7){
      break;
    }
    for(uint8_t direction = 0; direction < 2; ++direction){
      int file = square % 8;
      file += pawn_direction[direction];
      if(file >= 0 && file < 8){
        int target = rank * 8 + file;
        attack[std::to_underlying( Color::White )][static_cast<uint64_t>( square )] |= square_bb(static_cast<Square>(target));
      }}
    }
  for(int square = 63; square >= 0; --square){ 
    int rank = square / 8;
    rank--; 
    if(rank < 0){
      break;
    }
    for(uint8_t direction = 0; direction < 2; ++direction){
      int file = square % 8;
      file += pawn_direction[direction];
      if(file >= 0 && file < 8){
        int target = rank * 8 + file;
        attack[std::to_underlying( Color::Black )][static_cast<uint64_t>( square )] |= square_bb(static_cast<Square>(target));
      }}
    }
    return attack; 
  }
();