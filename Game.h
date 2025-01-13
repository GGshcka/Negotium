#ifndef NEGOTIUM_GAME_H
#define NEGOTIUM_GAME_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTextEdit>
#include <QMutex>
#include <QSequentialAnimationGroup>
#include "AnimatedGraphicsItem.h"

class Game : public QGraphicsView {
    Q_OBJECT

public:
    Game(QTextEdit *edit, QTextEdit *debugText);
    int Move(int direction);
    void Run();

private:
    const int gridSize = 64;

    void createGrid();

    QGraphicsScene *scene;
    AnimatedGraphicsItem *character;
    QTextEdit *textEdit, *debugTextView;

    QSequentialAnimationGroup *animationGroup;
};

#endif //NEGOTIUM_GAME_H


