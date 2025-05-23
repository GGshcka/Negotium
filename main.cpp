#include <Qt>
#include <QtWidgets>
#include <QWebEngineView>
#include "Game.h"
#include "PyAPI.h"
#include "SavesFileWorker.h"
#include "CodeHighlighter.h"
#include <python3.13/Python.h>

#include "MDISubWindow.h"

Game *gameView;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Negotium");

        QFile sfile(":/main/style");
        if(!sfile.open(QIODevice::ReadOnly))
        {
            qDebug() << "Cannot open stylesheet file.";
            return;
        }
        QString stylesheet = QString::fromUtf8(sfile.readAll());
        this->setStyleSheet(stylesheet);

        qMenuBar = this->menuBar();

        auto *systemMenuItem = new QMenu("System", this);

        auto *settingsMenuAction = new QAction("Settings", this);
        connect(settingsMenuAction, &QAction::triggered, this, [this]() {
            qDebug() << "!!!WIP!!!";
        });
        systemMenuItem->addAction(settingsMenuAction);

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
        mdiArea->setBackground(QBrush(qRgb(87, 70, 123)));
        mdiArea->setTabsClosable(false);
        mdiArea->setBackground(QBrush(QPixmap(":/main/bgPaws")));
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
        //! END OF INIT VIEW MDI
        //! INIT GUIDE MDI
        mdiArea->addSubWindow(guideMdiWindow);
        guideMdiWindow->close();
        //! END OF INIT GUID MDI

        runLevelMdiWindow = new QMdiSubWindow();
        runLevelMdiWindow->setWindowTitle(QString("Select level"));
        runLevelMdiWindow->setGeometry(150, 40, 300, 800);
        runLevelMdiWindow->setWindowIcon(pixmap);

        mdiArea->addSubWindow(runLevelMdiWindow);
        runLevelMdiWindow->close();
    }

protected:
    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
            case Qt::Key_Plus:
                view->scale(1.2, 1.2);
                view->update();
                break;
            case Qt::Key_Minus:
                view->scale(0.8, 0.8);
                view->update();
                break;
            case Qt::Key_F1:
                menuBar()->setActiveAction(menuBar()->actions()[0]);
                break;
            case Qt::Key_F2:
                menuBar()->setActiveAction(menuBar()->actions()[1]);
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
            auto *heartsIcons = new QAction(QPixmap(":/icons/heart"), nullptr);
            heartsIcons->setObjectName("Hearts");
            viewToolBar->addAction(heartsIcons);
        }

        viewToolBar->update();
    }
    void openLevelMDIWindows(QString lp) {
        auto *codeEditorCentralWidget = new QWidget(codeEditorMdiWindow);
        codeEditorMdiWindow->setWidget(codeEditorCentralWidget);

        auto *execButton = new QAction();
        execButton->setIcon(QIcon(":/icons/execute"));
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
        zoomInButton->setIcon(QIcon(":/icons/zoomIn"));
        zoomInButton->setToolTip("Zoom In");

        zoomOutButton = new QAction();
        zoomOutButton->setIcon(QIcon(":/icons/zoomOut"));
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

        //! GUIDE MDI INIT
        auto guideCentralWidget = new QWidget(guideMdiWindow);
        guideMdiWindow->setWidget(guideCentralWidget);

        guideLayout = new QGridLayout(guideCentralWidget);

        guideText = new QTextEdit();
        guideText->setReadOnly(true);

        //guideText->toMarkdown(QTextDocument::MarkdownDialectGitHub);
        guideText->setMarkdown("# Test H1\n## Test H2\n### Test H3\n* test");

        guideWebEngineView = new QWebEngineView();
        guideWebEngineView->load(QUrl(":/main/markdown/html"));

        QString markdownText = "# TEST H1"; // Load from file or resource
        QString js = QString("updateMarkdown(%1);").arg(QJsonDocument::fromVariant(markdownText).toJson(QJsonDocument::Compact));

        guideWebEngineView->page()->runJavaScript(js);

        guideLayout->addWidget(guideWebEngineView, 0, 0);

        viewMdiWindow->show();
        runOutputMdiWindow->show();
        codeEditorMdiWindow->show();
        guideMdiWindow->show();
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
    MDISubWindow *guideMdiWindow = new MDISubWindow(nullptr, "Guide", QRect(1000, 40, 450, 800));
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
    QMenuBar *qMenuBar;
    QWebEngineView *guideWebEngineView;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.showFullScreen();

    PyAPI::Finalize();
    return app.exec();
}

#include "main.moc"
