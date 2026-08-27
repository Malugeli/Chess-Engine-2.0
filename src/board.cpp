#include <bit>
#include <charconv>

#include "board.hpp"
#include "bitboard.hpp"

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
//statt einem std::stack nutze ich ein festes Array (vorerst)
void Board::do_move(Move m) noexcept {
  Square from_square = m.from_sq();
  Square to_square = m.to_sq();
  Piece from_piece = mailbox[+from_square];
  assert(from_piece != Piece::None);
  Piece captured = Piece::None; 
  Square captured_square = Square::None;

  assert(ply < history.size());
  history[ply] = game_state; 
  
  ++game_state.half_move_clock;  
  game_state.ep_square = Square::None;
  if (game_state.side_to_move == Color::Black) {
    ++game_state.total_move_count;
  }
  

  game_state.side_to_move = static_cast<Color>(!+game_state.side_to_move);
  game_state.castling_rights &=
      castling_mask[+from_square] & castling_mask[+to_square];
  
  
  switch(m.type_of()){
    case MoveType::Normal:
    captured = mailbox[+to_square];
    if(captured != Piece::None) { // Schlag
      remove_piece(color_of(captured), piece_of(captured), to_square);
    }
    
    if(piece_of(from_piece) == PieceType::Pawn && ( +from_square ^ +to_square ) == 16){ //Checkt für Doppelmove, die Erklärung liegt in docs unter XOR Operator Usage
      game_state.ep_square = static_cast<Square>( +to_square ^ 8 ); // setzt En Passant Square 2-4 = 3 und 5-7 = 6 
    }

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
      assert(false);
    }
    break;
    
    case MoveType::En_Passant:
      captured_square = static_cast<Square>(+to_square ^ 8);
      captured = mailbox[+captured_square];
      assert(piece_of(captured) == PieceType::Pawn);
      remove_piece(color_of(captured), piece_of(captured), captured_square);
      move_piece(color_of(from_piece), piece_of(from_piece), from_square,
                 to_square);
      break;
    
    case MoveType::Promotion:
      captured = mailbox[+to_square];
      if(captured != Piece::None){
        remove_piece(color_of(captured), piece_of(captured), to_square);
      }
      remove_piece(color_of(from_piece), piece_of(from_piece), from_square);
      add_piece(color_of( from_piece ), m.promotion_piece(), to_square);
      break;
  }

  if(piece_of(from_piece) == PieceType::Pawn || captured != Piece::None){ 
    game_state.half_move_clock = 0;
  }
  game_state.captured_piece = captured; // Das gecaptured Piece ist das NACH dem Move. Führe den undo erst aus und dann recover Gamestate!
  ++ply;
}

void Board::undo_move(Move m) noexcept{
  Square from_square = m.from_sq();
  Square to_square = m.to_sq();
  Piece return_piece = mailbox[+to_square];
  Piece recover_piece{game_state.captured_piece};

  switch(m.type_of()){
    case MoveType::Normal:
    move_piece(color_of(return_piece), piece_of(return_piece), to_square, from_square);
    if(recover_piece != Piece::None){
      add_piece(color_of(recover_piece), piece_of(recover_piece), to_square);
    }
    break;

    case MoveType::Castling:
    switch (to_square) {
      case Square::h1:
      move_piece(Color::White, PieceType::Rook, Square::f1, Square::h1);
      move_piece(Color::White, PieceType::King, Square::g1, Square::e1);
      break;
      
      case Square::a1:
      move_piece(Color::White, PieceType::Rook, Square::d1, Square::a1);
      move_piece(Color::White, PieceType::King, Square::c1, Square::e1);
      break;
      
      case Square::h8:
      move_piece(Color::Black, PieceType::Rook, Square::f8, Square::h8);
      move_piece(Color::Black, PieceType::King, Square::g8, Square::e8);
      break;
      
      case Square::a8:
      move_piece(Color::Black, PieceType::Rook, Square::d8, Square::a8);
      move_piece(Color::Black, PieceType::King, Square::c8, Square::e8);
      break;
      default:
      assert(false);
    }
    break;
    case MoveType::En_Passant:
      assert(piece_of(recover_piece) == PieceType::Pawn);
      move_piece(color_of(return_piece), piece_of(return_piece), to_square,
                 from_square);
      add_piece(color_of(recover_piece), piece_of(recover_piece),
                static_cast<Square>(+to_square ^ 8));
      break;
    case MoveType::Promotion:
      add_piece(color_of(return_piece), PieceType::Pawn, from_square);
      remove_piece(color_of(return_piece), piece_of(return_piece), to_square);
      if (recover_piece != Piece::None) {
        add_piece(color_of(recover_piece), piece_of(recover_piece), to_square);
      }
      break;
  }
  assert(ply > 0);
  ply--;
  game_state = history[ply];
}

