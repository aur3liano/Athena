#include "agents/Agent_02.h"
#include "engine/board.h"
#include <algorithm>

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------
Agent_02::Agent_02(int depth)
    : searchDepth(depth),
      rng(std::random_device{}()),
      noiseDist(-0.05, 0.05)
{
}

// ------------------------------------------------------------
// chooseMove()
// ------------------------------------------------------------
MoveData Agent_02::chooseMove(const Game& game)
{
    // Work on a copy of the board (safe and correct)
    Board board = game.getBoard();
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

        // NEW: use dynamic searchDepth instead of static SEARCH_DEPTH
        double score = minimax(board,
                               searchDepth - 1,
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
// minimax()
// ------------------------------------------------------------
double Agent_02::minimax(Board& board, int depth, bool maximizingPlayer,
                         MoveGenerator& gen)
{
    std::vector<MoveData> moves = gen.generateLegalMoves(board);

    if (moves.empty())
    {
        Color side = board.sideToMove();
        Color enemy = (side == Color::White ? Color::Black : Color::White);

        auto [kr, kc] =
            (side == Color::White ? board.whiteKing() : board.blackKing());

        bool inCheck = gen.isSquareAttacked(board, kr, kc, enemy);

        if (inCheck)
            return (side == Color::White ? -MATE_SCORE : +MATE_SCORE);
        else
            return 0.0;
    }

    if (depth == 0)
        return evaluate(board);

    double bestScore = maximizingPlayer
        ? -std::numeric_limits<double>::infinity()
        : +std::numeric_limits<double>::infinity();

    for (const MoveData& m : moves)
    {
        MoveState st;
        st.move = m;

        gen.applyMove(board, st);
        double score = minimax(board, depth - 1, !maximizingPlayer, gen);
        gen.undoMove(board, st);

        if (maximizingPlayer)
            bestScore = std::max(bestScore, score);
        else
            bestScore = std::min(bestScore, score);
    }

    return bestScore;
}

// ------------------------------------------------------------
// evaluate()
// ------------------------------------------------------------
double Agent_02::evaluate(const Board& board) const
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

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            const Piece& p = board.at(r, c);
            if (p.color == Color::White)
                white += pieceValues[(int)p.type];
            else if (p.color == Color::Black)
                black += pieceValues[(int)p.type];
        }
    }

    return (white - black) / 100.0;
}
