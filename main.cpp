#include <Qt>
#include <QtWidgets>
#include "Game.h"
#include "PyAPI.h"
#include "SavesFileWorker.h"
#include "CodeHighlighter.h"
#include <python3.13/Python.h>

Game *gameView;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Negotium");

        QMenuBar *menuBar = this->menuBar();

        auto *systemMenuItem = new QMenu("System");

        auto *settingsMenuAction = new QAction();
        settingsMenuAction->setText("Settings");
        connect(settingsMenuAction, &QAction::triggered, this, [this]() {
            qDebug() << "!!!WIP!!!";
        });
        systemMenuItem->addAction(settingsMenuAction);

        auto *exitMenuAction = new QAction();
        exitMenuAction->setText("Shutdown");
        connect(exitMenuAction, &QAction::triggered, this, &MainWindow::close);
        systemMenuItem->addAction(exitMenuAction);

        menuBar->addMenu(systemMenuItem);

        auto *gameMenuItem = new QMenu("Game");

        auto *runLevelMenuAction = new QAction();
        runLevelMenuAction->setText("Run level");
        connect(runLevelMenuAction, &QAction::triggered, this, &MainWindow::runLevelWindow);
        gameMenuItem->addAction(runLevelMenuAction);

        auto *closeGameMenuAction = new QAction();
        closeGameMenuAction->setText("Close all");
        connect(closeGameMenuAction, &QAction::triggered, this, [=, this]() { mdiArea->closeAllSubWindows(); });
        gameMenuItem->addAction(closeGameMenuAction);

        menuBar->addMenu(gameMenuItem);

        mdiArea = new QMdiArea(this);
        mdiArea->setBackground(QBrush(qRgb(87, 70, 123)));
        mdiArea->setTabsClosable(false);
        mdiArea->setBackground(QBrush(QPixmap(":/Resources/Icons/bgPaws.png")));
        setCentralWidget(mdiArea);

        QPixmap pixmap(32, 32);
        pixmap.fill(Qt::transparent);

        //! CODE EDITOR MDI START
        codeEditorMdiWindow = new QMdiSubWindow();
        codeEditorMdiWindow->setWindowTitle(QString("Code editor"));
        codeEditorMdiWindow->setGeometry(1550, 40, 300, 800);
        codeEditorMdiWindow->setWindowIcon(pixmap);

        mdiArea->addSubWindow(codeEditorMdiWindow);
        codeEditorMdiWindow->close();
        //! CODE EDITOR MDI START\n-----------------\n
        //! RUN OUTPUT MDI START
        runOutputMdiWindow = new QMdiSubWindow();
        runOutputMdiWindow->setWindowTitle(QString("Output"));
        runOutputMdiWindow->setGeometry(20, 850, 850, 200);
        runOutputMdiWindow->setWindowIcon(pixmap);

        mdiArea->addSubWindow(runOutputMdiWindow);
        runOutputMdiWindow->close();
        //! RUN OUTPUT MDI END\n-----------------\n
        //! VIEW MDI START
        viewMdiWindow = new QMdiSubWindow();
        viewMdiWindow->setWindowTitle(QString("Game screen"));
        viewMdiWindow->setGeometry(20, 40, 1450, 800);
        viewMdiWindow->setWindowIcon(pixmap);

        mdiArea->addSubWindow(viewMdiWindow);
        viewMdiWindow->close();
        //! VIEW MDI END

        runLevelMdiWindow = new QMdiSubWindow();
        runLevelMdiWindow->setWindowTitle(QString("Select level"));
        runLevelMdiWindow->setGeometry(150, 40, 300, 800);
        runLevelMdiWindow->setWindowIcon(pixmap);

        mdiArea->addSubWindow(runLevelMdiWindow);
        runLevelMdiWindow->close();

        QFile sfile(":/Resources/style.scss");
        if(!sfile.open(QIODevice::ReadOnly))
        {
            qDebug() << "Cannot open stylesheet file.";
            return;
        }
        QString stylesheet = QString::fromUtf8(sfile.readAll());
        this->setStyleSheet(stylesheet);
    }

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
            case Qt::Key_F3:
                runLevelWindow();
                break;
            case Qt::Key_F5:
                if (gameView != nullptr) gameView->Run();
                break;
        }
        QWidget::keyPressEvent(event);
    }
    void updateToolbarIcons(unsigned int iconCount) const {
        viewToolBar->clear();
        viewToolBar->addAction(zoomInButton);
        viewToolBar->addAction(zoomOutButton);
        auto *spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        viewToolBar->addWidget(spacer);

        for (int i = 1; i <= iconCount; ++i) {
            auto *heartsIcons = new QAction(QPixmap(":/Resources/Icons/heart.png"), nullptr);
            heartsIcons->setObjectName("Hearts");
            viewToolBar->addAction(heartsIcons);
        }

        viewToolBar->update();
    }
    void openLevelMDIWindows(QString lp) {
        auto *codeEditorCentralWidget = new QWidget(codeEditorMdiWindow);
        codeEditorMdiWindow->setWidget(codeEditorCentralWidget);

        auto *execButton = new QAction();
        execButton->setIcon(QIcon(":/Resources/Icons/iconExec.png"));
        execButton->setToolTip("Execute your code. You can execute, on press F5.");

        connect(execButton, &QAction::triggered, this, &MainWindow::execGame);

        auto *speedMultiplierBox = new QComboBox();
        speedMultiplierBox->setEditable(false);
        QStringList speedList;
        speedList << "0.5x" << "1.0x" << "3.0x" << "5.0x";
        speedMultiplierBox->addItems(speedList);
        speedMultiplierBox->setCurrentIndex(1);

        connect(speedMultiplierBox, &QComboBox::currentIndexChanged, this, [speedList, speedMultiplierBox]() {
            QString str = speedList.at(speedMultiplierBox->currentIndex());
            str.chop(1);
            gameView->speedMultiplier = str.toDouble();
        });

        codeEditorToolBar = new QToolBar();
        codeEditorToolBar->setMovable(false);
        codeEditorToolBar->addAction(execButton);
        auto *spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        codeEditorToolBar->addWidget(spacer);
        codeEditorToolBar->addWidget(speedMultiplierBox);

        edit = new QTextEdit();
        new CodeHighlighter(edit->document());
        edit->setTabStopDistance(12);

        auto *fw = new SavesFileWorker("code.isf");
        edit->setText(fw->getSaveFileText());

        codeEditorLayout = new QGridLayout(codeEditorCentralWidget);
        codeEditorLayout->addWidget(codeEditorToolBar, 0, 0);
        codeEditorLayout->addWidget(edit, 1, 0);


        auto *runOutputCentralWidget = new QWidget(runOutputMdiWindow);
        runOutputMdiWindow->setWidget(runOutputCentralWidget);

        debugText = new QTextEdit();
        debugText->setReadOnly(true);

        runOutputLayout = new QGridLayout(runOutputCentralWidget);
        runOutputLayout->addWidget(debugText, 0, 0);


        auto *viewCentralWidget = new QWidget(viewMdiWindow);
        viewMdiWindow->setWidget(viewCentralWidget);

        auto *scrollVBar = new QScrollBar();
        auto *scrollHBar = new QScrollBar();

        gameView = new Game(edit, debugText, lp);
        view = new QGraphicsView();
        view->setScene(gameView->scene);
        view->setVerticalScrollBar(scrollVBar);
        view->setHorizontalScrollBar(scrollHBar);
        view->scale(0.3, 0.3);

        zoomInButton = new QAction();
        zoomInButton->setIcon(QIcon(":/Resources/Icons/iconZoomIn.png"));
        zoomInButton->setToolTip("Zoom In");

        zoomOutButton = new QAction();
        zoomOutButton->setIcon(QIcon(":/Resources/Icons/iconZoomOut.png"));
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

        viewMdiWindow->show();
        runOutputMdiWindow->show();
        codeEditorMdiWindow->show();
    }

