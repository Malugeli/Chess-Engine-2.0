#include "debug.hpp"
#include "bitboard.hpp"

std::string pretty_bitboard(uint64_t board){
  std::string s = "+---+---+---+---+---+---+---+---+\n";
  
  for(int row = 7; row >= 0; --row){
    for(int column = 0; column < 8; ++column){
      s += board & square_bb(static_cast<Square>( (row << 3) + column )) ? "| X " : "|   ";
    }
    s += "| " + std::to_string(1 + row) + "\n+---+---+---+---+---+---+---+---+\n";
  }
  s += "  a   b   c   d   e   f   g   h\n";
  return s;
}
std::string square_to_uci(Square s) {
  const int sq = +s;
  return std::string{static_cast<char>('a' + (sq & 7)),
                     static_cast<char>('1' + (sq >> 3))};
}

std::string move_to_uci(Move m) {
  std::string s = square_to_uci(m.from_sq()) + square_to_uci(m.to_sq());
  if (m.type_of() == MoveType::Promotion) {
    switch (m.promotion_piece()) {
    case PieceType::Knight: s += 'n'; break;
    case PieceType::Bishop: s += 'b'; break;
    case PieceType::Rook:   s += 'r'; break;
    default:                s += 'q'; break;
    }
  }
  return s;
}
