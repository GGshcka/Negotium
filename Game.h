#ifndef NEGOTIUM_GAME_H
#define NEGOTIUM_GAME_H

#include <Qt>
#include <QtWidgets>
#include "AnimatedGraphicsItem.h"

class Game : public QGraphicsView {
    Q_OBJECT

public:
    Game(QTextEdit *edit, QTextEdit *debugText);
    void Move(int direction);
    bool CanMove(int direction);
    void Run();
    void PlayerHit(unsigned int dmg);
    unsigned int PlayerHPGet() const;
    QGraphicsScene *scene;
    QTextEdit *textEdit, *debugTextView;

Q_SIGNALS:
    void playerHealthChanged(unsigned int newHealth);
    void moveCompleted();

private:
    int gridSize = 547;
    unsigned int playerMaxHealth = 3, playerHealth = playerMaxHealth;
    QPointF endPos;
    bool canMove = true, silent = false;

    void createGrid() const;
    bool loadLevel();
    void moveBox(AnimatedGraphicsItem *targetBox, int direction);

    QPixmap *PDown = new QPixmap("../Resources/Sprites/Pinny_D-DOWN.png");
    QPixmap *PLeft = new QPixmap("../Resources/Sprites/Pinny_D-LEFT.png");
    QPixmap *PRight = new QPixmap("../Resources/Sprites/Pinny_D-RIGHT.png");
    QPixmap *PUp = new QPixmap("../Resources/Sprites/Pinny_D-UP.png");

    AnimatedGraphicsItem *character;

    int gridRowCount = 10, gridColumnCount = 10;
    QVector<QPoint> pitCords, boxesPos;
    QVector<AnimatedGraphicsItem*> boxes;
};

#endif //NEGOTIUM_GAME_H


