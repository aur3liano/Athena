#include "Catch2/catch_amalgamated.hpp"
#include "engine/board.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include "engine/FEN.h"

// ============================================================================
// Helper Utilities (consistent with style in test_promotion.cpp)
// ============================================================================
// Check if a move exists in a generated move list
static bool moveExists(const std::vector<MoveData>& moves,
                       int fr, int fc, int tr, int tc)
{
    for (const auto& m : moves)
        if (m.fromRow == fr && m.fromCol == fc &&
            m.toRow == tr && m.toCol == tc)
            return true;
    return false;
}

// Apply a single move (forward) using correct engine API
static void applyForward(Board& board, MoveGenerator& gen, const MoveData& m)
{
    MoveState st;
    st.move = m;
    gen.applyMove(board, st);
}

// Compare castling rights between boards
static void requireCastlingEqual(const Board& b1, const Board& b2)
{
    const auto& c1 = b1.getCastlingRights();
    const auto& c2 = b2.getCastlingRights();

    REQUIRE(c1.whiteKingSide  == c2.whiteKingSide);
    REQUIRE(c1.whiteQueenSide == c2.whiteQueenSide);
    REQUIRE(c1.blackKingSide  == c2.blackKingSide);
    REQUIRE(c1.blackQueenSide == c2.blackQueenSide);
}

// Compare full board equality
static void requireBoardEqual(const Board& before, const Board& after)
{
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
        {
            REQUIRE(before.at(r,c).type  == after.at(r,c).type);
            REQUIRE(before.at(r,c).color == after.at(r,c).color);
        }

    REQUIRE(before.sideToMove()     == after.sideToMove());
    REQUIRE(before.enPassantRow()   == after.enPassantRow());
    REQUIRE(before.enPassantCol()   == after.enPassantCol());
    REQUIRE(before.halfmoveClock()  == after.halfmoveClock());
    REQUIRE(before.fullmoveNumber() == after.fullmoveNumber());

    requireCastlingEqual(before, after);
}

// ============================================================================
// SECTION 1 — BASIC CASTLING BEHAVIOR (Clean starting FENs)
// ============================================================================
TEST_CASE("Castling available in clean FEN")
{
    MoveGenerator gen;
    Board b;

    // White
    FEN::loadFEN(b, "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(moveExists(moves, 7,4, 7,6)); // White O-O
    REQUIRE(moveExists(moves, 7,4, 7,2)); // White O-O-O

    // Black
    FEN::loadFEN(b, "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
    moves = gen.generateLegalMoves(b);

    REQUIRE(moveExists(moves, 0,4, 0,6)); // Black O-O
    REQUIRE(moveExists(moves, 0,4, 0,2)); // Black O-O-O
}

TEST_CASE("Castling blocked path is illegal")
{
    MoveGenerator gen;
    Board b;

    // Queen on d1 blocks O-O-O
    FEN::loadFEN(b, "r3k2r/8/8/8/8/8/8/R2QK2R w KQkq - 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(!moveExists(moves, 7,4, 7,2)); // Blocked O-O-O
}

TEST_CASE("Castling illegal if king passes through attack")
{
    MoveGenerator gen;
    Board b;

    // Queen attacking squares the king must pass through
    FEN::loadFEN(b, "r3k2r/8/8/8/8/8/3q4/R3K2R w KQkq - 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(!moveExists(moves, 7,4, 7,2)); // No O-O-O
    REQUIRE(!moveExists(moves, 7,4, 7,6)); // No O-O
}

// ============================================================================
// SECTION 2 — CASTLING RIGHTS REMOVAL WHEN ROOKS MOVE
// ============================================================================
TEST_CASE("Castling illegal after all rooks have moved")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

    // White kingside rook moves forth and back
    {
        MoveData m1{7,7,7,5};
        applyForward(b, gen, m1);

        MoveData m2{7,5,7,7};
        applyForward(b, gen, m2);
    }

    // White queenside rook moves forth and back
    {
        MoveData m1{7,0,7,2};
        applyForward(b, gen, m1);

        MoveData m2{7,2,7,0};
        applyForward(b, gen, m2);
    }

    // Black kingside rook moves forth and back
    {
        MoveData m1{0,7,0,5};
        applyForward(b, gen, m1);

        MoveData m2{0,5,0,7};
        applyForward(b, gen, m2);
    }

    // Black queenside rook moves forth and back
    {
        MoveData m1{0,0,0,2};
        applyForward(b, gen, m1);

        MoveData m2{0,2,0,0};
        applyForward(b, gen, m2);
    }

    auto moves = gen.generateLegalMoves(b);

    REQUIRE(!moveExists(moves, 7,4, 7,6)); // White O-O
    REQUIRE(!moveExists(moves, 7,4, 7,2)); // White O-O-O
    REQUIRE(!moveExists(moves, 0,4, 0,6)); // Black O-O
    REQUIRE(!moveExists(moves, 0,4, 0,2)); // Black O-O-O
}

// ============================================================================
// SECTION 3 — CASTLING RIGHTS REMOVAL WHEN KING MOVES
// ============================================================================
TEST_CASE("Castling illegal after king moves")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

    // White king moves forth and back (forward-forward semantics)
    {
        MoveData m1{7,4,7,5};
        applyForward(b, gen, m1);

        MoveData m2{7,5,7,4};
        applyForward(b, gen, m2);
    }

    // Black king moves forth and back
    {
        MoveData m1{0,4,0,5};
        applyForward(b, gen, m1);

        MoveData m2{0,5,0,4};
        applyForward(b, gen, m2);
    }

    auto moves = gen.generateLegalMoves(b);

    REQUIRE(!moveExists(moves, 7,4, 7,6)); // White O-O
    REQUIRE(!moveExists(moves, 7,4, 7,2)); // White O-O-O
    REQUIRE(!moveExists(moves, 0,4, 0,6)); // Black O-O
    REQUIRE(!moveExists(moves, 0,4, 0,2)); // Black O-O-O
}

// ============================================================================
// SECTION 4 — CASTLING UNDO CONSISTENCY
// ============================================================================
TEST_CASE("Castling undo restores original board")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    Board before = b;

    MoveData castle{7,4,7,6};
    MoveState st;
    st.move = castle;

    gen.applyMove(b, st);
    gen.undoMove(b, st);

    requireBoardEqual(before, b);
}

// ============================================================================
// SECTION 5 — ATTACK‑RELATED CASTLING RESTRICTIONS
// ============================================================================
TEST_CASE("Castling forbidden when king is in check")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "r3k2r/8/8/8/3q4/8/8/R3K2R w KQkq - 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(!moveExists(moves, 7,4, 7,6)); // O-O
    REQUIRE(!moveExists(moves, 7,4, 7,2)); // O-O-O
}

