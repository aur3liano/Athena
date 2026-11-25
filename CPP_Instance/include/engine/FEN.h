#ifndef FEN_H
#define FEN_H
#include <string>
#include "engine/board.h"
// FEN loader/writer for refactored Board
class FEN
{
public:
    // Load a FEN into the Board (overwrite all board state)
    static bool loadFEN(Board& board, const std::string& fen);
    // Export board state as a FEN string
    static std::string toFEN(const Board& board);
};

#endif
