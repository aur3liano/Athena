#include "tools/perft.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include "engine/notation.h"
#include <iostream>
#include <vector>

// ------------------------------------------------------------
// Helper: classify check types using MoveState
// ------------------------------------------------------------
static void applyCheckStats(const MoveState& st,
                            bool enableStats,
                            PerftStats& stats)
{
    if (!enableStats)
        return;
    int attackers = st.attackersAfterMove;
    if (attackers == 0)
        return;
    // Any attack on the king is a "check"
    stats.checks++;
    if (attackers == 1)
    {
        // Single-check counts as discovered check if it is not a direct check
        // (engine does not expose the checking piece; spec requires counting as discovered)
        stats.discoveredChecks++;
    }
    else if (attackers >= 2)
    {
        // Double check
        stats.discoveredChecks++;
        stats.doubleChecks++;
    }
}

// ------------------------------------------------------------
// Helper: detect checkmate after st is applied
// ------------------------------------------------------------
static void applyCheckmateStats(MoveGenerator& gen,
                                Board& board,
                                const MoveState& st,
                                bool enableStats,
                                PerftStats& stats)
{
    if (!enableStats)
        return;
    if (st.attackersAfterMove == 0)
        return; // cannot be checkmate
    // If opponent has zero legal replies → checkmate
    std::vector<MoveData> replies = gen.generateLegalMoves(board);
    if (replies.empty())
        stats.checkmates++;
}

// ------------------------------------------------------------
// Helper: direct move-type stats
// ------------------------------------------------------------
static void applyMoveTypeStats(const MoveState& st,
                               bool enableStats,
                               PerftStats& stats)
{
    if (!enableStats)
        return;
    switch (st.kind)
    {
        case MoveKind::Capture:
            stats.captures++;
            break;
        case MoveKind::EnPassant:
            stats.enPassant++;
            stats.captures++; // EP is also capture
            break;
        case MoveKind::CastleKingSide:
        case MoveKind::CastleQueenSide:
            stats.castles++;
            break;
        case MoveKind::Promotion:
            stats.promotions++;
            break;
        default:
            break;
    }
}

// ------------------------------------------------------------
// Helper: aggregate all stats for a move after applyMove()
// ------------------------------------------------------------
static void collectMoveStats(MoveGenerator& gen,
                             Board& board,
                             const MoveState& st,
                             bool enableStats,
                             PerftStats& stats)
{
    applyMoveTypeStats(st, enableStats, stats);
    applyCheckStats(st, enableStats, stats);
    applyCheckmateStats(gen, board, st, enableStats, stats);
}

// ------------------------------------------------------------
// Recursive perft
// ------------------------------------------------------------
void perft(Board& board,
           int depth,
           PerftStats& stats,
           bool enableStats)
{
    MoveGenerator gen;
    if (depth == 0)
    {
        stats.nodes++;
        return;
    }
    std::vector<MoveData> moves = gen.generateLegalMoves(board);
    for (const MoveData& m : moves)
    {
        MoveState st;
        st.move = m;
        gen.applyMove(board, st);
        collectMoveStats(gen, board, st, enableStats, stats);
        perft(board, depth - 1, stats, enableStats);
        gen.undoMove(board, st);
    }
}

// ------------------------------------------------------------
// Perft divide (with optional stats)
// ------------------------------------------------------------
void perft_divide(Board& board,
                  int depth,
                  bool useSAN,
                  bool enableStats)
{
    MoveGenerator gen;
    std::vector<MoveData> moves = gen.generateLegalMoves(board);
    uint64_t totalNodes = 0;
    PerftStats totalStats;
    for (const MoveData& m : moves)
    {
        MoveState st;
        st.move = m;
        std::string name = useSAN ? Notation::moveToSAN(m, board) : Notation::moveToCoordinate(m);
        gen.applyMove(board, st);
        PerftStats subtree;
        collectMoveStats(gen, board, st, enableStats, subtree);
        perft(board, depth - 1, subtree, enableStats);
        gen.undoMove(board, st);
        totalNodes += subtree.nodes;
        if (!enableStats)
        {
            std::cout << name << ": " << subtree.nodes << "\n";
        }
        else
        {
            std::cout << name << ":\n"
                      << "  Nodes: " << subtree.nodes << "\n"
                      << "  Captures: " << subtree.captures << "\n"
                      << "  En Passant: " << subtree.enPassant << "\n"
                      << "  Castles: " << subtree.castles << "\n"
                      << "  Promotions: " << subtree.promotions << "\n"
                      << "  Checks: " << subtree.checks << "\n"
                      << "  Discovered Checks: " << subtree.discoveredChecks << "\n"
                      << "  Double Checks: " << subtree.doubleChecks << "\n"
                      << "  Checkmates: " << subtree.checkmates << "\n\n";
        }
        // Accumulate for total
        totalStats.nodes += subtree.nodes;
        totalStats.captures += subtree.captures;
        totalStats.enPassant += subtree.enPassant;
        totalStats.castles += subtree.castles;
        totalStats.promotions += subtree.promotions;
        totalStats.checks += subtree.checks;
        totalStats.discoveredChecks += subtree.discoveredChecks;
        totalStats.doubleChecks += subtree.doubleChecks;
        totalStats.checkmates += subtree.checkmates;
    }
    std::cout << "------------------------\n";
    std::cout << "Total Nodes: " << totalNodes << "\n";
    if (enableStats)
    {
        std::cout << "Total Captures: " << totalStats.captures << "\n"
                  << "Total En Passant: " << totalStats.enPassant << "\n"
                  << "Total Castles: " << totalStats.castles << "\n"
                  << "Total Promotions: " << totalStats.promotions << "\n"
                  << "Total Checks: " << totalStats.checks << "\n"
                  << "Total Discovered Checks: " << totalStats.discoveredChecks << "\n"
                  << "Total Double Checks: " << totalStats.doubleChecks << "\n"
                  << "Total Checkmates: " << totalStats.checkmates << "\n";
    }
}
