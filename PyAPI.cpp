//
// Created by silly on 12.01.2025.
//

#include <python3.13/Python.h>
#include "PyAPI.h"

Game *PyAPI::game = nullptr;

void PyAPI::Initialize(Game *g) {
    game = g;

    if (PyImport_AppendInittab("game", &pyInit_game) == -1) {
        fprintf(stderr, "Error: could not extend in-built modules table\n");
        return;
    }

    Py_Initialize();
    if (!Py_IsInitialized()) {
        fprintf(stderr, "Python initialization failed\n");
        return;
    }
}

PyObject* PyAPI::move(PyObject* self, PyObject* args) {
    int direction;

    if (!PyArg_ParseTuple(args, "i", &direction)) {
        return nullptr;
    }

    game->Move(direction);

    QEventLoop loop;
    AnimatedGraphicsItem::connect(game, &Game::moveCompleted, &loop, &QEventLoop::quit);
    loop.exec();

    return PyLong_FromLong(1);
}

PyObject* PyAPI::canMove(PyObject* self, PyObject* args) {
    int direction;

    if (!PyArg_ParseTuple(args, "i", &direction)) {
        return nullptr;
    }

    return PyBool_FromLong(game->CanMove(direction));
}

PyMethodDef PyAPI::IntegrationMethods[] = {
        {"move", move, METH_VARARGS, "Move in a given direction"},
        {"canMove", canMove, METH_VARARGS, "Can move in direction?"},
        {nullptr, nullptr, 0, nullptr}
};

struct PyModuleDef PyAPI::IntegrationModule = {
        PyModuleDef_HEAD_INIT,
        "game",
        "Game integration module",
        -1,
        IntegrationMethods
};

PyObject* PyAPI::pyInit_game(void) {
    return PyModule_Create(&IntegrationModule);
}

void PyAPI::Finalize() {
    if (Py_IsInitialized()) {
        Py_Finalize();
    }
}

