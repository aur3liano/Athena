#include "engine/game.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include "engine/FEN.h"

#include <unordered_map>
#include <sstream>

// ---------------------------------------------------------------
// Utility: strip halfmove/fullmove counters from FEN
// ---------------------------------------------------------------
static std::string fenWithoutCounters(const std::string& fen)
{
    // Keep only fields 1–4:
    // pieces | side | castling | ep
    std::istringstream iss(fen);
    std::string a, b, c, d;
    iss >> a >> b >> c >> d;
    return a + " " + b + " " + c + " " + d;
}

// ---------------------------------------------------------------
// Game constructor
// ---------------------------------------------------------------
Game::Game()
{
    board.resetToStartingPosition();
    updateRepetitionTable();  // record starting position
}

// ---------------------------------------------------------------
// Get legal moves
// ---------------------------------------------------------------
std::vector<MoveData> Game::getLegalMoves()
{
    MoveGenerator gen;
    return gen.generateLegalMoves(board);
}

// ---------------------------------------------------------------
// Apply move (validated)
// ---------------------------------------------------------------
bool Game::applyMove(const MoveData& m)
{
    MoveGenerator gen;
    auto legal = getLegalMoves();

    bool found = false;
    for (const MoveData& lm : legal)
    {
        if (lm.fromRow == m.fromRow &&
            lm.fromCol == m.fromCol &&
            lm.toRow   == m.toRow   &&
            lm.toCol   == m.toCol   &&
            lm.promotion == m.promotion &&
            lm.isEnPassant == m.isEnPassant)
        {
            found = true;
            break;
        }
    }

    if (!found)
        return false;

    MoveState st;
    st.move = m;
    gen.applyMove(board, st);

    updateRepetitionTable();
    return true;
}

// ---------------------------------------------------------------
// Repetition table update
// ---------------------------------------------------------------
void Game::updateRepetitionTable()
{
    std::string fen = FEN::toFEN(board);
    std::string key = fenWithoutCounters(fen);
    repetitionTable[key]++;
}

// ---------------------------------------------------------------
// Draw rule checks
// ---------------------------------------------------------------
bool Game::isThreefoldRepetition() const
{
    std::string fen = FEN::toFEN(board);
    std::string key = fenWithoutCounters(fen);

    auto it = repetitionTable.find(key);
    if (it == repetitionTable.end())
        return false;

    return it->second >= 3;
}

bool Game::isFiftyMoveRule() const
{
    return board.halfmoveClock() >= 100;
}

bool Game::isDeadPosition() const
{
    int whiteBishops = 0;
    int blackBishops = 0;
    int whiteKnights = 0;
    int blackKnights = 0;
    bool whiteHasOther = false;
    bool blackHasOther = false;
    int whiteBishopColor = -1;
    int blackBishopColor = -1;

    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            const Piece& p = board.at(r, c);
            if (p.type == PieceType::Empty) continue;

            switch (p.type)
            {
                case PieceType::Pawn:
                case PieceType::Rook:
                case PieceType::Queen:
                    if (p.color == Color::White) whiteHasOther = true;
                    else                        blackHasOther = true;
                    break;

                case PieceType::Knight:
                    if (p.color == Color::White) whiteKnights++;
                    else                        blackKnights++;
                    break;

                case PieceType::Bishop:
                {
                    int squareColor = (r + c) % 2;
                    if (p.color == Color::White)
                    {
                        whiteBishops++;
                        if (whiteBishopColor == -1)
                            whiteBishopColor = squareColor;
                        else if (whiteBishopColor != squareColor)
                            whiteHasOther = true;
                    }
                    else
                    {
                        blackBishops++;
                        if (blackBishopColor == -1)
                            blackBishopColor = squareColor;
                        else if (blackBishopColor != squareColor)
                            blackHasOther = true;
                    }
                }
                break;

                default:
                    break;
            }
        }
    }

    if (whiteHasOther || blackHasOther)
        return false;

    int whiteMinors = whiteBishops + whiteKnights;
    int blackMinors = blackBishops + blackKnights;

    // K vs K
    if (whiteMinors == 0 && blackMinors == 0) return true;

    // K + minor vs K
    if (whiteMinors == 1 && blackMinors == 0) return true;
    if (whiteMinors == 0 && blackMinors == 1) return true;

    // KB vs KB same-color
    if (whiteBishops == 1 && blackBishops == 1 &&
        whiteKnights == 0 && blackKnights == 0 &&
        whiteBishopColor == blackBishopColor)
        return true;

    return false;
}

// ---------------------------------------------------------------
// Evaluate complete game state
// ---------------------------------------------------------------
GameResult Game::evaluateGameState()
{
    MoveGenerator gen;
    auto moves = getLegalMoves();
    Color side = board.sideToMove();

    auto [kr, kc] = (side == Color::White ?
                    board.whiteKing() : board.blackKing());
    Color opp = (side == Color::White ? Color::Black : Color::White);

    bool inCheck = gen.isSquareAttacked(board, kr, kc, opp);

    // Checkmate / Stalemate
    if (moves.empty())
    {
        if (inCheck)
            return (side == Color::White ?
                    GameResult::CheckmateBlackWins :
                    GameResult::CheckmateWhiteWins);
        else
            return GameResult::Stalemate;
    }

    // 50 move rule
    if (isFiftyMoveRule())
        return GameResult::DrawFiftyMoveRule;

    // Threefold repetition
    if (isThreefoldRepetition())
        return GameResult::DrawThreefoldRepetition;

    // Dead position
    if (isDeadPosition())
        return GameResult::DrawDeadPosition;

    return GameResult::InProgress;
}

// ---------------------------------------------------------------
// String message
// ---------------------------------------------------------------
std::string Game::getGameResultMessage() const
{
    Game* self = const_cast<Game*>(this);
    GameResult r = self->evaluateGameState();

    switch (r)
    {
        case GameResult::CheckmateWhiteWins:
            return "Checkmate! White wins.";
        case GameResult::CheckmateBlackWins:
            return "Checkmate! Black wins.";
        case GameResult::Stalemate:
            return "Stalemate!";
        case GameResult::DrawFiftyMoveRule:
            return "Draw by 50-move rule.";
        case GameResult::DrawThreefoldRepetition:
            return "Draw by threefold repetition.";
        case GameResult::DrawDeadPosition:
            return "Draw by insufficient material.";
        case GameResult::InProgress:
        default:
            return "Game in progress.";
    }
}
