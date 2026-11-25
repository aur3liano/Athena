#pragma once
#include <QWidget>
#include <QPixmap>
#include <map>
#include <vector>
#include <utility>

class EngineAdapter;

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget(EngineAdapter* adapter, QWidget* parent = nullptr);

signals:
    void gameEnded(const QString& resultText);
    void moveApplied();    // NEW: emitted after a human successfully makes a move

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    EngineAdapter* engine;
    bool squareSelected = false;
    int selectedRank = -1;
    int selectedFile = -1;
    std::vector<std::pair<int,int>> legalTargets;
    std::map<int, QPixmap> pieceImages;

    void loadPieceImages();
    std::pair<int,int> pixelToSquare(int x, int y) const;
    void selectSquare(int rank, int file);
    void clearSelection();
    char askPromotionPiece();
};