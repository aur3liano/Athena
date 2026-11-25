#pragma once
#include <string>
#include <vector>
#include "agents/IChessAgent.h"

class AgentTournamentV2
{
public:
    AgentTournamentV2();

    // Run a match across N opening positions (FENs).
    // Each opening is played twice (A white, B white).
    void runMatch(int agentA_id, int agentB_id, int openingsToUse);

private:
    // Factory (same as V1)
    IChessAgent* createAgent(int id, int depth = -1);
    std::string getAgentName(int id);

    // Run a game starting from a specific FEN
    std::string runSingleGameFromFEN(
        const std::string& fen,
        IChessAgent* white,
        IChessAgent* black,
        double& whiteTime,
        double& blackTime,
        int& whitePlies,
        int& blackPlies);

    // Opening list
    std::vector<std::string> openingFENs;

    void printSecondsPerPly(const std::string& label, double time, int plies)

    // Stats
    struct MatchStats
    {
        int A_white_wins = 0;
        int A_black_wins = 0;
        int B_white_wins = 0;
        int B_black_wins = 0;

        // Plies
        int A_white_plies = 0;
        int A_black_plies = 0;
        int B_white_plies = 0;
        int B_black_plies = 0;

        int stalemate = 0;
        int draw50 = 0;
        int draw3fold = 0;
        int drawInsufficient = 0;

        double A_white_time = 0.0;
        double A_black_time = 0.0;
        double B_white_time = 0.0;
        double B_black_time = 0.0;

        int totalPlies() const {
            return A_white_plies + A_black_plies + B_white_plies + B_black_plies;
        }
        
        int totalDraws() const {
            return stalemate + draw50 + draw3fold + drawInsufficient;
        }
    };
};
