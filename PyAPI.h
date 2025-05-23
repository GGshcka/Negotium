//
// Created by silly on 12.01.2025.
//

#ifndef NEGOTIUM_PYAPI_H
#define NEGOTIUM_PYAPI_H


#include "Game.h"
#include <python3.13/Python.h>

class PyAPI {
public:
    static void Initialize(Game *g);
    static void Finalize();
private:
    static Game *game;
    static PyObject *move(PyObject* self, PyObject* args);
    static PyObject *canMove(PyObject* self, PyObject* args);
    static PyObject *wait(PyObject* self, PyObject* args);
    static PyObject *pyInit_game(void);
    static PyMethodDef IntegrationMethods[];
    static struct PyModuleDef IntegrationModule;
};

#endif //NEGOTIUM_PYAPI_H
