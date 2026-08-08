#pragma once
#include <cstdint>
#include <utility>
#include <cassert>
static constexpr uint8_t PieceTypeCount = 6;

enum class Color : uint8_t {White = 0, Black = 1};
constexpr auto operator+(Color c){ return std::to_underlying(c);}

enum class PieceType : uint8_t {Pawn = 0, Knight = 1, Bishop = 2, Rook = 3, Queen = 4, King = 5, None = 6};
constexpr auto operator+(PieceType piece) { return std::to_underlying(piece); }

enum class Piece : uint8_t {
  WhitePawn = 0, WhiteKnight = 1, WhiteBishop = 2, WhiteRook = 3, WhiteQueen = 4, WhiteKing = 5,
  None = 7,
  BlackPawn = 8, BlackKnight = 9, BlackBishop = 10, BlackRook = 11, BlackQueen = 12, BlackKing = 13
};
constexpr auto operator+(Piece p){return std::to_underlying(p);}

constexpr Color color_of(Piece p) {
  return static_cast<Color>(+p >> 3);
}
static_assert(( color_of(Piece::BlackRook) == Color::Black));

constexpr PieceType piece_of(Piece p) {
  return static_cast<PieceType>(+p & 7);
}
static_assert(piece_of(Piece::WhiteBishop) == PieceType::Bishop);

constexpr Piece make_piece(Color c, PieceType p) {
  return static_cast<Piece>((+c << 3) | +p);
};

enum class Square : uint8_t {
    a1 = 0, b1 = 1, c1 = 2, d1 = 3, e1 = 4, f1 = 5, g1 = 6, h1 = 7,
    a2 = 8, b2 = 9, c2 = 10, d2 = 11, e2 = 12, f2 = 13, g2 = 14, h2 = 15,
    a3 = 16, b3 = 17, c3 = 18, d3 = 19, e3 = 20, f3 = 21, g3 = 22, h3 = 23,
    a4 = 24, b4 = 25, c4 = 26, d4 = 27, e4 = 28, f4 = 29, g4 = 30, h4 = 31,
    a5 = 32, b5 = 33, c5 = 34, d5 = 35, e5 = 36, f5 = 37, g5 = 38, h5 = 39,
    a6 = 40, b6 = 41, c6 = 42, d6 = 43, e6 = 44, f6 = 45, g6 = 46, h6 = 47,
    a7 = 48, b7 = 49, c7 = 50, d7 = 51, e7 = 52, f7 = 53, g7 = 54, h7 = 55,
    a8 = 56, b8 = 57, c8 = 58, d8 = 59, e8 = 60, f8 = 61, g8 = 62, h8 = 63,
    
    None = 64
};

constexpr auto operator+(Square s) { return std::to_underlying(s); }

enum class CastlingRight : uint8_t {
  // ich wollte die 0b Schreibweise mal nutzen
  White_Short = 0b0000'0001,
  White_Long = 0b0000'0010,
  Black_Short = 0b0000'0100,
  Black_Long = 0b0000'1000,
  Any = 0b0000'1111
};

constexpr auto operator+(CastlingRight cr){ return std::to_underlying(cr);}

constexpr CastlingRight operator|(CastlingRight left, CastlingRight right) {
  return static_cast<CastlingRight>(+left | +right);
}
constexpr CastlingRight operator&(CastlingRight left, CastlingRight right) {
  return static_cast<CastlingRight>(+left & +right);
}
constexpr CastlingRight operator~(CastlingRight castling_rights) {
  return static_cast<CastlingRight>(~+castling_rights);
}
constexpr CastlingRight& operator&=(CastlingRight& left, CastlingRight right){
  return left = left & right; 
}

// Der GameState (und damit auch eine FEN) speichert ausschließlich historische oder "unsichtbare" 
// Status-Informationen, die man nicht allein durch das Betrachten der aktuellen Figuren auf dem Brett herleiten kann.
struct GameState{ 
  Color side_to_move;
  CastlingRight castling_rights;
  Square ep_square;
  uint8_t half_move_clock;
  uint8_t total_move_count;
  Piece captured_piece;
};

inline constexpr uint64_t square_bb(Square square) {
  assert(square != Square::None);
  return 1ULL << +square;
};

enum class MoveType : uint16_t {
  Normal = 0,
  Promotion = 1 << 14,
  En_Passant = 2 << 14,
  Castling = 3 << 14
};

constexpr auto operator+(MoveType t){ return std::to_underlying(t);} 
//sollte ich eine Funktion schreiben die mir das Promotion Piece in Piecetype returned?

// Move braucht 16 Bits um einen Zug darstellen zu können
// bit  0- 5: Zielfeld (0 - 63 = 6 Bits)
// bit  6-11: Startfeld (s. o.)
// bit 12-13: Promotion (4 mögliche Figuren - 2 Bits)
// bit 14-15: "Special Move Flag": Promotion (1), En Passant (2), Rochade (3) die 0 ist Normalzug siehe oben für MoveType. 

class Move{
public:
  //Initialisierung 
  Move() = default;
  explicit Move(uint16_t d) : data(d) {};
  Move(Square from, Square to)
      : Move(static_cast<uint16_t>(+from << 6) +
             +to) {};

  //Make
  template<MoveType T> 
  static constexpr  Move make(Square from, Square to, PieceType pt = PieceType::Knight) {
    assert(+pt >= +PieceType::Knight && +pt <= +PieceType::Queen);
    return Move(static_cast<uint16_t>(+T + ((+pt - +PieceType::Knight) << 12)) +
                static_cast<uint16_t>(+from << 6) + +to);
  }

  // Unmake: extrahiere alle Parts des Moves vor dem ausführen
  constexpr Square from_sq() const {
    assert(is_ok());
    return static_cast<Square>((data >> 6) & 63);
  }
  constexpr Square to_sq() const {
    assert(is_ok());
    return static_cast<Square>(data & 63);
  }
  constexpr PieceType promotion_piece() const {
    assert(is_ok());
    return static_cast<PieceType>(((data >> 12) & 3) + +PieceType::Knight);
  }
  constexpr MoveType type_of() const {
    assert(is_ok());
    return static_cast<MoveType>(data & (3 << 14));
  }

  static constexpr Move null_move() { return Move(65); } // null ist: "Gegner darf zwei Züge machen" relevant für Algorithmus
  static constexpr Move none_move() { return Move(0); } // wenn z. B. noch kein Move gefunden wurden

  constexpr bool is_ok() const {return none_move().data != data && null_move().data != data;} //Magic Number = Bad

  //Operatoroverload
  constexpr bool operator==(const Move&) const = default; // neu in 20, kein != Operator nötig, default checkt einfach alle non static Member. Zero cost abstraction, gibt auch Nachteile falls gewisse Member nicht verglichen werden sollen aber mehr in den Notizen
  constexpr explicit operator bool() const { return data != 0;}
  constexpr uint16_t get_raw_move() const {return data;}

protected:
  uint16_t data{};
};