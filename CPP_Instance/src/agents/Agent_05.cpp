#include "agents/Agent_05.h"
#include "engine/board.h"
#include <algorithm>

// ----------------------------------------------------------------
// Piece-Square Tables
// ----------------------------------------------------------------
const int Agent_05::pawnTable[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

const int Agent_05::knightTable[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

const int Agent_05::bishopTable[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

const int Agent_05::rookTable[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 5, 10, 10, 10, 10, 10, 10,  5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    { 0,  0,  0,  5,  5,  0,  0,  0}
};

const int Agent_05::queenTable[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

const int Agent_05::kingTable[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};

// ----------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------
Agent_05::Agent_05(int depth)
    : maxDepth(depth),
      rng(std::random_device{}()),
      noiseDist(-0.05, 0.05)
{
}

// ----------------------------------------------------------------
// Move ordering helper
// ----------------------------------------------------------------
void Agent_05::orderMoves(Board& board,
                          std::vector<MoveData>& moves,
                          MoveGenerator& gen) const
{
    // Piece material values (same as evaluate())
    static constexpr int pieceValues[] = {
        0,    // Empty
        100,  // Pawn
        305,  // Knight
        333,  // Bishop
        563,  // Rook
        950,  // Queen
        0     // King (ignored)
    };

    auto moveScore = [&](const MoveData& m)
    {
        const Piece& attacker = board.at(m.fromRow, m.fromCol);
        int attackerValue = pieceValues[(int)attacker.type];

        // --- Detect capture ---
        bool isCapture = false;
        int capturedValue = 0;

        const Piece& target = board.at(m.toRow, m.toCol);

        if (target.type != PieceType::Empty) {
            isCapture = true;
            capturedValue = pieceValues[(int)target.type];
        }
        else if (m.isEnPassant) {
            // EP capture target is one row behind destination
            int capRow = (attacker.color == Color::White ? m.toRow + 1 : m.toRow - 1);
            const Piece& epTarget = board.at(capRow, m.toCol);
            isCapture = true;
            capturedValue = pieceValues[(int)epTarget.type];
        }

        // --- MVV-LVA scoring ---
        int mvv_lva_score = 0;
        if (isCapture)
            mvv_lva_score = 10000 + (capturedValue - attackerValue);

        // --- Promotion bonus ---
        int promoBonus = (m.promotion != 0 ? 5000 : 0);

        return mvv_lva_score + promoBonus;
    };

    std::sort(moves.begin(), moves.end(),
              [&](const MoveData& a, const MoveData& b)
              {
                  return moveScore(a) > moveScore(b);
              });
}

// ----------------------------------------------------------------
// chooseMove()
// ----------------------------------------------------------------
MoveData Agent_05::chooseMove(const Game& game)
{
    Board board = game.getBoard();
    MoveGenerator gen;
    std::vector<MoveData> legalMoves = gen.generateLegalMoves(board);

    if (legalMoves.empty())
        return MoveData();

    bool maximizingPlayer = (board.sideToMove() == Color::White);

    // Global best move over all completed depths
    MoveData bestMoveGlobal = legalMoves[0];

    // Iterative deepening loop
    for (int depth = 1; depth <= maxDepth; depth++)
    {
        // Copy legalMoves; reorder for this depth
        std::vector<MoveData> moves = legalMoves;
        orderMoves(board, moves, gen);

        double bestScoreThisDepth =
            maximizingPlayer ? -std::numeric_limits<double>::infinity() : +std::numeric_limits<double>::infinity();

        MoveData bestMoveThisDepth = moves[0];

        for (const MoveData& m : moves)
        {
            MoveState st;
            st.move = m;

            gen.applyMove(board, st);

            double score = alphabeta(
                board,
                depth - 1,
                -std::numeric_limits<double>::infinity(),
                +std::numeric_limits<double>::infinity(),
                !maximizingPlayer,
                gen);

            gen.undoMove(board, st);

            if ((maximizingPlayer && score > bestScoreThisDepth) ||
                (!maximizingPlayer && score < bestScoreThisDepth))
            {
                bestScoreThisDepth = score;
                bestMoveThisDepth = m;
            }
        }

        // Update global best move to the deepest fully completed iteration
        bestMoveGlobal = bestMoveThisDepth;
    }

    return bestMoveGlobal;
}

// ----------------------------------------------------------------
// Alpha-beta with move ordering
// ----------------------------------------------------------------
double Agent_05::alphabeta(Board& board,
                           int depth,
                           double alpha,
                           double beta,
                           bool maximizingPlayer,
                           MoveGenerator& gen)
{
    std::vector<MoveData> moves = gen.generateLegalMoves(board);

    if (moves.empty())
    {
        Color side = board.sideToMove();
        Color enemy = (side == Color::White ? Color::Black : Color::White);
        auto [kr, kc] = (side == Color::White ? board.whiteKing() : board.blackKing());

        bool inCheck = gen.isSquareAttacked(board, kr, kc, enemy);

        if (inCheck)
            return (side == Color::White ? -MATE_SCORE : +MATE_SCORE);
        else
            return 0.0;
    }

    if (depth == 0)
        return evaluate(board);

    orderMoves(board, moves, gen);

    if (maximizingPlayer)
    {
        double value = -std::numeric_limits<double>::infinity();

        for (const MoveData& m : moves)
        {
            MoveState st;
            st.move = m;
            gen.applyMove(board, st);

            value = std::max(value,
                             alphabeta(board, depth - 1,
                                       alpha, beta,
                                       false, gen));

            gen.undoMove(board, st);

            alpha = std::max(alpha, value);
            if (alpha >= beta)
                break;
        }

        return value;
    }
    else
    {
        double value = +std::numeric_limits<double>::infinity();

        for (const MoveData& m : moves)
        {
            MoveState st;
            st.move = m;
            gen.applyMove(board, st);

            value = std::min(value,
                             alphabeta(board, depth - 1,
                                       alpha, beta,
                                       true, gen));

            gen.undoMove(board, st);

            beta = std::min(beta, value);
            if (beta <= alpha)
                break;
        }

        return value;
    }
}

// ----------------------------------------------------------------
// evaluate() — material + PST + endgame heuristics
// ----------------------------------------------------------------
double Agent_05::evaluate(const Board& board) const
{
    static constexpr int pieceValues[] = {
        0,    // Empty
        100,  // Pawn
        305,  // Knight
        333,  // Bishop
        563,  // Rook
        950,  // Queen
        0     // King
    };

    int white = 0;
    int black = 0;
    int whiteRaw = 0;
    int blackRaw = 0;

    // --- Material + PST ---
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            const Piece& p = board.at(r, c);
            if (p.type == PieceType::Empty)
                continue;

            int base = pieceValues[(int)p.type];
            int pst  = 0;

            switch (p.type)
            {
                case PieceType::Pawn:   pst = pawnTable[r][c];   break;
                case PieceType::Knight: pst = knightTable[r][c]; break;
                case PieceType::Bishop: pst = bishopTable[r][c]; break;
                case PieceType::Rook:   pst = rookTable[r][c];   break;
                case PieceType::Queen:  pst = queenTable[r][c];  break;
                case PieceType::King:   pst = kingTable[r][c];   break;
                default: break;
            }

            int score = base + pst;

            if (p.color == Color::White)
            {
                white += score;
                whiteRaw += base;
            }
            else
            {
                black += score;
                blackRaw += base;
            }
        }
    }

    double baseScore = (white - black) / 100.0;

    // --- Endgame detection ---
    int myMaterialRaw =
        (board.sideToMove() == Color::White ? whiteRaw : blackRaw);
    int oppMaterialRaw =
        (board.sideToMove() == Color::White ? blackRaw : whiteRaw);

    int materialImbalance = myMaterialRaw - oppMaterialRaw;

    double ratio;
    if (oppMaterialRaw == 0)
        ratio = 999.0;
    else
        ratio = (double)materialImbalance / (double)oppMaterialRaw;

    int totalMaterialRaw = whiteRaw + blackRaw;

    double rav;
    if (totalMaterialRaw > 6500)
        rav = 0.85;
    else if (totalMaterialRaw > 3000)
        rav = 0.45;
    else
        rav = 0.25;

    bool triggerEndgame = false;

    if (ratio >= rav)
    {
        if (board.sideToMove() == Color::White)
        {
            // White must be ahead
            if (baseScore > 1.0)
                triggerEndgame = true;
        }
        else // Black to move
        {
            // Black must be ahead
            if (baseScore < -1.0)
                triggerEndgame = true;
        }
    }

    // ------------------------------------------------------------
    // Apply advanced endgame heuristics
    // ------------------------------------------------------------
    if (triggerEndgame)
    {
        MoveGenerator gen;
        // 1. Enhanced king-edge pressure
        baseScore += endgameKingPressure(board, gen);
        // 2. Enemy king mobility restriction
        baseScore += (0.3 * (8 - enemyKingMobility(board, gen)));
        // 3. Own king approaches enemy king (king-only endgame)
        if (oppMaterialRaw == 0)
            baseScore += kingApproachScore(board);
    else
        baseScore += kingSafety(board);
    }

    return baseScore;
}

// ----------------------------------------------------------------
// Endgame capability — kingDistToEdge() and endgameKingPressure()
// ----------------------------------------------------------------
int Agent_05::kingDistToEdge(int r, int c) const
{
    int fileDist = std::min(c, 7 - c);
    int rankDist = std::min(r, 7 - r);
    return std::min(fileDist, rankDist);
}

double Agent_05::enemyKingMobility(const Board& board, MoveGenerator& gen) const
{
    int kr, kc;

    if (board.sideToMove() == Color::White)
        std::tie(kr, kc) = board.blackKing();
    else
        std::tie(kr, kc) = board.whiteKing();

    Color myColor =
        (board.sideToMove() == Color::White ? Color::White : Color::Black);

    Color enemyColor =
        (myColor == Color::White ? Color::Black : Color::White);

    int mobility = 0;

    const int dirs[8][2] = {
        {1,0},{-1,0},{0,1},{0,-1},
        {1,1},{1,-1},{-1,1},{-1,-1}
    };

    for (auto& d : dirs)
    {
        int nr = kr + d[0];
        int nc = kc + d[1];

        if (nr < 0 || nr > 7 || nc < 0 || nc > 7)
            continue;

        const Piece& target = board.at(nr, nc);

        if (target.color == enemyColor)
            continue;

        if (gen.isSquareAttacked(board, nr, nc, myColor))
            continue;

        mobility++;
    }

    return mobility;
}

double Agent_05::endgameKingPressure(const Board& board, MoveGenerator& gen) const
{
    int kr, kc;

    if (board.sideToMove() == Color::White)
        std::tie(kr, kc) = board.blackKing();
    else
        std::tie(kr, kc) = board.whiteKing();

    int dist = kingDistToEdge(kr, kc);

    static constexpr double KEP_WEIGHT = 10;

    return KEP_WEIGHT * (3 - dist);
}

double Agent_05::kingApproachScore(const Board& board) const
{
    int myR, myC;
    int oppR, oppC;

    if (board.sideToMove() == Color::White)
    {
        std::tie(myR, myC) = board.whiteKing();
        std::tie(oppR, oppC) = board.blackKing();
    }
    else
    {
        std::tie(myR, myC) = board.blackKing();
        std::tie(oppR, oppC) = board.whiteKing();
    }

    int dist = std::abs(myR - oppR) + std::abs(myC - oppC);

    static constexpr double KING_APPROACH_WEIGHT = 2;

    return KING_APPROACH_WEIGHT * (14 - dist);
}

// ----------------------------------------------------------------
// King Safety — attackers, exposure, and pawns
// ----------------------------------------------------------------
double Agent_05::kingSafety(const Board& board) const
{
    double whiteKS = computeKingSafetyForSide(board, Color::White);
    double blackKS = computeKingSafetyForSide(board, Color::Black);

    // Positive score favors White (consistent with your eval convention)
    return (blackKS - whiteKS);
}

double Agent_05::computeKingSafetyForSide(const Board& board, Color side) const
{
    MoveGenerator gen;

    double zoneAttack   = kingZoneAttackScore(board, gen, side);
    double shieldScore  = pawnShieldScore(board, side);
    double exposureScore = kingExposureScore(board, side);

    // Larger = worse for that side
    return zoneAttack + shieldScore + exposureScore;
}

double Agent_05::kingZoneAttackScore(const Board& board,
                                     MoveGenerator& gen,
                                     Color side) const
{
    // Locate king
    int kr, kc;
    if (side == Color::White)
        std::tie(kr, kc) = board.whiteKing();
    else
        std::tie(kr, kc) = board.blackKing();

    Color enemy = (side == Color::White ? Color::Black : Color::White);

    // Attack weights
    static constexpr double pawnW   = 1.0;
    static constexpr double knightW = 2.0;
    static constexpr double bishopW = 2.0;
    static constexpr double rookW   = 3.0;
    static constexpr double queenW  = 4.0;

    const int dirs[8][2] =
    {
        { 1, 0}, {-1, 0},
        { 0, 1}, { 0,-1},
        { 1, 1}, { 1,-1},
        {-1, 1}, {-1,-1}
    };

    double total = 0.0;

    for (auto& d : dirs)
    {
        int nr = kr + d[0];
        int nc = kc + d[1];

        if (!board.inBounds(nr, nc))
            continue;

        // If no enemy attacks this square, skip attacker-type checks.
        if (!gen.isSquareAttacked(board, nr, nc, enemy))
            continue;

        // Pawn attacks
        if (gen.pawnAttackCheck(board, nr, nc, enemy))
            total += pawnW;

        // Knight attacks
        if (gen.knightAttackCheck(board, nr, nc, enemy))
            total += knightW;

        // King attacks (rarely relevant but included for consistency)
        if (gen.kingAttackCheck(board, nr, nc, enemy))
        {
            // King attacking the king-zone usually indicates close proximity.
            // We give it a light penalty:
            total += 1.0;
        }

        // Bishop / Queen diagonal ray attacks
        static const int diagDirs[4][2] =
        {
            { 1, 1}, { 1,-1},
            {-1, 1}, {-1,-1}
        };

        for (auto& dd : diagDirs)
        {
            if (gen.raycastAttack(board, nr, nc, enemy, dd[0], dd[1]))
                total += bishopW;   // bishop/queen share this weight
        }

        // Rook / Queen orthogonal ray attacks
        static const int orthoDirs[4][2] =
        {
            { 1, 0}, {-1, 0},
            { 0, 1}, { 0,-1}
        };

        for (auto& dd : orthoDirs)
        {
            if (gen.raycastAttack(board, nr, nc, enemy, dd[0], dd[1]))
                total += rookW;     // rook/queen share this weight
        }
    }

    return total;
}

double Agent_05::pawnShieldScore(const Board& board, Color side) const
{
    int kr, kc;
    if (side == Color::White)
        std::tie(kr, kc) = board.whiteKing();
    else
        std::tie(kr, kc) = board.blackKing();

    // Determine shield squares (3 squares)
    int shieldSquares[3][2];

    if (kc == 6) // King on g-file (likely castled)
    {
        int forward = (side == Color::White ? 1 : -1);
        shieldSquares[0][0] = kr + forward; shieldSquares[0][1] = 5;
        shieldSquares[1][0] = kr + forward; shieldSquares[1][1] = 6;
        shieldSquares[2][0] = kr + forward; shieldSquares[2][1] = 7;
    }
    else if (kc == 4) // King on e-file (default start/un-castled)
    {
        int forward = (side == Color::White ? 1 : -1);
        shieldSquares[0][0] = kr + forward; shieldSquares[0][1] = 3;
        shieldSquares[1][0] = kr + forward; shieldSquares[1][1] = 4;
        shieldSquares[2][0] = kr + forward; shieldSquares[2][1] = 5;
    }
    else
    {
        // Fallback: local 3-file zone
        int forward = (side == Color::White ? 1 : -1);
        for (int i = 0; i < 3; i++)
        {
            shieldSquares[i][0] = kr + forward;
            shieldSquares[i][1] = kc + (i - 1);
        }
    }

    static constexpr double pawnPresentBonus       = -0.5;
    static constexpr double pawnMissingPenalty     = +1.0;
    static constexpr double pawnTooAdvancedPenalty = +0.5;
    static constexpr double enemyPawnNearbyPenalty = +1.5;

    double total = 0.0;

    for (int i = 0; i < 3; i++)
    {
        int r = shieldSquares[i][0];
        int c = shieldSquares[i][1];

        if (!board.inBounds(r, c))
            continue;

        const Piece& p = board.at(r, c);

        if (p.type == PieceType::Pawn && p.color == side)
        {
            total += pawnPresentBonus;

            // Too advanced shield pawn?
            if ((side == Color::White && r <= 3) ||
                (side == Color::Black && r >= 4))
            {
                total += pawnTooAdvancedPenalty;
            }
        }
        else
        {
            total += pawnMissingPenalty;
        }

        // Enemy pawn nearby (same file, within 2 ranks)
        for (int rr = 0; rr < 8; rr++)
        {
            const Piece& ep = board.at(rr, c);
            if (ep.type == PieceType::Pawn && ep.color != side)
            {
                if (std::abs(rr - r) <= 2)
                    total += enemyPawnNearbyPenalty;
            }
        }
    }

    return total;
}

double Agent_05::kingExposureScore(const Board& board, Color side) const
{
    int kr, kc;
    if (side == Color::White)
        std::tie(kr, kc) = board.whiteKing();
    else
        std::tie(kr, kc) = board.blackKing();

    static constexpr double openFilePenalty      = 0.5;
    static constexpr double semiOpenFilePenalty  = 0.5;
    static constexpr double diagonalOpenPenalty  = 0.5;

    double score = 0.0;

    // Check if king's file has friendly/enemy pawns
    bool friendlyPawnFound = false;
    bool enemyPawnFound    = false;

    for (int r = 0; r < 8; r++)
    {
        const Piece& p = board.at(r, kc);
        if (p.type == PieceType::Pawn)
        {
            if (p.color == side)
                friendlyPawnFound = true;
            else
                enemyPawnFound = true;
        }
    }

    if (!friendlyPawnFound)
        score += openFilePenalty;

    if (!enemyPawnFound)
        score += semiOpenFilePenalty;

    // Diagonal openness
    const int diagDirs[4][2] =
    {
        { 1, 1}, { 1,-1},
        {-1, 1}, {-1,-1}
    };

    for (auto& d : diagDirs)
    {
        int r = kr + d[0];
        int c = kc + d[1];
        int openCount = 0;

        while (board.inBounds(r, c))
        {
            const Piece& p = board.at(r, c);
            if (p.type != PieceType::Empty)
                break; // blocked

            openCount++;
            r += d[0];
            c += d[1];
        }

        if (openCount >= 2)
            score += diagonalOpenPenalty;
    }

    return score;
}
