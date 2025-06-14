#include "Game.h"
#include <Qt>
#include <QtWidgets>
#include <QPoint>
#include <QList>
#include "PyAPI.h"
#include "SavesFileWorker.h"
#include <python3.13/Python.h>
#include <QtGlobal>

#include "AppWideVariables.h"

qreal customEasingFunction(qreal progress) {
    return sin(progress * M_PI / 2); // Плавное движение вперед //!Психованное движение tan(progress*M_PI*progress*M_PI)
}

Game::Game(QTextEdit *edit, QTextEdit *debugText, QString levelPath) {
    setBackgroundBrush(QImage(":/game/main/bg"));
    textEdit = edit;
    debugTextView = debugText;
    lvlPath = levelPath;
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QImage(":/game/main/bg"));
    setScene(scene);

    if(loadLevel()) qInfo() << "Successful loaded...";
    else qDebug() << "Can't load file...";

    character = new GraphicalGameObject(PDown->scaled(gridSize - 1 ,gridSize - 1));
    character->setStartPos(1 + gridSize, 1 + gridSize);
    endPos = character->pos();
    scene->addItem(character);

    setFocusPolicy(Qt::StrongFocus);

    easingCurve.setCustomType(customEasingFunction);
}

bool Game::CanMove(int direction) {
    canMove = true;
    switch (direction) {
        case 0: // UP
            if (
                    character->pos().y() - gridSize < gridSize ||
                    pitCords.contains((QPoint(character->pos().x(), character->pos().y() - gridSize) / gridSize) - QPoint(1, 1))
                ) canMove = false;
            break;
        case 1: // DOWN
            if (
                    character->pos().y() + gridSize >= scene->height() - gridSize ||
                    pitCords.contains((QPoint(character->pos().x(), character->pos().y() + gridSize) / gridSize) - QPoint(1, 1))
                ) canMove = false;
            break;
        case 2: // LEFT
            if (
                    character->pos().x() - gridSize < gridSize ||
                    pitCords.contains((QPoint(character->pos().x() - gridSize, character->pos().y()) - QPoint(1, 1)))
                ) canMove = false;
            break;
        case 3: // RIGHT
            if (
                    character->pos().x() + gridSize >= scene->width() - gridSize ||
                    pitCords.contains((QPoint(character->pos().x() + gridSize, character->pos().y()) - QPoint(1, 1)))
                ) canMove = false;
            break;
        default:
            break;
    }
    return canMove;
}

void Game::Move(int direction) {
    QPointF startPos = character->pos();
    int multiplier = 1;

    switch (direction) {
        case 0: // UP
            if (startPos.y() - gridSize >= gridSize) {
                character->setPixmap(QPixmap(PUp->scaled(gridSize - 1, gridSize - 1)));
                endPos.ry() -= gridSize;
            } else PlayerHit(1);
            break;
        case 1: // DOWN
            if (startPos.y() + gridSize < scene->height() - gridSize) {
                character->setPixmap(QPixmap(PDown->scaled(gridSize - 1, gridSize - 1)));
                endPos.ry() += gridSize;
            } else PlayerHit(1);
            break;
        case 2: // LEFT
            if (startPos.x() - gridSize >= gridSize) {
                character->setPixmap(QPixmap(PLeft->scaled(gridSize - 1, gridSize - 1)));
                endPos.rx() -= gridSize;
            } else PlayerHit(1);
            break;
        case 3: // RIGHT
            if (startPos.x() + gridSize < scene->width() - gridSize) {
                character->setPixmap(QPixmap(PRight->scaled(gridSize - 1, gridSize - 1)));
                endPos.rx() += gridSize;
            } else PlayerHit(1);
            break;
        default:
            break;
    }

    for (const auto &pit: pitCords) {
        if (endPos == pit * gridSize + QPoint(gridSize, gridSize) + QPoint(1, 1)) {
            debugTextView->append("Falled...");
            PlayerHit(playerHealth);
            fallAnim(character);
            break;
        }
    }

    for (const auto &soul : souls) {
        moveSoul(soul, multiplier);
        if (soul->pos() + QPoint(1, 1) == character->pos()) {
            PlayerHit(playerHealth);
            soul->showMood(speedMultiplier, "love");
            endPos = startPos;
        }
    }

    for (const auto &box: boxes) {
        if (endPos == box->pos() + QPoint(1, 1) && !box->isFalled()) {
            multiplier = 2;
            if (!moveBox(box, direction)) endPos = startPos;
            break;
        }
    }

    if (endPos == exit->pos() + QPoint(1, 1) && !exit->isClosed()) {
        character->showMood(speedMultiplier, "love");
        fallAnim(character);
        silent = true;
        PyErr_SetString(PyExc_StopIteration, "She get out!");
    }

    auto *animation = new QPropertyAnimation(character, "pos", this);
    animation->setDuration(700 * multiplier / speedMultiplier); // Время анимации
    animation->setStartValue(character->pos());
    animation->setEndValue(endPos);
    animation->setEasingCurve(easingCurve);
    connect(animation, &QPropertyAnimation::finished, this, [=, this]() {
        character->setPos(endPos);
        Q_EMIT moveCompleted();
    });
    animation->start();
}

