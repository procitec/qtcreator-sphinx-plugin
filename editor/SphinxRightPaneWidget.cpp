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
    insertTab(PAGE_PREVIEW, new TabWidget(mPreview), tr("Preview"));
    insertTab(PAGE_HTML, new TabWidget(mHtml), tr("Html"));
}

void RightPaneWidget::setCurrentTab(int idx)
{
    setCurrentIndex(idx);
}

} // namespace qtc::plugin::sphinx
