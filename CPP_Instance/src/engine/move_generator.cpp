// ============================================================================
// move_generator.cpp — FULL REWRITE (2026)
//
// Implements:
//   • Pseudo-legal move generation
//   • Legal move filtering
//   • Attack detection
//   • Castling (rights + geometry + safety)
//   • En passant
//   • Pawn promotions
//   • Fully reversible apply/undo using MoveState
// ============================================================================
#include "engine/move_generator.h"
#include "engine/board.h"
#include "engine/move_state.h"
#include <cmath>
#include <vector>

// ============================================================================
// Constructor — initialize white/black CastlingGeometry
// ============================================================================
MoveGenerator::MoveGenerator()
    : cgWhite(
        /* ksRookCol */ 7, /* qsRookCol */ 0,
        /* ksKingT   */ 6, /* qsKingT   */ 2,
        /* ksRookT   */ 5, /* qsRookT   */ 3,
        /* ksEmpty   */ {5,6},
        /* qsEmpty   */ {1,2,3},
        /* ksSafe    */ {4,5,6},
        /* qsSafe    */ {4,3,2}
      ),
      cgBlack(
        /* ksRookCol */ 7, /* qsRookCol */ 0,
        /* ksKingT   */ 6, /* qsKingT   */ 2,
        /* ksRookT   */ 5, /* qsRookT   */ 3,
        /* ksEmpty   */ {5,6},
        /* qsEmpty   */ {1,2,3},
        /* ksSafe    */ {4,5,6},
        /* qsSafe    */ {4,3,2}
      )
{}

// ============================================================================
// classifyMove — determine a MoveKind from MoveData + board state
// ============================================================================
MoveKind MoveGenerator::classifyMove(const Board& board, const MoveData& m) const
{
    const Piece& moving = board.at(m.fromRow, m.fromCol);
    const Piece& target = board.at(m.toRow, m.toCol);
    // Castling
    if (moving.type == PieceType::King && std::abs(m.toCol - m.fromCol) == 2)
        return (m.toCol > m.fromCol ? MoveKind::CastleKingSide
                                    : MoveKind::CastleQueenSide);
    // En passant
    if (m.isEnPassant)
        return MoveKind::EnPassant;
    // Promotion
    if (m.promotion != 0)
        return MoveKind::Promotion;
    // Capture
    if (target.color != Color::None &&
        target.color != moving.color)
        return MoveKind::Capture;
    return MoveKind::Quiet;
}

// ============================================================================
// generatePseudoLegalMoves — geometric-only moves (no king safety check)
// ============================================================================
std::vector<MoveData> MoveGenerator::generatePseudoLegalMoves(Board& board)
{
    std::vector<MoveData> moves;
    moves.reserve(128);
    generatePawnMoves(board, moves);
    generateKnightMoves(board, moves);
    generateSlidingMoves(board, moves);
    generateKingMoves(board, moves);
    return moves;
}

// ============================================================================
// generateLegalMoves — filter out king-in-check results
// ============================================================================
std::vector<MoveData> MoveGenerator::generateLegalMoves(Board& board)
{
    std::vector<MoveData> pseudo = generatePseudoLegalMoves(board);
    std::vector<MoveData> legal;
    legal.reserve(pseudo.size());
    for (const MoveData& m : pseudo)
    {
        MoveState st;
        st.move = m;
        if (!leavesKingInCheck(board, st))
            legal.push_back(m);
    }
    return legal;
}

// ============================================================================
// generateKingMoves — includes 1-step moves and castling
// ============================================================================
void MoveGenerator::generateKingMoves(Board& board,
                                      std::vector<MoveData>& moves)
{
    Color side = board.sideToMove();
    auto [kr, kc] = (side == Color::White ? board.whiteKing() : board.blackKing());
    static const int dirs[8][2] = {
        {1,0},{-1,0},{0,1},{0,-1},
        {1,1},{1,-1},{-1,1},{-1,-1}
    };
    // Normal king steps
    for (auto& d : dirs)
    {
        int nr = kr + d[0];
        int nc = kc + d[1];
        if (!board.inBounds(nr,nc)) continue;
        const Piece& t = board.at(nr,nc);
        if (t.color != side)
            moves.emplace_back(kr,kc,nr,nc);
    }
    // Castling — only generate if fully legal
    if (isCastlingMoveLegal(board, side, true))
        moves.emplace_back(kr, kc, kr, kc + 2);
    if (isCastlingMoveLegal(board, side, false))
        moves.emplace_back(kr, kc, kr, kc - 2);
}

