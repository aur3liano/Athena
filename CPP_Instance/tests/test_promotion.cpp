#include "Catch2/catch_amalgamated.hpp"
#include "engine/board.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include "engine/FEN.h"

// ============================================================================
// Helper utilities for promotion testing
// ============================================================================
namespace {

// Check that (fr,fc)->(tr,tc) appears in the move list as a promotion
bool containsPromotion(const std::vector<MoveData>& moves,
                       int fr, int fc, int tr, int tc)
{
    for (const auto& m : moves)
        if (m.fromRow == fr && m.fromCol == fc &&
            m.toRow == tr && m.toCol == tc &&
            m.promotion != 0)
            return true;
    return false;
}

// Count promotion moves from (fr,fc) -> (tr,tc)
int countPromotions(const std::vector<MoveData>& moves,
                    int fr, int fc, int tr, int tc)
{
    int count = 0;
    for (const auto& m : moves)
        if (m.fromRow == fr && m.fromCol == fc &&
            m.toRow == tr && m.toCol == tc &&
            m.promotion != 0)
            count++;
    return count;
}

// Apply + undo a move using the canonical engine pattern.
// Matches test_undo.cpp style.
void applyUndo(MoveGenerator& gen, Board& board, const MoveData& m)
{
    MoveState st;
    st.move = m;
    gen.applyMove(board, st);
    gen.undoMove(board, st);
}

// Compare every meaningful piece & metadata field for equality.
void requireBoardEqual(const Board& before, const Board& after)
{
    // Board squares
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
        {
            REQUIRE(before.at(r,c).type  == after.at(r,c).type);
            REQUIRE(before.at(r,c).color == after.at(r,c).color);
        }
    // Metadata
    REQUIRE(before.sideToMove()     == after.sideToMove());
    REQUIRE(before.enPassantRow()   == after.enPassantRow());
    REQUIRE(before.enPassantCol()   == after.enPassantCol());
    REQUIRE(before.halfmoveClock()  == after.halfmoveClock());
    REQUIRE(before.fullmoveNumber() == after.fullmoveNumber());
    // Castling rights (same pattern as test_undo.cpp)
    const CastlingRights& crBefore = before.getCastlingRights();
    const CastlingRights& crAfter  = after.getCastlingRights();
    REQUIRE(crBefore.whiteKingSide  == crAfter.whiteKingSide);
    REQUIRE(crBefore.whiteQueenSide == crAfter.whiteQueenSide);
    REQUIRE(crBefore.blackKingSide  == crAfter.blackKingSide);
    REQUIRE(crBefore.blackQueenSide == crAfter.blackQueenSide);
}

} // namespace

// ============================================================================
// SECTION 1 — WHITE PROMOTIONS
// ============================================================================
TEST_CASE("White promotion: quiet promotion produces 4 moves")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/P7/8/8/8/8/8/4k3 w - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    // a7 -> a8 = Q/R/B/N
    REQUIRE(containsPromotion(moves, 1,0, 0,0));
    REQUIRE(countPromotions(moves, 1,0, 0,0) == 4);
}

TEST_CASE("White promotion: capture promotions produce 8 moves (4 per target)")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "r1r5/1P6/8/8/8/8/8/4k3 w - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    REQUIRE(containsPromotion(moves, 1,1, 0,0)); // b7xa8
    REQUIRE(containsPromotion(moves, 1,1, 0,2)); // b7xc8
    int countA8 = countPromotions(moves, 1,1, 0,0);
    int countC8 = countPromotions(moves, 1,1, 0,2);
    REQUIRE(countA8 + countC8 == 8);
}

// ============================================================================
// SECTION 2 — BLACK PROMOTIONS
// ============================================================================
TEST_CASE("Black promotion: quiet promotion produces 4 moves")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "4k3/8/8/8/8/8/p7/8 b - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    // a2 -> a1
    REQUIRE(containsPromotion(moves, 6,0, 7,0));
    REQUIRE(countPromotions(moves, 6,0, 7,0) == 4);
}

TEST_CASE("Black promotion: capture promotions produce 8 moves (4 per target)")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "4k3/8/8/8/8/8/1p6/R1R5 b - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    REQUIRE(containsPromotion(moves, 6,1, 7,0)); // b2xa1
    REQUIRE(containsPromotion(moves, 6,1, 7,2)); // b2xc1
    int countA1 = countPromotions(moves, 6,1, 7,0);
    int countC1 = countPromotions(moves, 6,1, 7,2);
    REQUIRE(countA1 + countC1 == 8);
}

