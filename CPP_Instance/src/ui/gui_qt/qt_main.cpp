#include "ui/gui_qt/windows/ChessWindow.h"
#include <random>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    ChessWindow window;
    window.show();

    return app.exec();
}
