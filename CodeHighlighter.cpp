//
// Created by silly on 24.02.2025.
//

#include "CodeHighlighter.h"

CodeHighlighter::CodeHighlighter(QTextDocument *parent)
        : QSyntaxHighlighter(parent) {

    // 1. Подсветка ключевых слов (Python)
    keywordFormat.setForeground(QBrush(qRgb(235, 149, 45)));
    QStringList keywords = {
            "def", "return", "class", "if", "else", "elif", "for", "while",
            "import", "from", "as", "with", "try", "except", "finally",
            "True", "False", "None", "break", "continue", "pass", "in"
    };
    for (const QString &keyword : keywords) {
        rules.append({QRegularExpression("\\b" + keyword + "\\b"), keywordFormat});
    }

    keywordFormat.setForeground(QBrush(qRgb(202, 179, 255)));
    rules.append({QRegularExpression("(?<=\\bdef\\s)\\b[A-Za-z_][A-Za-z0-9_]*\\b"), keywordFormat});
    rules.append({QRegularExpression("\\b[A-Za-z_][A-Za-z0-9_]*\\s*(?=\\()"), keywordFormat});

    keywordFormat.setForeground(QBrush(qRgb(77, 201, 255)));
    rules.append({QRegularExpression("(\\d+)"), keywordFormat});

    stringFormat.setForeground(Qt::green);
    rules.append({QRegularExpression("\".*?\""), stringFormat});
    rules.append({QRegularExpression("\'.*?\'"), stringFormat});

    commentFormat.setForeground(Qt::gray);
    rules.append({QRegularExpression("#[^\n]*"), commentFormat});

    commentFormat.setForeground(QBrush(qRgb(252, 91, 91)));
    rules.append({QRegularExpression("#![^\n]*"), commentFormat});
}

void CodeHighlighter::highlightBlock(const QString &text) {
    for (const HighlightRule &rule : rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}