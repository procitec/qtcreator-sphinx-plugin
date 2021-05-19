#include "SphinxPreviewPage.h"
#include "SphinxWidgetHelpers.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QVBoxLayout>

namespace qtc::plugin::sphinx {

static QByteArray getData(const QUrl &url)
{
    QByteArray data;
    if (url.isLocalFile()) {
        QFile file(url.toLocalFile());
        file.open(QIODevice::ReadOnly);
        data = file.readAll();
        file.close();
    }

    return data;
}

PreviewPage::PreviewPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    mView = new QLiteHtmlWidget(this);

    mView->show();
    layout->addWidget(mView);

    QPalette p = palette();
    p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Active, QPalette::Highlight));
    p.setColor(QPalette::Inactive,
               QPalette::HighlightedText,
               p.color(QPalette::Active, QPalette::HighlightedText));
    p.setColor(QPalette::Base, Qt::white);
    p.setColor(QPalette::Text, Qt::black);
    mView->setPalette(p);
    mView->setResourceHandler([](const QUrl &url) { return getData(url); });
    setLayout(layout);
}

PreviewPage::~PreviewPage() {}

void PreviewPage::setHtml(const QString &html)
{
    // preserve scroll position
    auto pos = SphinxWidgetHelpers::scrollBarPos(mView);
    mView->setHtml(html);
    SphinxWidgetHelpers::setScrollBarPos(mView, pos);
}

void PreviewPage::updateView()
{
    mView->update();
}
} // namespace qtc::plugin::sphinx
