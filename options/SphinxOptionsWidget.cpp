#include "SphinxOptionsWidget.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

namespace qtcreator::plugin::sphinx {

OptionsWidget::OptionsWidget(const Settings *settings)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    auto *customHighLighter = new QGroupBox(tr("Custom Highlighter"));
    auto *customLayout = new QVBoxLayout(customHighLighter);
    auto *customFirstRow = new QHBoxLayout();

    mUseCustomHighlighter.setChecked(settings->useCustomHighlighter());

    customFirstRow->addWidget(&mUseCustomHighlighter);

    QLabel *label = new QLabel(tr("Use plugin highlighter"));
    customFirstRow->addWidget(label);
    customFirstRow->addSpacerItem(
        new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    customLayout->addLayout(customFirstRow);

    layout->addWidget(customHighLighter);

    auto *reSTCheckHighlighter = new QGroupBox(tr("rstcheck Highlighter"));
    auto *reSTLayout = new QVBoxLayout(reSTCheckHighlighter);
    auto *reSTFirstRow = new QHBoxLayout();
    reSTLayout->addLayout(reSTFirstRow);
    auto *reSTSecondRow = new QHBoxLayout();
    reSTLayout->addLayout(reSTSecondRow);

    mUseReSTCheckHighlighter.setChecked(settings->useReSTCheckHighlighter());
    reSTFirstRow->addWidget(&mUseReSTCheckHighlighter);
    //    reSTLayout->addWidget(&mUseReSTCheckHighlighter, 0, 0);

    auto *reSTCheckLabel = new QLabel(
        tr("Enable rstcheck (<a href=\"https://github.com/myint/rstcheck\">rstcheck "
           "Homepage</a>)"));
    reSTCheckLabel->setTextFormat(Qt::RichText);
    reSTCheckLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    reSTCheckLabel->setOpenExternalLinks(true);

    reSTFirstRow->addWidget(reSTCheckLabel);

    reSTFirstRow->addSpacerItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    auto *reSTCheckFilePathLabel = new QLabel(tr("python3 command"));
    reSTSecondRow->addWidget(reSTCheckFilePathLabel);

    mReSTCheckFilePath.setText(settings->pythonFilePath());

    reSTSecondRow->addWidget(&mReSTCheckFilePath);

    auto *reSTCheckFilePathBrowse = new QPushButton(QIcon::fromTheme("document-open"), QString());
    reSTCheckFilePathBrowse->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    reSTSecondRow->addWidget(reSTCheckFilePathBrowse);

    connect(reSTCheckFilePathBrowse, &QAbstractButton::pressed, this, [=]() {
        auto fileName = QFileDialog::getOpenFileName(this, tr("python3 executable"), "");
        QFileInfo file(fileName);
        if (file.exists() && file.isExecutable()) {
            mReSTCheckFilePath.setText(file.absoluteFilePath());
        } else {
            mReSTCheckFilePath.clear();
            mReSTCheckFilePath.setToolTip(
                QString("invalid python3 executable %1").arg(file.absoluteFilePath()));
        }
    });
    reSTSecondRow->addSpacerItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    layout->addWidget(reSTCheckHighlighter);

    layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
} // namespace qtcreator::plugin::sphinx

Settings OptionsWidget::settings() const
{
    Settings settings;
    settings.setUseCustomHighlighter(mUseCustomHighlighter.isChecked());

    QFileInfo info(mReSTCheckFilePath.text());
    if (info.exists() && info.isExecutable()) {
        settings.setPythonFilePath(info.absoluteFilePath());
    } else {
        settings.setPythonFilePath(QString());
    }

    settings.setUseReSTCheckHighlighter(mUseReSTCheckHighlighter.isChecked());

    return settings;
}

} // namespace qtcreator::plugin::sphinx
