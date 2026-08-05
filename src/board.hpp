#pragma once
#include <array>
#include "types.hpp"

class Board {
private:
  //Membervariablen:
  std::array<uint64_t, 14> bitmaps{};
  std::array<Piece, 64>mailbox{};
  std::array<CastlingRight, 64> castling_mask;
  std::array<uint64_t, 2> color_board{};
  std::array<GameState, 1024> history;
  GameState game_state{};
  uint64_t ply{};
  
    
  //Hilfsfunktionen:
  uint8_t get_index(Color color, PieceType piece) const noexcept;
  uint64_t get_color_bitmap(Color color) const noexcept;
  void sync_mailbox_with_bitmaps() noexcept;
  void add_piece(Color c, PieceType p, Square s) noexcept;
  void move_piece(Color c, PieceType p, Square from, Square to) noexcept;
  void remove_piece(Color color, PieceType piece, Square square) noexcept;
  void init_bitmaps();
  void init_castling_bitmask(); 

public:
  Board();
  void do_move(Move m) noexcept;
  void undo_move(Move m) noexcept;
  uint64_t get_bitmap(Color color, PieceType piece) const noexcept;
  uint64_t get_color_board(Color color) const noexcept;
  const GameState& get_gamestate() const noexcept;
  Piece get_piece_at(Square square) const noexcept;
  uint64_t occupied() const noexcept;
};