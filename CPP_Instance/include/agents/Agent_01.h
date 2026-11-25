#pragma once
#include "agents/IChessAgent.h"
#include <random>

class Agent_01 : public IChessAgent {
public:
    Agent_01();

    MoveData chooseMove(const Game& game) override;
    const char* name() const override { return "Agent_01 (Random)"; }

private:
    std::mt19937 rng;
};
