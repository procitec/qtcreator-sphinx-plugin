#pragma once
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

namespace qtcreator::plugin::sphinx {

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

protected:
    //QWebEngineView *mView;
    QWidget *mView = nullptr;
    QLineEdit *mHtml = nullptr;
};

} // namespace qtcreator::plugin::sphinx
