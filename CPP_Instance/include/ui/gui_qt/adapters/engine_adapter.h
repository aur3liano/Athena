#pragma once
#include <vector>
#include <string>
#include <utility>
#include "engine/move_state.h"

class Game;

class EngineAdapter
{
public:
    EngineAdapter();

    // NEW accessor needed by ChessWindow.cpp for AI logic
    Game* getGamePtr() { return gamePtr; }

    const Game& getGame() const { return *gamePtr; }
    Game& getGame() { return *gamePtr; }

    // Game control
    void newGame();
    void loadFEN(const std::string& fen);
    std::string exportFEN() const;

    // Board queries
    int pieceAt(int rank, int file) const;

    // Move queries
    std::vector<std::pair<int,int>> legalTargetsFrom(int rank, int file) const;

    MoveData getMoveData(int fromRank, int fromFile, int toRank, int toFile) const;

    bool applyMove(int fromRank, int fromFile, int toRank, int toFile);
    bool applyMove(const MoveData& move);

    bool isGameOver() const;
    std::string getGameResultMessage() const;

private:
    Game* gamePtr = nullptr;   // REQUIRED
};