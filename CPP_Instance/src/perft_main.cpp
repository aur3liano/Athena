#include "tools/perft.h"
#include "engine/FEN.h"
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>

static void print_help()
{
    std::cout <<
        "\nChess Engine Perft Tool\n"
        "Usage:\n"
        "  perft <depth> [fen] [options]\n\n"
        "Options:\n"
        "  --divide   Per-move breakdown\n"
        "  --san      Use SAN instead of coordinate notation\n"
        "  --stats    Enable full perft statistics\n"
        "  --help     Show this message\n\n";
}

int main(int argc, char** argv)
{
    if (argc == 1 ||
        std::string(argv[1]) == "--help")
    {
        print_help();
        return 0;
    }

    int depth = std::max(0, std::stoi(argv[1]));

    std::string fen = "startpos";
    if (argc >= 3)
        fen = argv[2];

    bool divide = false;
    bool useSAN = false;
    bool stats = false;

    for (int i = 3; i < argc; i++)
    {
        std::string a = argv[i];
        if (a == "--divide") divide = true;
        else if (a == "--san") useSAN = true;
        else if (a == "--stats") stats = true;
    }

    Board board;
    if (fen == "startpos")
        board.resetToStartingPosition();
    else if (!FEN::loadFEN(board, fen))
    {
        std::cerr << "Invalid FEN\n";
        return 1;
    }

    auto start = std::chrono::steady_clock::now();

    if (divide)
    {
        perft_divide(board, depth, useSAN, stats);
    }
    else
    {
        PerftStats s;
        perft(board, depth, s, stats);

        std::cout << "Perft(" << depth << ") = " << s.nodes << "\n";

        if (stats)
        {
            std::cout
                << "Captures: " << s.captures << "\n"
                << "En Passant: " << s.enPassant << "\n"
                << "Castles: " << s.castles << "\n"
                << "Promotions: " << s.promotions << "\n"
                << "Checks: " << s.checks << "\n"
                << "Discovered Checks: " << s.discoveredChecks << "\n"
                << "Double Checks: " << s.doubleChecks << "\n"
                << "Checkmates: " << s.checkmates << "\n";
        }
    }

    auto end = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "Time: " << ms << " ms\n";

    return 0;
}
