//
// Created by silly on 29.05.25.
//

#include "CodeEditor.h"
#include "CodeHighlighter.h"
#include "SavesFileWorker.h"

CodeEditor::CodeEditor() : MDISubWindow(nullptr, "Code Editor", QRect(1550, 40, 300, 800))
{
    centerWidget = new QWidget(this);
    layout()->addWidget(centerWidget);

    auto *execButton = new QAction();
    execButton->setIcon(QIcon(":/icons/execute"));
    execButton->setToolTip("Execute your code. You can execute, on press F5.");

    connect(execButton, &QAction::triggered, this, &CodeEditor::execGame);

    auto *speedMultiplierBox = new QComboBox();
    speedMultiplierBox->setEditable(false);
    QStringList speedList;
    speedList << "0.5x" << "1.0x" << "3.0x" << "5.0x";
    speedMultiplierBox->addItems(speedList);
    speedMultiplierBox->setCurrentIndex(1);

    connect(speedMultiplierBox, &QComboBox::currentIndexChanged, this, [speedList, speedMultiplierBox]() {
        QString str = speedList.at(speedMultiplierBox->currentIndex());
        str.chop(1);
        AppWideVariables::instance().game->speedMultiplier = str.toDouble();
    });

    toolBar = new QToolBar();
    toolBar->setMovable(false);
    toolBar->addAction(execButton);
    auto *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);
    toolBar->addWidget(speedMultiplierBox);

    textEdit = new QTextEdit();
    new CodeHighlighter(textEdit->document());
    textEdit->setTabStopDistance(12);

    sublayout = new QGridLayout(centerWidget);
    sublayout->addWidget(toolBar, 0, 0);
    sublayout->addWidget(textEdit, 1, 0);
}

void CodeEditor::loadLevelCode()
{
    auto *fw = new SavesFileWorker("code.isf");
    textEdit->setText(fw->getSaveFileText());
}