std::expected<void, FenError> Board::set_fen(std::string_view fen) {
  // Hilfsfunktion: baut einen Fehler mit Position im Original-String
  auto fail = [](FenErrorCode code, size_t pos, char found) {
    return std::unexpected(FenError{code, pos, found});
  };

  // ------------------------------------------------------------------
  // Schritt 1: FEN in Felder zerlegen.
  // Kein istringstream, weil ich die Position im Original-String brauche,
  // um sie in den FenError zu schreiben.
  // ------------------------------------------------------------------
  struct Field {
    std::string_view text{};
    size_t offset{};
  };
  std::array<Field, 6> fields{};
  size_t field_count = 0;

  for (size_t i = 0; i < fen.size() && field_count < fields.size();) {
    while (i < fen.size() && fen[i] == ' ') ++i;// Im Grunde ignorieren wir die anfangs Leerzeichen
    if (i >= fen.size()) break; // Idk warum wir das so machen eine Line vorher checken wir noch i < fen.size()
    const size_t start = i; //hier wissen wo es wirklcih anfängt
    while (i < fen.size() && fen[i] != ' ') ++i; //Hier checken wie lange es ist
    fields[field_count++] = Field{fen.substr(start, i - start), start}; //Offset?
  }

  // Die ersten vier Felder sind Pflicht, die Clocks sind optional (EPD).
  if (field_count < 4) {
    return fail(FenErrorCode::WrongFieldCount, fen.size(), '\0');
  }

  // ------------------------------------------------------------------
  // Schritt 2: In LOKALE Variablen parsen. this bleibt bis Schritt 8 unberuehrt,
  // damit ein kaputter FEN kein halb befuelltes Board hinterlaesst.
  // ------------------------------------------------------------------
  std::array<Piece, 64> new_mailbox{};
  new_mailbox.fill(Piece::None);

  GameState new_state{};
  new_state.side_to_move = Color::White;
  new_state.castling_rights = static_cast<CastlingRight>(0);
  new_state.ep_square = Square::None;
  new_state.half_move_clock = 0;
  new_state.total_move_count = 1;
  new_state.captured_piece = Piece::None;

  // ------------------------------------------------------------------
  // Schritt 3: Figurenfeld. Eine Schleife, die gleichzeitig prueft und baut.
  // rank laeuft von 7 (Rang 8) runter nach 0, file von 0 (a) nach 7 (h).
  // ------------------------------------------------------------------
  {
    const Field& f = fields[0];
    int rank = 7;
    int file = 0;
    int white_king = 0;
    int black_king = 0; 

    for (size_t i = 0; i < f.text.size(); ++i) {
      const char ch = f.text[i];
      const size_t pos = f.offset + i;

      if (ch == '/') {
        if (file != 8) return fail(FenErrorCode::InvalidRankWidth, pos, ch);
        if (rank == 0) return fail(FenErrorCode::InvalidRankCount, pos, ch);
        --rank;
        file = 0;
        continue;
      }

      if (ch >= '1' && ch <= '8') {
        file += ch - '0';
        if (file > 8) return fail(FenErrorCode::InvalidRankWidth, pos, ch);
        continue;
      }

      // Ab hier muss es eine Figur sein.
      PieceType pt = PieceType::None;
      switch (ch) {
      case 'P':
      case 'p':
        if (rank == 0 || rank == 7)
          return fail(FenErrorCode::InvalidPiece, pos, ch);
        pt = PieceType::Pawn;
        break;
      case 'N':
      case 'n':
        pt = PieceType::Knight;
        break;
      case 'B':
      case 'b':
        pt = PieceType::Bishop;
        break;
      case 'R':
      case 'r':
        pt = PieceType::Rook;
        break;
      case 'Q':
      case 'q':
        pt = PieceType::Queen;
        break;
      case 'K':
        pt = PieceType::King;
        ++white_king;
        break;
      case 'k':
        pt = PieceType::King;
        ++black_king;
        break;
      default:
        return fail(FenErrorCode::InvalidPiece, pos, ch);
      }

      if (file > 7) return fail(FenErrorCode::InvalidRankWidth, pos, ch);

      const Color c = (ch >= 'A' && ch <= 'Z') ? Color::White : Color::Black;
      new_mailbox[static_cast<size_t>(rank * 8 + file)] = make_piece(c, pt);
      ++file;
    }
    //Ab hier muss File 8 und Rank 0 sein. Wenn nicht ist etwas schief gelaufen:
    if (file != 8) {
      return fail(FenErrorCode::InvalidRankWidth, f.offset + f.text.size(), '\0');
    }
    if (rank != 0) {
      return fail(FenErrorCode::InvalidRankCount, f.offset + f.text.size(), '\0');
    }
    if(black_king != 1 || white_king != 1){
      return fail(FenErrorCode::InvalidKing, 0, '\0');
    }
  }

  // ------------------------------------------------------------------
  // Schritt 4: Side to move
  // ------------------------------------------------------------------
  {
    const Field& f = fields[1];
    if (f.text == "w") {
      new_state.side_to_move = Color::White;
    } else if (f.text == "b") {
      new_state.side_to_move = Color::Black;
    } else {
      return fail(FenErrorCode::InvalidActiveColor, f.offset,
                  f.text.empty() ? '\0' : f.text[0]);
    }
  }

  // ------------------------------------------------------------------
  // Schritt 5: Rochaderechte. Entweder "-" oder eine Teilmenge von KQkq,
  // jedes Zeichen hoechstens einmal.
  // ------------------------------------------------------------------
  {
    const Field& f = fields[2];
    if (f.text.empty()) {
      return fail(FenErrorCode::InvalidCastlingRights, f.offset, '\0');
    }
    if (f.text != "-") {
      CastlingRight rights = static_cast<CastlingRight>(0);
      for (size_t i = 0; i < f.text.size(); ++i) {
        const char ch = f.text[i];
        const size_t pos = f.offset + i;

        CastlingRight bit;
        switch (ch) {
        case 'K':
          if (new_mailbox[+Square::e1] != Piece::WhiteKing ||
              new_mailbox[+Square::h1] != Piece::WhiteRook) {
            return fail(FenErrorCode::InvalidCastlingRights, pos, ch);
          }
          bit = CastlingRight::White_Short; break;

          case 'Q':
            if (new_mailbox[+Square::e1] != Piece::WhiteKing ||
                new_mailbox[+Square::a1] != Piece::WhiteRook) {
              return fail(FenErrorCode::InvalidCastlingRights, pos, ch);
            }
          bit = CastlingRight::White_Long;  break;

          case 'k':
            if (new_mailbox[+Square::e8] != Piece::BlackKing ||
                new_mailbox[+Square::a8] != Piece::BlackRook) {
              return fail(FenErrorCode::InvalidCastlingRights, pos, ch);
            }
            bit = CastlingRight::Black_Short;
            break;

          case 'q':
            if (new_mailbox[+Square::e8] != Piece::BlackKing ||
                new_mailbox[+Square::a8] != Piece::BlackRook) {
              return fail(FenErrorCode::InvalidCastlingRights, pos, ch);
            }
            bit = CastlingRight::Black_Long;
            break;
          default: return fail(FenErrorCode::InvalidCastlingRights, pos, ch);
        }
        // Doppeltes Zeichen, z. B. "KKkq" -- Den Check muss ich mir merken der ist crazy gut
        if (+(rights & bit) != 0) {
          return fail(FenErrorCode::InvalidCastlingRights, pos, ch);
        }
        rights = rights | bit;
      }
      new_state.castling_rights = rights;
    }
  }

  // ------------------------------------------------------------------
  // Schritt 6: En-Passant-Feld. Entweder "-" oder zwei Zeichen a-h + 3 oder 6.
  // Der Rang muss zur Seite am Zug passen: Weiss am Zug => Schwarz hat gerade
  // doppelt gezogen => EP-Feld liegt auf Rang 6.
  // ------------------------------------------------------------------
  {
    const Field& f = fields[3];
    if (f.text.empty()) {
      return fail(FenErrorCode::InvalidEnPassantSquare, f.offset, '\0');
    }
    if (f.text != "-") {
      if (f.text.size() != 2) {
        return fail(FenErrorCode::InvalidEnPassantSquare, f.offset, f.text[0]);
      }
      const char file_ch = f.text[0];
      const char rank_ch = f.text[1];

      if (file_ch < 'a' || file_ch > 'h') {
        return fail(FenErrorCode::InvalidEnPassantSquare, f.offset, file_ch);
      }
      const char expected_rank = // das auch holy shit ist das schön
          (new_state.side_to_move == Color::White) ? '6' : '3';
      if (rank_ch != expected_rank) {
        return fail(FenErrorCode::InvalidEnPassantSquare, f.offset + 1, rank_ch);
      }

      const int file = file_ch - 'a';
      const int rank = rank_ch - '1';
      Square ep_square = static_cast<Square>(rank * 8 + file);
      if (new_mailbox[+ep_square] != Piece::None ||
          new_mailbox[+ep_square ^ 16] != Piece::None ||
          (rank_ch == 3 && new_mailbox[+ep_square ^ 8] != Piece::WhitePawn) ||
          (rank_ch == 6 && new_mailbox[+ep_square ^ 8] != Piece::BlackPawn)) {
        return fail(FenErrorCode::InvalidEnPassantSquare, f.offset, '\0');
      }
        new_state.ep_square = ep_square;
    }
  }

  // ------------------------------------------------------------------
  // Schritt 7: Halbzug- und Zugzaehler. Beide optional (EPD hat sie nicht).
  // ------------------------------------------------------------------
  if (field_count > 4) {
    const Field& f = fields[4];
    unsigned value = 0;
    const char* first = f.text.data();
    const char* last = first + f.text.size();
    const auto res = std::from_chars(first, last, value);
    if (res.ec != std::errc{} || res.ptr != last) {
      return fail(FenErrorCode::InvalidHalfmoveClock, f.offset,
                  f.text[0]);
    }
    if (value > 0xFFFFu) {  // passt sonst nicht in uint16_t
      return fail(FenErrorCode::InvalidHalfmoveClock, f.offset, f.text[0]);
    }
    new_state.half_move_clock = static_cast<uint16_t>(value);
  }

  if (field_count > 5) {
    const Field& f = fields[5];
    unsigned value = 0;
    const char* first = f.text.data();
    const char* last = first + f.text.size();
    const auto res = std::from_chars(first, last, value);
    if (res.ec != std::errc{} || res.ptr != last || value == 0) {
      return fail(FenErrorCode::InvalidFullmoveNumber, f.offset,
                  f.text[0]);
    }
    new_state.total_move_count = static_cast<uint16_t>( value );
  }

  // ------------------------------------------------------------------
  // Schritt 8: Erst jetzt das echte Objekt anfassen. Ab hier kann nichts
  // mehr schiefgehen, also gibt es auch kein halb befuelltes Board mehr.
  // ------------------------------------------------------------------
  bitmaps.fill(0ULL);
  color_board.fill(0ULL);
  mailbox = new_mailbox;

  for (size_t sq = 0; sq < mailbox.size(); ++sq) {
    const Piece piece = mailbox[sq];
    if (piece == Piece::None) continue;

    const uint64_t mask = square_bb(static_cast<Square>(sq));
    bitmaps[get_index(color_of(piece), piece_of(piece))] |= mask;
    color_board[+color_of(piece)] |= mask;
  }

  game_state = new_state;
  ply = 0;

  return {};
}