// ============================================================================
// SECTION 3 — APPLY/UNDO REVERSIBILITY
// ============================================================================
TEST_CASE("Promotion apply/undo is perfectly reversible for both colors")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/P6p/8/8/8/8/p6P/4k3 w - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    for (const auto& m : moves)
    {
        if (m.promotion == 0)
            continue;
        Board before = board;
        // Canonical apply/undo
        applyUndo(gen, board, m);
        // Board equality must hold
        requireBoardEqual(before, board);
    }
}

// ============================================================================
// SECTION 4 — PROMOTION PIECE CORRECTNESS
// ============================================================================
TEST_CASE("Promotion chooses correct promoted piece for both colors")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/P6p/8/8/8/8/p6P/4k3 w - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    for (const auto& m : moves)
    {
        if (m.promotion == 0)
            continue;
        Board tmp = board;
        MoveState st;
        st.move = m;
        gen.applyMove(tmp, st);
        const Piece& p = tmp.at(m.toRow, m.toCol);
        switch (m.promotion)
        {
            case 'Q': REQUIRE(p.type == PieceType::Queen);  break;
            case 'R': REQUIRE(p.type == PieceType::Rook);   break;
            case 'B': REQUIRE(p.type == PieceType::Bishop); break;
            case 'N': REQUIRE(p.type == PieceType::Knight); break;
            default:  REQUIRE(false); // invalid promotion specifier
        }
    }
}

// ============================================================================
// SECTION 5 — TACTICAL PROMOTION CONSEQUENCES
// ============================================================================
TEST_CASE("Promotion gives immediate check to enemy king", "[promotion][check]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/6P1/6k1/8/8/8/8/4K3 w - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    bool foundCheckPromotion = false;
    for (const auto& m : moves)
    {
        // g7 -> g8
        if (m.fromRow == 1 && m.fromCol == 6 &&
            m.toRow == 0 && m.toCol == 6 &&
            m.promotion != 0)
        {
            Board tmp = board;
            MoveState st;
            st.move = m;
            gen.applyMove(tmp, st);
            // Q/R promotions must give check on g6
            if (m.promotion == 'Q' || m.promotion == 'R')
            {
                REQUIRE(gen.isSquareAttacked(tmp, 2,6, Color::White));
                foundCheckPromotion = true;
            }
            gen.undoMove(tmp, st);
        }
    }
    REQUIRE(foundCheckPromotion);
}

TEST_CASE("Promotion creates discovered check", "[promotion][discover]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "8/R3P1k1/8/8/8/8/8/6K1 w - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    bool foundDiscovered = false;
    for (const auto& m : moves)
    {
        // d7 -> d8
        if (m.fromRow == 1 && m.fromCol == 4 &&
            m.toRow == 0 && m.toCol == 4 &&
            m.promotion != 0)
        {
            Board tmp = board;
            MoveState st;
            st.move = m;
            gen.applyMove(tmp, st);
            // Rook on a7 must now attack g7
            REQUIRE(gen.isSquareAttacked(tmp, 1,6, Color::White));
            gen.undoMove(tmp, st);
            foundDiscovered = true;
        }
    }
    REQUIRE(foundDiscovered);
}

TEST_CASE("Promotion results in immediate checkmate", "[promotion][checkmate]")
{
    MoveGenerator gen;
    Board board;
    REQUIRE(FEN::loadFEN(board, "7k/4P3/6K1/8/8/8/8/8 w - - 0 1"));
    auto moves = gen.generateLegalMoves(board);
    bool foundMatePromotion = false;
    for (const auto& m : moves)
    {
        // e7 -> e8
        if (m.fromRow == 1 && m.fromCol == 4 &&
            m.toRow == 0 && m.toCol == 4 &&
            m.promotion != 0)
        {
            Board tmp = board;
            MoveState st;
            st.move = m;
            gen.applyMove(tmp, st);
            auto replies = gen.generateLegalMoves(tmp);
            if (replies.empty())
                foundMatePromotion = true;
            gen.undoMove(tmp, st);
        }
    }
    REQUIRE(foundMatePromotion);
}
