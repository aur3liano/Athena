#include "Catch2/catch_amalgamated.hpp"
#include "engine/board.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include "engine/FEN.h"

// Helper for detecting moves in a list
static bool containsMove(const std::vector<MoveData>& moves,
                         int fr, int fc, int tr, int tc)
{
    for (auto& m : moves)
        if (m.fromRow == fr &&
            m.fromCol == fc &&
            m.toRow == tr &&
            m.toCol == tc)
            return true;
    return false;
}

// ------------------------------------------------------------
// 1. KING SAFETY — PINNED PIECES
// ------------------------------------------------------------
TEST_CASE("King safety: pinned pawn cannot move", "[king][pin][safety]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3"));
    auto moves = gen.generateLegalMoves(board);
    // Construct illegal move by pinned pawn
    MoveData illegalMove{4,2, 5,2};
    MoveState st;
    st.move = illegalMove;
    REQUIRE(gen.leavesKingInCheck(board, st));
    REQUIRE(!containsMove(moves, 4,2, 5,2));
}

TEST_CASE("King safety: pinned knight cannot move", "[king][pin][safety]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/8/4k3/8/8/8/4R3/4N3 b - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    REQUIRE(!containsMove(moves, 7,4, 6,2));
}

TEST_CASE("King safety: pinned bishop cannot move", "[king][pin][safety]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "4k3/8/4b3/8/8/8/4R3/8 b - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    REQUIRE(!containsMove(moves, 2,4, 5,7));
    REQUIRE(!containsMove(moves, 2,4, 4,6));
    REQUIRE(!containsMove(moves, 2,4, 3,5));
    REQUIRE(!containsMove(moves, 2,4, 1,3));
    REQUIRE(!containsMove(moves, 2,4, 0,2));
}

TEST_CASE("King safety: pinned rook cannot move", "[king][pin][safety]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "4k3/8/8/8/8/8/4b3/4R3 w - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    REQUIRE(!containsMove(moves, 7,4, 1,4));
}

TEST_CASE("King safety: pinned queen cannot move", "[king][pin][safety]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "4k3/8/8/8/8/8/4r3/4Q3 w - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    REQUIRE(!containsMove(moves, 7,4, 1,4));
}

// ------------------------------------------------------------
// 2. ATTACK DETECTION — isSquareAttacked
// ------------------------------------------------------------
TEST_CASE("Attack detection: pawn attacks king squares", "[attack][pawn]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/8/8/3k4/3P4/8/8/4K3 b - - 0 1"));
    REQUIRE(gen.isSquareAttacked(board, 3,2, Color::White));
    REQUIRE(gen.isSquareAttacked(board, 3,4, Color::White));
}

TEST_CASE("Attack detection: bishop sliding blocked", "[attack][bishop]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3"));
    REQUIRE(!gen.isSquareAttacked(board, 3,3, Color::White));
}

TEST_CASE("Attack detection: rook line attack", "[attack][rook]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "4k3/8/8/8/8/8/4R3/4K3 w - - 0 1"));
    REQUIRE(gen.isSquareAttacked(board, 0,4, Color::White));
}

// ------------------------------------------------------------
// 3. leavesKingInCheck — direct validation
// ------------------------------------------------------------
TEST_CASE("leavesKingInCheck: pinned pawn detected","[king][safety][leavesKingInCheck]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3"));
    MoveData illegalMove{4,2, 5,2};
    MoveState st;
    st.move = illegalMove;
    REQUIRE(gen.leavesKingInCheck(board, st));
}

TEST_CASE("leavesKingInCheck: EP capture does NOT expose king","[ep][safety]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/8/8/5k2/2pPp3/8/B7/4K3 b - d3 0 1"));
    MoveData ep{4,4, 5,3};
    ep.isEnPassant = true;
    MoveState st;
    st.move = ep;
    REQUIRE(!gen.leavesKingInCheck(board, st));
}

// ------------------------------------------------------------
// 4. Discovered check sanity
// ------------------------------------------------------------
TEST_CASE("Discovered check detection sanity","[attack][discover]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "4k3/8/8/4r3/4B3/8/8/4K3 w - - 0 1"));
    MoveData m{4,4, 3,3};
    MoveState st;
    st.move = m;
    REQUIRE(gen.leavesKingInCheck(board, st));
}
