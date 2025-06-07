//
// Created by silly on 29.05.25.
//

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "MDISubWindow.h"
#include <Qt>
#include <QtWidgets>

#include "AppWideVariables.h"

class CodeEditor : public MDISubWindow {
public:
    CodeEditor();

    void loadLevelCode();

    QTextEdit *textEdit;

private Q_SLOTS:
    static void execGame() {
        AppWideVariables::instance().game->Run();
    }

private:
    QWidget *centerWidget;
    QToolBar *toolBar;
    QGridLayout *sublayout;
};

#endif //CODEEDITOR_H
