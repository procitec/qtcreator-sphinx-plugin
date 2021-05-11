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
    mPreview = new PreviewPage();
    mHtml = new HtmlPage();
    mPreview->setEnabled(false);
    mHtml->setEnabled(false);
    insertTab(PAGE_PREVIEW, new TabWidget(mPreview), tr("Preview"));
    insertTab(PAGE_HTML, new TabWidget(mHtml), tr("Html"));
    setTabEnabled(PAGE_PREVIEW, false);
    setTabEnabled(PAGE_HTML, false);
}

void RightPaneWidget::setCurrentTab(int idx)
{
    setCurrentIndex(idx);
    setTabEnabled(idx, true);
}

} // namespace qtc::plugin::sphinx
