#include "Catch2/catch_amalgamated.hpp"
#include "engine/board.h"

TEST_CASE("Board initializes correctly")
{
    Board b;
    b.resetToStartingPosition();

    REQUIRE(b.at(7, 4).type == PieceType::King);
    REQUIRE(b.at(7, 4).color == Color::White);

    REQUIRE(b.at(0, 4).type == PieceType::King);
    REQUIRE(b.at(0, 4).color == Color::Black);

    REQUIRE(b.sideToMove() == Color::White);
}