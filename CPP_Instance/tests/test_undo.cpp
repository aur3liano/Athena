#include "Catch2/catch_amalgamated.hpp"
#include "engine/board.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include "engine/FEN.h"

// ---------------------------------------------------------------------------
// BASIC UNDO: STATE RESTORATION
// ---------------------------------------------------------------------------
TEST_CASE("Undo restores position exactly for basic moves")
{
    Board b;
    b.resetToStartingPosition();
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(b);
    for (const MoveData& m : moves)
    {
        Board before = b;
        MoveState st;
        st.move = m;
        gen.applyMove(b, st);
        gen.undoMove(b, st);
        // board equality
        for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
        {
            REQUIRE(before.at(r,c).type  == b.at(r,c).type);
            REQUIRE(before.at(r,c).color == b.at(r,c).color);
        }
        // metadata equality
        REQUIRE(before.sideToMove() == b.sideToMove());
        REQUIRE(before.enPassantRow() == b.enPassantRow());
        REQUIRE(before.enPassantCol() == b.enPassantCol());
        REQUIRE(before.halfmoveClock() == b.halfmoveClock());
        REQUIRE(before.fullmoveNumber() == b.fullmoveNumber());
        // Castling rights
        const CastlingRights& crBefore = before.getCastlingRights();
        const CastlingRights& crAfter  = b.getCastlingRights();
        REQUIRE(crBefore.whiteKingSide  == crAfter.whiteKingSide);
        REQUIRE(crBefore.whiteQueenSide == crAfter.whiteQueenSide);
        REQUIRE(crBefore.blackKingSide  == crAfter.blackKingSide);
        REQUIRE(crBefore.blackQueenSide == crAfter.blackQueenSide);
    }
}

// ---------------------------------------------------------------------------
// EP UNDO
// ---------------------------------------------------------------------------
TEST_CASE("Undo handles en-passant correctly")
{
    Board b;
    MoveGenerator gen;
    FEN::loadFEN(b, "8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    MoveData ep{3,4, 2,3};
    ep.isEnPassant = true;
    Board before = b;
    MoveState st;
    st.move = ep;
    gen.applyMove(b, st);
    gen.undoMove(b, st);
    for (int r = 0; r < 8; r++)
    for (int c = 0; c < 8; c++)
    {
        REQUIRE(before.at(r,c).type  == b.at(r,c).type);
        REQUIRE(before.at(r,c).color == b.at(r,c).color);
    }
}

// ---------------------------------------------------------------------------
// CASTLING UNDO
// ---------------------------------------------------------------------------
TEST_CASE("Undo handles castling")
{
    Board b;
    MoveGenerator gen;
    FEN::loadFEN(b, "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    MoveData castle{7,4, 7,6};
    Board before = b;
    MoveState st;
    st.move = castle;
    gen.applyMove(b, st);
    gen.undoMove(b, st);
    for (int r = 0; r < 8; r++)
    for (int c = 0; c < 8; c++)
    {
        REQUIRE(before.at(r,c).type  == b.at(r,c).type);
        REQUIRE(before.at(r,c).color == b.at(r,c).color);
    }
}
