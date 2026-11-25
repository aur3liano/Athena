#include "engine/notation.h"
#include "engine/move_generator.h"
#include "engine/move_state.h"
#include <cctype>
#include <algorithm>

// ============================================================================
// Piece letter helper
// ============================================================================
std::string Notation::pieceLetter(PieceType t)
{
    switch (t)
    {
        case PieceType::King:   return "K";
        case PieceType::Queen:  return "Q";
        case PieceType::Rook:   return "R";
        case PieceType::Bishop: return "B";
        case PieceType::Knight: return "N";
        default:                return "";
    }
}

// ============================================================================
// Castling detection helpers
// ============================================================================
bool Notation::isCastleKingSide(const std::string& san)
{
    return (san == "O-O" || san == "0-0");
}

bool Notation::isCastleQueenSide(const std::string& san)
{
    return (san == "O-O-O" || san == "0-0-0");
}

// ============================================================================
// algebraicToMove("e2e4", "a7a8q", etc.)
// ============================================================================
MoveData Notation::algebraicToMove(const std::string& s,const Board& board,Color /*sideToMove*/)
{
    (void)board;
    MoveData md{};
    if (s.size() < 4)
        return md;
    md.fromCol = colFromFile(s[0]);
    md.fromRow = rowFromRank(s[1]);
    md.toCol   = colFromFile(s[2]);
    md.toRow   = rowFromRank(s[3]);
    if (s.size() == 5)
        md.promotion = char(std::toupper(s[4]));
    return md;
}

// ============================================================================
// move → coordinate string ("e2e4")
// ============================================================================
std::string Notation::moveToCoordinate(const MoveData& m)
{
    std::string s;
    s += fileFromCol(m.fromCol);
    s += rankFromRow(m.fromRow);
    s += fileFromCol(m.toCol);
    s += rankFromRow(m.toRow);
    if (m.promotion)
        s += char(std::tolower(m.promotion));

    return s;
}

// ============================================================================
// SAN → MoveData
// ============================================================================
MoveData Notation::sanToMove(const std::string& san, Board& board)
{
    MoveData invalid{};
    invalid.fromRow = -1;
    MoveGenerator gen;
    auto legal = gen.generateLegalMoves(board);
    // Remove check/mate symbols
    std::string trimmed = san;
    trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), '+'), trimmed.end());
    trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), '#'), trimmed.end());
    // --- Castling ---
    if (isCastleKingSide(trimmed))
    {
        for (const MoveData& m : legal)
            if (m.fromCol == 4 && m.toCol == 6)
                return m;
        return invalid;
    }
    if (isCastleQueenSide(trimmed))
    {
        for (const MoveData& m : legal)
            if (m.fromCol == 4 && m.toCol == 2)
                return m;
        return invalid;
    }
    // --- Promotion ---
    char promo = 0;
    size_t eq = trimmed.find('=');
    if (eq != std::string::npos && eq + 1 < trimmed.size())
    {
        promo = char(std::toupper(trimmed[eq + 1]));
        trimmed = trimmed.substr(0, eq);
    }
    // --- Piece type ---
    PieceType piece = PieceType::Pawn;
    if (!trimmed.empty() && std::isupper(trimmed[0]))
    {
        switch (trimmed[0])
        {
            case 'K': piece = PieceType::King;   break;
            case 'Q': piece = PieceType::Queen;  break;
            case 'R': piece = PieceType::Rook;   break;
            case 'B': piece = PieceType::Bishop; break;
            case 'N': piece = PieceType::Knight; break;
            default: break;
        }
        trimmed = trimmed.substr(1);
    }
    // remove capture 'x'
    size_t x = trimmed.find('x');
    if (x != std::string::npos)
        trimmed.erase(x, 1);
    if (trimmed.size() < 2)
        return invalid;
    int toCol = colFromFile(trimmed[trimmed.size() - 2]);
    int toRow = rowFromRank(trimmed[trimmed.size() - 1]);
    // --- Disambiguation ---
    int disFile = -1;
    int disRank = -1;
    for (size_t i = 0; i + 2 < trimmed.size(); i++)
    {
        char d = trimmed[i];
        if (std::isdigit(d))
            disRank = rowFromRank(d);
        else if (std::isalpha(d))
            disFile = colFromFile(d);
    }
    // --- Match against legal moves ---
    for (const MoveData& m : legal)
    {
        if (m.toCol != toCol || m.toRow != toRow)
            continue;
        const Piece& p = board.at(m.fromRow, m.fromCol);
        if (p.type != piece)
            continue;
        if (disFile != -1 && m.fromCol != disFile)
            continue;
        if (disRank != -1 && m.fromRow != disRank)
            continue;
        if (promo && m.promotion != promo)
            continue;
        return m;
    }
    return invalid;
}

// ============================================================================
// moveToSAN
// ============================================================================
std::string Notation::moveToSAN(const MoveData& m, Board& board)
{
    std::string san;
    const Piece moved = board.at(m.fromRow, m.fromCol);
    const Piece target = board.at(m.toRow, m.toCol);
    // --- Castling ---
    if (moved.type == PieceType::King && std::abs(m.toCol - m.fromCol) == 2)
        return (m.toCol == 6 ? "O-O" : "O-O-O");
    // --- Piece letter ---
    san += pieceLetter(moved.type);
    // Disambiguation
    MoveGenerator gen;
    auto legal = gen.generateLegalMoves(board);
    bool needFile = false;
    bool needRank = false;
    for (const MoveData& other : legal)
    {
        if (other.fromRow == m.fromRow && other.fromCol == m.fromCol)
            continue;
        const Piece& op = board.at(other.fromRow, other.fromCol);
        if (op.type != moved.type)
            continue;
        if (other.toRow == m.toRow && other.toCol == m.toCol)
        {
            if (other.fromCol != m.fromCol) needFile = true;
            if (other.fromRow != m.fromRow) needRank = true;
        }
    }
    // Pawn capture requires file disambiguation
    if (moved.type == PieceType::Pawn && target.type != PieceType::Empty)
        needFile = true;
    if (needFile)
        san += fileFromCol(m.fromCol);
    if (needRank)
        san += rankFromRow(m.fromRow);
    // capture indicator
    if (target.type != PieceType::Empty || m.isEnPassant)
    {
        if (moved.type == PieceType::Pawn && san.empty())
            san += fileFromCol(m.fromCol);
        san += "x";
    }
    // destination
    san += fileFromCol(m.toCol);
    san += rankFromRow(m.toRow);
    // promotion
    if (m.promotion)
    {
        san += "=";
        san += char(std::toupper(m.promotion));
    }
    // check / checkmate
    Board copy = board;
    MoveState st;
    st.move = m;
    gen.applyMove(copy, st);
    auto [kr, kc] = (copy.sideToMove() == Color::White ? copy.whiteKing() : copy.blackKing());
    Color opp = (copy.sideToMove() == Color::White ? Color::Black : Color::White);
    bool inCheck = gen.isSquareAttacked(copy, kr, kc, opp);
    if (inCheck)
    {
        auto oppMoves = gen.generateLegalMoves(copy);
        san += (oppMoves.empty() ? "#" : "+");
    }
    return san;
}
