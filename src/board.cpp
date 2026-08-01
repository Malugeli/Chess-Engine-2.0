#include "board.hpp"
#include <bit>

uint8_t Board::get_index(Color c, PieceType p) const noexcept {
  assert(p != PieceType::None);
  return static_cast<uint8_t>(( +c << 3 ) | +p); //disjunktes Bitlayout, in den untersten 4 Bits liegt die Color als Bit und 3 Bits für das Piece
}

uint64_t Board::get_color_bitmap(Color c) const noexcept {
  uint64_t all = 0ULL;
  for (uint8_t i = 0; i < PieceTypeCount; ++i) {
    all = all | bitmaps[get_index(c, static_cast<PieceType>(i))];
  }
  return all;
}

void Board::sync_mailbox_with_bitmaps() noexcept{
  mailbox.fill(Piece::None);

  for (Color c : {Color::White, Color::Black}) {
    for (PieceType p : {PieceType::Pawn, PieceType::Knight, PieceType::Bishop,
                        PieceType::Rook, PieceType::Queen, PieceType::King}) {
      uint64_t bitscan = bitmaps[get_index(c, p)];
      Piece piece = make_piece(c, p);
      while (bitscan) {
        uint8_t bit = static_cast<uint8_t>(std::countr_zero(bitscan));
        mailbox[bit] = piece;
        bitscan &= bitscan - 1;
      }
    }
  }
}

uint64_t Board::get_bitmap(Color c, PieceType p) const noexcept{
  return bitmaps[get_index(c, p)];
}

const GameState& Board::get_gamestate() const noexcept{
  return game_state;
}

uint64_t Board::get_color_board(Color c) const noexcept{
  return color_board[+c];
}

Piece Board::get_piece_at(Square s) const noexcept{
  return mailbox[+s];
}

uint64_t Board::occupied() const noexcept{
  return get_color_board(Color::White) | get_color_board(Color::Black);
}

void Board::add_piece(Color c, PieceType p, Square s) noexcept {
  uint64_t mask = square_bb(s);

  bitmaps[get_index(c, p)] |= mask;
  color_board[+c] |= mask;
  mailbox[+s] = make_piece(c, p);
}

void Board::move_piece(Color c, PieceType p, Square from, Square to) noexcept {
  uint64_t from_bb = square_bb(from);
  uint64_t to_bb = square_bb(to);

  bitmaps[get_index(c, p)] ^= from_bb;
  bitmaps[get_index(c, p)] |= to_bb;

  color_board[+c] ^= from_bb;
  color_board[+c] |= to_bb;

  mailbox[+from] = Piece::None;
  mailbox[+to] = make_piece(c, p);
}

void Board::remove_piece(Color c, PieceType p, Square s) noexcept {
  uint64_t mask = square_bb(s);

  bitmaps[get_index(c, p)] &= ~mask;
  color_board[+c] &= ~mask;
  mailbox[+s] = Piece::None;
}
void Board::make_move(Move m) noexcept {
  auto from_square = m.from_sq();
  auto to_square = m.to_sq();
  auto from_piece = mailbox[+from_square];
  auto to_piece = mailbox[+to_square];

  if (m.type_of() == MoveType::Normal) {
    if (to_piece != Piece::None) {
      remove_piece(color_of(to_piece), piece_of(to_piece), to_square);
    }
    move_piece(color_of(from_piece), piece_of(from_piece), from_square,
               to_square);

  }
  else{
    assert(false && "Movetypes noch nicht implementiert");
  }
  game_state.side_to_move = static_cast<Color>(!+game_state.side_to_move); // so ein genialer Trick um zu switchen omg
};

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

  color_board[+Color::White] = get_color_bitmap(Color::White);
  color_board[+Color::Black] = get_color_bitmap(Color::Black);
  game_state = {Color::White, CastlingRight::Any, Square::None, 0, 1};
  sync_mailbox_with_bitmaps();
}