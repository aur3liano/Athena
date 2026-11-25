#pragma once
#include <cstdint>
#include "engine/board.h"

// ------------------------------------------------------------
// Perft statistics container
// ------------------------------------------------------------
struct PerftStats
{
    uint64_t nodes = 0;
    uint64_t captures = 0;
    uint64_t enPassant = 0;
    uint64_t castles = 0;
    uint64_t promotions = 0;
    uint64_t checks = 0;
    uint64_t discoveredChecks = 0;
    uint64_t doubleChecks = 0;
    uint64_t checkmates = 0;
};

// ------------------------------------------------------------
// Perft interfaces
// ------------------------------------------------------------
void perft(Board& board,
           int depth,
           PerftStats& stats,
           bool enableStats);

void perft_divide(Board& board,
                  int depth,
                  bool useSAN,
                  bool enableStats);