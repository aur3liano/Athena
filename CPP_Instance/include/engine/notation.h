#ifndef NOTATION_H
#define NOTATION_H
#include <string>
#include <vector>
#include "engine/board.h"
#include "engine/move_state.h"
// ============================================================================
// Notation
// Provides translations between MoveData and external notations:
//   - Coordinate notation (e2e4, e7e8q)
//   - SAN (Nf3, exd5, O-O, O-O-O, etc.)
// ============================================================================
class Notation
{
public:
    // Convert a MoveData into SAN (requires mutable Board)
    static std::string moveToSAN(const MoveData& m, Board& board);
    // Parse SAN → MoveData (returns invalid MoveData if parsing fails)
    static MoveData sanToMove(const std::string& san, Board& board);
    // Parse coordinate algebraic notation ("e2e4", "a7a8q") → MoveData
    static MoveData algebraicToMove(const std::string& s,const Board& board,Color sideToMove);
    // Convert MoveData → coordinate string ("e2e4", "e7e8q")
    static std::string moveToCoordinate(const MoveData& m);
private:
    static char fileFromCol(int col) { return char('a' + col); }
    static char rankFromRow(int row) { return char('8' - row); }
    static int colFromFile(char f) { return f - 'a'; }
    static int rowFromRank(char r) { return 8 - (r - '0'); }
    static std::string pieceLetter(PieceType t);
    static bool isCastleKingSide(const std::string& san);
    static bool isCastleQueenSide(const std::string& san);
};

#endif
