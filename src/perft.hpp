#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "board.hpp"


uint64_t perft(Board& b, int depth);

// Ein Eintrag der Aufschluesselung: erster Zug + wie viele Blaetter darunter haengen.
struct PerftDivideEntry {
  Move move;
  uint64_t nodes;
};

// Perft, aber nach dem ersten Zug aufgeschluesselt. Sortiert nach UCI-Notation,
// damit man die Ausgabe direkt mit Stockfishs "go perft" vergleichen kann.
std::vector<PerftDivideEntry> perft_divide(Board& b, int depth);

// Gibt die Aufschluesselung im Stockfish-Format aus und liefert die Gesamtsumme.
uint64_t print_perft_divide(Board& b, int depth);
