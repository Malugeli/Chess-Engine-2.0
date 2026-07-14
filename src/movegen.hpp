#pragma once
#include "board.hpp"

//Knight-Attack-Array
inline constexpr std::array<uint64_t, 64> kKnightAttack = [](){
  std::array<uint64_t, 64> table{};

  // Die 8 gültigen Springer-Sprünge als feste (rank, file)-Paare.
  // Immer: einer der Werte ±1, der andere ±2 — niemals beide gleich.
  constexpr std::array<int8_t, 8> rank_offsets{+2, +2, -2, -2, +1, +1, -1, -1};
  constexpr std::array<int8_t, 8> file_offsets{+1, -1, +1, -1, +2, -2, +2, -2};

  for (uint8_t square = 0; square < 64; ++square) {
    //uint8_t: bei negative Overflow wird die Zahl > 7, wichtig für unseren Check später
    uint8_t rank = square / 8;
    uint8_t file = square % 8;

    for (uint8_t i = 0; i < 8; ++i) {
      uint8_t new_rank = rank + rank_offsets[i];
      uint8_t new_file = file + file_offsets[i];

      // Bound-Check: nur wenn Ziel noch auf dem Brett liegt.
      if (new_rank <= 7 && new_file <= 7) {
        uint8_t target = new_rank * 8 + new_file;
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

  for(uint8_t square = 0; square < 64; ++square){
    uint8_t rank = square / 8;
    uint8_t file = square % 8;

    for(uint8_t offset = 0; offset < 8; ++offset){
      uint8_t new_rank = rank + rank_offsets[offset];
      uint8_t new_file = file + file_offsets[offset];

      if(new_rank <= 7 && new_file <= 7){
        table[square] |= square_bb(static_cast<Square>((new_rank * 8) + new_file));
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
