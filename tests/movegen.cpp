#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "board.hpp"
#include "movegen.hpp"

static bool contains_move(const MoveList& moves, Move expected) {
  for (const Move move : moves) {
    if (move == expected) {
      return true;
    }
  }
  return false;
}

TEST_CASE("Legal move generation returns legal moves and preserves the board",
          "[movegen]") {
  constexpr std::string_view fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Board board;
  REQUIRE(board.set_fen(fen));

  MoveList legal_moves;
  generate_legal_moves(board, legal_moves);
  CHECK(legal_moves.size() == 20);

  // Ein erneuter Aufruf muss die uebergebene Ausgabeliste ersetzen.
  generate_legal_moves(board, legal_moves);
  CHECK(legal_moves.size() == 20);
  CHECK(board.to_fen() == fen);
}

TEST_CASE("Legal move generation filters moves exposing the own king",
          "[movegen]") {
  SECTION("pinned piece") {
    constexpr std::string_view fen =
        "k3r3/8/8/8/8/8/4R3/4K3 w - - 0 1";
    const Move exposes_king{Square::e2, Square::d2};
    Board board;
    REQUIRE(board.set_fen(fen));

    MoveList pseudo_legal_moves;
    generate_pseudo_legal_moves(board, pseudo_legal_moves);
    REQUIRE(contains_move(pseudo_legal_moves, exposes_king));

    MoveList legal_moves;
    generate_legal_moves(board, legal_moves);
    CHECK_FALSE(contains_move(legal_moves, exposes_king));
    CHECK(board.to_fen() == fen);
  }

  SECTION("en passant opens a rook line") {
    constexpr std::string_view fen =
        "4r1k1/8/8/3pP3/8/8/8/4K3 w - d6 0 1";
    const Move exposes_king = Move::make<MoveType::En_Passant>(
        Square::e5, Square::d6);
    Board board;
    REQUIRE(board.set_fen(fen));

    MoveList pseudo_legal_moves;
    generate_pseudo_legal_moves(board, pseudo_legal_moves);
    REQUIRE(contains_move(pseudo_legal_moves, exposes_king));

    MoveList legal_moves;
    generate_legal_moves(board, legal_moves);
    CHECK_FALSE(contains_move(legal_moves, exposes_king));
    CHECK(board.to_fen() == fen);
  }
}
