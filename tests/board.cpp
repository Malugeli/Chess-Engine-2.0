#include <array>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "board.hpp"

static void check_do_and_undo(std::string_view before, Move move,
                              std::string_view after) {
  Board board;
  REQUIRE(board.set_fen(before));

  board.do_move(move);
  CHECK(board.to_fen() == after);

  board.undo_move(move);
  CHECK(board.to_fen() == before);
}

TEST_CASE("Board: normal moves are performed and undone", "[board]") {
  SECTION("quiet move") {
    check_do_and_undo("4k3/8/8/8/8/8/8/4K3 w - - 7 12",
                      Move{Square::e1, Square::e2},
                      "4k3/8/8/8/8/8/4K3/8 b - - 8 12");
  }

  SECTION("capture") {
    check_do_and_undo("4k3/8/8/8/8/3p4/4K3/8 w - - 7 12",
                      Move{Square::e2, Square::d3},
                      "4k3/8/8/8/8/3K4/8/8 b - - 0 12");
  }

  SECTION("pawn double move sets the en passant square") {
    check_do_and_undo("4k3/8/8/8/8/8/4P3/4K3 w - - 7 12",
                      Move{Square::e2, Square::e4},
                      "4k3/8/8/8/4P3/8/8/4K3 b - e3 0 12");
  }
}

TEST_CASE("Board: all castlings are performed and undone", "[board]") {
  struct CastlingCase {
    std::string_view name;
    std::string_view before;
    Move move;
    std::string_view after;
  };

  const std::array<CastlingCase, 4> cases{{
      {"white kingside castling", "4k3/8/8/8/8/8/8/4K2R w K - 7 12",
       Move::make<MoveType::Castling>(Square::e1, Square::h1),
       "4k3/8/8/8/8/8/8/5RK1 b - - 8 12"},
      {"white queenside castling", "4k3/8/8/8/8/8/8/R3K3 w Q - 7 12",
       Move::make<MoveType::Castling>(Square::e1, Square::a1),
       "4k3/8/8/8/8/8/8/2KR4 b - - 8 12"},
      {"black kingside castling", "4k2r/8/8/8/8/8/8/4K3 b k - 7 12",
       Move::make<MoveType::Castling>(Square::e8, Square::h8),
       "5rk1/8/8/8/8/8/8/4K3 w - - 8 13"},
      {"black queenside castling", "r3k3/8/8/8/8/8/8/4K3 b q - 7 12",
       Move::make<MoveType::Castling>(Square::e8, Square::a8),
       "2kr4/8/8/8/8/8/8/4K3 w - - 8 13"},
  }};

  for (const CastlingCase& test_case : cases) {
    CAPTURE(test_case.name);
    check_do_and_undo(test_case.before, test_case.move, test_case.after);
  }
}

TEST_CASE("Board: en passant is performed and undone", "[board]") {
  SECTION("white pawn captures") {
    check_do_and_undo("4k3/8/8/3pP3/8/8/8/4K3 w - d6 7 12",
                      Move::make<MoveType::En_Passant>(Square::e5, Square::d6),
                      "4k3/8/3P4/8/8/8/8/4K3 b - - 0 12");
  }

  SECTION("black pawn captures") {
    check_do_and_undo("4k3/8/8/8/3pP3/8/8/4K3 b - e3 7 12",
                      Move::make<MoveType::En_Passant>(Square::d4, Square::e3),
                      "4k3/8/8/8/8/4p3/8/4K3 w - - 0 13");
  }
}

TEST_CASE("Board: promotions are performed and undone", "[board]") {
  SECTION("white pawn captures and promotes to queen") {
    check_do_and_undo("5r2/4P2k/8/8/8/8/8/4K3 w - - 7 12",
                      Move::make<MoveType::Promotion>(Square::e7, Square::f8,
                                                       PieceType::Queen),
                      "5Q2/7k/8/8/8/8/8/4K3 b - - 0 12");
  }

  SECTION("black pawn captures and promotes to queen") {
    check_do_and_undo("4k3/8/8/8/8/8/4p3/5R1K b - - 7 12",
                      Move::make<MoveType::Promotion>(Square::e2, Square::f1,
                                                       PieceType::Queen),
                      "4k3/8/8/8/8/8/8/5q1K w - - 0 13");
  }
}

TEST_CASE("Board: multiple moves are undone in LIFO order", "[board]") {
  constexpr std::string_view start_position =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Board board;
  REQUIRE(board.set_fen(start_position));

  const std::array<Move, 3> moves{
      Move{Square::e2, Square::e4},
      Move{Square::e7, Square::e5},
      Move{Square::g1, Square::f3},
  };

  for (const Move move : moves) {
    board.do_move(move);
  }
  for (auto it = moves.rbegin(); it != moves.rend(); ++it) {
    board.undo_move(*it);
  }

  CHECK(board.to_fen() == start_position);
}
