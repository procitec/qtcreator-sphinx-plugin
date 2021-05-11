#pragma once
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

#include <qlitehtmlwidget.h>

namespace qtc::plugin::sphinx {

class LivePreviewPage : public QWidget
{
    Q_OBJECT
public:
    explicit LivePreviewPage(QWidget *parent = nullptr);
    virtual ~LivePreviewPage() override;

    void updateView();
    void setHtml(const QString &);

private:
    //QWebEngineView *mView;
    QLiteHtmlWidget *mView = nullptr;
};

} // namespace qtc::plugin::sphinx
