#include "Catch2/catch_amalgamated.hpp"
#include "engine/board.h"
#include "engine/FEN.h"

TEST_CASE("FEN loads starting position")
{
    Board b;
    bool ok = FEN::loadFEN(
        b,
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    );
    REQUIRE(ok == true);
    REQUIRE(b.sideToMove() == Color::White);
    const CastlingRights& cr = b.getCastlingRights();
    REQUIRE(cr.whiteKingSide  == true);
    REQUIRE(cr.whiteQueenSide == true);
    REQUIRE(cr.blackKingSide  == true);
    REQUIRE(cr.blackQueenSide == true);
    REQUIRE(b.at(7,0).type  == PieceType::Rook);
    REQUIRE(b.at(7,0).color == Color::White);
}
