#pragma once
#include <string>
#include "agents/IChessAgent.h"

class AgentTournament
{
public:
    AgentTournament() = default;

    // Run a full match with a specified EVEN number of games.
    void runMatch(int agentA_id, int agentB_id, int totalGames);

private:
    // Factory
    IChessAgent* createAgent(int id, int depth = -1);
    std::string getAgentName(int id);

    // Runs a single headless game (white vs black).
    // Returns the engine's result message.
    std::string runSingleGame(IChessAgent* white, IChessAgent* black,
                            double& whiteTime, double& blackTime,
                            int& whitePlies, int& blackPlies);

    // Summary structure
    struct MatchStats
    {
        // Wins
        int A_white_wins = 0;
        int A_black_wins = 0;
        int B_white_wins = 0;
        int B_black_wins = 0;

        // Plies
        int A_white_plies = 0;
        int A_black_plies = 0;
        int B_white_plies = 0;
        int B_black_plies = 0;

        // Draw types
        int stalemate = 0;
        int drawFiftyMove = 0;
        int drawThreefold = 0;
        int drawDeadPosition = 0;

        // Time totals (seconds)
        double A_white_time = 0.0;
        double A_black_time = 0.0;
        double B_white_time = 0.0;
        double B_black_time = 0.0;

        int totalPlies() const {
            return A_white_plies + A_black_plies + B_white_plies + B_black_plies;
        }
        
        int totalDraws() const {
            return stalemate + drawFiftyMove + drawThreefold + drawDeadPosition;
        }
    };
};
