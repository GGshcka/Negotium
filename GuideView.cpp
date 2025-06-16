//
// Created by silly on 04.06.25.
//

#include "GuideView.h"

GuideView::GuideView() : MDISubWindow(nullptr, "Guide", QRect(1050, 40, 500, 800))
{
    centerWidget = new QWidget(this);
    layout()->addWidget(centerWidget);

    webView = new QWebEngineView();

    sublayout = new QGridLayout(centerWidget);
    sublayout->addWidget(webView, 0, 0);
}

void GuideView::setMarkdown(const QString& markdown)
{
    QByteArray utf8 = markdown.toUtf8();
    char *html = cmark_markdown_to_html(utf8.constData(), utf8.size(), CMARK_OPT_DEFAULT);
    webView->setHtml(QString("<style>* {color: rgb(174, 149, 229);} body {background-color: rgb(59, 48, 84);} code {background-color: rgb(87, 70, 123); color: white; padding: 10px; border-radius: 5px; border: 1px solid rgb(174, 149, 229);}</style>") + QString::fromUtf8(html));
    qDebug() << html;
    free(html);
}
