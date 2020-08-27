#pragma once

#include "../editor/SphinxEditorWidget.h"

#include <memory>
#include <QtCore/QObject>

class TestFormatter : public QObject
{
    Q_OBJECT

public:
    TestFormatter();
    ~TestFormatter();

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

    void testCommentInEmptyDocument();
    void testSingleLineComment();
    void testSingleLineCommentCursorPos();
    void testSingleLineCommentCursorPos_data();
    void testSingleLineCommentSelection();
    void testSingleLineCommentSelection_data();
    void testSingleLineHeading();
    void testSingleLineHeadingCursorPos();
    void testSingleLineHeadingCursorPos_data();
    void testSingleLineHeadingSelection();
    void testSingleLineHeadingSelection_data();
    void testInsertTextAtBlockStart();
    void testInsertTextAtBlockStart_data();
    void testRemoveTextAtBlockStart();
    void testRemoveTextAtBlockStart_data();
    void testSingleLineIndent();
    void testSingleLineIndent_data();
    void testSingleLineSelectIndent();
    void testSingleLineSelectIndent_data();
    void testBlockIndent();
    void testBlockIndent_data();
    void testFormatBoldItalic();
    void testFormatBoldItalic_data();
    void testFormatDirective();
    void testFormatDirective_data();
    void testFormatIndent();
    void testFormatIndent_data();

    void testInsertLineText();
    void testInsertLineText_data();

public:
    qtcreator::plugin::sphinx::EditorWidget *mEditor = nullptr;
    Core::IEditor *mIEditor = nullptr;
    int mIndentSize = 0;
    QString mIndentStr;
};
