#include "Game.h"
#include "PyAPI.h"
#include <python3.13/Python.h>
#include <QTimer>
#include <QMessageBox>
#include <QThread>
#include <QPropertyAnimation>
#include <QTapAndHoldGesture>
#include <QRegularExpression>


Game::Game(QTextEdit *edit, QTextEdit *debugText) {
    textEdit = edit;
    debugTextView = debugText;
    scene = new QGraphicsScene(this);
    setScene(scene);

    scene->setSceneRect(10, 10, 640, 640);

    createGrid();

    character = new AnimatedGraphicsItem(QPixmap("../robot.png").scaled(gridSize,gridSize));
    character->setPos(0, 0);
    scene->addItem(character);

    animationGroup = new QSequentialAnimationGroup(this);

    setFocusPolicy(Qt::StrongFocus);
}

int Game::Move(int direction) {
    QPointF startPos = character->pos(); // Текущая позиция
    QPointF endPos = startPos;           // Конечная позиция

    // Определяем новое положение на основе направления
    switch (direction) {
        case 0: // UP
            if (startPos.y() - gridSize >= 0) {
                endPos.ry() -= gridSize;
            }
            break;
        case 1: // DOWN
            if (startPos.y() + gridSize < scene->height()) {
                endPos.ry() += gridSize;
            }
            break;
        case 2: // LEFT
            if (startPos.x() - gridSize >= 0) {
                endPos.rx() -= gridSize;
            }
            break;
        case 3: // RIGHT
            if (startPos.x() + gridSize < scene->width()) {
                endPos.rx() += gridSize;
            }
            break;
        default:
            return -1; // Некорректное направление
    }

    // Создаем анимацию
    QPropertyAnimation *animation = new QPropertyAnimation(character, "pos", this);
    animation->setDuration(750); // Время анимации
    animation->setStartValue(startPos); // Текущее значение pos
    animation->setEndValue(endPos);             // Конечная позиция
    animation->setEasingCurve(QEasingCurve::InOutElastic);

    if (endPos != startPos) {
        // Добавляем анимацию в группу
        animationGroup->addAnimation(animation);

        // Запускаем группу, если она не запущена
        if (animationGroup->state() != QAbstractAnimation::Running) {
            animationGroup->start();
        }

        character->setPos(endPos);
    }

    return direction;
}

void Game::Run() {
    if (character->pos() != QPointF(0, 0)) {
        character->setPos(0, 0);
    }
    animationGroup->stop();
    animationGroup->clear();
    textEdit->clearFocus();

    PyAPI::Initialize(this);

    PyObject *globals = PyDict_New();
    PyObject *locals = PyDict_New();

    QString text = textEdit->toPlainText();
    const char *textData = text.toUtf8().constData();

    const char* filename = "script.py";
    FILE *file = fopen(filename, "w+");
    if (!file) {
        debugTextView->append("Error: Could not open script file.");
        return;
    }
    fwrite(textData, sizeof(char), text.length(), file);
    fclose(file);

    PyObject *result = PyRun_String(textData, Py_file_input, globals, locals);

    if (result == nullptr) {
        QTime *time = new QTime();
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);
        debugTextView->append(time->currentTime().toString() + " | Error: " + PyUnicode_AsUTF8(PyObject_Str(value)));
        PyErr_Restore(type, value, traceback);
    }

    Py_DECREF(globals);
    Py_DECREF(locals);

    PyAPI::Finalize();
}

void Game::createGrid() {
    for (int x = 0; x <= scene->width(); x += gridSize) {
        scene->addLine(x, 0, x, scene->height(), QPen(Qt::lightGray));
    }

    for (int y = 0; y <= scene->height(); y += gridSize) {
        scene->addLine(0, y, scene->width(), y, QPen(Qt::lightGray));
    }
}

/*QFile file("script.py");
   if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
       QMessageBox::warning(this, tr("Error"), tr("Cannot save file: %1").arg(file.errorString()));
       return; // Если не удалось открыть файл для записи, выходим
   }
   QTextStream out(&file); // Создаем QTextStream для записи в файл
   out << textEdit->toPlainText(); // Записываем текст из QTextEdit
   file.close(); // Закрываем файл*/

/*const char* filename = "script.py";
FILE *file = fopen(filename, "w+");

QString text = textEdit->toPlainText();
const char *textData = text.toUtf8().constData();

size_t written = fwrite(textData, sizeof(char), text.length(), file);

PyObject *result = PyRun_File(file, filename, Py_file_input, PyEval_GetGlobals(), PyEval_GetGlobals());

fclose(file);*/

/*if (luaL_dofile(luaApi::lstate, "script.lua") != LUA_OK) {
    QTime *time = new QTime();
    debugTextView->append(time->currentTime().toString() + " | Error: " + lua_tostring(luaApi::lstate, -1));
    lua_pop(luaApi::lstate, 1);
}*/