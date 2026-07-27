#include "board.hpp"
#include <bit>

uint8_t Board::get_index(Color color, PieceType piece) const noexcept {
  return static_cast<uint8_t>(+color * PieceTypeCount + +piece);
}

uint64_t Board::get_all_bitmaps(Color color) const noexcept {
  uint64_t all = 0ULL;
  for (uint8_t i = 0; i < PieceTypeCount; ++i) {
    all = all | bitmaps[get_index(color, static_cast<PieceType>(i))];
  }
  return all;
}

void Board::sync_mailbox_with_bitmaps() {
  for(uint8_t i = 0; i < 2; ++i){
    mailbox[i].fill(PieceType::None);
  }

  for (Color c : {Color::White, Color::Black}) {
    for (PieceType p : {PieceType::Pawn, PieceType::Knight, PieceType::Bishop,
                        PieceType::Rook, PieceType::Queen, PieceType::King}) {
      uint64_t bitscan = bitmaps[get_index(c, p)];
      while (bitscan) {
        uint8_t bit = static_cast<uint8_t>(std::countr_zero(bitscan));
        mailbox[+c][bit] = p;
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
  return color_board[+color];
}

PieceType Board::get_piece_at(Color c, Square square) const noexcept{
  return mailbox[+c][+square];
}

uint64_t Board::occupied() const noexcept{
  return get_color_board(Color::White) | get_color_board(Color::Black);
}


void Board::remove_piece(Color color, PieceType piece, Square square) noexcept{
  uint64_t mask = 1ULL << +square;

  bitmaps[get_index(color, piece)] &= ~mask;
  color_board[+color] &= ~mask;
  mailbox[+color][+square] = PieceType::None;
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

  color_board[+Color::White] = get_all_bitmaps(Color::White);
  color_board[+Color::Black] = get_all_bitmaps(Color::Black);
  game_state = {Color::White, CastlingRight::Any, Square::None, 0, 1};
  sync_mailbox_with_bitmaps();
}