void Game::fallAnim(GraphicalGameObject *target) {
    QEasingCurve easingCurve;
    easingCurve.setCustomType(customEasingFunction);

    auto *scaleAnim = new QPropertyAnimation(target, "scale", this);
    scaleAnim->setDuration(2500 / speedMultiplier); // Время анимации
    scaleAnim->setStartValue(1.0);
    scaleAnim->setEndValue(0.0);
    scaleAnim->setEasingCurve(easingCurve);

    auto *opacityAnim = new QPropertyAnimation(target, "opacity", this);
    opacityAnim->setDuration(2500 / speedMultiplier); // Время анимации
    opacityAnim->setKeyValueAt(0, 1.0);
    opacityAnim->setKeyValueAt(0.3, 1.0);
    opacityAnim->setKeyValueAt(1, 0.0);
    opacityAnim->setEasingCurve(easingCurve);

    auto *anim = new QParallelAnimationGroup;
    anim->addAnimation(scaleAnim);
    anim->addAnimation(opacityAnim);
    anim->start();
}

bool Game::moveBox(GameBox *targetBox, int direction) {
    QPointF startPos = targetBox->pos();
    QPointF boxEndPos = startPos;

    switch (direction) {
        case 0: // UP
            if (startPos.y() - gridSize >= gridSize) boxEndPos.ry() -= gridSize;
            break;
        case 1: // DOWN
            if (startPos.y() + gridSize < scene->height() - gridSize) boxEndPos.ry() += gridSize;
            break;
        case 2: // LEFT
            if (startPos.x() - gridSize >= gridSize) boxEndPos.rx() -= gridSize;
            break;
        case 3: // RIGHT
            if (startPos.x() + gridSize < scene->width() - gridSize) boxEndPos.rx() += gridSize;
            break;
        default:
            break;
    }

    if (boxEndPos == startPos) return false;

    for (const auto &pit: pitCords) {
        if (boxEndPos == pit * gridSize + QPoint(gridSize, gridSize)) {
            fallAnim(targetBox);
            targetBox->setFalled(true);
        }
    }

    for (const auto &button: buttons) {
        if (boxEndPos == button->pos()) {
            button->setPressed(true);
            qDebug() << "Button pressed with box!";
        }
        else {
            if (button->isPressed() && button->pos() == startPos) {
                button->setPressed(false);
                qDebug() << "Button unpressed with box!";
            }
        }
    }

    auto *animation = new QPropertyAnimation(targetBox, "pos", this);
    animation->setDuration(700 * 2 / speedMultiplier); // Время анимации
    animation->setStartValue(startPos);
    animation->setEndValue(boxEndPos);
    animation->setEasingCurve(easingCurve);
    animation->start();

    return true;
}

void Game::moveSoul(SoulEntity *soul, int multiplier) {
    QPointF startPos = soul->pos();
    QPointF soulEndPos = startPos;

    int wPos = soul->getWanderingPosition();
    if (wPos < 0 || wPos > 0) {
        soul->reversMoveDirection();
    }

    if (soul->isVertical()) {
        if (!soul->isBackward()) soulEndPos.ry() += gridSize;
        else soulEndPos.ry() -= gridSize;
    } else {
        if (!soul->isBackward()) soulEndPos.rx() += gridSize;
        else soulEndPos.rx() -= gridSize;
    }
    soul->increaseWanderingPosition();

    auto *animation = new QPropertyAnimation(soul, "pos", this);
    animation->setDuration(700 * multiplier / speedMultiplier);
    animation->setStartValue(startPos);
    animation->setEndValue(soulEndPos);
    animation->setEasingCurve(easingCurve);
    animation->start();
}

void Game::Run() { //! ВАЖНО ! двойной запуск ломает игру!!!!
    if (Py_IsInitialized()) return;

    PyAPI::Initialize(this);

    if (character->pos() != character->startPos()) {
        character->setPos(character->startPos());
        if (character->getOpacity() != 1.0) character->setOpacity(1.0);
        if (character->getScale() != 1.0) character->setScale(1.0);
        endPos = character->pos();
        playerHealth = playerMaxHealth;
        Q_EMIT playerHealthChanged(playerHealth);
        exit->setClosed(exit->isStartClosed());
        debugTextView->clear();
        for (const auto &box: boxes) {
            box->setPos(box->startPos());
            if (box->isFalled()) box->setFalled(false);
        }
        for (const auto &soul: souls) {
            soul->reset();
        }
    }

    PyObject *globals = PyDict_New();
    PyObject *locals = PyDict_New();

    QString text = textEdit->toPlainText();

    auto *fw = new SavesFileWorker("code.isf");
    fw->setSaveFileText(text);

    QString readyPythonCode = QString("from negotiumGameIntegrationModule import *\n") + text;

    PyObject *result = PyRun_String(readyPythonCode.toUtf8().constData(), Py_file_input, globals, locals);

    if (result == nullptr && !silent) {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);
        debugTextView->append(QTime::currentTime().toString() + " | Error: " + PyUnicode_AsUTF8(PyObject_Str(value)));
        PyErr_Restore(type, value, traceback);
    } else {
        debugTextView->append("Done");
        silent = false;
    }

    Py_DECREF(globals);
    Py_DECREF(locals);

    if (Py_IsInitialized()) {
        PyAPI::Finalize();
    }
}

