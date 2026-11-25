#include "engine/FEN.h"
#include "engine/board.h"
#include <sstream>
#include <cctype>
// Convert rank/file to internal board coordinates
static int rankToRow(char r) { return 8 - (r - '0'); }
static int fileToCol(char f) { return f - 'a'; }

bool FEN::loadFEN(Board& board, const std::string& fen)
{
    std::istringstream iss(fen);
    std::string piecePlacement, sideToken, castling, ep, halfmove, fullmove;
    if (!(iss >> piecePlacement >> sideToken >> castling >> ep >> halfmove >> fullmove))
        return false;
    // ----------------------------------------------------------------------
    // 1. Clear board
    // ----------------------------------------------------------------------
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            board.placePiece(r, c, {PieceType::Empty, Color::None});
    // ----------------------------------------------------------------------
    // 2. Piece placement
    // ----------------------------------------------------------------------
    int row = 0, col = 0;
    for (char ch : piecePlacement)
    {
        if (ch == '/') {
            row++;
            col = 0;
            if (row > 7) return false;
            continue;
        }
        if (std::isdigit(ch)) {
            col += (ch - '0');
            if (col > 8) return false;
            continue;
        }
        if (!std::isalpha(ch) || col >= 8)
            return false;
        PieceType pt;
        switch (std::toupper(ch))
        {
            case 'P': pt = PieceType::Pawn; break;
            case 'N': pt = PieceType::Knight; break;
            case 'B': pt = PieceType::Bishop; break;
            case 'R': pt = PieceType::Rook; break;
            case 'Q': pt = PieceType::Queen; break;
            case 'K': pt = PieceType::King; break;
            default: return false;
        }
        Color clr = std::isupper(ch) ? Color::White : Color::Black;
        board.placePiece(row, col, {pt, clr});
        col++;
    }
    // ----------------------------------------------------------------------
    // 3. Side to move
    // ----------------------------------------------------------------------
    if (sideToken != "w" && sideToken != "b")
        return false;
    board.stm = (sideToken == "w" ? Color::White : Color::Black);
    // ----------------------------------------------------------------------
    // 4. Castling rights (using new CastlingRights struct)
    // ----------------------------------------------------------------------
    board.castling.whiteKingSide  = false;
    board.castling.whiteQueenSide = false;
    board.castling.blackKingSide  = false;
    board.castling.blackQueenSide = false;
    if (castling != "-")
    {
        for (char c : castling)
        {
            switch (c)
            {
                case 'K': board.castling.whiteKingSide  = true; break;
                case 'Q': board.castling.whiteQueenSide = true; break;
                case 'k': board.castling.blackKingSide  = true; break;
                case 'q': board.castling.blackQueenSide = true; break;
                default: return false;
            }
        }
    }
    // ----------------------------------------------------------------------
    // 5. Halfmove / Fullmove counters
    // ----------------------------------------------------------------------
    board.halfmoveClock_ = std::stoi(halfmove);
    board.fullmoveNumber_ = std::stoi(fullmove);
    // ----------------------------------------------------------------------
    // 6. EP field
    // ----------------------------------------------------------------------
    int epRow = -1, epCol = -1;
    if (ep != "-")
    {
        if (ep.size() != 2) return false;
        char file = ep[0];
        char rank = ep[1];
        if (file < 'a' || file > 'h') return false;
        if (rank < '1' || rank > '8') return false;
        epCol = fileToCol(file);
        epRow = rankToRow(rank);
        if (epRow < 0 || epRow > 7)
            epRow = epCol = -1;
    }
    // EP must be rank 2 or 5
    if (epRow != -1)
    {
        if (epRow != 2 && epRow != 5)
        {
            epRow = epCol = -1;
        }
        else
        {
            int victimRow = (epRow == 2 ? 3 : 4);
            const Piece& victim = board.at(victimRow, epCol);
            if (victim.type != PieceType::Pawn)
                epRow = epCol = -1;
        }
    }
    board.epRow = epRow;
    board.epCol = epCol;
    // ----------------------------------------------------------------------
    // 7. Validate castling rights vs actual pieces
    // ----------------------------------------------------------------------
    // WHITE
    const Piece& wk = board.at(7,4);
    if (!(wk.type == PieceType::King && wk.color == Color::White))
    {
        board.castling.whiteKingSide  = false;
        board.castling.whiteQueenSide = false;
    }
    else
    {
        if (board.castling.whiteKingSide)
        {
            const Piece& r = board.at(7,7);
            if (!(r.type == PieceType::Rook && r.color == Color::White))
                board.castling.whiteKingSide = false;
        }
        if (board.castling.whiteQueenSide)
        {
            const Piece& r = board.at(7,0);
            if (!(r.type == PieceType::Rook && r.color == Color::White))
                board.castling.whiteQueenSide = false;
        }
    }
    // BLACK
    const Piece& bk = board.at(0,4);
    if (!(bk.type == PieceType::King && bk.color == Color::Black))
    {
        board.castling.blackKingSide  = false;
        board.castling.blackQueenSide = false;
    }
    else
    {
        if (board.castling.blackKingSide)
        {
            const Piece& r = board.at(0,7);
            if (!(r.type == PieceType::Rook && r.color == Color::Black))
                board.castling.blackKingSide = false;
        }
        if (board.castling.blackQueenSide)
        {
            const Piece& r = board.at(0,0);
            if (!(r.type == PieceType::Rook && r.color == Color::Black))
                board.castling.blackQueenSide = false;
        }
    }
    return true;
}

