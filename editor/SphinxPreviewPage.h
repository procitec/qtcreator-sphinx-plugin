#pragma once
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

#include <qlitehtmlwidget.h>

namespace qtc::plugin::sphinx {

class PreviewPage : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewPage(QWidget *parent = nullptr);
    virtual ~PreviewPage() override;

    void updateView();
    void setHtml(const QString &);

private:
    //QWebEngineView *mView;
    QLiteHtmlWidget *mView = nullptr;
};

} // namespace qtc::plugin::sphinx
