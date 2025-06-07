//
// Created by silly on 29.05.25.
//

#include "LevelSelector.h"

#include "AppWideVariables.h"
#include "MainWindow.h"

LevelSelector::LevelSelector() : MDISubWindow(nullptr, "Select level:", QRect(150, 40, 700, 800))
{
    centerWidget = new QWidget(this);
    layout()->addWidget(centerWidget);

    const QString path = QDir::currentPath() + "/levels/";
    model->setRootPath(path);

    tree = new QTreeView();
    tree->setHeaderHidden(true);
    tree->header()->setSectionResizeMode(QHeaderView::Stretch);
    tree->setModel(model);
    tree->setRootIndex(model->index(path));

    connect(tree, &QTreeView::activated, this, [this](const QModelIndex &index) {
        if (!index.isValid() && !model) return;
        if (!model->fileInfo(index).isDir())
        {
            const auto fileInfo = model->fileInfo(index);
            QCoreApplication::instance()->setProperty("levelName", fileInfo.dir().dirName()+"|"+fileInfo.fileName());
            AppWideVariables::instance().levelPath = model->filePath(index);
            MainWindow::instance()->openLevelMDIWindows();
            close();
        }
    });

    sublayout = new QGridLayout(centerWidget);
    sublayout->addWidget(tree, 0, 0);
}
