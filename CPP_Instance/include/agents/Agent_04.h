#pragma once
#include "agents/IChessAgent.h"
#include "engine/game.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include <vector>
#include <limits>
#include <random>

class Agent_04 : public IChessAgent
{
public:
    Agent_04(int depth);

    MoveData chooseMove(const Game& game) override;
    const char* name() const override { return "Agent_04 (Material + PST + MMwABP + MoveOrdering)"; }

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

    // Alpha-beta search with move ordering
    double alphabeta(Board& board, int depth,
                     double alpha, double beta,
                     bool maximizingPlayer,
                     MoveGenerator& gen);

    // Evaluation (material + piece-square tables)
    double evaluate(const Board& board) const;

    // Move ordering helper
    void orderMoves(Board& board,
                    std::vector<MoveData>& moves,
                    MoveGenerator& gen) const;

    // Piece-square tables
    static const int pawnTable[8][8];
    static const int knightTable[8][8];
    static const int bishopTable[8][8];
    static const int rookTable[8][8];
    static const int queenTable[8][8];
    static const int kingTable[8][8];
};
