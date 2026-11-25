#include "Catch2/catch_amalgamated.hpp"
#include "engine/board.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include "engine/FEN.h"

// Helper: check if a move exists in a move list
static bool exists(const std::vector<MoveData>& mv,int fr, int fc, int tr, int tc,bool ep = false)
{
    for (auto& m : mv)
        if (m.fromRow == fr &&
            m.fromCol == fc &&
            m.toRow == tr &&
            m.toCol == tc &&
            m.isEnPassant == ep)
            return true;
    return false;
}

TEST_CASE("En Passant square created after double pawn push")
{
    Board b;
    // This FEN asserts a pre-existing EP square (from original test)
    FEN::loadFEN(b, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b - e3 0 1");
    REQUIRE(b.enPassantRow() == 5);
    REQUIRE(b.enPassantCol() == 4);
}

TEST_CASE("En Passant only legal immediately before any other move")
{
    Board b;
    MoveGenerator gen;
    FEN::loadFEN(b, "rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w - - 0 2");
    auto moves = gen.generateLegalMoves(b);
    REQUIRE(b.enPassantRow() == -1);
    REQUIRE(!exists(moves, 4,4, 5,5, true));
}

TEST_CASE("En Passant legal when correct position is set by FEN")
{
    Board b;
    MoveGenerator gen;
    FEN::loadFEN(b, "8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    auto moves = gen.generateLegalMoves(b);
    REQUIRE(exists(moves, 3,4, 2,3, true));
}

TEST_CASE("En Passant illegal if pawn is pinned")
{
    Board b;
    MoveGenerator gen;
    FEN::loadFEN(b, "4k3/8/4r3/3pP3/8/8/8/4K3 w - d6 0 1");
    auto moves = gen.generateLegalMoves(b);
    REQUIRE(!exists(moves, 4,4, 3,3, true));
}

TEST_CASE("EP forbidden when capturing exposes king to discovery")
{
    Board b;
    MoveGenerator gen;
    FEN::loadFEN(b, "4k3/8/4r3/3pP3/8/8/8/4K3 w - d6 0 1");
    auto moves = gen.generateLegalMoves(b);
    REQUIRE(!exists(moves, 4,4, 3,3, true));
}

TEST_CASE("EP capture undo restores position exactly")
{
    Board b;
    MoveGenerator gen;
    FEN::loadFEN(b, "8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    // Prepare EP capture move
    MoveData ep{3, 4, 2, 3};
    ep.isEnPassant = true;
    // Snapshot board
    Board before = b;
    // Apply + undo
    MoveState st;
    st.move = ep;
    gen.applyMove(b, st);
    gen.undoMove(b, st);
    // Compare all squares
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
        {
            REQUIRE(before.at(r,c).type  == b.at(r,c).type);
            REQUIRE(before.at(r,c).color == b.at(r,c).color);
        }
}
