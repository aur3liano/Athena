#include "Catch2/catch_amalgamated.hpp"
#include "engine/board.h"
#include "engine/move_generator.h"
#include "engine/FEN.h"

static constexpr Color W = Color::White;
static constexpr Color B = Color::Black;

// -----------------------------------------------------------------------------
// Knight Attack Tests
// -----------------------------------------------------------------------------
TEST_CASE("isSquareAttacked: knight attacks", "[attack][knight]")
{
    MoveGenerator gen;
    std::string fen = "8/5N2/8/8/8/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(gen.isSquareAttacked(b, 0,3, W));
    REQUIRE(gen.isSquareAttacked(b, 2,3, W));
    REQUIRE(gen.isSquareAttacked(b, 3,4, W));
    REQUIRE(gen.isSquareAttacked(b, 3,6, W));
    REQUIRE(gen.isSquareAttacked(b, 2,7, W));
    REQUIRE(gen.isSquareAttacked(b, 0,7, W));
}

TEST_CASE("isSquareAttacked: knight wrong color does not attack", "[attack][knight]")
{
    MoveGenerator gen;
    std::string fen = "8/5n2/8/8/8/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(!gen.isSquareAttacked(b, 0,3, W));
    REQUIRE(!gen.isSquareAttacked(b, 2,3, W));
    REQUIRE(!gen.isSquareAttacked(b, 3,4, W));
    REQUIRE(!gen.isSquareAttacked(b, 3,6, W));
    REQUIRE(!gen.isSquareAttacked(b, 2,7, W));
    REQUIRE(!gen.isSquareAttacked(b, 0,7, W));
}

// -----------------------------------------------------------------------------
// Pawn Attack Tests
// -----------------------------------------------------------------------------
TEST_CASE("isSquareAttacked: white pawn diagonal attacks", "[attack][pawn]")
{
    MoveGenerator gen;
    std::string fen = "8/8/8/3P1P2/8/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(gen.isSquareAttacked(b, 2,2, W));
    REQUIRE(gen.isSquareAttacked(b, 2,4, W));
    REQUIRE(gen.isSquareAttacked(b, 2,6, W));
    REQUIRE(!gen.isSquareAttacked(b, 4,2, W));
    REQUIRE(!gen.isSquareAttacked(b, 4,4, W));
    REQUIRE(!gen.isSquareAttacked(b, 4,6, W));
}

TEST_CASE("isSquareAttacked: black pawn diagonal attacks", "[attack][pawn]")
{
    MoveGenerator gen;
    std::string fen = "8/8/8/3p1p2/8/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(gen.isSquareAttacked(b, 4,2, B));
    REQUIRE(gen.isSquareAttacked(b, 4,4, B));
    REQUIRE(gen.isSquareAttacked(b, 4,6, B));
    REQUIRE(!gen.isSquareAttacked(b, 2,2, B));
    REQUIRE(!gen.isSquareAttacked(b, 2,4, B));
    REQUIRE(!gen.isSquareAttacked(b, 2,6, B));
}

TEST_CASE("isSquareAttacked: pawn does not attack forward", "[attack][pawn]")
{
    MoveGenerator gen;
    std::string fen = "8/8/8/4P3/8/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(!gen.isSquareAttacked(b, 2,4, W));
}

// -----------------------------------------------------------------------------
// King adjacency
// -----------------------------------------------------------------------------
TEST_CASE("isSquareAttacked: king adjacent squares", "[attack][king]")
{
    MoveGenerator gen;
    std::string fen = "8/8/8/4K3/8/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(gen.isSquareAttacked(b, 2,3, W));
    REQUIRE(gen.isSquareAttacked(b, 3,3, W));
    REQUIRE(gen.isSquareAttacked(b, 4,3, W));
    REQUIRE(gen.isSquareAttacked(b, 2,4, W));
    REQUIRE(gen.isSquareAttacked(b, 4,4, W));
    REQUIRE(gen.isSquareAttacked(b, 2,5, W));
    REQUIRE(gen.isSquareAttacked(b, 3,5, W));
    REQUIRE(gen.isSquareAttacked(b, 4,5, W));
}

// -----------------------------------------------------------------------------
// Sliding Pieces
// -----------------------------------------------------------------------------
TEST_CASE("isSquareAttacked: rook horizontal attack", "[attack][rook]")
{
    MoveGenerator gen;
    std::string fen = "8/8/8/R7/8/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(gen.isSquareAttacked(b, 0,0, W));
    REQUIRE(gen.isSquareAttacked(b, 7,0, W));
    REQUIRE(gen.isSquareAttacked(b, 3,4, W));
    REQUIRE(gen.isSquareAttacked(b, 3,7, W));
}

TEST_CASE("isSquareAttacked: queen diagonal attack", "[attack][queen]")
{
    MoveGenerator gen;
    std::string fen = "8/7Q/8/8/3Q4/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(gen.isSquareAttacked(b, 1,6, W));
}

// -----------------------------------------------------------------------------
// Ray obstruction
// -----------------------------------------------------------------------------
TEST_CASE("isSquareAttacked: ray blocked by own piece", "[attack][ray][block]")
{
    MoveGenerator gen;
    std::string fen = "8/8/8/R2P4/8/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(!gen.isSquareAttacked(b, 4,7, W));
}

TEST_CASE("isSquareAttacked: ray blocked by enemy piece", "[attack][ray][block]")
{
    MoveGenerator gen;
    std::string fen = "8/8/8/R2p4/8/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(!gen.isSquareAttacked(b, 4,7, W));
}

// -----------------------------------------------------------------------------
// Empty board
// -----------------------------------------------------------------------------
TEST_CASE("isSquareAttacked: empty board", "[attack][empty]")
{
    MoveGenerator gen;
    std::string fen = "8/8/8/8/8/8/8/8 w - - 0 1";
    Board b;
    REQUIRE(FEN::loadFEN(b, fen));
    REQUIRE(!gen.isSquareAttacked(b, 4,4, W));
}