std::string Board::to_fen() const{
  //Schritt 1: Board
  constexpr std::array<char, 14> piece_chars{
      'P', 'N', 'B', 'R', 'Q', 'K', '\0', '\0', 'p', 'n', 'b', 'r', 'q', 'k'};
  std::string fen{};
  for (int rank = 7; rank >= 0; --rank) {
    int empty{};
    for (int file = 0; file < 8; ++file) {
      size_t square = static_cast<size_t>(rank * 8 + file);
      Piece p = mailbox[square];

      if (p == Piece::None) {
        ++empty;
        continue;
      }

      if (empty) {
        fen += std::to_string(empty);
        empty = 0;
      }

      fen += piece_chars[+p];
    }

    if (empty) {
      fen += std::to_string(empty);
    }

    if (rank != 0) {
      fen += "/";
    }
  }

  fen += ' ';

  //Schritt 2: Side to Move
  fen += game_state.side_to_move == Color::White ? 'w' : 'b';

  fen += ' ';
  
  //Schritt 3: CastlingRights...
  CastlingRight rights = game_state.castling_rights;
  if (+rights == 0) {
    fen += '-';
  }
  else{
    if (+rights & +CastlingRight::White_Short) {
      fen += "K";
    }
    if (+rights & +CastlingRight::White_Long) {
      fen += "Q";
    }
    if (+rights & +CastlingRight::Black_Short) {
      fen += "k";
    }
    if (+rights & +CastlingRight::Black_Long) {
      fen += "q";
    }
}

  fen += ' ';

  //Schritt 4: EnPassant
  Square ep = game_state.ep_square;
  if(ep != Square::None){
    //Erst File dann Rank
    fen += static_cast<char>( 'a' + ( +ep % 8 ) );
    fen += static_cast<char>('1' + (+ep / 8));
  }
  else{
    fen += '-';
  }


  fen += ' ';

  //Schritt 5: Half_Move_Count
  fen += std::to_string(game_state.half_move_clock);

  fen += ' ';

  //Schritt 6: Total_Move_Count
  fen += std::to_string(game_state.total_move_count);

  //Schritt 7: Return
  return fen;
}

Board::Board() {
  init_bitmaps();
  init_castling_bitmask();

  color_board[+Color::White] = get_color_bitmap(Color::White);
  color_board[+Color::Black] = get_color_bitmap(Color::Black);
  game_state = {Color::White, CastlingRight::Any, Square::None, 0, 1, Piece::None}; // halfmove_clock und total_move sind magic numbers.. kann ich das irgendwie richten?

  sync_mailbox_with_bitmaps();
}