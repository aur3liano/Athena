#include "tournament/agent_tournament.h"
#include <iostream>
#include <memory>
#include <chrono>

// Engine
#include "engine/game.h"

// Agents
#include "agents/Agent_01.h"
#include "agents/Agent_02.h"
#include "agents/Agent_03.h"
#include "agents/Agent_04.h"
#include "agents/Agent_05.h"
#include "agents/Agent_06.h"

// =====================================================================
// Agent Factory
// =====================================================================
IChessAgent* AgentTournament::createAgent(int id, int depth)
{
    switch (id)
    {
        case 1: return new Agent_01();
        case 2: return new Agent_02(depth);
        case 3: return new Agent_03(depth);
        case 4: return new Agent_04(depth);
        case 5: return new Agent_05(depth);
        case 6: return new Agent_06(depth);
    }
    return nullptr;
}

std::string AgentTournament::getAgentName(int id)
{
    switch (id)
    {
        case 1: return "Agent_01";
        case 2: return "Agent_02";
        case 3: return "Agent_03";
        case 4: return "Agent_04";
        case 5: return "Agent_05";
        case 6: return "Agent_06";
    }
    return "UnknownAgent";
}

// =====================================================================
// Run a single agent-vs-agent game AND measure per-side total time
// =====================================================================
std::string AgentTournament::runSingleGame(
    IChessAgent* white,
    IChessAgent* black,
    double& whiteTime,
    double& blackTime,
    int& whitePlies,
    int& blackPlies)
{
    Game game;
    using clock = std::chrono::high_resolution_clock;

    whitePlies = 0;
    blackPlies = 0;

    while (!game.isGameOver())
    {
        Color side = game.getBoard().sideToMove();
        IChessAgent* agent = (side == Color::White ? white : black);

        auto start = clock::now();
        MoveData mv = agent->chooseMove(game);
        auto end = clock::now();

        double sec = std::chrono::duration<double>(end - start).count();

        if (side == Color::White)
        {
            whiteTime += sec;
            whitePlies++;      // ← NEW: Only increment white’s plies
        }
        else
        {
            blackTime += sec;
            blackPlies++;      // ← NEW: Only increment black’s plies
        }

        game.applyMove(mv);
    }

    return game.getGameResultMessage();
}

void printSecondsPerPly(const std::string& label, double time, int plies)
{
    if (plies > 0)
        std::cout << " Seconds per Ply (" << label << "): "
                  << (time / plies) << " s\n";
    else
        std::cout << " Seconds per Ply (" << label << "): n/a (0 plies)\n";
}

