#include "ui/gui_qt/windows/ChessWindow.h"
#include "ui/gui_qt/adapters/engine_adapter.h"
#include "ui/gui_qt/widgets/BoardWidget.h"

#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTimer>

#include "agents/Agent_01.h"
#include "agents/Agent_02.h"
#include "agents/Agent_03.h"
#include "agents/Agent_04.h"
#include "agents/Agent_05.h"
#include "agents/Agent_06.h"

ChessWindow::ChessWindow(QWidget* parent)
    : QMainWindow(parent)
{
    engine = new EngineAdapter();
    board = new BoardWidget(engine, this);
    initializeLayout();
    initializeMenus();
    // Game-over signal
    connect(board, &BoardWidget::gameEnded,
            this, &ChessWindow::showGameResultAnimation);
    // Human move signal
    connect(board, &BoardWidget::moveApplied,
            this, &ChessWindow::onMoveCompleted);
}

// ================================================================
// MENUS
// ================================================================
void ChessWindow::initializeMenus()
{
    QMenuBar* bar = menuBar();
    // GAME MENU
    QMenu* gameMenu = bar->addMenu("Game");
    QAction* startGame = gameMenu->addAction("Start Game");
    connect(startGame, &QAction::triggered, this, &ChessWindow::onStartGameClicked);
    // PLAYER SELECTION MENU
    QMenu* agentMenu = bar->addMenu("Players");
    // ===============================================================
    // AI SETTINGS MENU
    // ===============================================================
    QMenu* aiSettings = bar->addMenu("AI Settings");
    // Depth submenu
    QMenu* depthMenu = aiSettings->addMenu("Agent Depth");
    // Depth 1
    QAction* depth1 = depthMenu->addAction("Depth 1");
    connect(depth1, &QAction::triggered, this, [this]() {
        currentAgentDepth = 1;
    });
    // Depth 2
    QAction* depth2 = depthMenu->addAction("Depth 2");
    connect(depth2, &QAction::triggered, this, [this]() {
        currentAgentDepth = 2;
    });
    // Depth 3
    QAction* depth3 = depthMenu->addAction("Depth 3");
    connect(depth3, &QAction::triggered, this, [this]() {
        currentAgentDepth = 3;
    });
    // Depth 4
    QAction* depth4 = depthMenu->addAction("Depth 4");
    connect(depth4, &QAction::triggered, this, [this]() {
        currentAgentDepth = 4;
    });

    QMenu* whiteMenu = agentMenu->addMenu("White Player");
    QMenu* blackMenu = agentMenu->addMenu("Black Player");

    // ===============================================================
    // Players for the White Team
    // ===============================================================
    QAction* whiteHuman = whiteMenu->addAction("Human");
    connect(whiteHuman, &QAction::triggered, this, [this]() {
        setWhiteAgent(nullptr);
    });
    // White = Agent_01
    QAction* whiteA01 = whiteMenu->addAction("Agent_01");
    connect(whiteA01, &QAction::triggered, this, [this]() {
        setWhiteAgent(new Agent_01());
    });
    // White = Agent_02
    QAction* whiteA02 = whiteMenu->addAction("Agent_02");
    connect(whiteA02, &QAction::triggered, this, [this]() {
        setWhiteAgent(new Agent_02(currentAgentDepth));
    });
    // White = Agent_03
    QAction* whiteA03 = whiteMenu->addAction("Agent_03");
    connect(whiteA03, &QAction::triggered, this, [this]() {
        setWhiteAgent(new Agent_03(currentAgentDepth));
    });
    // White = Agent_04
    QAction* whiteA04 = whiteMenu->addAction("Agent_04");
    connect(whiteA04, &QAction::triggered, this, [this]() {
        setWhiteAgent(new Agent_04(currentAgentDepth));
    });
    // White = Agent_05
    QAction* whiteA05 = whiteMenu->addAction("Agent_05");
    connect(whiteA05, &QAction::triggered, this, [this]() {
        setWhiteAgent(new Agent_05(currentAgentDepth));
    });
    // White = Agent_06
    QAction* whiteA06 = whiteMenu->addAction("Agent_06");
    connect(whiteA06, &QAction::triggered, this, [this]() {
        setWhiteAgent(new Agent_06(currentAgentDepth));
    });

    // ===============================================================
    // Players for the Black Team
    // ===============================================================
    // Black = Human
    QAction* blackHuman = blackMenu->addAction("Human");
    connect(blackHuman, &QAction::triggered, this, [this]() {
        setBlackAgent(nullptr);
    });
    // Black = Agent_01
    QAction* blackA01 = blackMenu->addAction("Agent_01");
    connect(blackA01, &QAction::triggered, this, [this]() {
        setBlackAgent(new Agent_01());
    });
    // Black = Agent_02
    QAction* blackA02 = blackMenu->addAction("Agent_02");
    connect(blackA02, &QAction::triggered, this, [this]() {
        setBlackAgent(new Agent_02(currentAgentDepth));
    });
    // Black = Agent_03
    QAction* blackA03 = blackMenu->addAction("Agent_03");
    connect(blackA03, &QAction::triggered, this, [this]() {
        setBlackAgent(new Agent_03(currentAgentDepth));
    });
    // Black = Agent_04
    QAction* blackA04 = blackMenu->addAction("Agent_04");
    connect(blackA04, &QAction::triggered, this, [this]() {
        setBlackAgent(new Agent_04(currentAgentDepth));
    });
    // Black = Agent_05
    QAction* blackA05 = blackMenu->addAction("Agent_05");
    connect(blackA05, &QAction::triggered, this, [this]() {
        setBlackAgent(new Agent_05(currentAgentDepth));
    });
    // Black = Agent_06
    QAction* blackA06 = blackMenu->addAction("Agent_06");
    connect(blackA06, &QAction::triggered, this, [this]() {
        setBlackAgent(new Agent_06(currentAgentDepth));
    });
}

