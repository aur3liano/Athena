#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H

#include <string>
#include "engine/board.h"

class IChessUI {
public:
    virtual ~IChessUI() {}

    virtual std::string getMoveInput() = 0;
    virtual void displayBoard(const Board& board) = 0;
    virtual void showMessage(const std::string& msg) = 0;
};

#endif
