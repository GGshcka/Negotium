#include <QtWidgets>
#include "PyAPI.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow::instance()->showFullScreen();

    PyAPI::Finalize();
    return app.exec();
}
