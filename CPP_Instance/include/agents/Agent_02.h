#pragma once
#include "agents/IChessAgent.h"
#include "engine/game.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include <vector>
#include <limits>
#include <random>

class Agent_02 : public IChessAgent
{
public:
    Agent_02(int depth);
    MoveData chooseMove(const Game& game) override;
    const char* name() const override { return "Agent_02 (Material + Minimax)"; }
private:
    // Random number generator for evaluation noise
    mutable std::mt19937 rng;
    mutable std::uniform_real_distribution<double> noiseDist;

    // NEW: store depth per instance
    int searchDepth;

    // Mate score large enough to dwarf any material score
    static constexpr double MATE_SCORE = 1000000.0;

    // Core search routine
    double minimax(Board& board, int depth, bool maximizingPlayer, MoveGenerator& gen);
    // Static evaluation (material only)
    double evaluate(const Board& board) const;
    // Helpers
    bool isCheck(const Board& board, MoveGenerator& gen, Color side) const;
};
