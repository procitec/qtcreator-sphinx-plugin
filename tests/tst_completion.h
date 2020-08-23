#pragma once

#include "../editor/SphinxCompletionAssist.h"
#include "../editor/SphinxEditorWidget.h"

#include <memory>
#include <QtCore/QObject>

class TestCompletion : public QObject
{
    Q_OBJECT

public:
    TestCompletion();
    ~TestCompletion();

    void cleanupEditor();
    void createEditor();
    QString selectedText() const;
    void selectText(int start, int end);
    int cursorPos() const;
    int setCursorPos(int pos);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testSimpleDirective();
    void testSimpleDirective_data();

    void testSimpleRole();
    void testSimpleRole_data();

    void testSimpleSnippetDirective();
    void testSimpleSnippetDirective_data();

    void testSimpleSnippetRole();
    void testSimpleSnippetRole_data();

public:
    qtcreator::plugin::sphinx::EditorWidget *mEditor = nullptr;
    Core::IEditor *mIEditor = nullptr;
    int mIndentSize = 0;
    QString mIndentStr;
};
