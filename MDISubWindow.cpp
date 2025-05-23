//
// Created by silly on 23.05.25.
//

#include "MDISubWindow.h"

MDISubWindow::MDISubWindow(QWidget* parent, QString title, QRect winSize) : QMdiSubWindow(parent), winSize(winSize)
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    setWindowTitle(title);
    setGeometry(winSize.x(), winSize.y(), winSize.width(), winSize.height());
    setWindowIcon(pixmap);
}
