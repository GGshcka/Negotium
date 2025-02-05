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
    unsigned int PlayerHPGet() const;
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
    void fallAnim(GraphicalGameObject *target);

    QPixmap *PDown = new QPixmap(":/Resources/Sprites/Pinny_D-DOWN.png");
    QPixmap *PLeft = new QPixmap(":/Resources/Sprites/Pinny_D-LEFT.png");
    QPixmap *PRight = new QPixmap(":/Resources/Sprites/Pinny_D-RIGHT.png");
    QPixmap *PUp = new QPixmap(":/Resources/Sprites/Pinny_D-UP.png");

    GraphicalGameObject *character;
    GameExit *exit;

    int gridRowCount = 10, gridColumnCount = 10;
    QVector<QPoint> pitCords;
    QVector<GameBox*> boxes;
    QVector<GameButton*> buttons;
    QString lvlPath;
};

#endif //NEGOTIUM_GAME_H


