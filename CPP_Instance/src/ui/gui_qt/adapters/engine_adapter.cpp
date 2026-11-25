#include "ui/gui_qt/adapters/engine_adapter.h"
#include "engine/game.h"
#include "engine/board.h"
#include "engine/move_generator.h"
#include "engine/FEN.h"

// ============================================================
// Constructor
// ============================================================
EngineAdapter::EngineAdapter()
{
    gamePtr = new Game();
    newGame();
}

// ============================================================
// Game lifecycle
// ============================================================
void EngineAdapter::newGame()
{
    // Best approach: create a fresh Game() object
    *gamePtr = Game();
}

void EngineAdapter::loadFEN(const std::string& fen)
{
    FEN::loadFEN(gamePtr->getBoard(), fen);
}

std::string EngineAdapter::exportFEN() const
{
    return FEN::toFEN(gamePtr->getBoard());
}

// ============================================================
// Board access
// ============================================================
int EngineAdapter::pieceAt(int rank, int file) const
{
    const Board& b = gamePtr->getBoard();

    if (!b.inBounds(rank, file))
        return 0;

    const Piece& p = b.at(rank, file);   // from board.h / board.cpp
                                          // returns Piece { type, color }

    if (p.type == PieceType::Empty || p.color == Color::None)
        return 0;

    // Map engine piece → GUI integer code
    int base = 0;
    switch (p.type)
    {
        case PieceType::Pawn:   base = 1; break;
        case PieceType::Knight: base = 2; break;
        case PieceType::Bishop: base = 3; break;
        case PieceType::Rook:   base = 4; break;
        case PieceType::Queen:  base = 5; break;
        case PieceType::King:   base = 6; break;

        case PieceType::Empty: default:
            return 0;
    }

    return (p.color == Color::White ? base : -base);
}

// ============================================================
// Move generation (GUI-friendly)
// ============================================================
std::vector<std::pair<int,int>> EngineAdapter::legalTargetsFrom(int rank, int file) const
{
    std::vector<std::pair<int,int>> out;

    auto moves = gamePtr->getLegalMoves();  // from game.cpp
    for (const MoveData& m : moves)
    {
        if (m.fromRow == rank && m.fromCol == file)
            out.emplace_back(m.toRow, m.toCol);
    }

    return out;
}

// ============================================================
// Move application (GUI-friendly)
// ============================================================
MoveData EngineAdapter::getMoveData(int fromRank, int fromFile, int toRank, int toFile) const
{
    auto moves = gamePtr->getLegalMoves();
    for (const MoveData& m : moves)
    {
        if (m.fromRow == fromRank && m.fromCol == fromFile &&
            m.toRow   == toRank   && m.toCol   == toFile)
        {
            return m;
        }
    }
    return MoveData{};
}

bool EngineAdapter::applyMove(int fromRank, int fromFile, int toRank, int toFile)
{
    MoveData m;
    m.fromRow = fromRank;
    m.fromCol = fromFile;
    m.toRow   = toRank;
    m.toCol   = toFile;

    // Default values – your engine handles promotion/EP rules
    m.promotion = 0;
    m.isEnPassant = false;

    return gamePtr->applyMove(m);   // from game.cpp
}

bool EngineAdapter::applyMove(const MoveData& move)
{
    return gamePtr->applyMove(move);
}

bool EngineAdapter::isGameOver() const
{
    return gamePtr->isGameOver();
}

std::string EngineAdapter::getGameResultMessage() const
{
    return gamePtr->getGameResultMessage();
}