// ============================================================================
// generateSlidingMoves — bishops / rooks / queens
// ============================================================================
void MoveGenerator::generateSlidingMoves(Board& board,
                                         std::vector<MoveData>& moves)
{
    Color side = board.sideToMove();
    static const int bishopDirs[4][2] = {
        {1,1}, {1,-1}, {-1,1}, {-1,-1}
    };
    static const int rookDirs[4][2] = {
        {1,0}, {-1,0}, {0,1}, {0,-1}
    };
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            const Piece& p = board.at(r,c);
            if (p.color != side) continue;
            // Bishop / Rook
            if (p.type == PieceType::Bishop || p.type == PieceType::Rook)
            {
                const int (*dirs)[2] =
                    (p.type == PieceType::Bishop ? bishopDirs : rookDirs);
                int dcount = 4;
                for (int d = 0; d < dcount; d++)
                {
                    int dr = dirs[d][0];
                    int dc = dirs[d][1];
                    int nr = r + dr;
                    int nc = c + dc;
                    while (board.inBounds(nr,nc))
                    {
                        const Piece& t = board.at(nr,nc);
                        if (t.type == PieceType::Empty)
                        {
                            moves.emplace_back(r,c,nr,nc);
                        }
                        else
                        {
                            if (t.color != side)
                                moves.emplace_back(r,c,nr,nc);
                            break;
                        }
                        nr += dr; nc += dc;
                    }
                }
            }
            // Queen = Bishop + Rook rays
            if (p.type == PieceType::Queen)
            {
                static const int qDirs[8][2] = {
                    {1,1},{1,-1},{-1,1},{-1,-1},
                    {1,0},{-1,0},{0,1},{0,-1}
                };
                for (int d = 0; d < 8; d++)
                {
                    int dr = qDirs[d][0];
                    int dc = qDirs[d][1];
                    int nr = r + dr;
                    int nc = c + dc;
                    while (board.inBounds(nr,nc))
                    {
                        const Piece& t = board.at(nr,nc);
                        if (t.type == PieceType::Empty)
                        {
                            moves.emplace_back(r,c,nr,nc);
                        }
                        else
                        {
                            if (t.color != side)
                                moves.emplace_back(r,c,nr,nc);
                            break;
                        }
                        nr += dr; nc += dc;
                    }
                }
            }
        }
    }
}

// ============================================================================
// generateKnightMoves
// ============================================================================
void MoveGenerator::generateKnightMoves(Board& board,
                                        std::vector<MoveData>& moves)
{
    Color side = board.sideToMove();
    static const int kOff[8][2] = {
        {2,1},{2,-1},{-2,1},{-2,-1},
        {1,2},{1,-2},{-1,2},{-1,-2}
    };
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            const Piece& p = board.at(r,c);
            if (p.type != PieceType::Knight || p.color != side) continue;
            for (auto& off : kOff)
            {
                int nr = r + off[0];
                int nc = c + off[1];
                if (!board.inBounds(nr,nc)) continue;
                const Piece& t = board.at(nr,nc);
                if (t.color != side)
                    moves.emplace_back(r,c,nr,nc);
            }
        }
    }
}

