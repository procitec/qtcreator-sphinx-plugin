#include "SphinxLivePreviewPage.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QVBoxLayout>

namespace qtc::plugin::sphinx {

static QByteArray getData(const QUrl &url)
{
    //    // TODO: this is just a hack for Qt documentation
    //    // which decides to use a simpler CSS if the viewer does not have JavaScript
    //    // which was a hack to decide if we are viewing in QTextBrowser or QtWebEngine et al
    //    QUrl actualUrl = url;
    //    QString path = url.path(QUrl::FullyEncoded);
    //    static const char simpleCss[] = "/offline-simple.css";
    //    if (path.endsWith(simpleCss)) {
    //        path.replace(simpleCss, "/offline.css");
    //        actualUrl.setPath(path);
    //    }
    //    const LocalHelpManager::HelpData help = LocalHelpManager::helpData(actualUrl);
    QByteArray data;
    if (url.isLocalFile()) {
        QFile file(url.toLocalFile());
        file.open(QIODevice::ReadOnly);
        data = file.readAll();
        file.close();
    }

    return data;
}

LivePreviewPage::LivePreviewPage(QWidget *parent)
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

LivePreviewPage::~LivePreviewPage() {}


void LivePreviewPage::setHtml(const QString &html)
{
    // preserve scroll position
    auto vPos = -1;
    auto hPos = -1;
    auto vBar = mView->verticalScrollBar();
    auto hBar = mView->horizontalScrollBar();
    if (vBar) {
        vPos = vBar->sliderPosition();
    }
    if (hBar) {
        hPos = hBar->sliderPosition();
    }

    mView->setHtml(html);
    if (0 < vPos && vBar) {
        vBar->setSliderPosition(vPos);
    }
    if (0 < hPos && hBar) {
        hBar->setSliderPosition(hPos);
    }
}

void LivePreviewPage::updateView()
{
    mView->update();
}
} // namespace qtc::plugin::sphinx
