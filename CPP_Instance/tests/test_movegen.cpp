#include "Catch2/catch_amalgamated.hpp"
#include "engine/board.h"
#include "engine/move_generator.h"

TEST_CASE("MoveGenerator produces legal moves from initial position")
{
    Board b;
    MoveGenerator gen;

    auto moves = gen.generateLegalMoves(b);

    // Exactly 20 legal moves in a standard starting position
    REQUIRE(moves.size() == 20);

    bool foundE2E4 = false;

    for (auto& m : moves)
    {
        if (m.fromRow == 6 && m.fromCol == 4)
        if (m.toRow == 4 && m.toCol == 4)
            foundE2E4 = true;
    }

    REQUIRE(foundE2E4 == true);
}
