#include "agents/Agent_03.h"
#include "engine/board.h"
#include <algorithm>

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------
Agent_03::Agent_03(int depth)
    : searchDepth(depth),
      rng(std::random_device{}()),
      noiseDist(-0.05, 0.05)
{
}

// ------------------------------------------------------------
// chooseMove()
// ------------------------------------------------------------
MoveData Agent_03::chooseMove(const Game& game)
{
    Board board = game.getBoard();             // safe copy
    MoveGenerator gen;
    std::vector<MoveData> moves = gen.generateLegalMoves(board);

    if (moves.empty())
        return MoveData();

    bool maximizing = (board.sideToMove() == Color::White);

    double bestScore = maximizing
        ? -std::numeric_limits<double>::infinity()
        : +std::numeric_limits<double>::infinity();

    MoveData bestMove = moves[0];

    for (const MoveData& m : moves)
    {
        MoveState st;
        st.move = m;
        gen.applyMove(board, st);

        double score = alphabeta(board,
                                 searchDepth - 1,
                                 -std::numeric_limits<double>::infinity(),
                                 +std::numeric_limits<double>::infinity(),
                                 !maximizing,
                                 gen);

        gen.undoMove(board, st);

        double finalScore = score + noiseDist(rng);

        if ((maximizing && finalScore > bestScore) ||
            (!maximizing && finalScore < bestScore))
        {
            bestScore = finalScore;
            bestMove = m;
        }
    }

    return bestMove;
}

// ------------------------------------------------------------
// alphabeta()
// ------------------------------------------------------------
double Agent_03::alphabeta(Board& board,
                           int depth,
                           double alpha,
                           double beta,
                           bool maximizingPlayer,
                           MoveGenerator& gen)
{
    std::vector<MoveData> moves = gen.generateLegalMoves(board);

    // Terminal: no moves → checkmate or stalemate
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

    if (maximizingPlayer)
    {
        double value = -std::numeric_limits<double>::infinity();

        for (const MoveData& m : moves)
        {
            MoveState st;
            st.move = m;
            gen.applyMove(board, st);

            value = std::max(value, alphabeta(board, depth - 1, alpha, beta, false, gen));

            gen.undoMove(board, st);

            alpha = std::max(alpha, value);
            if (alpha >= beta)
                break;  // beta cutoff
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

            value = std::min(value, alphabeta(board, depth - 1, alpha, beta, true, gen));

            gen.undoMove(board, st);

            beta = std::min(beta, value);
            if (beta <= alpha)
                break;  // alpha cutoff
        }

        return value;
    }
}

// ------------------------------------------------------------
// evaluate() — material + endgame heuristics
// ------------------------------------------------------------
double Agent_03::evaluate(const Board& board) const
{
    static constexpr int pieceValues[] = {
        0,   // Empty
        100, // Pawn
        305, // Knight
        333, // Bishop
        563, // Rook
        950, // Queen
        0    // King
    };

    int white = 0;
    int black = 0;
    int whiteRaw = 0;
    int blackRaw = 0;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            const Piece& p = board.at(r, c);

            if (p.type == PieceType::Empty)
                continue;

            int base = pieceValues[(int)p.type];

            if (p.color == Color::White)
            {
                white += base;
                whiteRaw += base;
            }
            else
            {
                black += base;
                blackRaw += base;
            }
        }
    }

    double baseScore = (white - black) / 100.0;

    // ------------------------------------------------------------
    // Material-based endgame trigger (same as Agent_04)
    // ------------------------------------------------------------
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
        rav = 1.0;   // opening
    else if (totalMaterialRaw > 3000)
        rav = 0.6;   // middlegame
    else
        rav = 0.2;   // endgame

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
    // Apply endgame heuristics
    // ------------------------------------------------------------
    if (triggerEndgame)
    {
        MoveGenerator gen;

        // 1. Enhanced King pressure
        baseScore += endgameKingPressure(board, gen);

        // 2. Enemy king mobility restriction
        baseScore += (0.3 * (8 - enemyKingMobility(board, gen)));

        // 3. Own king approaches enemy king when king-only
        if (myMaterialRaw == 0)
            baseScore += kingApproachScore(board);
    }

    return baseScore;
}

// ----------------------------------------------------------------
// Endgame Heuristics
// ----------------------------------------------------------------
int Agent_03::kingDistToEdge(int r, int c) const
{
    int fileDist = std::min(c, 7 - c);
    int rankDist = std::min(r, 7 - r);
    return std::min(fileDist, rankDist);
}

double Agent_03::enemyKingMobility(const Board& board, MoveGenerator& gen) const
{
    int kr, kc;

    if (board.sideToMove() == Color::White)
        std::tie(kr, kc) = board.blackKing();
    else
        std::tie(kr, kc) = board.whiteKing();

    Color enemyColor =
        (board.sideToMove() == Color::White ? Color::Black : Color::White);

    Color myColor = 
        (board.sideToMove() == Color::White ? Color::White : Color::Black);

    int mobility = 0;

    // All 8 king directions
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

        // Cannot move onto friendly piece
        if (target.color == enemyColor)
            continue;

        // Cannot move to a square attacked by us
        if (gen.isSquareAttacked(board, nr, nc, myColor))
            continue;

        mobility++;
    }

    return mobility;
}

double Agent_03::endgameKingPressure(const Board& board, MoveGenerator& gen) const
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

double Agent_03::kingApproachScore(const Board& board) const
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
