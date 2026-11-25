#ifndef BOARD_H
#define BOARD_H
#include <utility>
// ============================================================
// Piece / Color definitions
// ============================================================
enum class PieceType { Empty, Pawn, Knight, Bishop, Rook, Queen, King };
enum class Color { None, White, Black };
struct Piece
{
    PieceType type = PieceType::Empty;
    Color color = Color::None;
};
// ------------------------------------------------------------
// Unified Castling Rights Structure
// ------------------------------------------------------------
// This is now the *only* source of castling rights.
// No kingMoved / rookMoved flags exist anymore.
struct CastlingRights
{
    bool whiteKingSide  = false;
    bool whiteQueenSide = false;
    bool blackKingSide  = false;
    bool blackQueenSide = false;
};
// Forward declare MoveState
struct MoveState;
// Forward declare MoveGenerator
class MoveGenerator;
// ============================================================
// Board class — PURE STATE CONTAINER (NO MOVE LOGIC)
// ============================================================
class Board
{
public:
    Board();
    // Accessors
    const Piece& at(int r, int c) const;
    void placePiece(int r, int c, const Piece& p);
    void removePiece(int r, int c);
    void movePiece(int fr, int fc, int tr, int tc);
    bool inBounds(int r, int c) const;
    // Position control
    void resetToStartingPosition();
    // Game state getters
    Color sideToMove() const { return stm; }
    int enPassantRow() const { return epRow; }
    int enPassantCol() const { return epCol; }
    // Castling rights getters
    const CastlingRights& getCastlingRights() const { return castling; }
    int halfmoveClock() const { return halfmoveClock_; }
    int fullmoveNumber() const { return fullmoveNumber_; }
    std::pair<int,int> whiteKing() const { return {whiteKingRow, whiteKingCol}; }
    std::pair<int,int> blackKing() const { return {blackKingRow, blackKingCol}; }
    // No isSquareAttacked
    // No castling logic
    friend class FEN;
    friend class MoveGenerator;
private:
    Piece squares[8][8];
    // Basic position state
    Color stm;
    int epRow;
    int epCol;
    // Unified castling rights struct
    CastlingRights castling;
    // Halfmove/fullmove counters
    int halfmoveClock_;
    int fullmoveNumber_;
    // King locations
    int whiteKingRow, whiteKingCol;
    int blackKingRow, blackKingCol;
    // Utility
    void clearEnPassant();
};

#endif
