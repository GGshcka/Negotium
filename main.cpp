#include <Qt>
#include <QtWidgets>
#include "Game.h"
#include "PyAPI.h"
#include <python3.13/Python.h>

Game *gameView;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Negotium");

        QPixmap pixmap(32, 32);
        pixmap.fill(Qt::transparent);

        QMdiArea *mdiArea = new QMdiArea(this);
        mdiArea->setBackground(QBrush(qRgb(87, 70, 123)));
        mdiArea->setTabsClosable(false);
        setCentralWidget(mdiArea);

        //! CODE EDITOR MDI START
        QMdiSubWindow *codeEditorMdiWindow = new QMdiSubWindow();
        codeEditorMdiWindow->setWindowTitle(QString("Редактор кода"));
        codeEditorMdiWindow->setGeometry(1550, 40, 300, 800);
        codeEditorMdiWindow->setWindowIcon(pixmap);

        auto *codeEditorCentralWidget = new QWidget(codeEditorMdiWindow);
        codeEditorMdiWindow->setWidget(codeEditorCentralWidget);

        auto *execButton = new QAction();
        execButton->setIcon(QIcon("../Resources/Icons/iconExec.png"));
        execButton->setToolTip("Execute your code. You can execute, on press F5.");

        connect(execButton, &QAction::triggered, this, &MainWindow::execGame);

        codeEditorToolBar = new QToolBar();
        codeEditorToolBar->setMovable(false);
        codeEditorToolBar->addAction(execButton);

        edit = new QTextEdit();
        edit->setTabStopDistance(12);

        QFile file("main.ssf");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug("Can't open file.");
        }
        QTextStream out(&file);
        edit->setText(out.readAll());
        file.close();

        codeEditorLayout = new QGridLayout(codeEditorCentralWidget);
        codeEditorLayout->addWidget(codeEditorToolBar, 0, 0);
        codeEditorLayout->addWidget(edit, 1, 0);

        mdiArea->addSubWindow(codeEditorMdiWindow);
        codeEditorMdiWindow->show();
        //! CODE EDITOR MDI START\n-----------------\n
        //! RUN OUTPUT MDI START
        QMdiSubWindow *runOutputMdiWindow = new QMdiSubWindow();
        runOutputMdiWindow->setWindowTitle(QString("Вывод"));
        runOutputMdiWindow->setGeometry(20, 850, 850, 200);
        runOutputMdiWindow->setWindowIcon(pixmap);

        auto *runOutputCentralWidget = new QWidget(runOutputMdiWindow);
        runOutputMdiWindow->setWidget(runOutputCentralWidget);

        debugText = new QTextEdit();
        debugText->setReadOnly(true);

        runOutputLayout = new QGridLayout(runOutputCentralWidget);
        runOutputLayout->addWidget(debugText, 0, 0);

        mdiArea->addSubWindow(runOutputMdiWindow);
        runOutputMdiWindow->show();
        //! RUN OUTPUT MDI END\n-----------------\n
        //! VIEW MDI START
        QMdiSubWindow *viewMdiWindow = new QMdiSubWindow();
        viewMdiWindow->setWindowTitle(QString("Поле"));
        viewMdiWindow->setGeometry(20, 40, 1450, 800);
        viewMdiWindow->setWindowIcon(pixmap);

        auto *viewCentralWidget = new QWidget(viewMdiWindow);
        viewMdiWindow->setWidget(viewCentralWidget);

        auto *scrollVBar = new QScrollBar();
        auto *scrollHBar = new QScrollBar();

        gameView = new Game(edit, debugText);
        view = new QGraphicsView();
        view->setScene(gameView->scene);
        view->setVerticalScrollBar(scrollVBar);
        view->setHorizontalScrollBar(scrollHBar);
        view->scale(0.3, 0.3);

        zoomInButton = new QAction();
        zoomInButton->setIcon(QIcon("../Resources/Icons/iconZoomIn.png"));
        zoomInButton->setToolTip("Zoom In");

        zoomOutButton = new QAction();
        zoomOutButton->setIcon(QIcon("../Resources/Icons/iconZoomOut.png"));
        zoomOutButton->setToolTip("Zoom Out");

        connect(zoomInButton, &QAction::triggered, this, &MainWindow::zoomIn);
        connect(zoomOutButton, &QAction::triggered, this, &MainWindow::zoomOut);

        viewToolBar = new QToolBar();
        viewToolBar->setMovable(false);

        viewLayout = new QGridLayout(viewCentralWidget);
        viewLayout->addWidget(viewToolBar, 0, 0);
        viewLayout->addWidget(view, 1, 0);

        updateToolbarIcons(gameView->PlayerHPGet());
        connect(gameView, &Game::playerHealthChanged, this, [this](unsigned int newHealth) {
            updateToolbarIcons(newHealth);
        });

        scrollVBar->setValue(0);
        scrollHBar->setValue(0);

        mdiArea->addSubWindow(viewMdiWindow);
        viewMdiWindow->show();
        //! VIEW MDI END

        QFile sfile("../Resources/style.scss");
        if(!sfile.open(QIODevice::ReadOnly))
        {
            qDebug() << "Cannot open stylesheet file.";
            return;
        }
        QString stylesheet = QString::fromUtf8(sfile.readAll());
        this->setStyleSheet(stylesheet);
    }

    void updateToolbarIcons(unsigned int iconCount) const {
        viewToolBar->clear();
        viewToolBar->addAction(zoomInButton);
        viewToolBar->addAction(zoomOutButton);
        auto *spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        viewToolBar->addWidget(spacer);

        for (int i = 1; i <= iconCount; ++i) {
            auto *heartsIcons = new QAction(QPixmap("../Resources/Icons/heart.png"), nullptr);
            heartsIcons->setObjectName("Hearts");
            viewToolBar->addAction(heartsIcons);
        }

        viewToolBar->update();
    }

    QGridLayout *codeEditorLayout;
    QGridLayout *runOutputLayout;
    QGridLayout *viewLayout;
    QTextEdit *edit;
    QTextEdit *debugText;
    QGraphicsView *view;
    QToolBar *viewToolBar;
    QToolBar *codeEditorToolBar;
    QAction *zoomInButton, *zoomOutButton;

protected:
    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
            case Qt::Key_F1:
                view->scale(1.2, 1.2);
                view->update();
                break;
            case Qt::Key_F2:
                view->scale(0.8, 0.8);
                view->update();
                break;
            case Qt::Key_F5:
                gameView->Run();
                break;
        }
        QWidget::keyPressEvent(event);
    }

private Q_SLOTS:
    void zoomIn() {
        view->scale(1.2, 1.2); // Увеличиваем масштаб
    }
    void zoomOut() {
        view->scale(0.8, 0.8); // Уменьшаем масштаб
    }
    void execGame() {
        gameView->Run();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.showFullScreen();

    PyAPI::Finalize();
    return app.exec();
}

#include "main.moc"
