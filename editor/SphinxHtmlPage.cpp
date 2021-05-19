#include "SphinxHtmlPage.h"
#include "SphinxWidgetHelpers.h"

#include <projectexplorer/buildmanager.h>

#include <QtCore/QUrl>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
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

HtmlPage::HtmlPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
    auto *customFirstRow = new QHBoxLayout();

    auto *label = new QLabel(tr("Preview Url:"));
    customFirstRow->addWidget(label);
    mHtml = new QLineEdit(this);
    mHtml->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

    connect(mHtml, &QLineEdit::editingFinished, this, [=]() { onChangedHtmlFile(mHtml->text()); });
    customFirstRow->addWidget(mHtml);

    auto *htmlFilePathBrowse = new QPushButton(QIcon::fromTheme("document-open"), QString());
    htmlFilePathBrowse->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    customFirstRow->addWidget(htmlFilePathBrowse);

    connect(htmlFilePathBrowse, &QAbstractButton::pressed, this, &HtmlPage::onOpenFile);

    //    customFirstRow->addSpacerItem(
    //        new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    layout->addLayout(customFirstRow);

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

    ProjectExplorer::BuildManager *buildManager = ProjectExplorer::BuildManager::instance();
    connect(buildManager,
            &ProjectExplorer::BuildManager::buildQueueFinished,
            this,
            &HtmlPage::onBuildQueueFinished);
}

HtmlPage::~HtmlPage() {}

void HtmlPage::onChangedHtmlFile(const QString &html)
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

void HtmlPage::updateView()
{
    if (htmlFile().isEmpty()) {
        onOpenFile();
    } else {
        setSourceInternal(htmlFile());
        mView->update();
    }
}

void HtmlPage::onBuildQueueFinished()
{
    if (!htmlFile().isEmpty()) {
        auto pos = SphinxWidgetHelpers::scrollBarPos(mView);
        reload();
        //mView->update();
        SphinxWidgetHelpers::setScrollBarPos(mView, pos);
    }
}

void HtmlPage::setSourceInternal(const QUrl &url)
{
    QUrl currentUrlWithoutFragment = mView->url();
    currentUrlWithoutFragment.setFragment({});
    QUrl newUrlWithoutFragment = url;
    newUrlWithoutFragment.setFragment({});
    mView->setUrl(url);
    //qDebug() << "current url is set to " << mView->url().toString();
    if (currentUrlWithoutFragment != newUrlWithoutFragment) {
        mView->setHtml(QString::fromUtf8(getData(url)));
    }

    //mView->setZoomFactor(1.1);
}
void HtmlPage::reload()
{
    if (!mView->url().isEmpty()) {
        mView->setHtml(QString::fromUtf8(getData(mView->url())));
    }
    //mView->setZoomFactor(1.1);
}

void HtmlPage::onOpenFile()
{
    {
        auto text = mHtml->text();
        auto dir = QString();
        if (!text.isEmpty()) {
            auto fileinfo = QFileInfo(text);
            dir = (fileinfo.absoluteDir().exists()) ? fileinfo.dir().absolutePath() : QString();
        }
        auto fileName = QFileDialog::getOpenFileName(this,
                                                     tr("html file"),
                                                     dir,
                                                     tr("HTML (*.html *.htm)"));
        QFileInfo file(fileName);
        if (file.exists() && file.isReadable()) {
            mHtml->setText(file.absoluteFilePath());
            onChangedHtmlFile(file.absoluteFilePath());

        } else {
            mHtml->clear();
            mHtml->setToolTip(QString("invalid html file %1").arg(file.absoluteFilePath()));
        }
    }
}

QString HtmlPage::htmlFile() const
{
    return mHtml->text();
}

void HtmlPage::setHtmlFile(const QFileInfo &file)
{
    mHtml->setText(file.absoluteFilePath());
    onChangedHtmlFile(mHtml->text());
}

} // namespace qtc::plugin::sphinx
