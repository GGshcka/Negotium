#include <QApplication>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QGraphicsPixmapItem>
#include <QFile>
#include <QWidget>
#include <QMainWindow>
#include <QMenuBar>
#include "Game.h"
#include "PyAPI.h"

#define PY_SSIZE_T_CLEAN
#include <python3.13/Python.h>

Game *gameView;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Negotium");

        QWidget *centralWidget = new QWidget(this);
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
        debugText->setMaximumHeight(300);
        debugText->setReadOnly(true);

        gameView = new Game(edit, debugText);

        subLayout->addWidget(gameView, 0, 0);
        subLayout->addWidget(edit, 0, 1);

        mainLayout->addLayout(subLayout, 0, 0);
        mainLayout->addWidget(debugText, 1, 0);
    }
    QGridLayout *mainLayout;
    QGridLayout *subLayout;
    QTextEdit *edit;
    QTextEdit *debugText;

protected:
    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
            case Qt::Key_F5:
                gameView->Run();
                break;
        }
        QWidget::keyPressEvent(event);
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
