//
// Created by silly on 29.05.25.
//

#ifndef APPWIDEVARIABLES_H
#define APPWIDEVARIABLES_H

#include <Qt>
#include <QtWidgets>

#include "Game.h"

class AppWideVariables {

public:
    static AppWideVariables& instance() {
        static AppWideVariables inst;
        return inst;
    }

    Game *game;
    QString levelPath, markdownText;

private:
    AppWideVariables() = default;
    ~AppWideVariables() = default;

    AppWideVariables(const AppWideVariables&) = delete;
    AppWideVariables& operator=(const AppWideVariables&) = delete;
};



#endif //APPWIDEVARIABLES_H