// ============================================================================
// generatePawnMoves — includes EP + promotions
// ============================================================================
void MoveGenerator::generatePawnMoves(Board& board,
                                      std::vector<MoveData>& moves)
{
    Color side = board.sideToMove();
    int forward = (side == Color::White ? -1 : 1);
    int startRow = (side == Color::White ? 6 : 1);
    int promoRow = (side == Color::White ? 0 : 7);
    int epR = board.enPassantRow();
    int epC = board.enPassantCol();
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            const Piece& p = board.at(r,c);
            if (p.type != PieceType::Pawn || p.color != side) continue;
            int nr = r + forward;
            // Single push
            if (board.inBounds(nr,c) &&
                board.at(nr,c).type == PieceType::Empty)
            {
                if (nr == promoRow)
                {
                    const char promos[4] = {'Q','R','B','N'};
                    for (char pr : promos)
                    {
                        MoveData m(r,c,nr,c);
                        m.promotion = pr;
                        moves.push_back(m);
                    }
                }
                else
                {
                    moves.emplace_back(r,c,nr,c);
                }
                // Double push
                if (r == startRow)
                {
                    int nr2 = r + 2*forward;
                    if (board.inBounds(nr2,c) &&
                        board.at(nr2,c).type == PieceType::Empty)
                    {
                        moves.emplace_back(r,c,nr2,c);
                    }
                }
            }
            // Diagonal captures
            for (int dc = -1; dc <= 1; dc += 2)
            {
                int nc = c + dc;
                if (!board.inBounds(nr,nc)) continue;

                const Piece& t = board.at(nr,nc);
                if (t.color != Color::None && t.color != side)
                {
                    if (nr == promoRow)
                    {
                        const char promos[4] = {'Q','R','B','N'};
                        for (char pr : promos)
                        {
                            MoveData m(r,c,nr,nc);
                            m.promotion = pr;
                            moves.push_back(m);
                        }
                    }
                    else
                    {
                        moves.emplace_back(r,c,nr,nc);
                    }
                }
            }
            // En Passant
            if (epR == nr && std::abs(epC - c) == 1)
            {
                MoveData m(r,c,epR,epC);
                m.isEnPassant = true;
                moves.push_back(m);
            }
        }
    }
}

// ============================================================================
// isCastlingMoveLegal — full right/geometry/safety check
// ============================================================================
bool MoveGenerator::isCastlingMoveLegal(const Board& board,
                                        Color side,
                                        bool kingSide) const
{
    const CastlingGeometry& CG = (side == Color::White ? cgWhite : cgBlack);
    const CastlingRights& cr = board.getCastlingRights();
    bool hasRight = false;
    if (side == Color::White)
        hasRight = (kingSide ? cr.whiteKingSide : cr.whiteQueenSide);
    else
        hasRight = (kingSide ? cr.blackKingSide : cr.blackQueenSide);
    if (!hasRight)
        return false;
    auto [kr,kc] = (side == Color::White ? board.whiteKing() : board.blackKing());
    const std::vector<int>& emptyCols = (kingSide ? CG.kingSideEmptyCols
                                                  : CG.queenSideEmptyCols);
    const std::vector<int>& safeCols  = (kingSide ? CG.kingSideSafeCols
                                                  : CG.queenSideSafeCols);
    // 1. Required squares must be empty
    for (int col : emptyCols)
    {
        if (board.at(kr,col).type != PieceType::Empty)
            return false;
    }
    // 2. Check that none of the king’s traversed squares are attacked
    Color enemy = (side == Color::White ? Color::Black : Color::White);
    for (int col : safeCols)
    {
        if (isSquareAttacked(board, kr, col, enemy))
            return false;
    }
    return true;
}

