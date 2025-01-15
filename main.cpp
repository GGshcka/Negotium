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

        int btnSize = 25;

        auto *execButton = new QPushButton();
        execButton->setIcon(QIcon("../Resources/Icons/iconExec.png"));
        execButton->setFixedSize(btnSize, btnSize);
        execButton->setFlat(true);
        execButton->setToolTip("Execute your code. You can execute, on press F5.");
        execButton->setToolTipDuration(2000);

        auto *zoomInButton = new QPushButton();
        zoomInButton->setIcon(QIcon("../Resources/Icons/iconZoomIn.png"));
        zoomInButton->setFixedSize(btnSize, btnSize);
        zoomInButton->setFlat(true);
        zoomInButton->setToolTip("Zoom In");
        zoomInButton->setToolTipDuration(2000);

        auto *zoomOutButton = new QPushButton();
        zoomOutButton->setIcon(QIcon("../Resources/Icons/iconZoomOut.png"));
        zoomOutButton->setFixedSize(btnSize, btnSize);
        zoomOutButton->setFlat(true);
        zoomOutButton->setToolTip("Zoom Out");
        zoomOutButton->setToolTipDuration(2000);

        connect(zoomInButton, &QPushButton::clicked, this, &MainWindow::zoomIn);
        connect(zoomOutButton, &QPushButton::clicked, this, &MainWindow::zoomOut);
        connect(execButton, &QPushButton::clicked, this, &MainWindow::execGame);

        auto *spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        toolBar = new QToolBar();
        toolBar->setMovable(false);
        toolBar->addWidget(spacer);
        toolBar->addWidget(zoomInButton);
        toolBar->addWidget(zoomOutButton);
        toolBar->addSeparator();
        toolBar->addWidget(execButton);
        toolBar->isRightToLeft();
        this->addToolBar(toolBar);

        auto *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        mainLayout = new QGridLayout(centralWidget);
        subLayout = new QGridLayout();

        edit = new QTextEdit();
        edit->setTabStopDistance(12);

        QFile file("script.py");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug("Can't open file.");
        }
        QTextStream out(&file);
        edit->setText(out.readAll());
        file.close();

        debugText = new QTextEdit();
        debugText->setReadOnly(true);

        auto *scrollVBar = new QScrollBar();
        auto *scrollHBar = new QScrollBar();

        gameView = new Game(edit, debugText);
        view = new QGraphicsView();
        view->setScene(gameView->scene);
        view->setVerticalScrollBar(scrollVBar);
        view->setHorizontalScrollBar(scrollHBar);
        view->scale(0.3, 0.3);
        scrollVBar->setValue(0);
        scrollHBar->setValue(0);

        subLayout->addWidget(view, 0, 0);
        subLayout->addWidget(edit, 0, 1);

        mainLayout->addLayout(subLayout, 0, 0);
        mainLayout->addWidget(debugText, 1, 0);

        QFile sfile("../Resources/style.scss");
        if(!sfile.open(QIODevice::ReadOnly))
        {
            qDebug() << "Cannot open stylesheet file.";
            return;
        }
        QString stylesheet = QString::fromUtf8(sfile.readAll());
        this->setStyleSheet(stylesheet);
    }
    QGridLayout *mainLayout;
    QGridLayout *subLayout;
    QTextEdit *edit;
    QTextEdit *debugText;
    QGraphicsView *view;
    QToolBar *toolBar;

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
    void resizeEvent(QResizeEvent *event) override {
        QMainWindow::resizeEvent(event);

        QSize newSize = event->size();
        edit->setMaximumWidth(newSize.width() * 0.3);
        debugText->setMaximumHeight(newSize.height() * 0.1);
        //mainLayout->setRowMinimumHeight(1, newSize.height() * 0.1);
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
