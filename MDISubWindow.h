//
// Created by silly on 23.05.25.
//

#ifndef MDISUBWINDOW_H
#define MDISUBWINDOW_H

#include <Qt>
#include <QtWidgets>

class MDISubWindow : public QMdiSubWindow {
public:
    MDISubWindow(QWidget *parent = nullptr, QString title = "SubWindow", QRect winSize = QRect(0, 0, 300, 500));

protected:
    QRect winSize;
};

#endif //MDISUBWINDOW_H
