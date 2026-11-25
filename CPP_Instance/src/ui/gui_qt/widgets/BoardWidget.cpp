#include "ui/gui_qt/widgets/BoardWidget.h"
#include "ui/gui_qt/adapters/engine_adapter.h"
#include "engine/move_state.h"
#include "engine/board.h"
#include <QInputDialog>
#include <QMouseEvent>
#include <QPainter>
#include <algorithm>

BoardWidget::BoardWidget(EngineAdapter* adapter, QWidget* parent)
    : QWidget(parent), engine(adapter)
{
    setMinimumSize(400, 400);
    loadPieceImages();
}

char BoardWidget::askPromotionPiece()
{
    QStringList items = { "Queen", "Rook", "Bishop", "Knight" };

    bool ok = false;
    QString choice = QInputDialog::getItem(
        this,
        "Promote Pawn",
        "Choose promotion piece:",
        items,
        0,
        false,
        &ok
    );

    if (!ok) return 5;  // Default to Queen

    if (choice == "Queen")  return 'Q';
    if (choice == "Rook")   return 'R';
    if (choice == "Bishop") return 'B';
    if (choice == "Knight") return 'N';

    return 'Q'; // default
}

std::pair<int,int> BoardWidget::pixelToSquare(int x, int y) const
{
    int size = std::min(width(), height());
    int squareSize = size / 8;

    int file = x / squareSize;
    int rank = y / squareSize;

    // Bounds checking
    if (file < 0 || file > 7 || rank < 0 || rank > 7)
        return {-1, -1};

    return {rank, file};
}

void BoardWidget::loadPieceImages()
{
    pieceImages[ 1] = QPixmap(":/chess/pieces/white_pawn.png");
    pieceImages[ 2] = QPixmap(":/chess/pieces/white_knight.png");
    pieceImages[ 3] = QPixmap(":/chess/pieces/white_bishop.png");
    pieceImages[ 4] = QPixmap(":/chess/pieces/white_rook.png");
    pieceImages[ 5] = QPixmap(":/chess/pieces/white_queen.png");
    pieceImages[ 6] = QPixmap(":/chess/pieces/white_king.png");

    pieceImages[-1] = QPixmap(":/chess/pieces/black_pawn.png");
    pieceImages[-2] = QPixmap(":/chess/pieces/black_knight.png");
    pieceImages[-3] = QPixmap(":/chess/pieces/black_bishop.png");
    pieceImages[-4] = QPixmap(":/chess/pieces/black_rook.png");
    pieceImages[-5] = QPixmap(":/chess/pieces/black_queen.png");
    pieceImages[-6] = QPixmap(":/chess/pieces/black_king.png");
}

void BoardWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    int size = std::min(width(), height());
    int squareSize = size / 8;

    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            bool lightSquare = ((rank + file) % 2 == 0);
            QColor baseColor = lightSquare
                               ? QColor(240, 217, 181)
                               : QColor(181, 136, 99);

            QRect rect(file * squareSize, rank * squareSize, squareSize, squareSize);

            // Draw base square
            painter.fillRect(rect, baseColor);

            // Draw piece if any
            int piece = engine->pieceAt(rank, file);
            if (piece != 0)
            {
                const QPixmap& pix = pieceImages[piece];
                painter.drawPixmap(
                    rect,
                    pix.scaled(squareSize, squareSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)
                );
            }

            // ---------------------------------------------------
            // Highlight selected square
            // ---------------------------------------------------
            if (squareSelected && rank == selectedRank && file == selectedFile)
            {
                painter.fillRect(rect, QColor(255, 255, 0, 120));  // yellow overlay
            }

            // ---------------------------------------------------
            // Highlight legal targets
            // ---------------------------------------------------
            for (const auto& target : legalTargets)
            {
                if (target.first == rank && target.second == file)
                {
                    painter.fillRect(rect, QColor(0, 255, 0, 120));  // green overlay
                }
            }
        }
    }
}

void BoardWidget::mousePressEvent(QMouseEvent* event)
{
    auto pos = event->position();
    auto [rank, file] = pixelToSquare(pos.x(), pos.y());

    // Click outside board
    if (rank == -1 || file == -1)
        return;

    // If nothing selected yet → select
    if (!squareSelected)
    {
        selectSquare(rank, file);
        update();
        return;
    }

    // If selecting same square → clear
    if (rank == selectedRank && file == selectedFile)
    {
        clearSelection();
        update();
        return;
    }

    // Check if the clicked square is a legal target
    for (auto& target : legalTargets)
    {
        if (target.first == rank && target.second == file)
        {
            MoveData move = engine->getMoveData(selectedRank, selectedFile, rank, file);
            // If promotion
            if (move.promotion != 0)
            {
                char chosen = askPromotionPiece();
                move.promotion = chosen;

                if (engine->applyMove(move))
                {
                    clearSelection();
                    update();

                    emit moveApplied();

                    if (engine->isGameOver())
                    {
                        QString result = QString::fromStdString(engine->getGameResultMessage());
                        emit gameEnded(result);
                    }
                }
                return;
            }
            // Now ALWAYS use MoveData version:
            if (engine->applyMove(move))
            {
                clearSelection();
                update();

                emit moveApplied();

                // NEW: Check for end of game
                if (engine->isGameOver())
                {
                    QString result = QString::fromStdString(engine->getGameResultMessage());
                    emit gameEnded(result);
                }
                return;
            }
        }
    }
    // Otherwise select new square
    selectSquare(rank, file);
    update();
}

void BoardWidget::selectSquare(int rank, int file)
{
    clearSelection();

    int piece = engine->pieceAt(rank, file);
    if (piece == 0)
        return;

    squareSelected = true;
    selectedRank = rank;
    selectedFile = file;

    legalTargets = engine->legalTargetsFrom(rank, file);
}

void BoardWidget::clearSelection()
{
    squareSelected = false;
    selectedRank = -1;
    selectedFile = -1;
    legalTargets.clear();
}
