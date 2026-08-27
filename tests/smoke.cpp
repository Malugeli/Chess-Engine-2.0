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
  std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  CHECK( board.set_fen(fen) );
  CHECK(board.to_fen() == fen);
}
