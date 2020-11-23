#pragma once

#include <QtWidgets/QTabWidget>

#include "SphinxLivePreviewPage.h"
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
    enum { PAGE_LIVEPV = 0, PAGE_PV };

    LivePreviewPage &livePreview()
    {
        assert(mLivePreview);
        return *mLivePreview;
    }
    PreviewPage &preview()
    {
        assert(mPreview);
        return *mPreview;
    };
    void setCurrentTab(int);

signals:

private:
    LivePreviewPage *mLivePreview = nullptr;
    PreviewPage *mPreview = nullptr;
};

} // namespace qtc::plugin::sphinx
