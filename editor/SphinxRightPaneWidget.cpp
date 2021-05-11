#include "SphinxRightPaneWidget.h"

#include <QtWidgets/QVBoxLayout>

namespace qtc::plugin::sphinx {

TabWidget::TabWidget(QWidget *w)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(w);

    setLayout(layout);
}

RightPaneWidget::RightPaneWidget(QWidget *parent)
    : QTabWidget(parent)
{
    mLivePreview = new LivePreviewPage();
    mPreview = new PreviewPage();
    mLivePreview->setEnabled(false);
    mPreview->setEnabled(false);
    insertTab(PAGE_LIVEPV, new TabWidget(mLivePreview), tr("LivePreview"));
    insertTab(PAGE_PV, new TabWidget(mPreview), tr("Preview"));
    setTabEnabled(PAGE_LIVEPV, false);
    setTabEnabled(PAGE_PV, false);
}

void RightPaneWidget::setCurrentTab(int idx)
{
    setCurrentIndex(idx);
    setTabEnabled(idx, true);
}

} // namespace qtc::plugin::sphinx
