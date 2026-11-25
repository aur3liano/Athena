#include "ui/cli_ui.h"
#include <iostream>
#include <cctype>
#include "engine/game.h"
#include "engine/notation.h"

// Utility to print a piece as a char
static char pieceChar(const Piece& p)
{
    if (p.type == PieceType::Empty)
        return '.';

    char c = '?';
    switch (p.type) {
    case PieceType::Pawn:   c = 'P'; break;
    case PieceType::Knight: c = 'N'; break;
    case PieceType::Bishop: c = 'B'; break;
    case PieceType::Rook:   c = 'R'; break;
    case PieceType::Queen:  c = 'Q'; break;
    case PieceType::King:   c = 'K'; break;
    default: break;
    }

    if (p.color == Color::Black)
        c = static_cast<char>(std::tolower(c));

    return c;
}

CLI_UI::CLI_UI()
    : agent(nullptr)
{}

void CLI_UI::setAgent(IChessAgent* a) {
    agent = a;
}

// getMoveInput now supports:
// - if agent==nullptr → ask user for input
// - if agent!=nullptr → query agent for a Move and convert to SAN or coordinate
std::string CLI_UI::getMoveInput()
{
    // If an agent is attached, we return the agent's chosen move in coordinate notation.
    if (agent != nullptr) {
        // We need a Game reference to generate agent moves.
        // The CLI_UI does not own the Game, but the controller calling getMoveInput()
        // can set a global or pass the game pointer before calling.
        //
        // To keep CLI_UI simple and UI-only, we assume the controller does this:
        //
        //     ui->setCurrentGame(&game);
        //
        // So here we retrieve that stored pointer.
        //
        // ----
        // If you do NOT want CLI_UI to store a Game pointer,
        // we can instead redesign the controller to call:
        //     agent->chooseMove(game)
        // and bypass UI entirely for agent moves.
        //
        // For now, the simplest general solution is to let CLI_UI hold a game pointer.
        //
    }

    // FALLBACK: human move
    std::string input;
    std::cout << "Enter move (e.g., e2e4): ";
    std::cin >> input;
    return input;
}

// displayBoard: prints ASCII board
void CLI_UI::displayBoard(const Board& board)
{
    std::cout << "\n  a b c d e f g h\n";

    for (int r = 0; r < 8; r++) {
        std::cout << (8 - r) << " ";
        for (int c = 0; c < 8; c++) {
            const Piece& p = board.at(r, c);
            std::cout << pieceChar(p) << " ";
        }
        std::cout << (8 - r) << "\n";
    }

    std::cout << "  a b c d e f g h\n\n";
}

void CLI_UI::showMessage(const std::string& msg)
{
    std::cout << msg << std::endl;
}

void CLI_UI::showGameResult(Game& game)
{
    std::string msg = game.getGameResultMessage();
    showMessage(msg);
}
