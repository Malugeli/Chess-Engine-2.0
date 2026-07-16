#include "debug.hpp"

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