// =====================================================================
// Run a full symmetric match with custom game count
// =====================================================================
void AgentTournament::runMatch(int agentA_id, int agentB_id, int totalGames)
{
    if (totalGames % 2 != 0)
    {
        std::cout << "ERROR: totalGames must be even.\n";
        return;
    }

    int half = totalGames / 2;

    std::string nameA = getAgentName(agentA_id);
    std::string nameB = getAgentName(agentB_id);

    int depthA = -1;
    int depthB = -1;
    int wPlies = 0;
    int bPlies = 0;

    if (agentA_id >= 2) {
        std::cout << "Select depth for Agent " << nameA << ": ";
        std::cin >> depthA;
    }
    if (agentB_id >= 2) {
        std::cout << "Select depth for Agent " << nameB << ": ";
        std::cin >> depthB;
    }

    std::unique_ptr<IChessAgent> agentA(createAgent(agentA_id, depthA));
    std::unique_ptr<IChessAgent> agentB(createAgent(agentB_id, depthB));

    MatchStats stats;

    std::cout << "\n======================================\n";
    std::cout << " TOURNAMENT: " << nameA << " vs " << nameB << "\n";
    std::cout << " Games: " << totalGames << "\n";
    std::cout << "======================================\n\n";

    // =============================================================
    // FIRST HALF: A plays White, B plays Black
    // =============================================================
    for (int i = 0; i < half; i++)
    {
        double whiteTime = 0.0, blackTime = 0.0;

        std::string result = runSingleGame(agentA.get(), agentB.get(),
                                           whiteTime, blackTime, wPlies, bPlies);

        std::cout << "Game " << (i + 1)
                  << ": " << nameA << " (White) vs "
                  << nameB << " (Black) results in " << result << "\n";

        std::cout << "   Time: White=" << whiteTime
                << "s, Black=" << blackTime
                << "s, Plies(W)=" << wPlies
                << ", Plies(B)=" << bPlies << "\n";

        stats.A_white_plies += wPlies;
        stats.B_black_plies += bPlies;

        stats.A_white_time += whiteTime;
        stats.B_black_time += blackTime;


        // --- Classify result ---
        if (result.find("White wins") != std::string::npos)
            stats.A_white_wins++;
        else if (result.find("Black wins") != std::string::npos)
            stats.B_black_wins++;
        else if (result.find("Stalemate") != std::string::npos)
            stats.stalemate++;
        else if (result.find("50-move") != std::string::npos)
            stats.drawFiftyMove++;
        else if (result.find("threefold") != std::string::npos)
            stats.drawThreefold++;
        else if (result.find("insufficient") != std::string::npos)
            stats.drawDeadPosition++;
    }

    // =============================================================
    // SECOND HALF: B plays White, A plays Black
    // =============================================================
    for (int i = 0; i < half; i++)
    {
        double whiteTime = 0.0, blackTime = 0.0;

        std::string result = runSingleGame(agentA.get(), agentB.get(),
                                           whiteTime, blackTime, wPlies, bPlies);

        std::cout << "Game " << (i + 1 + half)
                  << ": " << nameB << " (White) vs "
                  << nameA << " (Black) results in " << result << "\n";

        std::cout << "   Time: White=" << whiteTime
                << "s, Black=" << blackTime
                << "s, Plies(W)=" << wPlies
                << ", Plies(B)=" << bPlies << "\n";

        stats.B_white_plies += wPlies;
        stats.A_black_plies += bPlies;

        stats.B_white_time += whiteTime;
        stats.A_black_time += blackTime;

        // --- Classify result ---
        if (result.find("White wins") != std::string::npos)
            stats.B_white_wins++;
        else if (result.find("Black wins") != std::string::npos)
            stats.A_black_wins++;
        else if (result.find("Stalemate") != std::string::npos)
            stats.stalemate++;
        else if (result.find("50-move") != std::string::npos)
            stats.drawFiftyMove++;
        else if (result.find("threefold") != std::string::npos)
            stats.drawThreefold++;
        else if (result.find("insufficient") != std::string::npos)
            stats.drawDeadPosition++;
    }

    // =====================================================================
    // MATCH SUMMARY
    // =====================================================================
    std::cout << "\n======================================\n";
    std::cout << " TOURNAMENT SUMMARY\n";
    std::cout << "======================================\n\n";

    std::cout << nameA << ":\n";
    std::cout << " Wins as White: " << stats.A_white_wins << "\n";
    std::cout << " Wins as Black: " << stats.A_black_wins << "\n";
    std::cout << " Total Time (White): " << stats.A_white_time << " s\n";
    std::cout << " Total Time (Black): " << stats.A_black_time << " s\n";
    printSecondsPerPly("White", stats.A_white_time, stats.A_white_plies);
    printSecondsPerPly("Black", stats.A_black_time, stats.A_black_plies);
    std::cout << "\n";

    std::cout << nameB << ":\n";
    std::cout << " Wins as White: " << stats.B_white_wins << "\n";
    std::cout << " Wins as Black: " << stats.B_black_wins << "\n";
    std::cout << " Total Time (White): " << stats.B_white_time << " s\n";
    std::cout << " Total Time (Black): " << stats.B_black_time << " s\n";
    printSecondsPerPly("White", stats.B_white_time, stats.B_white_plies);
    printSecondsPerPly("Black", stats.B_black_time, stats.B_black_plies);
    std::cout << "\n";

    std::cout << "Draw statistics:\n";
    std::cout << "  Stalemates:            " << stats.stalemate << "\n";
    std::cout << "  50-move rule draws:    " << stats.drawFiftyMove << "\n";
    std::cout << "  Threefold repetition:  " << stats.drawThreefold << "\n";
    std::cout << "  Insufficient material: " << stats.drawDeadPosition << "\n\n";

    std::cout << "Average Plies per game: "
            << (stats.totalPlies() / static_cast<double>(totalGames))
            << "\n\n";

    std::cout << "======================================\n";
    std::cout << " FINAL SCOREBOARD\n";
    std::cout << "======================================\n";

    int totalA = stats.A_white_wins + stats.A_black_wins;
    int totalB = stats.B_white_wins + stats.B_black_wins;

    std::cout << " " << nameA << ": " << totalA << "\n";
    std::cout << " " << nameB << ": " << totalB << "\n";

    std::cout << "======================================\n\n";
}

int main()
{
    AgentTournament tournament;

    std::cout << "==============================\n";
    std::cout << "        ENGINE TOURNAMENT\n";
    std::cout << "==============================\n\n";

    std::cout << "Available Agents:\n";
    std::cout << "1. Agent_01\n";
    std::cout << "2. Agent_02\n";
    std::cout << "3. Agent_03\n";
    std::cout << "4. Agent_04\n";
    std::cout << "5. Agent_05\n";
    std::cout << "6. Agent_06\n";
    std::cout << "\n";

    int a, b, games;
    std::cout << "Select Agent A: ";
    std::cin >> a;

    std::cout << "Select Agent B: ";
    std::cin >> b;

    std::cout << "Enter total number of games (EVEN number): ";
    std::cin >> games;

    tournament.runMatch(a, b, games);

    return 0;
}
