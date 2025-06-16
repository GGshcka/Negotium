//
// Created by silly on 23.05.25.
//

#ifndef GAMEVIEW_H
#define GAMEVIEW_H
#include "Game.h"
#include "MDISubWindow.h"


class GameView : public MDISubWindow {
public:
    GameView();

    void setGame(Game* game);
    void updateGame();

private Q_SLOTS:
    void zoomIn() const {
        view->scale(1.2, 1.2); // Увеличиваем масштаб
    }
    void zoomOut() const {
        view->scale(0.8, 0.8); // Уменьшаем масштаб
    }

private:
    void updateToolbarIcons();

    QToolBar *toolBar;
    QAction *zoomInButton, *zoomOutButton;
    QGraphicsView *view;
    Game* game;
    QGridLayout *viewLayout;
    QWidget *centralWidget, *spacer;
    QScrollBar *scrollVBar, *scrollHBar;
    QIcon heartIcon = QPixmap(":/icons/heart");
    QVector<QAction*> hearts;
};

#endif //GAMEVIEW_H
