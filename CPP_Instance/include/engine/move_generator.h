#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H
#include <vector>
#include "engine/board.h"
#include "engine/move_state.h"

// ------------------------------------------------------------
// Castling geometry description
// ------------------------------------------------------------
struct CastlingGeometry
{
    int kingSideRookCol;
    int queenSideRookCol;
    int kingSideKingTargetCol;
    int queenSideKingTargetCol;
    int kingSideRookTargetCol;
    int queenSideRookTargetCol;

    std::vector<int> kingSideEmptyCols;
    std::vector<int> queenSideEmptyCols;
    std::vector<int> kingSideSafeCols;
    std::vector<int> queenSideSafeCols;

    // ✔ Actual CastlingGeometry constructor
    CastlingGeometry(
        int ksRook, int qsRook,
        int ksKingT, int qsKingT,
        int ksRookT, int qsRookT,
        std::initializer_list<int> ksEmpty,
        std::initializer_list<int> qsEmpty,
        std::initializer_list<int> ksSafe,
        std::initializer_list<int> qsSafe
    )
        : kingSideRookCol(ksRook),
          queenSideRookCol(qsRook),
          kingSideKingTargetCol(ksKingT),
          queenSideKingTargetCol(qsKingT),
          kingSideRookTargetCol(ksRookT),
          queenSideRookTargetCol(qsRookT),
          kingSideEmptyCols(ksEmpty),
          queenSideEmptyCols(qsEmpty),
          kingSideSafeCols(ksSafe),
          queenSideSafeCols(qsSafe)
    {}
};

// ============================================================
// MoveGenerator — Responsible for move creation, application,
// legality checking, and attack detection.
// ============================================================
class MoveGenerator
{
public:
    // Initializes castling geometry tables.
    MoveGenerator();
    // ---------------------------------------------------------
    // Top-level move generation
    // ---------------------------------------------------------
    std::vector<MoveData> generatePseudoLegalMoves(Board& board);
    std::vector<MoveData> generateLegalMoves(Board& board);
    // ---------------------------------------------------------
    // Apply / Undo (MoveState-bounded reversible operations)
    // ---------------------------------------------------------
    void applyMove(Board& board, MoveState& state);
    void undoMove(Board& board, const MoveState& state);
    // ---------------------------------------------------------
    // King-safety logic
    // ---------------------------------------------------------
    bool leavesKingInCheck(Board& board, MoveState& state);
    bool isSquareAttacked(const Board& board, int r, int c,Color attackerColor) const;
    int countAttackers(const Board& board, int r, int c, Color attackerColor) const;
    // ---------------------------------------------------------
    // Attack detection components
    // ---------------------------------------------------------
    bool pawnAttackCheck(const Board& board, int r, int c,Color attackerColor) const;
    bool knightAttackCheck(const Board& board, int r, int c,Color attackerColor) const;
    bool kingAttackCheck(const Board& board, int r, int c,Color attackerColor) const;
    bool raycastAttack(const Board& board, int r, int c,Color attackerColor, int dr, int dc) const;
private:
    // ---------------------------------------------------------
    // Pseudo-legal move generators
    // ---------------------------------------------------------
    void generatePawnMoves  (Board& board, std::vector<MoveData>& moves);
    void generateKnightMoves(Board& board, std::vector<MoveData>& moves);
    void generateSlidingMoves(Board& board, std::vector<MoveData>& moves);
    void generateKingMoves  (Board& board, std::vector<MoveData>& moves);
    // ---------------------------------------------------------
    // Castling logic (new unified system)
    // ---------------------------------------------------------
    bool isCastlingMoveLegal(const Board& board, Color side,bool kingSide) const;
    void evaluateCastlingRightsAfterMove(Board& board,const MoveState& state);
    // geometry tables for white / black
    CastlingGeometry cgWhite;
    CastlingGeometry cgBlack;
    // ---------------------------------------------------------
    // Move classification
    // ---------------------------------------------------------
    MoveKind classifyMove(const Board& board, const MoveData& move) const;
    // ---------------------------------------------------------
    // Helpers (unchanged)
    // ---------------------------------------------------------
    bool isOwnPiece(const Piece& p, Color side) const
    {
        return p.color == side;
    }

    bool isEnemyPiece(const Piece& p, Color side) const
    {
        return (p.color != Color::None && p.color != side);
    }
};

#endif // MOVE_GENERATOR_H
