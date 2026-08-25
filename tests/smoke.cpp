// Smoke-Test: prüft NICHT die Engine, sondern nur die Verkabelung.
// Wenn dieser Test durch ctest läuft, stimmen Build, Link und Discovery.
// Danach kann er weg — echte Tests kommen in test_fen.cpp etc.
#include <catch2/catch_test_macros.hpp>

#include "board.hpp"

TEST_CASE("Verkabelung steht", "[smoke]") {
  REQUIRE(1 + 1 == 2);
}

TEST_CASE("chess_core ist gelinkt", "[smoke]") {
  Board board;
  REQUIRE(board.get_piece_at(Square::e1) == Piece::WhiteKing);
}
