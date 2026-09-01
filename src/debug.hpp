#pragma once
#include <string>
#include <cstdint>
#include "types.hpp"

std::string pretty_bitboard(uint64_t board);

// Feld/Zug in UCI-Notation ("e2", "e7e8q") - zum Vergleichen mit Stockfish.
std::string square_to_uci(Square s);
std::string move_to_uci(Move m);