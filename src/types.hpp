#pragma once
#include <cstdint>
#include <utility>
#include <cassert>

enum class Color : uint8_t {White = 0, Black = 1};

enum class PieceType : uint8_t {Pawn = 0, Knight = 1, Bishop = 2, Rook = 3, Queen = 4, King = 5, None = 6};

static constexpr uint8_t PieceTypeCount = 6;

enum class Square : uint8_t {
    a1 = 0, b1 = 1, c1 = 2, d1 = 3, e1 = 4, f1 = 5, g1 = 6, h1 = 7,
    a2 = 8, b2 = 9, c2 = 10, d2 = 11, e2 = 12, f2 = 13, g2 = 14, h2 = 15,
    a3 = 16, b3 = 17, c3 = 18, d3 = 19, e3 = 20, f3 = 21, g3 = 22, h3 = 23,
    a4 = 24, b4 = 25, c4 = 26, d4 = 27, e4 = 28, f4 = 29, g4 = 30, h4 = 31,
    a5 = 32, b5 = 33, c5 = 34, d5 = 35, e5 = 36, f5 = 37, g5 = 38, h5 = 39,
    a6 = 40, b6 = 41, c6 = 42, d6 = 43, e6 = 44, f6 = 45, g6 = 46, h6 = 47,
    a7 = 48, b7 = 49, c7 = 50, d7 = 51, e7 = 52, f7 = 53, g7 = 54, h7 = 55,
    a8 = 56, b8 = 57, c8 = 58, d8 = 59, e8 = 60, f8 = 61, g8 = 62, h8 = 63,
    
    None = 64
};

enum class CastlingRight : uint8_t {
  white_short = 0b0000'0001,
  white_long = 0b0000'0010,
  black_short = 0b0000'0100,
  black_long = 0b0000'1000, // ich wollte die Schreibweise einfach mal nehmen =)
  any = 0b0000'1111
};

constexpr CastlingRight operator|(CastlingRight left, CastlingRight right) {
  return static_cast<CastlingRight>(std::to_underlying(left) |
                                    std::to_underlying(right));
}

constexpr CastlingRight operator&(CastlingRight left, CastlingRight right) {
  return static_cast<CastlingRight>(std::to_underlying(left) &
                                    std::to_underlying(right));
}

constexpr CastlingRight operator~(CastlingRight castling_rights) {
  return static_cast<CastlingRight>(~std::to_underlying(castling_rights));
}

struct GameState{ // "Der GameState (und damit auch eine FEN) speichert ausschließlich historische oder "unsichtbare" Status-Informationen, die man nicht allein durch das Betrachten der aktuellen Figuren auf dem Brett herleiten kann."
  Color side_to_move;
  CastlingRight castling_rights;
  Square ep_square;
  uint8_t half_move_clock;
  uint8_t total_move_number;
}; 

constexpr uint64_t square_bb(Square square){ // macht draus eine Inline
  assert(square != Square::None);
  return 1ULL << std::to_underlying( square );
};
