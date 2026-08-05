#include "board.hpp"
#include <bit>

void Board::init_bitmaps(){
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
}

void Board::init_castling_bitmask(){
  castling_mask.fill(CastlingRight::Any);
  castling_mask[static_cast<int>(Square::e1)] &=
      ~(CastlingRight::White_Short | CastlingRight::White_Long);
  castling_mask[static_cast<int>(Square::h1)] &= ~CastlingRight::White_Short;
  castling_mask[static_cast<int>(Square::a1)] &= ~CastlingRight::White_Long;

  castling_mask[static_cast<int>(Square::e8)] &=
      ~(CastlingRight::Black_Short | CastlingRight::Black_Long);
  castling_mask[static_cast<int>(Square::h8)] &= ~CastlingRight::Black_Short;
  castling_mask[static_cast<int>(Square::a8)] &= ~CastlingRight::Black_Long;
}

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

//bevor ich den Zug ausführe muss den Gamestate speichern um es später wieder rückgängig zu machen
//statt einem std::stack nutze ich ein festes Array von (vorerst)
void Board::do_move(Move m) noexcept {
  auto from_square = m.from_sq();
  auto to_square = m.to_sq();
  auto from_piece = mailbox[+from_square];
  auto to_piece = mailbox[+to_square];
  
  ++game_state.half_move_clock;  
  game_state.ep_square = Square::None;
  
  switch(m.type_of()){
    case MoveType::Normal:
    if(piece_of(from_piece) == PieceType::Pawn || to_piece != Piece::None){ // Wenn Bauer sich bewegt oder Figur geschlagen, setze halfmove auf 0
      game_state.half_move_clock = 0;
    }
    
    if(to_piece != Piece::None) { // Schlag
      game_state.captured_piece = to_piece;
      remove_piece(color_of(to_piece), piece_of(to_piece), to_square);
    }
    
    if(piece_of(from_piece) == PieceType::Pawn && std::abs( +to_square - +from_square ) == 16){ // En Passant
      game_state.ep_square = static_cast<Square>( ( +from_square + +to_square ) / 2 ); // genial
    }
    
    game_state.castling_rights &= castling_mask[+from_square];
    game_state.castling_rights &= castling_mask[+to_square];
    move_piece(color_of(from_piece), piece_of(from_piece), from_square,
    to_square);
    
    break;
    
    case MoveType::Castling:
    switch (to_square) {
      case Square::h1:
      move_piece(Color::White, PieceType::Rook, Square::h1, Square::f1);
      move_piece(Color::White, PieceType::King, Square::e1, Square::g1);
      break;
      
      case Square::a1:
      move_piece(Color::White, PieceType::Rook, Square::a1, Square::d1);
      move_piece(Color::White, PieceType::King, Square::e1, Square::c1);
      move_piece(Color::Black, PieceType::King, Square::e8, Square::g8);
      break;
      
      case Square::h8:
      move_piece(Color::Black, PieceType::Rook, Square::h8, Square::f8);
      move_piece(Color::Black, PieceType::King, Square::e8, Square::g8);
      break;
      
      case Square::a8:
      move_piece(Color::Black, PieceType::Rook, Square::a8, Square::d8);
      move_piece(Color::Black, PieceType::King, Square::e8, Square::c8);
      break;
      default:
      break;
    }
    break;
    
    case MoveType::En_Passant:
    break;
    
    case MoveType::Promotion:
    break;
  }
  // Was mache ich mit Captured Piece? Wenn kein Zug ein Piece gecaptured hat kann ich doch nicht im Gamestate das letzte gecapturete Piece behalten oder nicht?
  if (game_state.side_to_move == Color::Black)
  ++game_state.total_move_count;

game_state.castling_rights &= castling_mask[+from_square] & castling_mask[+to_square];
game_state.side_to_move = static_cast<Color>(
  !+game_state.side_to_move); // so ein genialer Trick um zu switchen omg
  history[ply] = game_state;
  ++ply;
};

//eine Art Stack muss beigelegt werden oder? do_move müsste eine Art Stack füllen mit moves und das reichen wir undo weiter.
//ich sehe nicht wie undo jemals etwas "zurück" bringen soll wenn er nicht weiß.. doch einfach das VOR do_move einfach reinlegen?
//heißt das ich brauche literally einen std::stack<GameState>? Wo ich pro do_move reinlege und pro undo_move es rausnehme und wiederherstelle?

// void Board::undo_move(Move m) noexcept{

// };

Board::Board() {
  init_bitmaps();
  init_castling_bitmask();

  color_board[+Color::White] = get_color_bitmap(Color::White);
  color_board[+Color::Black] = get_color_bitmap(Color::Black);
  game_state = {Color::White, CastlingRight::Any, Square::None, 0, 1, Piece::None}; // halfmove_clock und total_move sind magic numbers.. kann ich das irgendwie richten?

  sync_mailbox_with_bitmaps();
}