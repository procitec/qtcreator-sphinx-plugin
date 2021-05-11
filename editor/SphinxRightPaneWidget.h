#pragma once

#include <QtWidgets/QTabWidget>

#include "SphinxHtmlPage.h"
#include "SphinxPreviewPage.h"

namespace qtc::plugin::sphinx {

class TabWidget : public QWidget
{
    Q_OBJECT
public:
    TabWidget(QWidget *);

    virtual ~TabWidget() override = default;
};

class RightPaneWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit RightPaneWidget(QWidget *parent = nullptr);
    enum { PAGE_PREVIEW = 0, PAGE_HTML };

    PreviewPage &preview()
    {
        assert(mPreview);
        return *mPreview;
    }
    HtmlPage &html()
    {
        assert(mHtml);
        return *mHtml;
    };
    void setCurrentTab(int);

signals:

private:
    PreviewPage *mPreview = nullptr;
    HtmlPage *mHtml = nullptr;
};

} // namespace qtc::plugin::sphinx
