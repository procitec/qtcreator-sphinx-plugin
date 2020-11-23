#include "SphinxOptionsWidget.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

namespace qtc::plugin::sphinx {

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

    auto *reSTToolsGroup = new QGroupBox(tr("ReST Tools"));
    auto *reSTLayout = new QVBoxLayout(reSTToolsGroup);
    auto *reSTFirstRow = new QHBoxLayout();
    reSTLayout->addLayout(reSTFirstRow);
    auto *reSTSecondRow = new QHBoxLayout();
    reSTLayout->addLayout(reSTSecondRow);
    auto *reSTThirdRow = new QHBoxLayout();
    reSTLayout->addLayout(reSTThirdRow);

    auto *reSTCheckFilePathLabel = new QLabel(tr("python3 command"));
    reSTFirstRow->addWidget(reSTCheckFilePathLabel);

    mReSTCheckFilePath.setText(settings->pythonFilePath());

    reSTFirstRow->addWidget(&mReSTCheckFilePath);

    auto *reSTCheckFilePathBrowse = new QPushButton(QIcon::fromTheme("document-open"), QString());
    reSTCheckFilePathBrowse->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    reSTFirstRow->addWidget(reSTCheckFilePathBrowse);

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
    reSTFirstRow->addSpacerItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    mUseReSTCheckHighlighter.setChecked(settings->useReSTCheckHighlighter());
    reSTSecondRow->addWidget(&mUseReSTCheckHighlighter);
    //    reSTLayout->addWidget(&mUseReSTCheckHighlighter, 0, 0);

    auto *reSTCheckLabel = new QLabel(
        tr("Enable rstcheck (<a href=\"https://github.com/myint/rstcheck\">rstcheck "
           "Homepage</a>)"));
    reSTCheckLabel->setTextFormat(Qt::RichText);
    reSTCheckLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    reSTCheckLabel->setOpenExternalLinks(true);

    reSTSecondRow->addWidget(reSTCheckLabel);
    reSTSecondRow->addSpacerItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    mUseRST2HTML.setChecked(settings->useRST2HTML());
    reSTThirdRow->addWidget(&mUseRST2HTML);
    auto *rst2htmlLabel = new QLabel(
        tr("Enable rst2html (<a href=\"https://pypi.org/project/rst2html5/\">rst2html5 "
           "Homepage</a>)"));
    rst2htmlLabel->setTextFormat(Qt::RichText);
    rst2htmlLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    rst2htmlLabel->setOpenExternalLinks(true);

    reSTThirdRow->addWidget(rst2htmlLabel);
    reSTThirdRow->addSpacerItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    layout->addWidget(reSTToolsGroup);

    layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
}

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
    settings.setUseRST2HTML(mUseRST2HTML.isChecked());

    return settings;
}

} // namespace qtc::plugin::sphinx
