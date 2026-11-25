#include "engine/board.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include <cstdlib>
#include <cmath>
// ============================================================================
// Constructor / Reset
// ============================================================================
Board::Board()
{
    resetToStartingPosition();
}

void Board::resetToStartingPosition()
{
    // Clear board
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            squares[r][c] = {PieceType::Empty, Color::None};
    // White pieces
    squares[7][0] = {PieceType::Rook,   Color::White};
    squares[7][1] = {PieceType::Knight, Color::White};
    squares[7][2] = {PieceType::Bishop, Color::White};
    squares[7][3] = {PieceType::Queen,  Color::White};
    squares[7][4] = {PieceType::King,   Color::White};
    squares[7][5] = {PieceType::Bishop, Color::White};
    squares[7][6] = {PieceType::Knight, Color::White};
    squares[7][7] = {PieceType::Rook,   Color::White};
    for (int c = 0; c < 8; c++)
        squares[6][c] = {PieceType::Pawn, Color::White};
    // Black pieces
    squares[0][0] = {PieceType::Rook,   Color::Black};
    squares[0][1] = {PieceType::Knight, Color::Black};
    squares[0][2] = {PieceType::Bishop, Color::Black};
    squares[0][3] = {PieceType::Queen,  Color::Black};
    squares[0][4] = {PieceType::King,   Color::Black};
    squares[0][5] = {PieceType::Bishop, Color::Black};
    squares[0][6] = {PieceType::Knight, Color::Black};
    squares[0][7] = {PieceType::Rook,   Color::Black};
    for (int c = 0; c < 8; c++)
        squares[1][c] = {PieceType::Pawn, Color::Black};
    // Turn & EP
    stm = Color::White;
    epRow = epCol = -1;
    // Unified castling rights
    castling.whiteKingSide  = true;
    castling.whiteQueenSide = true;
    castling.blackKingSide  = true;
    castling.blackQueenSide = true;
    // Clocks
    halfmoveClock_ = 0;
    fullmoveNumber_ = 1;
    // Track king locations
    whiteKingRow = 7; whiteKingCol = 4;
    blackKingRow = 0; blackKingCol = 4;
}

// ============================================================================
// Accessors
// ============================================================================
const Piece& Board::at(int r, int c) const
{
    return squares[r][c];
}

void Board::placePiece(int r, int c, const Piece& p)
{
    squares[r][c] = p;
    if (p.type == PieceType::King)
    {
        if (p.color == Color::White)
        {
            whiteKingRow = r;
            whiteKingCol = c;
        }
        else
        {
            blackKingRow = r;
            blackKingCol = c;
        }
    }
}

void Board::removePiece(int r, int c)
{
    squares[r][c] = {PieceType::Empty, Color::None};
}

void Board::movePiece(int fr, int fc, int tr, int tc)
{
    Piece p = at(fr, fc);
    placePiece(tr, tc, p);
    removePiece(fr, fc);
}

bool Board::inBounds(int r, int c) const
{
    return (r >= 0 && r < 8 && c >= 0 && c < 8);
}

// ============================================================================
// Utility
// ============================================================================
void Board::clearEnPassant()
{
    epRow = epCol = -1;
}
