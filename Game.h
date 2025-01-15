#ifndef NEGOTIUM_GAME_H
#define NEGOTIUM_GAME_H

#include <Qt>
#include <QtWidgets>
#include "AnimatedGraphicsItem.h"

class Game : public QGraphicsView {
    Q_OBJECT

public:
    Game(QTextEdit *edit, QTextEdit *debugText);
    QVector<std::function<void()>> actions;
    void Move(int direction);
    void Run();
    QGraphicsScene *scene;

private:
    int gridSize = 547;
    int currentActionIndex = 0;
    QPointF endPos;

    void createGrid() const;
    bool loadLevel();

    void execActions();

    AnimatedGraphicsItem *character;
    QTextEdit *textEdit, *debugTextView;

    QSequentialAnimationGroup *animationGroup;
    int gridRowCount = 10, gridColumnCount = 10;
    QVector<QPoint> pitCords;
};

#endif //NEGOTIUM_GAME_H


