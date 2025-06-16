// MainWindow.cpp

#include "MainWindow.h"

MainWindow* MainWindow::s_instance = nullptr;

MainWindow* MainWindow::instance(QWidget *parent) {
    if (!s_instance) {
        s_instance = new MainWindow(parent);
    }
    return s_instance;
}

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {
    setWindowTitle("Negotium");

    QFile sfile(":/main/style");
    if (!sfile.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open stylesheet file.";
        return;
    }
    QString stylesheet = QString::fromUtf8(sfile.readAll());
    this->setStyleSheet(stylesheet);

    qMenuBar = this->menuBar();

    auto *systemMenuItem = new QMenu("System", this);

    /*auto *settingsMenuAction = new QAction("Settings", this);
    connect(settingsMenuAction, &QAction::triggered, this, []() {
        qDebug() << "!!!WIP!!!";
    });
    systemMenuItem->addAction(settingsMenuAction);*/

    auto *exitMenuAction = new QAction("Shutdown", this);
    connect(exitMenuAction, &QAction::triggered, this, &MainWindow::close);
    systemMenuItem->addAction(exitMenuAction);

    qMenuBar->addMenu(systemMenuItem);

    auto *gameMenuItem = new QMenu("Game", this);
    auto *runLevelMenuAction = new QAction("Run level", this);
    connect(runLevelMenuAction, &QAction::triggered, this, &MainWindow::runLevelWindow);
    gameMenuItem->addAction(runLevelMenuAction);

    auto *closeGameMenuAction = new QAction("Close all", this);
    connect(closeGameMenuAction, &QAction::triggered, this, [=, this]() { mdiArea->closeAllSubWindows(); });
    gameMenuItem->addAction(closeGameMenuAction);

    qMenuBar->addMenu(gameMenuItem);

    mdiArea = new QMdiArea(this);
    mdiArea->setBackground(QBrush(QPixmap(":/main/bgPaws")));
    setCentralWidget(mdiArea);

    mdiArea->addSubWindow(codeEditorMdiWindow);
    codeEditorMdiWindow->close();
    mdiArea->addSubWindow(runOutputMdiWindow);
    runOutputMdiWindow->close();
    mdiArea->addSubWindow(viewMdiWindow);
    viewMdiWindow->close();
    mdiArea->addSubWindow(runLevelMdiWindow);
    runLevelMdiWindow->close();
    mdiArea->addSubWindow(guideMdiWindow);
    guideMdiWindow->close();
}

MainWindow::~MainWindow() {
    s_instance = nullptr;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_F1:
            menuBar()->setActiveAction(menuBar()->actions()[0]);
            break;
        case Qt::Key_F2:
            menuBar()->setActiveAction(menuBar()->actions()[1]);
            break;
        case Qt::Key_F5:
            if (AppWideVariables::instance().game != nullptr)
                AppWideVariables::instance().game->Run();
            break;
    }
    QWidget::keyPressEvent(event);
}

void MainWindow::execGame() {
    AppWideVariables::instance().game->Run();
}

void MainWindow::runLevelWindow() {
    runLevelMdiWindow->show();
}

void MainWindow::openLevelMDIWindows() {
    auto *inst = MainWindow::instance();  // Получаем ссылку на экземпляр

    inst->codeEditorMdiWindow->loadLevelCode();

    AppWideVariables::instance().game = new Game(
        inst->codeEditorMdiWindow->textEdit,
        inst->runOutputMdiWindow->textView,
        AppWideVariables::instance().levelPath
    );
    inst->viewMdiWindow->setGame(AppWideVariables::instance().game);

    inst->viewMdiWindow->execute();
    inst->viewMdiWindow->update();
    inst->viewMdiWindow->updateGame();

    inst->guideMdiWindow->setMarkdown(AppWideVariables::instance().markdownText);

    inst->viewMdiWindow->show();
    inst->runOutputMdiWindow->show();
    inst->codeEditorMdiWindow->show();
    inst->guideMdiWindow->show();
}
