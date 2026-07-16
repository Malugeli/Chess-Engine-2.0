#include "board.hpp"
#include <bit>

uint8_t Board::get_index(Color color, PieceType piece) const noexcept {
  return static_cast<uint8_t>(std::to_underlying(color) * PieceTypeCount + //Integerpromtion causes warning
                              std::to_underlying(piece));
}

uint64_t Board::get_all_bitmaps(Color color) const noexcept {
  uint64_t result = 0ULL;
  for (uint8_t i = 0; i < PieceTypeCount; ++i) {
    result =
        result | bitmaps[get_index(color, static_cast<PieceType>(i))];
  }
  return result;
}

void Board::sync_mailbox_with_bitmaps() {
  mailbox.fill(PieceType::None);

  for (Color c : {Color::White, Color::Black}) {
    for (PieceType p : {PieceType::Pawn, PieceType::Knight, PieceType::Bishop,
                        PieceType::Rook, PieceType::Queen, PieceType::King}) {
      uint64_t bitscan = bitmaps[get_index(c, p)];
      while (bitscan) {
        uint8_t bit = static_cast<uint8_t>(std::countr_zero(bitscan));
        mailbox[bit] = p;
        bitscan &= bitscan-1;
      }
    }
  }
}

uint64_t Board::get_bitmap(Color color, PieceType piece) const noexcept{
  return bitmaps[get_index(color, piece)];
}

const GameState& Board::get_gamestate() const noexcept{
  return game_state;
}

uint64_t Board::get_color_board(Color color) const noexcept{
  return color_board[std::to_underlying(color)];
}

PieceType Board::get_piece_at(Square square) const noexcept{
  return mailbox[std::to_underlying(square)];
}

uint64_t Board::occupied() const noexcept{
  return get_color_board(Color::White) | get_color_board(Color::Black);
}


void Board::remove_piece(Color color, PieceType piece, Square square) noexcept{
  uint64_t mask = 1ULL << std::to_underlying(square);

  bitmaps[get_index(color, piece)] &= ~mask;
  color_board[std::to_underlying(color)] &= ~mask;
  mailbox[std::to_underlying(square)] = PieceType::None;
}

Board::Board() {
  bitmaps[get_index(Color::White, PieceType::Pawn)] = 0xFF00ULL;
  bitmaps[get_index(Color::White, PieceType::Knight)] = 0x42ULL;
  bitmaps[get_index(Color::White, PieceType::Bishop)] = 0x24ULL;
  bitmaps[get_index(Color::White, PieceType::Rook)] = 0x81ULL;
  bitmaps[get_index(Color::White, PieceType::Queen)] = 0x08ULL;
  bitmaps[get_index(Color::White, PieceType::King)] = 0x10ULL;

  bitmaps[get_index(Color::Black, PieceType::Pawn)] = 0xFF00ULL << 40;
  bitmaps[get_index(Color::Black, PieceType::Knight)] = 0x42ULL << 56;
  bitmaps[get_index(Color::Black, PieceType::Bishop)] = 0x24ULL << 56;
  bitmaps[get_index(Color::Black, PieceType::Rook)] = 0x81ULL << 56;
  bitmaps[get_index(Color::Black, PieceType::Queen)] = 0x08ULL << 56;
  bitmaps[get_index(Color::Black, PieceType::King)] = 0x10ULL << 56;

  color_board[std::to_underlying(Color::White)] = get_all_bitmaps(Color::White);
  color_board[std::to_underlying(Color::Black)] = get_all_bitmaps(Color::Black);
  game_state = {Color::White, CastlingRight::any, Square::None, 0, 1};
  sync_mailbox_with_bitmaps();
}