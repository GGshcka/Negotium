//
// Created by silly on 29.05.25.
//

#include "Output.h"

Output::Output() : MDISubWindow(nullptr, "Output", QRect(20, 850, 850, 200))
{
    centerWidget = new QWidget(this);
    layout()->addWidget(centerWidget);

    textView = new QTextEdit();
    textView->setReadOnly(true);

    sublayout = new QGridLayout(centerWidget);
    sublayout->addWidget(textView, 0, 0);
}
