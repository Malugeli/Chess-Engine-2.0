#pragma once
#include <array>
#include "types.hpp"

class Board {
private:
  //Membervariablen:
  std::array<uint64_t, 14> bitmaps{};
  std::array<Piece, 64>mailbox{};
  std::array<uint64_t, 2> color_board{};
  GameState game_state{};

  //Hilfsfunktionen:
  uint8_t get_index(Color color, PieceType piece) const noexcept;
  uint64_t get_all_bitmaps(Color color) const noexcept;
  void sync_mailbox_with_bitmaps();
  void remove_piece(Color color, PieceType piece, Square square) noexcept;
public:
  Board();
  uint64_t get_bitmap(Color color, PieceType piece) const noexcept;
  uint64_t get_color_board(Color color) const noexcept;
  const GameState& get_gamestate() const noexcept;
  Piece get_piece_at(Square square) const noexcept;
  uint64_t occupied() const noexcept;
};