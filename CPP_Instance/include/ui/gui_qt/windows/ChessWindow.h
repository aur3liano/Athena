#pragma once
#include <QMainWindow>
#include <QString>
#include <QStackedLayout>
#include "agents/IChessAgent.h"

class EngineAdapter;
class BoardWidget;

class ChessWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ChessWindow(QWidget* parent = nullptr);

    // Allow GUI menus to select AI players
    void setWhiteAgent(IChessAgent* a) { whiteAgent = a; }
    void setBlackAgent(IChessAgent* a) { blackAgent = a; }

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void showGameResultAnimation(const QString& resultText);
    void onNewGameClicked();
    void onExitClicked();
    void onStartGameClicked();     // start game button
    void handleAIMove();          // performs an AI move
    void onMoveCompleted();        // NEW: unifies turn sequencing (AI or human)

private:
    EngineAdapter* engine = nullptr;
    BoardWidget* board = nullptr;

    IChessAgent* whiteAgent = nullptr;
    IChessAgent* blackAgent = nullptr;

    QWidget* overlay = nullptr;
    QStackedLayout* m_stack = nullptr;

    // --- Agent Depth Setting (default = 3) --
    int currentAgentDepth = 3;
    void initializeLayout();
    void initializeMenus();
    void createOverlay(const QString& text);
};
