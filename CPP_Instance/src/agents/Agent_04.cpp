#include "agents/Agent_04.h"
#include "engine/board.h"
#include <algorithm>

// ----------------------------------------------------------------
// Piece-Square Tables
// ----------------------------------------------------------------
const int Agent_04::pawnTable[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

const int Agent_04::knightTable[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

const int Agent_04::bishopTable[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

const int Agent_04::rookTable[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 5, 10, 10, 10, 10, 10, 10,  5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    { 0,  0,  0,  5,  5,  0,  0,  0}
};

const int Agent_04::queenTable[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

const int Agent_04::kingTable[8][8] = {
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
Agent_04::Agent_04(int depth)
    : searchDepth(depth),
      rng(std::random_device{}()),
      noiseDist(-0.05, 0.05)
{
}

// ----------------------------------------------------------------
// Move ordering helper
// ----------------------------------------------------------------
void Agent_04::orderMoves(Board& board,
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
MoveData Agent_04::chooseMove(const Game& game)
{
    Board board = game.getBoard();
    MoveGenerator gen;
    std::vector<MoveData> moves = gen.generateLegalMoves(board);

    if (moves.empty())
        return MoveData();

    bool maximizing = (board.sideToMove() == Color::White);

    orderMoves(board, moves, gen);

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

// ----------------------------------------------------------------
// Alpha-beta with move ordering
// ----------------------------------------------------------------
double Agent_04::alphabeta(Board& board,
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
double Agent_04::evaluate(const Board& board) const
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
                white     += score;
                whiteRaw  += base;
            }
            else
            {
                black     += score;
                blackRaw  += base;
            }
        }
    }

    double baseScore = (white - black) / 100.0;

    // ------------------------------------------------------------
    // Endgame trigger using material-based ratio
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
        if (myMaterialRaw == 0)
            baseScore += kingApproachScore(board);
    }

    return baseScore;
}

// ----------------------------------------------------------------
// Endgame capability — kingDistToEdge() and endgameKingPressure()
// ----------------------------------------------------------------
int Agent_04::kingDistToEdge(int r, int c) const
{
    int fileDist = std::min(c, 7 - c);
    int rankDist = std::min(r, 7 - r);
    return std::min(fileDist, rankDist);
}

double Agent_04::enemyKingMobility(const Board& board, MoveGenerator& gen) const
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

double Agent_04::endgameKingPressure(const Board& board, MoveGenerator& gen) const
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

double Agent_04::kingApproachScore(const Board& board) const
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
