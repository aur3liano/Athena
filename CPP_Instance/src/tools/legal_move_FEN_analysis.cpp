#include "tools/legal_move_FEN_analysis.h"
#include "engine/board.h"
#include "engine/FEN.h"
#include "engine/move_generator.h"
#include "engine/notation.h"
#include <iostream>
#include <vector>
#include <string>

int LegalMoveFENAnalysis::analyzeFEN(const std::string& fen, bool useSAN)
{
    Board board;
    if (!FEN::loadFEN(board, fen))
    {
        std::cerr << "ERROR: Invalid FEN string.\n";
        return -1;
    }
    MoveGenerator gen;
    std::vector<MoveData> moves = gen.generateLegalMoves(board);
    std::cout << "FEN: " << fen << "\n";
    std::cout << "Side to move: "
              << (board.sideToMove() == Color::White ? "White" : "Black")
              << "\n\n";
    std::cout << "Total legal moves: " << moves.size() << "\n\n";
    for (const MoveData& m : moves)
    {
        std::string coord = Notation::moveToCoordinate(m);
        if (useSAN)
        {
            std::string san = Notation::moveToSAN(m, board);
            std::cout << "  " << coord << " (" << san << ")\n";
        }
        else
        {
            std::cout << "  " << coord << "\n";
        }
    }
    std::cout << std::endl;
    return static_cast<int>(moves.size());
}