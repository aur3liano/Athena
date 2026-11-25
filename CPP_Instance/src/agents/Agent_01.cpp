#include "agents/Agent_01.h"
#include "engine/game.h"
#include "engine/move_generator.h"

Agent_01::Agent_01()
{
    std::random_device rd;
    rng = std::mt19937(rd());
}

MoveData Agent_01::chooseMove(const Game& game)
{
    // The game exposes a reference to its internal board.
    Board boardCopy = game.getBoard();   // safe: returned by value

    MoveGenerator gen;
    std::vector<MoveData> legal = gen.generateLegalMoves(boardCopy);

    // No legal moves means stalemate or checkmate
    if (legal.empty())
        return MoveData();  // null-move

    std::uniform_int_distribution<size_t> dist(0, legal.size() - 1);
    return legal[dist(rng)];
}
