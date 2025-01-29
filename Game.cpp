#include "Game.h"
#include <Qt>
#include <QtWidgets>
#include <QPoint>
#include <QList>
#include "PyAPI.h"
#include <python3.13/Python.h>
#include <QtGlobal>

Game::Game(QTextEdit *edit, QTextEdit *debugText) {
    textEdit = edit;
    debugTextView = debugText;
    scene = new QGraphicsScene(this);
    setScene(scene);

    if(loadLevel()) qInfo() << "Successful loaded...";
    else qDebug() << "Can't load file...";

    character = new AnimatedGraphicsItem(PDown->scaled(gridSize - 1 ,gridSize - 1));
    character->setPos(1, 1);
    endPos = character->pos();
    scene->addItem(character);

    setFocusPolicy(Qt::StrongFocus);
}

bool Game::CanMove(int direction) {
    canMove = true;
    switch (direction) {
        case 0: // UP
            if (character->pos().y() - gridSize < 0) canMove = false;
            break;
        case 1: // DOWN
            if (character->pos().y() + gridSize >= scene->height()) canMove = false;
            break;
        case 2: // LEFT
            if (character->pos().x() - gridSize < 0) canMove = false;
            break;
        case 3: // RIGHT
            if (character->pos().x() + gridSize >= scene->width()) canMove = false;
            break;
    }
    return canMove;
}

qreal customEasingFunction(qreal progress) {
    return sin(progress * M_PI / 2); // Плавное движение вперед //!Психованное движение tan(progress*M_PI*progress*M_PI)
}

void Game::Move(int direction) {
    QPointF startPos = character->pos();

    switch (direction) {
        case 0: // UP
            if (startPos.y() - gridSize >= 0) {
                character->setPixmap(QPixmap(PUp->scaled(gridSize - 1, gridSize - 1)));
                endPos.ry() -= gridSize;
            } else PlayerHit(1);
            break;
        case 1: // DOWN
            if (startPos.y() + gridSize < scene->height()) {
                character->setPixmap(QPixmap(PDown->scaled(gridSize - 1, gridSize - 1)));
                endPos.ry() += gridSize;
            } else PlayerHit(1);
            break;
        case 2: // LEFT
            if (startPos.x() - gridSize >= 0) {
                character->setPixmap(QPixmap(PLeft->scaled(gridSize - 1, gridSize - 1)));
                endPos.rx() -= gridSize;
            } else PlayerHit(1);
            break;
        case 3: // RIGHT
            if (startPos.x() + gridSize < scene->width()) {
                character->setPixmap(QPixmap(PRight->scaled(gridSize - 1, gridSize - 1)));
                endPos.rx() += gridSize;
            } else PlayerHit(1);
            break;
    }

    for (const auto &pit: pitCords) {
        if (endPos == pit * gridSize + QPoint(1, 1)) {
            endPos = character->pos();
            debugTextView->append("Falled...");
            PlayerHit(playerMaxHealth);
            break;
        }
    }

    QPropertyAnimation *animation = new QPropertyAnimation(character, "pos", this);
    animation->setDuration(700); // Время анимации
    animation->setStartValue(character->pos()); // Текущее значение pos
    animation->setEndValue(endPos);      // Конечная позиция
    QEasingCurve easingCurve;
    easingCurve.setCustomType(customEasingFunction);
    animation->setEasingCurve(easingCurve);
    connect(animation, &QPropertyAnimation::finished, this, [=]() {
        character->setPos(endPos);
        Q_EMIT moveCompleted();
    });
    animation->start();
}

void Game::Run() {
    if (character->pos() != QPointF(1, 1)) {
        character->setPos(1, 1);
        endPos = character->pos();
        actions.resize(0);
        playerHealth = playerMaxHealth;
        Q_EMIT playerHealthChanged(playerHealth);
        debugTextView->clear();
        silent = false;
    }

    PyAPI::Initialize(this);

    PyObject *globals = PyDict_New();
    PyObject *locals = PyDict_New();

    QString text = textEdit->toPlainText();
    const char *textData = text.toUtf8().constData();

    const char* filename = "script.py";
    FILE *file = fopen(filename, "w+");
    if (!file) {
        debugTextView->append("Error: Could not open/create script file and save!");
        return;
    }
    fwrite(textData, sizeof(char), text.length(), file);
    fclose(file);

    QString readyPythonCode = QString("from game import *\n") + textData;

    PyObject *result = PyRun_String(readyPythonCode.toUtf8().constData(), Py_file_input, globals, locals);

    //execActions();

    if (result == nullptr && !silent) {
        QTime *time = new QTime();
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);
        debugTextView->append(time->currentTime().toString() + " | Error: " + PyUnicode_AsUTF8(PyObject_Str(value)));
        PyErr_Restore(type, value, traceback);
    } else debugTextView->append("Done");

    Py_DECREF(globals);
    Py_DECREF(locals);

    PyAPI::Finalize();
}

void Game::createGrid() const {
    for (int x = 0; x <= scene->width(); x += gridSize) {
        scene->addLine(x, 0, x, scene->height(), QPen(qRgb(174, 149, 229)));
    }

    for (int y = 0; y <= scene->height(); y += gridSize) {
        scene->addLine(0, y, scene->width(), y, QPen(qRgb(174, 149, 229)));
    }
}

bool Game::loadLevel() {
    QFile* file = new QFile("../Resources/Levels/level-Debug.xml");
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamReader xml(file);
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            auto tagName = xml.qualifiedName();
            
            if (tagName == "level") {
                QXmlStreamAttributes attributes = xml.attributes();
                gridColumnCount = attributes.value("columns").toInt();
                gridRowCount = attributes.value("rows").toInt();
            } else if (tagName == "pit") {
                QXmlStreamAttributes attributes = xml.attributes();
                pitCords.append(QPoint(attributes.value("x").toInt(), attributes.value("y").toInt()));
            }
        }
    }

    scene->setSceneRect(0, 0, gridSize * gridColumnCount, gridSize * gridRowCount);
    scene->addRect(0,0,scene->width(), scene->height(), QPen(qRgb(69, 56, 96)), QBrush(qRgb(69, 56, 96)));

    createGrid();

    for (int i = 0; i < pitCords.length(); i++) {
        scene->addRect(gridSize * pitCords[i].x(), gridSize * pitCords[i].y(), gridSize, gridSize, QPen(qRgb(0, 0, 0)), QBrush(qRgb(0, 0, 0)));
    }

    return true;
}

int Game::PlayerHPGet() const {
    return playerHealth;
}

void Game::PlayerHit(unsigned int dmg) {
    if (playerHealth > 0) {
        playerHealth-=dmg;
        Q_EMIT playerHealthChanged(playerHealth);
        debugTextView->append("She got " + QString::number(dmg) + " dmg!");
        if (playerHealth <= 0) {
            actions.resize(0);
            PyErr_SetString(PyExc_StopIteration, "Dies from cringe...");
            silent = true;
        }
    }
}