// ============================================================================
// SAVE FEN
// ============================================================================
std::string FEN::toFEN(const Board& board)
{
    std::ostringstream os;
    // 1. Piece placement
    for (int r = 0; r < 8; r++)
    {
        int empty = 0;
        for (int c = 0; c < 8; c++)
        {
            const Piece& p = board.at(r, c);
            if (p.type == PieceType::Empty)
            {
                empty++;
                continue;
            }
            if (empty > 0)
            {
                os << empty;
                empty = 0;
            }
            char ch;
            switch (p.type)
            {
                case PieceType::Pawn:   ch = (p.color==Color::White?'P':'p'); break;
                case PieceType::Knight: ch = (p.color==Color::White?'N':'n'); break;
                case PieceType::Bishop: ch = (p.color==Color::White?'B':'b'); break;
                case PieceType::Rook:   ch = (p.color==Color::White?'R':'r'); break;
                case PieceType::Queen:  ch = (p.color==Color::White?'Q':'q'); break;
                case PieceType::King:   ch = (p.color==Color::White?'K':'k'); break;
                default: ch = '?';
            }
            os << ch;
        }
        if (empty > 0)
            os << empty;
        if (r != 7)
            os << '/';
    }
    // 2. Side to move
    os << ' ' << (board.sideToMove() == Color::White ? 'w' : 'b');
    // 3. Castling rights
    os << ' ';
    const CastlingRights& cr = board.castling;
    if (!cr.whiteKingSide && !cr.whiteQueenSide &&
        !cr.blackKingSide && !cr.blackQueenSide)
    {
        os << '-';
    }
    else
    {
        if (cr.whiteKingSide)  os << 'K';
        if (cr.whiteQueenSide) os << 'Q';
        if (cr.blackKingSide)  os << 'k';
        if (cr.blackQueenSide) os << 'q';
    }
    // 4. EP square
    os << ' ';
    if (board.epRow == -1)
    {
        os << '-';
    }
    else
    {
        char file = char('a' + board.epCol);
        char rank = char('8' - board.epRow);
        os << file << rank;
    }
    // 5. Halfmove / Fullmove clocks
    os << ' ' << board.halfmoveClock_
       << ' ' << board.fullmoveNumber_;
    return os.str();
}
