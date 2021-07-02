#pragma once

#include "../editor/SphinxEditorWidget.h"

#include <memory>
#include <QtCore/QObject>
namespace qtc::plugin::sphinx {
class TestRightPane : public QObject
{
    Q_OBJECT

public:
    TestRightPane();
    ~TestRightPane();

    void cleanupEditor();
    void createEditor();
    QString selectedText() const;
    void selectText(int start, int end);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testOpenRightPane();

public:
    qtc::plugin::sphinx::EditorWidget *mEditor = nullptr;
    Core::IEditor *mIEditor = nullptr;
};
} // namespace qtc::plugin::sphinx
