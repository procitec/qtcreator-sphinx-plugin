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
    virtual ~PreviewPage();

    QString url() const;
    void setUrl(const QUrl &);

public Q_SLOTS:
    void onOpenUrl();
protected Q_SLOTS:
    void onChangedHtml(const QString &);

private:
    void setSourceInternal(const QUrl &url);

private:
    //QWebEngineView *mView;
    QLiteHtmlWidget *mView = nullptr;
    QLineEdit *mHtml = nullptr;
};

} // namespace qtc::plugin::sphinx
