#include "tools/legal_move_FEN_analysis.h"
#include <iostream>
#include <string>

void print_help()
{
    std::cout
        << "\nLegal Move FEN Analysis Tool\n"
        << "Usage:\n"
        << "  legalmoves <fen> [--san]\n\n"
        << "Options:\n"
        << "  --san   Print SAN notation in addition to coordinate notation\n"
        << "  --help  Show this message\n\n";
}

int main(int argc, char* argv[])
{
    if (argc == 1 ||
        std::string(argv[1]) == "--help")
    {
        print_help();
        return 0;
    }
    bool useSAN = false;
    std::string fen = argv[1];
    // Accept multi-token FENs
    for (int i = 2; i < argc; i++)
    {
        std::string a = argv[i];
        if (a == "--san")
            useSAN = true;
        else
        {
            fen += " ";
            fen += a;
        }
    }
    int count = LegalMoveFENAnalysis::analyzeFEN(fen, useSAN);
    return (count < 0 ? 1 : 0);
}
