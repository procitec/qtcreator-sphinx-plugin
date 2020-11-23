#include "SphinxLivePreviewPage.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include <QtWidgets/QFileDialog>

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

void LivePreviewPage::onChangedHtml(const QString &html)
{
    if (!html.isEmpty()) {
        auto url = QUrl(html);
        if (url.isValid()) {
            setSourceInternal(QUrl(QString("file://%1").arg(html)));
        }
    } else {
        //mView->clear();
    }
}

void LivePreviewPage::setSourceInternal(const QUrl &url)
{
    QUrl currentUrlWithoutFragment = mView->url();
    currentUrlWithoutFragment.setFragment({});
    QUrl newUrlWithoutFragment = url;
    newUrlWithoutFragment.setFragment({});
    mView->setUrl(url);
    if (currentUrlWithoutFragment != newUrlWithoutFragment)
        mView->setHtml(QString::fromUtf8(getData(url)));

    mView->setZoomFactor(1.1);
}

void LivePreviewPage::setHtml(const QString &html)
{
    mView->setHtml(html);
}

void LivePreviewPage::updateView()
{
    mView->update();
}

void LivePreviewPage::onOpenUrl()
{
    {
        auto fileName = QFileDialog::getOpenFileName(this,
                                                     tr("html file"),
                                                     "",
                                                     tr("HTML (*.html *.htm)"));
        QFileInfo file(fileName);
        if (file.exists() && file.isReadable()) {
            onChangedHtml(file.absoluteFilePath());

        } else {
        }
    }
}

void LivePreviewPage::setUrl(const QUrl &url)
{
    setSourceInternal(url);
}

} // namespace qtc::plugin::sphinx
