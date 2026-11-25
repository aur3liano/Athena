#pragma once
#include <string>

class LegalMoveFENAnalysis
{
public:
    // Analyze the FEN, print legal moves, and return number of moves.
    static int analyzeFEN(const std::string& fen, bool useSAN);
};
