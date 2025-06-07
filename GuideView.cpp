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
    webView->setHtml(QString::fromUtf8(html));
    free(html);
}
