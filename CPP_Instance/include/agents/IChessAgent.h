#pragma once
#include "engine/game.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"   // for MoveData

class IChessAgent {
public:
    virtual ~IChessAgent() = default;

    // Return a fully populated MoveData representing the chosen move.
    // The agent MUST NOT modify the game.
    virtual MoveData chooseMove(const Game& game) = 0;

    // UI/tournament-friendly name
    virtual const char* name() const = 0;
};
