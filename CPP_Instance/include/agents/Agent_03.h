#pragma once
#include "agents/IChessAgent.h"
#include "engine/game.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include <vector>
#include <limits>
#include <random>

class Agent_03 : public IChessAgent
{
public:
    Agent_03(int depth);

    MoveData chooseMove(const Game& game) override;
    const char* name() const override { return "Agent_03 (Material + MMwABP)"; }

private:
    // Random number generator for evaluation noise
    mutable std::mt19937 rng;
    mutable std::uniform_real_distribution<double> noiseDist;

    int searchDepth;
    static constexpr double MATE_SCORE = 1000000.0;
    
    // Endgame heuristics
    int kingDistToEdge(int r, int c) const;
    double enemyKingMobility(const Board& board, MoveGenerator& gen) const;
    double endgameKingPressure(const Board& board, MoveGenerator& gen) const;
    double kingApproachScore(const Board& board) const;

    // Alpha-beta version
    double alphabeta(Board& board, int depth,
                     double alpha, double beta,
                     bool maximizingPlayer,
                     MoveGenerator& gen);

    // Same material eval as Agent_02
    double evaluate(const Board& board) const;
};
