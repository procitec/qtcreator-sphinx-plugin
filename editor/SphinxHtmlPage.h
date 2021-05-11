#pragma once
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

#include <qlitehtmlwidget.h>

namespace qtc::plugin::sphinx {

class HtmlPage : public QWidget
{
    Q_OBJECT
public:
    explicit HtmlPage(QWidget *parent = nullptr);
    virtual ~HtmlPage();

    QString url() const;
    void setUrl(const QUrl &);
    void updateView();

public Q_SLOTS:
    void onOpenUrl();
protected Q_SLOTS:
    void onChangedHtml(const QString &);
    void onBuildQueueFinished();

private:
    void setSourceInternal(const QUrl &url);

private:
    //QWebEngineView *mView;
    QLiteHtmlWidget *mView = nullptr;
    QLineEdit *mHtml = nullptr;
};

} // namespace qtc::plugin::sphinx
