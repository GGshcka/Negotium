//
// Created by silly on 04.06.25.
//

#ifndef GUIDEVIEW_H
#define GUIDEVIEW_H

#include <QtWidgets>
#include "MDISubWindow.h"
#include <cmark.h>
#include <QWebEngineView>
#include <QWebEngineProfile>

class GuideView : public MDISubWindow {
public:
    GuideView();

    void setMarkdown(const QString& markdown);

private:
    QWidget *centerWidget;
    QGridLayout *sublayout;
    QWebEngineView *webView;
};



#endif //GUIDEVIEW_H
