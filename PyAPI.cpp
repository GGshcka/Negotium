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

    int result = game->Move(direction);

    return PyLong_FromLong(result);
}

PyMethodDef PyAPI::IntegrationMethods[] = {
        {"move", move, METH_VARARGS, "Move in a given direction"},
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

