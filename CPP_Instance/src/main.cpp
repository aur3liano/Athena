#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include "engine/game.h"
#include "engine/notation.h"
#include "ui/cli_ui.h"

// Agents
#include "agents/IChessAgent.h"
#include "agents/Agent_01.h"
#include "agents/Agent_02.h"
#include "agents/Agent_03.h"
#include "agents/Agent_04.h"
#include "agents/Agent_05.h"
#include "agents/Agent_06.h"

// =============================================================
// Agent Registry Entry
// =============================================================
struct AgentEntry
{
    int id;
    std::string name;
    bool needsDepth;
    std::function<IChessAgent*(int)> factory;
};

// =============================================================
// Utility: Ask user to select an agent by ID
// =============================================================
int promptAgentID(const std::vector<AgentEntry>& agents)
{
    std::cout << "\nSelect an agent:\n";
    for (const auto& a : agents)
        std::cout << "  " << a.id << " = " << a.name << "\n";

    std::cout << "Enter choice: ";
    int id;
    std::cin >> id;
    return id;
}

// =============================================================
// Utility: Ask user for depth only if needed
// =============================================================
int promptDepth(const std::string& agentName)
{
    std::cout << "Enter search depth for " << agentName << ": ";
    int d;
    std::cin >> d;
    return d;
}

int main()
{
    // =========================================================
    // Initialize game + UI
    // =========================================================
    Game game;
    CLI_UI ui;

    ui.showMessage("Welcome to the C++ Chess Engine!");

    // =========================================================
    // Agent Registry
    // Expand this list as you add Agent_03 ... Agent_10
    // =========================================================
    std::vector<AgentEntry> agents = {
        {1, "Agent_01", false, [](int){ return new Agent_01(); }},
        {2, "Agent_02", true,  [](int depth){ return new Agent_02(depth); }},
        {3, "Agent_03", true,  [](int depth){ return new Agent_03(depth); }},
        {4, "Agent_04", true,  [](int depth){ return new Agent_04(depth); }},
        {5, "Agent_05", true,  [](int depth){ return new Agent_05(depth); }},
        {6, "Agent_06", true,  [](int depth){ return new Agent_06(depth); }}
    };

    // =========================================================
    // USER SELECTS GAME MODE
    // =========================================================
    ui.showMessage("Choose mode:");
    ui.showMessage("1 = Human (White) vs Human (Black)");
    ui.showMessage("2 = Agent (White) vs Human (Black)");
    ui.showMessage("3 = Human (White) vs Agent (Black)");
    ui.showMessage("4 = Agent (White) vs Agent (Black)");

    int mode = 1;
    std::cin >> mode;

    IChessAgent* whiteAgent = nullptr;
    IChessAgent* blackAgent = nullptr;

    // =========================================================
    // SELECTION LOGIC FOR EACH MODE
    // =========================================================
    if (mode == 2)
    {
        // agent plays as White
        int idWhite = promptAgentID(agents);

        // find agent
        for (const auto& a : agents)
        {
            if (a.id == idWhite)
            {
                int depth = (a.needsDepth ? promptDepth(a.name) : -1);
                whiteAgent = a.factory(depth);
            }
        }

        ui.showMessage("Mode: Agent (White) vs Human (Black)");
    }
    else if (mode == 3)
    {
        // agent plays as Black
        int idBlack = promptAgentID(agents);

        for (const auto& a : agents)
        {
            if (a.id == idBlack)
            {
                int depth = (a.needsDepth ? promptDepth(a.name) : -1);
                blackAgent = a.factory(depth);
            }
        }

        ui.showMessage("Mode: Human (White) vs Agent (Black)");
    }
    else if (mode == 4)
    {
        // agent plays as White
        std::cout << "\nAgent for White:\n";
        int idWhite = promptAgentID(agents);
        for (const auto& a : agents)
        {
            if (a.id == idWhite)
            {
                int depth = (a.needsDepth ? promptDepth(a.name) : -1);
                whiteAgent = a.factory(depth);
            }
        }

        // agent plays as Black
        std::cout << "\nAgent for Black:\n";
        int idBlack = promptAgentID(agents);
        for (const auto& a : agents)
        {
            if (a.id == idBlack)
            {
                int depth = (a.needsDepth ? promptDepth(a.name) : -1);
                blackAgent = a.factory(depth);
            }
        }

        ui.showMessage("Mode: Agent (White) vs Agent (Black)");
    }
    else
    {
        ui.showMessage("Mode: Human (White) vs Human (Black)");
    }

    // =========================================================
    // GAME STARTS — draw initial board
    // =========================================================
    ui.displayBoard(game.getBoard());

    // =========================================================
    // MAIN GAME LOOP (unchanged)
    // =========================================================
    while (!game.isGameOver())
    {
        const Board& board = game.getBoard();
        Color side = board.sideToMove();

        ui.showMessage(side == Color::White ? "White to move." : "Black to move.");

        IChessAgent* agentToMove =
            (side == Color::White ? whiteAgent : blackAgent);

        // -----------------------------------------------------
        // AGENT MOVE
        // -----------------------------------------------------
        if (agentToMove != nullptr)
        {
            MoveData mv = agentToMove->chooseMove(game);

            if (!game.applyMove(mv))
            {
                ui.showMessage("Agent attempted invalid move; terminating.");
                return 1;
            }

            ui.displayBoard(game.getBoard());
            continue;
        }

        // -----------------------------------------------------
        // HUMAN MOVE (unchanged)
        // -----------------------------------------------------
        std::string input = ui.getMoveInput();

        if (input == "quit" || input == "exit")
        {
            ui.showMessage("Exiting game...");
            return 0;
        }

        input.erase(0, input.find_first_not_of(" \t\r\n"));
        input.erase(input.find_last_not_of(" \t\r\n") + 1);

        if (input.empty())
        {
            ui.showMessage("Empty input. Try again.");
            continue;
        }

        MoveData mv = Notation::algebraicToMove(input, board, side);
        std::vector<MoveData> legal = game.getLegalMoves();

        bool found = false;
        for (const MoveData& m : legal)
        {
            if (m.fromRow == mv.fromRow &&
                m.fromCol == mv.fromCol &&
                m.toRow == mv.toRow &&
                m.toCol == mv.toCol)
            {
                if (input.size() >= 5)
                {
                    char promo = static_cast<char>(
                        std::toupper(static_cast<unsigned char>(input.back()))
                    );
                    if (promo=='Q' || promo=='R' || promo=='B' || promo=='N')
                        mv.promotion = promo;
                }

                found = true;
                break;
            }
        }

        if (!found)
        {
            ui.showMessage("Illegal move. Try again.");
            continue;
        }

        if (!game.applyMove(mv))
        {
            ui.showMessage("Move could not be applied.");
            continue;
        }

        ui.displayBoard(game.getBoard());
    }

    return 0;
}
