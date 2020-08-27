#include "SphinxPreviewPage.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include <QtWidgets/QFileDialog>

namespace qtc::plugin::sphinx {

PreviewPage::PreviewPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    auto *customFirstRow = new QHBoxLayout();

    auto *label = new QLabel(tr("Preview Url:"));
    customFirstRow->addWidget(label);
    mHtml = new QLineEdit();
    mHtml->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

    connect(mHtml, &QLineEdit::editingFinished, this, [=]() { onChangedHtml(mHtml->text()); });
    customFirstRow->addWidget(mHtml);

    auto *htmlFilePathBrowse = new QPushButton(QIcon::fromTheme("document-open"), QString());
    htmlFilePathBrowse->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    customFirstRow->addWidget(htmlFilePathBrowse);

    connect(htmlFilePathBrowse, &QAbstractButton::pressed, this, &PreviewPage::onOpenUrl);

    //    customFirstRow->addSpacerItem(
    //        new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    layout->addLayout(customFirstRow);

    mView = new QWidget(this);
    layout->addWidget(mView);

    setLayout(layout);
}

PreviewPage::~PreviewPage() {}

void PreviewPage::onChangedHtml(const QString &html)
{
    if (!html.isEmpty()) {
        auto url = QUrl(html);
        if (url.isValid()) {
            //mView->showUrl(QString("file://%1).arg(html))
        }
    } else {
        //mView->clear();
    }
}

void PreviewPage::onOpenUrl()
{
    {
        auto fileName = QFileDialog::getOpenFileName(this,
                                                     tr("html file"),
                                                     "",
                                                     tr("HTML (*.html *.htm)"));
        QFileInfo file(fileName);
        if (file.exists() && file.isReadable()) {
            mHtml->setText(file.absoluteFilePath());
            onChangedHtml(file.absoluteFilePath());

        } else {
            mHtml->clear();
            mHtml->setToolTip(QString("invalid html file %1").arg(file.absoluteFilePath()));
        }
    }
}

QString PreviewPage::url() const
{
    return mHtml->text();
}

void PreviewPage::setUrl(const QUrl &url)
{
    mHtml->setText(url.toString());
    //mView->loadUrl()
}

} // namespace qtc::plugin::sphinx
