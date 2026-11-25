#ifndef CLI_UI_H
#define CLI_UI_H

#include "ui_interface.h"
#include "agents/IChessAgent.h"

class Game;

class CLI_UI : public IChessUI {
public:
    CLI_UI();

    std::string getMoveInput() override;
    void displayBoard(const Board& board) override;
    void showMessage(const std::string& msg) override;

    void setAgent(IChessAgent* a);

    // NEW: show game over messages (mate, stalemate, draw types)
    void showGameResult(Game& game);

private:
    IChessAgent* agent;
};

#endif
