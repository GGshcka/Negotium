// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QWebEngineView>

#include "AppWideVariables.h"
#include "CodeEditor.h"
#include "GameView.h"
#include "GuideView.h"
#include "LevelSelector.h"
#include "Output.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
    static MainWindow* instance(QWidget *parent = nullptr); // доступ к синглтону
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

public Q_SLOTS:
    void openLevelMDIWindows();

private Q_SLOTS:
    static void execGame();
    void runLevelWindow();

private:
    explicit MainWindow(QWidget *parent = nullptr); // закрываем прямое создание

    CodeEditor *codeEditorMdiWindow = new CodeEditor();
    Output *runOutputMdiWindow = new Output();
    GameView *viewMdiWindow = new GameView();
    GuideView *guideMdiWindow = new GuideView();
    LevelSelector *runLevelMdiWindow = new LevelSelector();
    QMdiArea *mdiArea;
    QGridLayout *guideLayout, *runLevelLayout;
    QTextEdit *guideText;
    QMenuBar *qMenuBar;
    QWebEngineView *guideWebEngineView;

    static MainWindow *s_instance; // указатель на экземпляр
};

#endif // MAINWINDOW_H
