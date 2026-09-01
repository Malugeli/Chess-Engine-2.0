#include <array>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "board.hpp"

static void check_fen_error(std::string_view fen, FenErrorCode expected_code,
                            size_t expected_position) {
  Board board;
  const auto result = board.set_fen(fen);

  REQUIRE_FALSE(result.has_value());
  CHECK(result.error().code == expected_code);
  CHECK(result.error().position == expected_position);
}

TEST_CASE("FEN: exactly one king of each color is required", "[fen]") {
  SECTION("no white king") {
    constexpr std::string_view fen = "4k3/8/8/8/8/8/8/8 w - - 0 1";
    check_fen_error(fen, FenErrorCode::NoKing, 17);
  }

  SECTION("no black king") {
    constexpr std::string_view fen = "4K3/8/8/8/8/8/8/8 w - - 0 1";
    check_fen_error(fen, FenErrorCode::NoKing, 17);
  }

  SECTION("exactly one king of each color") {
    constexpr std::string_view fen = "4k3/8/8/8/8/8/8/4K3 w - - 0 1";
    Board board;

    REQUIRE(board.set_fen(fen));
    CHECK(board.to_fen() == fen);
  }

  SECTION("two white kings") {
    constexpr std::string_view fen = "4k3/8/8/8/8/8/8/3K1K2 w - - 0 1";
    check_fen_error(fen, FenErrorCode::MultipleKings, 19);
  }

  SECTION("two black kings") {
    constexpr std::string_view fen = "3k1k2/8/8/8/8/8/8/4K3 w - - 0 1";
    check_fen_error(fen, FenErrorCode::MultipleKings, 3);
  }
}

TEST_CASE("FEN: pawns on first or eighth rank are invalid", "[fen]") {
  SECTION("white pawn on rank 8") {
    check_fen_error("P3k3/8/8/8/8/8/8/4K3 w - - 0 1",
                    FenErrorCode::InvalidPiece, 0);
  }

  SECTION("black pawn on rank 8") {
    check_fen_error("p3k3/8/8/8/8/8/8/4K3 w - - 0 1",
                    FenErrorCode::InvalidPiece, 0);
  }

  SECTION("white pawn on rank 1") {
    check_fen_error("4k3/8/8/8/8/8/8/P3K3 w - - 0 1",
                    FenErrorCode::InvalidPiece, 16);
  }

  SECTION("black pawn on rank 1") {
    check_fen_error("4k3/8/8/8/8/8/8/p3K3 w - - 0 1",
                    FenErrorCode::InvalidPiece, 16);
  }
}

TEST_CASE("FEN: round-trip preserves all fields", "[fen]") {
  SECTION("Kiwipete") {
    constexpr std::string_view fen =
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;

    REQUIRE(board.set_fen(fen));
    CHECK(board.to_fen() == fen);
  }

  SECTION("En passant") {
    constexpr std::string_view fen =
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2";
    Board board;

    REQUIRE(board.set_fen(fen));
    CHECK(board.to_fen() == fen);
  }

  SECTION("promoted queen") {
    constexpr std::string_view fen = "4k3/8/8/8/8/8/8/4K2Q w - - 0 1";
    Board board;

    REQUIRE(board.set_fen(fen));
    CHECK(board.to_fen() == fen);
  }
}

TEST_CASE("FEN: round-trip for every castling-rights combination", "[fen]") {
  constexpr std::array<std::string_view, 16> castling_rights{
      "-", "K", "Q", "k", "q", "KQ", "Kk", "Kq",
      "Qk", "Qq", "kq", "KQk", "KQq", "Kkq", "Qkq", "KQkq"};

  for (const std::string_view rights : castling_rights) {
    DYNAMIC_SECTION("castling rights: " << rights) {
      const std::string fen = "r3k2r/8/8/8/8/8/8/R3K2R w " +
                              std::string(rights) + " - 0 1";
      Board board;

      REQUIRE(board.set_fen(fen));
      CHECK(board.to_fen() == fen);
    }
  }
}
