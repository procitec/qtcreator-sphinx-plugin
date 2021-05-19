#pragma once
#include <QtCore/QFileInfo>
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

    QString htmlFile() const;
    void setHtmlFile(const QFileInfo &);
    void updateView();

public Q_SLOTS:
    void onOpenFile();
protected Q_SLOTS:
    void onChangedHtmlFile(const QString &);
    void onBuildQueueFinished();

private:
    void setSourceInternal(const QUrl &url);
    void reload();

private:
    //QWebEngineView *mView;
    QLiteHtmlWidget *mView = nullptr;
    QLineEdit *mHtml = nullptr;
};

} // namespace qtc::plugin::sphinx
