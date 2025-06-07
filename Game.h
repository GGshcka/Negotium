#ifndef NEGOTIUM_GAME_H
#define NEGOTIUM_GAME_H

#include <Qt>
#include <QtWidgets>
#include "GraphicalGameObject.h"

class Game : public QGraphicsView {
    Q_OBJECT

public:
    Game(QTextEdit *edit, QTextEdit *debugText, QString levelPath);
    void Move(int direction);
    bool CanMove(int direction);
    void Run();
    void PlayerHit(unsigned int dmg);
    unsigned int getPlayerHP() const;
    QGraphicsScene *scene;
    QTextEdit *textEdit, *debugTextView;
    double speedMultiplier = 1.0;
    int gridSize = 547;

Q_SIGNALS:
    void playerHealthChanged(unsigned int newHealth);
    void moveCompleted();

private:
    unsigned int playerMaxHealth = 3, playerHealth = playerMaxHealth;
    QPointF endPos;
    bool canMove = true, silent = false;

    void createGrid() const;
    bool loadLevel();
    bool moveBox(GameBox *targetBox, int direction);
    void moveSoul(SoulEntity *soul, int multiplier);
    void fallAnim(GraphicalGameObject *target);

    QPixmap *PDown = new QPixmap(":/player/main/down");
    QPixmap *PLeft = new QPixmap(":/player/main/left");
    QPixmap *PRight = new QPixmap(":/player/main/right");
    QPixmap *PUp = new QPixmap(":/player/main/up");

    GraphicalGameObject *character;
    GameExit *exit;

    int gridRowCount = 10, gridColumnCount = 10;
    QVector<QPoint> pitCords;
    QVector<GameBox*> boxes;
    QVector<GameButton*> buttons;
    QVector<SoulEntity*> souls;
    QString lvlPath;

    QEasingCurve easingCurve;
};

#endif //NEGOTIUM_GAME_H