TEST_CASE("Castling forbidden when intermediate square is attacked")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "r3k2r/8/8/8/8/8/3q4/R3K2R w KQkq - 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(!moveExists(moves, 7,4, 7,2));
    REQUIRE(!moveExists(moves, 7,4, 7,6));
}

TEST_CASE("Castling forbidden when destination square is attacked")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "r3k2r/8/8/8/5q2/8/8/R3K2R w KQkq - 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(!moveExists(moves, 7,4, 7,2));
    REQUIRE(!moveExists(moves, 7,4, 7,6));
}

// ============================================================================
// SECTION 6 — CASTLING STRESS TEST FAMILY
// ============================================================================
// 1. Minimal empty‑board queenside castling
TEST_CASE("Castling stress: empty board with only king + rook (white queenside)")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "8/8/8/8/8/8/8/R3K3 w Q - 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(moveExists(moves, 7,4, 7,2)); // Should be legal
}

// 2. Minimal empty‑board kingside castling
TEST_CASE("Castling stress: empty board with only king + rook (white kingside)")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "8/8/8/8/8/8/8/4K2R w K - 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(moveExists(moves, 7,4, 7,6));
}

// 3. Black queenside empty‑board
TEST_CASE("Castling stress: empty board with only king + rook (black queenside)")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "r3k3/8/8/8/8/8/8/8 b q - 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(moveExists(moves, 0,4, 0,2));
}

// 4. Black kingside empty‑board
TEST_CASE("Castling stress: empty board with only king + rook (black kingside)")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "4k2r/8/8/8/8/8/8/8 b k - 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(moveExists(moves, 0,4, 0,6));
}

// 5. Rook moves once → castling lost
TEST_CASE("Castling stress: rook moved once, rights removed")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "8/8/8/8/8/8/8/R3K3 w Q - 0 1");

    MoveData m1{7,0,7,1};
    applyForward(b, gen, m1);

    auto moves = gen.generateLegalMoves(b);
    REQUIRE(!moveExists(moves, 7,4, 7,2)); // No O-O-O
}

// 6. King moves once → castling lost
TEST_CASE("Castling stress: king moved once, rights removed")
{
    MoveGenerator gen;
    Board b;

    FEN::loadFEN(b, "8/8/8/8/8/8/8/R3K3 w Q - 0 1");

    MoveData m1{7,4,7,5};
    applyForward(b, gen, m1);

    auto moves = gen.generateLegalMoves(b);
    REQUIRE(!moveExists(moves, 7,4, 7,2)); // No O-O-O
}

// 7. EP rights do not interfere with castling logic
TEST_CASE("Castling stress: EP square irrelevant to castling")
{
    MoveGenerator gen;
    Board b;

    // White EP square artificially set
    FEN::loadFEN(b, "8/8/8/8/8/8/p7/R3K3 w Q a6 0 1");
    auto moves = gen.generateLegalMoves(b);

    REQUIRE(moveExists(moves, 7,4, 7,2)); // Still legal
}

// 8. Castling remains legal after non‑involving minor moves
TEST_CASE("Castling stress: unrelated moves preserve castling rights")
{
    MoveGenerator gen;
    Board b;
    // Add a knight far away; white has Q-side castling
    FEN::loadFEN(b, "8/3k4/8/8/8/4N3/8/R3K3 w Q - 0 1");
    // 1. White moves knight
    MoveData whiteMove{5,4,3,5}; // Ne3->c4 pattern
    applyForward(b, gen, whiteMove);
    // 2. Black must move before testing white's castling rights
    auto blackMoves = gen.generateLegalMoves(b);
    REQUIRE(!blackMoves.empty());
    // Use the first legal black move
    applyForward(b, gen, blackMoves[0]);
    // 3. Now it is White's turn again — check castling
    auto whiteMoves = gen.generateLegalMoves(b);
    REQUIRE(moveExists(whiteMoves, 7,4, 7,2)); // O-O-O still legal
}
