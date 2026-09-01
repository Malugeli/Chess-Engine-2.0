#include <catch2/catch_test_macros.hpp>

#include "board.hpp"
#include "perft.hpp"

TEST_CASE("Perft: starting position", "[perft][fast]") {
  Board board;
  const std::string fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 0) == 1);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 1) == 20);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 2) == 400);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 3) == 8'902);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft: Position 2 (Kiwipete)", "[perft][fast]") {
  Board board;
  const std::string fen =
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w "
      "KQkq - 0 1";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 1) == 48);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 2) == 2'039);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 3) == 97'862);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft: Position 3", "[perft][fast]") {
  Board board;
  const std::string fen =
      "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 1) == 14);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 2) == 191);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 3) == 2'812);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft: Position 4", "[perft][fast]") {
  Board board;
  const std::string fen =
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w "
      "kq - 0 1";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 1) == 6);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 2) == 264);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 3) == 9'467);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft: Position 5", "[perft][fast]") {
  Board board;
  const std::string fen =
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 1) == 44);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 2) == 1'486);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 3) == 62'379);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft: Position 6", "[perft][fast]") {
  Board board;
  const std::string fen =
      "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 "
      "w - - 0 10";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 1) == 46);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 2) == 2'079);
  CHECK(board.to_fen() == fen);
  CHECK(perft(board, 3) == 89'890);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft slow: starting position", "[perft][slow]") {
  Board board;
  const std::string fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 5) == 4'865'609);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft slow: Position 2 (Kiwipete)", "[perft][slow]") {
  Board board;
  const std::string fen =
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w "
      "KQkq - 0 1";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 4) == 4'085'603);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft slow: Position 3", "[perft][slow]") {
  Board board;
  const std::string fen =
      "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 6) == 11'030'083);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft slow: Position 4", "[perft][slow]") {
  Board board;
  const std::string fen =
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w "
      "kq - 0 1";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 5) == 15'833'292);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft slow: Position 5", "[perft][slow]") {
  Board board;
  const std::string fen =
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 4) == 2'103'487);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Perft slow: Position 6", "[perft][slow]") {
  Board board;
  const std::string fen =
      "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 "
      "w - - 0 10";

  REQUIRE(board.set_fen(fen));
  REQUIRE(board.to_fen() == fen);

  CHECK(perft(board, 4) == 3'894'594);
  CHECK(board.to_fen() == fen);
}
