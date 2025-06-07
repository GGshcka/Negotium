//
// Created by silly on 29.05.25.
//

#ifndef OUTPUT_H
#define OUTPUT_H
#include "MDISubWindow.h"


class Output : public MDISubWindow {
public:
    Output();

    QTextEdit *textView;

private:
    QWidget *centerWidget;
    QGridLayout *sublayout;

};

#endif //OUTPUT_H