private Q_SLOTS:
    void zoomIn() const {
        view->scale(1.2, 1.2); // Увеличиваем масштаб
    }
    void zoomOut() const {
        view->scale(0.8, 0.8); // Уменьшаем масштаб
    }
    static void execGame() {
        gameView->Run();
    }
    void runLevelWindow() {
        auto *runLevelCentralWidget = new QWidget(runLevelMdiWindow);
        runLevelMdiWindow->setWidget(runLevelCentralWidget);

        model->clear();
        model->setHorizontalHeaderLabels(QStringList() << "Name");

        QStandardItem *rootItem = model->invisibleRootItem();
        rootItem->setEditable(false);

        auto *folderDebug = new QStandardItem("Chapter: Debug");
        folderDebug->setEditable(false);
        folderDebug->setSelectable(false);
        /*auto *folderChapterOne = new QStandardItem("Chapter: One");
        folderDebug->setEditable(false);*/

        rootItem->appendRow(folderDebug);
        //rootItem->appendRow(folderChapterOne);

        QDir dir(":/levels/Debug");
        for (const QString &entry : dir.entryList(QDir::Files)) {
            auto *item = new QStandardItem(entry);
            item->setEditable(false);
            folderDebug->appendRow(item);
        }

        auto *tree = new QTreeView();
        tree->setHeaderHidden(true);
        tree->setModel(model);

        connect(tree, &QTreeView::doubleClicked, this, [this](const QModelIndex &index) {
            if (!index.isValid()) return;
            if (!model) return;

            QStandardItem *item = model->itemFromIndex(index);
            if (item && !item->text().startsWith("Chapter:")) {
                QCoreApplication::instance()->setProperty("levelName", item->text());
                openLevelMDIWindows(QString(":/levels/" + item->parent()->text().slice(9) + "/" + item->text()));
                runLevelMdiWindow->close();
            }
        });

        runOutputLayout = new QGridLayout(runLevelCentralWidget);
        runOutputLayout->addWidget(tree, 0, 0);

        runLevelMdiWindow->show();
    }

private:
    QMdiSubWindow *codeEditorMdiWindow = nullptr;
    QMdiSubWindow *runOutputMdiWindow = nullptr;
    QMdiSubWindow *viewMdiWindow = nullptr;
    QMdiSubWindow *guideMdiWindow = nullptr;
    QMdiSubWindow *runLevelMdiWindow = nullptr;
    QMdiArea *mdiArea;
    QGridLayout *codeEditorLayout;
    QGridLayout *guideLayout;
    QGridLayout *runOutputLayout;
    QGridLayout *viewLayout;
    QTextEdit *edit;
    QTextEdit *debugText;
    QTextEdit *guideText;
    QGraphicsView *view;
    QToolBar *viewToolBar;
    QToolBar *codeEditorToolBar;
    QAction *zoomInButton, *zoomOutButton;
    QStandardItemModel *model = new QStandardItemModel();
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.showFullScreen();

    PyAPI::Finalize();
    return app.exec();
}

#include "main.moc"
