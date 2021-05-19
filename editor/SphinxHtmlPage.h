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

protected:
    void wheelEvent(QWheelEvent *) override;
public Q_SLOTS:
    void onOpenFile();
protected Q_SLOTS:
    void onChangedHtmlFile(const QString &);
    void onBuildQueueFinished();
    void onContextMenuRequested(const QPoint &pos, const QUrl &url);

private:
    void setSourceInternal(const QUrl &url);
    void reload();

private:
    //QWebEngineView *mView;
    QLiteHtmlWidget *mView = nullptr;
    QLineEdit *mHtml = nullptr;
    const double mZoomStep = 0.1;
};

} // namespace qtc::plugin::sphinx
