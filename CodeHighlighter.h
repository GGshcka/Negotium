//
// Created by silly on 24.02.2025.
//

#ifndef NEGOTIUM_CODEHIGHLIGHTER_H
#define NEGOTIUM_CODEHIGHLIGHTER_H

#include <Qt>
#include <QtGlobal>
#include <QWidget>
#include <qtextdocument.h>
#include <qsyntaxhighlighter.h>
#include <qregularexpression.h>

class CodeHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit CodeHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightRule> rules;

    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat stringFormat;
};


#endif //NEGOTIUM_CODEHIGHLIGHTER_H
