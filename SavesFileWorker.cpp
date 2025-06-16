//
// Created by silly on 19.02.2025.
//

#include "SavesFileWorker.h"

SavesFileWorker::SavesFileWorker(const QString& fileName) {
    file = new QFile(fileName);
    qDebug() << "FILE: Name:" << fileName;
}

QString SavesFileWorker::getSaveFileText() {
    QString str = QCoreApplication::instance()->property("levelName").toString();
    qDebug() << str;
    auto lvlName = str.chopped(9).split('|');
    qDebug() << lvlName;
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("FILE: Error: Can't open file.");
        return nullptr;
    }
    QTextStream out(file);

    QString result;
    bool insideCategory = false;
    bool insideLevel = false;

    while (!out.atEnd()) {
        QString line = out.readLine();

        if (line == "#CTG/START: " + lvlName[0]) {
            insideCategory = true;
        }

        if (insideCategory && line == "#CTG/END! " + lvlName[0]) {
            insideCategory = false;
        }

        if (insideCategory && line == ">#LVL/START: " + lvlName[1]) {
            insideLevel = true;
            continue;
        }

        if (insideLevel && line == ">#LVL/END! " + lvlName[1]) {
            insideLevel = false;
            break;
        }

        // Сохраняем текст внутри нужного уровня
        if (insideLevel) {
            result += line + "\n";
        }
    }



    file->close();
    return result;
}

QString SavesFileWorker::setSaveFileText(QString &text) {
    text.replace(QRegularExpression("[ \n\t]+$"), "");

    QString str = QCoreApplication::instance()->property("levelName").toString();
    qDebug() << str;
    auto lvlName = str.chopped(9).split('|');
    qDebug() << lvlName;

    if (lvlName.size() < 2) {
        return "FILE: Error: Invalid level name format!";
    }

    QString category = lvlName[0];  // Имя категории
    QString level = lvlName[1];     // Имя уровня

    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "FILE: Error: Could not open script file and save!";
    }
    QTextStream in(file);

    QStringList lines;
    bool insideCategory = false;
    bool insideLevel = false;
    bool categoryExists = false;
    bool levelExists = false;

    int insertIndex = -1;  // Позиция для нового уровня
    int categoryStartIndex = -1;
    int categoryEndIndex = -1;

    while (!in.atEnd()) {
        QString line = in.readLine();

        // Найдено начало категории
        if (line.trimmed() == "#CTG/START: " + category) {
            insideCategory = true;
            categoryExists = true;
            categoryStartIndex = lines.size();
        }

        // Найден конец категории
        if (insideCategory && line.trimmed() == "#CTG/END! " + category) {
            insideCategory = false;
            categoryEndIndex = lines.size();
        }

        // Найдено начало уровня
        if (insideCategory && line.trimmed().startsWith(">#LVL/START: ")) {
            QString existingLevel = line.split(": ")[1].trimmed();

            // Если нашли нужный уровень, заменяем его содержимое
            if (existingLevel == level) {
                insideLevel = true;
                levelExists = true;
                lines.append(line);
                lines.append(text);  // Заменяем содержимое
                continue;
            }

            // Если уровень больше по номеру, запомним индекс для вставки перед ним
            if (existingLevel.toInt() > level.toInt() && insertIndex == -1) {
                insertIndex = lines.size();
            }
        }

        // Найден конец уровня
        if (insideLevel && line.trimmed().startsWith(">#LVL/END! ")) {
            insideLevel = false;
        }

        // Пропускаем строки внутри старого уровня
        if (insideLevel) {
            continue;
        }

        lines.append(line);
    }
    file->close();

    // Если категории нет — создаём её в правильном месте
    if (!categoryExists) {
        QStringList newCategory;
        newCategory << "#CTG/START: " + category;
        newCategory << ">#LVL/START: " + level;
        newCategory << text;
        newCategory << ">#LVL/END! " + level;
        newCategory << "#CTG/END! " + category;

        // Вставляем перед первой категорией или в конец, если категорий нет
        int categoryInsertIndex = (categoryStartIndex == -1) ? lines.size() : categoryStartIndex;
        lines.insert(categoryInsertIndex, newCategory.join("\n"));
    }
        // Если категория есть, но уровень отсутствует — вставляем по порядку
    else if (!levelExists) {
        QStringList newLevel;
        newLevel << ">#LVL/START: " + level;
        newLevel << text;
        newLevel << ">#LVL/END! " + level;

        // Вставляем перед первым уровнем, который имеет номер больше
        if (insertIndex != -1) {
            lines.insert(insertIndex, newLevel.join("\n"));
        }
            // Если таких нет — перед концом категории
        else if (categoryEndIndex != -1) {
            lines.insert(categoryEndIndex, newLevel.join("\n"));
        }
    }

    // Записываем изменения в файл
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return "FILE: Error: Could not open script file and save!";
    }
    QTextStream out(file);
    for (const QString &l : lines) {
        out << l << "\n";
    }
    file->close();

    return nullptr;
}




