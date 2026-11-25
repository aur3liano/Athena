#ifndef GAME_H
#define GAME_H

#include "engine/board.h"
#include "engine/move_state.h"
#include <vector>
#include <string>
#include <unordered_map>

enum class GameResult {
    InProgress,
    CheckmateWhiteWins,
    CheckmateBlackWins,
    Stalemate,
    DrawFiftyMoveRule,
    DrawThreefoldRepetition,
    DrawDeadPosition
};

class Game
{
public:
    Game();

    std::vector<MoveData> getLegalMoves();
    bool applyMove(const MoveData& m);

    const Board& getBoard() const { return board; }
    Board& getBoard() { return board; }

    GameResult evaluateGameState();
    bool isGameOver() { return evaluateGameState() != GameResult::InProgress; }

    std::string getGameResultMessage() const;

private:
    Board board;

    // --- New: Repetition Table ---
    std::unordered_map<std::string,int> repetitionTable;

    // --- New: draw detection helpers ---
    bool isDeadPosition() const;
    bool isThreefoldRepetition() const;
    bool isFiftyMoveRule() const;

    // Called every time a move is applied
    void updateRepetitionTable();
};

#endif
