#pragma once
#include "engine/board.h"     // For Piece, Color, CastlingRights
#include <utility>
// ------------------------------------------------------------
// MoveKind
// ------------------------------------------------------------
enum class MoveKind {
    Quiet,
    Capture,
    EnPassant,
    Promotion,
    CastleKingSide,
    CastleQueenSide
};
// ------------------------------------------------------------
// Internal move-identity container
// This replaces the old Move struct entirely.
// ------------------------------------------------------------
struct MoveData
{
    int fromRow = 0;
    int fromCol = 0;
    int toRow   = 0;
    int toCol   = 0;
    char promotion = 0; 
    bool isEnPassant = false;
    MoveData() = default;
    MoveData(int fr, int fc, int tr, int tc)
        : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc)
    {}
};
// ------------------------------------------------------------
// MoveState — complete reversible snapshot of game state
// + identity of the move via MoveData
// ------------------------------------------------------------
struct MoveState
{
    // Identify the move
    MoveData move; 
    MoveKind kind = MoveKind::Quiet;
    // Piece snapshots
    Piece movedPiece;
    Piece capturedPiece;
    // Pre-move game state
    Color prevSideToMove;
    // EP state
    int prevEnPassantRow;
    int prevEnPassantCol;
    // Counters
    int prevHalfmoveClock;
    int prevFullmoveNumber;
    // King snapshots
    int prevWhiteKingRow;
    int prevWhiteKingCol;
    int prevBlackKingRow;
    int prevBlackKingCol;
    // Castling rights snapshot (replaces moved flags completely)
    CastlingRights prevCastlingRights;
    bool wasPromotion = false;
    // Number of enemy attackers on the king square after this move.
    int attackersAfterMove = 0;
};
