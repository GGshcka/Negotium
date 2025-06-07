//
// Created by silly on 29.05.25.
//

#ifndef LEVELSELECTOR_H
#define LEVELSELECTOR_H
#include "MDISubWindow.h"


class LevelSelector : public MDISubWindow {
public:
    LevelSelector();

private:
    QWidget *centerWidget;
    QGridLayout *sublayout;
    QTreeView *tree;
    QStandardItem *rootItem, *folderDebug;
    QFileSystemModel *model = new QFileSystemModel();
};

#endif //LEVELSELECTOR_H