// ================================================================
// LAYOUT + OVERLAY SYSTEM
// ================================================================
void ChessWindow::initializeLayout()
{
    QWidget* container = new QWidget(this);
    QStackedLayout* stack = new QStackedLayout(container);

    stack->setContentsMargins(0, 0, 0, 0);
    stack->setStackingMode(QStackedLayout::StackAll);
    stack->addWidget(board);

    setCentralWidget(container);
    m_stack = stack;
}

void ChessWindow::showGameResultAnimation(const QString& resultText)
{
    createOverlay(resultText);
}

void ChessWindow::createOverlay(const QString& text)
{
    overlay = new QWidget();
    overlay->setAttribute(Qt::WA_StyledBackground, true);
    overlay->setStyleSheet("background-color: rgba(0,0,0,180);");

    QVBoxLayout* layout = new QVBoxLayout(overlay);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 48px; color: white; font-weight: bold;");
    layout->addWidget(label);
    layout->addSpacing(40);

    QPushButton* newGameBtn = new QPushButton("New Game");
    QPushButton* exitBtn = new QPushButton("Exit");
    layout->addWidget(newGameBtn);
    layout->addWidget(exitBtn);

    connect(newGameBtn, &QPushButton::clicked,
            this, &ChessWindow::onNewGameClicked);
    connect(exitBtn, &QPushButton::clicked,
            this, &ChessWindow::onExitClicked);

    m_stack->addWidget(overlay);
    m_stack->setCurrentWidget(overlay);

    board->setEnabled(false);

    auto* effect = new QGraphicsOpacityEffect(label);
    label->setGraphicsEffect(effect);

    auto* anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(1500);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QPropertyAnimation::DeleteWhenStopped);
}

// ================================================================
// NEW GAME BUTTON (after game over)
// ================================================================
void ChessWindow::onNewGameClicked()
{
    if (overlay)
    {
        overlay->deleteLater();
        overlay = nullptr;
    }

    engine->newGame();
    board->setEnabled(true);
    board->update();
}

// ================================================================
// START GAME
// ================================================================
void ChessWindow::onStartGameClicked()
{
    engine->newGame();
    board->setEnabled(true);
    board->update();

    onMoveCompleted();
}

void ChessWindow::onExitClicked()
{
    close();
}

void ChessWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    if (overlay)
        overlay->setGeometry(this->rect());
}

// ================================================================
// TURN SEQUENCER (NEW)
// ================================================================
void ChessWindow::onMoveCompleted()
{
    Game* game = engine->getGamePtr();
    if (!game)
        return;

    if (engine->isGameOver())
    {
        QString result =
            QString::fromStdString(engine->getGameResultMessage());
        emit board->gameEnded(result);
        return;
    }

    const Board& b = game->getBoard();
    Color side = b.sideToMove();

    IChessAgent* agent =
        (side == Color::White ? whiteAgent : blackAgent);

    if (agent)
        QTimer::singleShot(10, this, &ChessWindow::handleAIMove);
}

// ================================================================
// AI MOVE EXECUTION
// ================================================================
void ChessWindow::handleAIMove()
{
    Game* game = engine->getGamePtr();
    if (!game)
        return;

    if (engine->isGameOver())
    {
        QString result =
            QString::fromStdString(engine->getGameResultMessage());
        emit board->gameEnded(result);
        return;
    }

    const Board& b = game->getBoard();
    Color side = b.sideToMove();

    IChessAgent* agent =
        (side == Color::White ? whiteAgent : blackAgent);

    if (!agent)
        return;

    MoveData aiMove = agent->chooseMove(*game);
    engine->applyMove(aiMove);
    board->update();

    onMoveCompleted();
}