void Game::createGrid() const {
    for (int x = gridSize; x <= scene->width() - gridSize; x += gridSize) {
        scene->addLine(x, gridSize, x, scene->height() - gridSize, QPen(qRgb(0, 0, 0))); //174, 149, 229
    }

    for (int y = gridSize; y <= scene->height() - gridSize; y += gridSize) {
        scene->addLine(gridSize, y, scene->width() - gridSize, y, QPen(qRgb(0, 0, 0)));
    }
}

bool Game::loadLevel() {
    auto *file = new QFile(lvlPath.toUtf8().constData());
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamReader xml(file);
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            auto tagName = xml.qualifiedName();

            QXmlStreamAttributes attributes = xml.attributes();

            if (tagName == "level") {
                gridColumnCount = attributes.value("columns").toInt();
                gridRowCount = attributes.value("rows").toInt();
            } else if (tagName == "exit") {
                if (attributes.value("closed").toInt() == 0) {
                    exit = new GameExit(QPixmap(":/game/obj/door-open").scaled(gridSize, gridSize), nullptr);
                    exit->setStartClosed(false);
                }
                else  {
                    exit = new GameExit(QPixmap(":/game/obj/door-closed").scaled(gridSize, gridSize), nullptr);
                    exit->setStartClosed(true);
                }
                exit->setStartPos(attributes.value("x").toInt() * gridSize + gridSize, attributes.value("y").toInt() * gridSize + gridSize);
                connect(exit, &GameExit::signalChangeState, this, [this]{
                    bool allPressed = true;
                    for (const auto &button: buttons) {
                        if (!button->isPressed()) { allPressed = false; break; }
                    }
                    if (allPressed) exit->setClosed(false);
                    else exit->setClosed(true);
                });
            } else if (tagName == "pit") {
                pitCords.append(QPoint(attributes.value("x").toInt(), attributes.value("y").toInt()));
            } else if (tagName == "box") {
                auto *newBox = new GameBox(QPixmap(":/game/obj/box").scaled(gridSize, gridSize),nullptr);
                newBox->setStartPos(attributes.value("x").toInt() * gridSize + gridSize, attributes.value("y").toInt() * gridSize + gridSize);
                boxes.append(newBox);
            } else if (tagName == "button") {
                auto *newButton = new GameButton(QPixmap(":/game/obj/button").scaled(gridSize, gridSize), nullptr);
                newButton->setStartPos(attributes.value("x").toInt() * gridSize + gridSize, attributes.value("y").toInt() * gridSize + gridSize);
                newButton->setPressed(false);
                connect(newButton, &GameButton::buttonStateChanged, this, [this]() {
                    Q_EMIT exit->signalChangeState();
                });
                buttons.append(newButton);
            } else if (tagName == "soul") {
                auto *newSoul = new SoulEntity(QPixmap(":/game/entity/soul").scaled(gridSize, gridSize),
                                               nullptr, attributes.value("vertical").toInt(), attributes.value("backward").toInt());
                newSoul->setStartPos(attributes.value("x").toInt() * gridSize + gridSize, attributes.value("y").toInt() * gridSize + gridSize);
                souls.append(newSoul);
            } else if (tagName == "lvlguide") {
                AppWideVariables::instance().markdownText = xml.readElementText();
            }
        }
    }

    scene->setSceneRect(0, 0, gridSize * gridColumnCount + gridSize*2, gridSize * gridRowCount + gridSize*2);
    //scene->addRect(0,0,scene->width(), scene->height(), QPen(qRgb(69, 56, 96)), QBrush(qRgb(69, 56, 96)));

    scene->addItem(exit);

    createGrid();

    for (const auto &pit: pitCords) {
        scene->addRect(
                gridSize * pit.x() + gridSize,
                gridSize * pit.y() + gridSize,
                gridSize,
                gridSize,
                QPen(qRgb(0, 0, 0)),
                QBrush(qRgb(0, 0, 0))
                );
    }
    for (const auto &button: buttons) {
        scene->addItem(button);
    }
    for (const auto &box: boxes) {
        scene->addItem(box);
    }
    for (const auto &soul: souls) {
        scene->addItem(soul);
    }
    file->close();
    return true;
}

unsigned int Game::getPlayerHP() const {
    return playerHealth;
}

void Game::PlayerHit(unsigned int dmg) {
    if (playerHealth > 0) {
        playerHealth-=dmg;
        Q_EMIT playerHealthChanged(playerHealth);
        debugTextView->append("She got " + QString::number(dmg) + " dmg!");

        if (playerHealth <= 0) {
            character->showMood(speedMultiplier, "sad");
            PyErr_SetString(PyExc_StopIteration, "Dies from cringe...");
            silent = true;
        } else character->showMood(speedMultiplier, "annoyed");
    }
}