// ============================================================================
// evaluateCastlingRightsAfterMove — update rights based on king/rook moves
// ============================================================================
void MoveGenerator::evaluateCastlingRightsAfterMove(Board& board,
                                                    const MoveState& st)
{
    const Piece& moved = st.movedPiece;
    const Piece& captured = st.capturedPiece;
    CastlingRights newCR = st.prevCastlingRights;
    int fr = st.move.fromRow;
    int fc = st.move.fromCol;
    int tr = st.move.toRow;
    int tc = st.move.toCol;
    // King moved -> lose both rights for that color
    if (moved.type == PieceType::King)
    {
        if (moved.color == Color::White)
        {
            newCR.whiteKingSide = false;
            newCR.whiteQueenSide = false;
        }
        else
        {
            newCR.blackKingSide = false;
            newCR.blackQueenSide = false;
        }
    }
    // Rook moved -> remove appropriate right
    if (moved.type == PieceType::Rook)
    {
        if (moved.color == Color::White)
        {
            if (fr == 7 && fc == cgWhite.queenSideRookCol)
                newCR.whiteQueenSide = false;
            if (fr == 7 && fc == cgWhite.kingSideRookCol)
                newCR.whiteKingSide = false;
        }
        else
        {
            if (fr == 0 && fc == cgBlack.queenSideRookCol)
                newCR.blackQueenSide = false;
            if (fr == 0 && fc == cgBlack.kingSideRookCol)
                newCR.blackKingSide = false;
        }
    }
    // Rook captured on starting square -> remove appropriate right
    if (captured.type == PieceType::Rook)
    {
        if (captured.color == Color::White)
        {
            if (tr == 7 && tc == cgWhite.queenSideRookCol)
                newCR.whiteQueenSide = false;
            if (tr == 7 && tc == cgWhite.kingSideRookCol)
                newCR.whiteKingSide = false;
        }
        else
        {
            if (tr == 0 && tc == cgBlack.queenSideRookCol)
                newCR.blackQueenSide = false;
            if (tr == 0 && tc == cgBlack.kingSideRookCol)
                newCR.blackKingSide = false;
        }
    }
    board.castling = newCR;
}

// ============================================================================
// leavesKingInCheck — apply -> test -> undo
// ============================================================================
bool MoveGenerator::leavesKingInCheck(Board& board, MoveState& state)
{
    applyMove(board, state);
    Color side = state.prevSideToMove;
    Color enemy = (side == Color::White ? Color::Black : Color::White);
    int kr, kc;
    if (side == Color::White)
        std::tie(kr,kc) = board.whiteKing();
    else
        std::tie(kr,kc) = board.blackKing();
    bool check = isSquareAttacked(board, kr, kc, enemy);
    undoMove(board, state);
    return check;
}

