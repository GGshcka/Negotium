//
// Created by silly on 19.02.2025.
//

#ifndef NEGOTIUM_SAVESFILEWORKER_H
#define NEGOTIUM_SAVESFILEWORKER_H


#include <Qt>
#include <QtWidgets>
#include <QtGlobal>
#include "Game.h"

class SavesFileWorker {
public:
    SavesFileWorker(const QString& fileName);

    QString getSaveFileText();
    QString setSaveFileText(QString &text);

private:
    QFile *file;
    QString *levelName;
};


#endif //NEGOTIUM_SAVESFILEWORKER_H