// ============================================================================
// Attack Detection
// ============================================================================
bool MoveGenerator::isSquareAttacked(const Board& board,
                                     int r, int c,
                                     Color attackerColor) const
{
    if (pawnAttackCheck(board,r,c,attackerColor))   return true;
    if (knightAttackCheck(board,r,c,attackerColor)) return true;
    static const int diag[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (auto& d : diag)
        if (raycastAttack(board,r,c,attackerColor,d[0],d[1]))
            return true;
    static const int ortho[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (auto& d : ortho)
        if (raycastAttack(board,r,c,attackerColor,d[0],d[1]))
            return true;
    if (kingAttackCheck(board,r,c,attackerColor)) return true;
    return false;
}

// ============================================================================
// countAttackers — counts how many attack channels threaten (r,c)
// Uses existing attack components: NO added chess logic
// ============================================================================
int MoveGenerator::countAttackers(const Board& board,
                                  int r,
                                  int c,
                                  Color attackerColor) const
{
    int attackers = 0;
    // Pawn
    if (pawnAttackCheck(board, r, c, attackerColor))
        attackers++;
    // Knight
    if (knightAttackCheck(board, r, c, attackerColor))
        attackers++;
    // Bishop/Queen diagonals
    static const int diag[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (auto &d : diag)
        if (raycastAttack(board, r, c, attackerColor, d[0], d[1]))
            attackers++;
    // Rook/Queen orthogonals
    static const int ortho[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (auto &d : ortho)
        if (raycastAttack(board, r, c, attackerColor, d[0], d[1]))
            attackers++;
    // King adjacency
    if (kingAttackCheck(board, r, c, attackerColor))
        attackers++;
    return attackers;
}

bool MoveGenerator::pawnAttackCheck(const Board& board,
                                    int r, int c,
                                    Color attackerColor) const
{
    int dir = (attackerColor == Color::White ? 1 : -1);
    int pr = r + dir;
    if (board.inBounds(pr, c-1))
    {
        const Piece& p = board.at(pr,c-1);
        if (p.type == PieceType::Pawn && p.color == attackerColor)
            return true;
    }
    if (board.inBounds(pr, c+1))
    {
        const Piece& p = board.at(pr,c+1);
        if (p.type == PieceType::Pawn && p.color == attackerColor)
            return true;
    }
    return false;
}

bool MoveGenerator::knightAttackCheck(const Board& board,
                                      int r, int c,
                                      Color attackerColor) const
{
    static const int kOff[8][2] = {
        {2,1},{2,-1},{-2,1},{-2,-1},
        {1,2},{1,-2},{-1,2},{-1,-2}
    };
    for (auto& o : kOff)
    {
        int nr = r + o[0];
        int nc = c + o[1];
        if (!board.inBounds(nr,nc)) continue;
        const Piece& p = board.at(nr,nc);
        if (p.type == PieceType::Knight && p.color == attackerColor)
            return true;
    }
    return false;
}

bool MoveGenerator::kingAttackCheck(const Board& board,
                                    int r, int c,
                                    Color attackerColor) const
{
    static const int dirs[8][2] = {
        {1,0},{-1,0},{0,1},{0,-1},
        {1,1},{1,-1},{-1,1},{-1,-1}
    };
    for (auto& d : dirs)
    {
        int nr = r + d[0];
        int nc = c + d[1];
        if (!board.inBounds(nr,nc)) continue;
        const Piece& p = board.at(nr,nc);
        if (p.type == PieceType::King && p.color == attackerColor)
            return true;
    }
    return false;
}

bool MoveGenerator::raycastAttack(const Board& board,
                                  int r, int c,
                                  Color attackerColor,
                                  int dr, int dc) const
{
    int nr = r + dr;
    int nc = c + dc;
    while (board.inBounds(nr,nc))
    {
        const Piece& p = board.at(nr,nc);
        if (p.type != PieceType::Empty)
        {
            if (p.color == attackerColor)
            {
                if (dr != 0 && dc != 0 &&
                    (p.type == PieceType::Bishop || p.type == PieceType::Queen))
                    return true;

                if ((dr == 0 || dc == 0) &&
                    (p.type == PieceType::Rook || p.type == PieceType::Queen))
                    return true;
            }
            return false;
        }
        nr += dr;
        nc += dc;
    }
    return false;
}

// ============================================================================
// applyMove — fully reversible move application
// ============================================================================
void MoveGenerator::applyMove(Board& board, MoveState& st)
{
    const MoveData& m = st.move;
    int fr = m.fromRow;
    int fc = m.fromCol;
    int tr = m.toRow;
    int tc = m.toCol;
    Piece moving = board.at(fr,fc);
    Piece target = board.at(tr,tc);
    // Save previous state
    st.movedPiece = moving;
    st.capturedPiece = target;
    st.prevSideToMove = board.sideToMove();
    st.prevEnPassantRow = board.enPassantRow();
    st.prevEnPassantCol = board.enPassantCol();
    st.prevHalfmoveClock = board.halfmoveClock();
    st.prevFullmoveNumber = board.fullmoveNumber();
    std::tie(st.prevWhiteKingRow, st.prevWhiteKingCol) = board.whiteKing();
    std::tie(st.prevBlackKingRow, st.prevBlackKingCol) = board.blackKing();
    st.prevCastlingRights = board.getCastlingRights();
    st.kind = classifyMove(board, m);
    st.wasPromotion = false;
    // Clear EP if not a 2-step pawn move
    board.clearEnPassant();
    // Update halfmove clock
    if (moving.type == PieceType::Pawn ||
        target.type != PieceType::Empty)
        board.halfmoveClock_ = 0;
    else
        board.halfmoveClock_++;
    // Increment fullmove number after Black moves
    if (board.sideToMove() == Color::Black)
        board.fullmoveNumber_++;
    Color side = board.sideToMove();
    Color enemy = (side == Color::White ? Color::Black : Color::White);
    // === Special move types ===
    if (st.kind == MoveKind::EnPassant)
    {
        int capRow = fr;
        int capCol = tc;
        st.capturedPiece = board.at(capRow, capCol);
        board.removePiece(capRow, capCol);
        board.movePiece(fr,fc,tr,tc);
    }
    else if (st.kind == MoveKind::CastleKingSide ||
             st.kind == MoveKind::CastleQueenSide)
    {
        bool ks = (st.kind == MoveKind::CastleKingSide);
        const CastlingGeometry& CG = (side == Color::White ? cgWhite : cgBlack);
        int newKcol = (ks ? CG.kingSideKingTargetCol : CG.queenSideKingTargetCol);
        int newRcol = (ks ? CG.kingSideRookTargetCol : CG.queenSideRookTargetCol);
        int rookCol = (ks ? CG.kingSideRookCol : CG.queenSideRookCol);
        // Move king
        board.movePiece(fr,fc, fr,newKcol);
        // Move rook
        board.movePiece(fr,rookCol, fr,newRcol);
    }
    else if (st.kind == MoveKind::Promotion)
    {
        st.wasPromotion = true;
        board.removePiece(fr,fc);
        PieceType promoType = PieceType::Queen;
        switch (m.promotion)
        {
            case 'Q': promoType = PieceType::Queen; break;
            case 'R': promoType = PieceType::Rook; break;
            case 'B': promoType = PieceType::Bishop; break;
            case 'N': promoType = PieceType::Knight; break;
        }
        board.placePiece(tr,tc, {promoType, side});
    }
    else // normal move
    {
        board.movePiece(fr,fc,tr,tc);
    }
    // If double pawn push, set EP square
    if (moving.type == PieceType::Pawn && std::abs(tr - fr) == 2)
    {
        board.epRow = (fr + tr) / 2;
        board.epCol = fc;
    }
    // Update castling rights
    evaluateCastlingRightsAfterMove(board, st);
    // Switch side to move
    board.stm = enemy;
    // ================================================
    // Compute number of attackers on the NEW king
    // ================================================
    int kr, kc;
    if (enemy == Color::White)
        std::tie(kr, kc) = board.whiteKing();
    else
        std::tie(kr, kc) = board.blackKing();
}

// ============================================================================
// undoMove — perfect inversion of applyMove
// ============================================================================
void MoveGenerator::undoMove(Board& board, const MoveState& st)
{
    const MoveData& m = st.move;
    int fr = m.fromRow;
    int fc = m.fromCol;
    int tr = m.toRow;
    int tc = m.toCol;
    // Restore base game state
    board.stm = st.prevSideToMove;
    board.epRow = st.prevEnPassantRow;
    board.epCol = st.prevEnPassantCol;
    board.halfmoveClock_ = st.prevHalfmoveClock;
    board.fullmoveNumber_ = st.prevFullmoveNumber;
    board.whiteKingRow = st.prevWhiteKingRow;
    board.whiteKingCol = st.prevWhiteKingCol;
    board.blackKingRow = st.prevBlackKingRow;
    board.blackKingCol = st.prevBlackKingCol;
    board.castling = st.prevCastlingRights;
    Piece moving = st.movedPiece;
    Piece captured = st.capturedPiece;
    // Restore pieces according to move type
    switch (st.kind)
    {
        case MoveKind::EnPassant:
        {
            board.movePiece(tr,tc, fr,fc);
            board.placePiece(fr, tc, captured);
            break;
        }
        case MoveKind::CastleKingSide:
        case MoveKind::CastleQueenSide:
        {
            bool ks = (st.kind == MoveKind::CastleKingSide);
            const CastlingGeometry& CG =
                (moving.color == Color::White ? cgWhite : cgBlack);
            int newKcol = (ks ? CG.kingSideKingTargetCol : CG.queenSideKingTargetCol);
            int newRcol = (ks ? CG.kingSideRookTargetCol : CG.queenSideRookTargetCol);
            int rookCol = (ks ? CG.kingSideRookCol : CG.queenSideRookCol);
            // Move king back
            board.movePiece(fr,newKcol, fr,fc);
            // Move rook back
            board.movePiece(fr,newRcol, fr,rookCol);
            break;
        }
        case MoveKind::Promotion:
        {
            board.removePiece(tr,tc);
            board.placePiece(fr,fc, moving);
            if (captured.type != PieceType::Empty)
                board.placePiece(tr,tc, captured);
            break;
        }
        case MoveKind::Capture:
        {
            board.movePiece(tr,tc, fr,fc);
            board.placePiece(tr,tc, captured);
            break;
        }
        case MoveKind::Quiet:
        default:
        {
            board.movePiece(tr,tc, fr,fc);
            break;
        }
    }
}
