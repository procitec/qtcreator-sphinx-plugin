#include "tst_formatter.h"

#include "../editor/SphinxHighlighter.h"
#include "../qtcreator-sphinx-pluginconstants.h"

#include "coreplugin/coreplugin.h"
#include "texteditor/syntaxhighlighter.h"
#include <texteditor/textdocumentlayout.h>

#include <QtCore/QCoreApplication>
#include <QtGui/QTextDocumentFragment>
#include <QtTest/QtTest>

// add necessary includes here
TestFormatter::TestFormatter() {}

TestFormatter::~TestFormatter() {}

void TestFormatter::initTestCase() {}

void TestFormatter::cleanupTestCase()
{
    cleanupEditor();
}

void TestFormatter::init()
{
    createEditor();
}

void TestFormatter::cleanup()
{
    cleanupEditor();
}

void TestFormatter::createEditor()
{
    mIEditor = Core::EditorManager::openEditorWithContents(
        qtcreator::plugin::sphinx::Constants::EditorId);
    mEditor = static_cast<qtcreator::plugin::sphinx::EditorWidget *>(mIEditor->widget());
    QVERIFY(TextEditor::TabSettings::SpacesOnlyTabPolicy
            == mEditor->textDocument()->tabSettings().m_tabPolicy);
    mIndentStr = QString().fill(' ', mEditor->indentSize());

    mEditor->show();
    qApp->processEvents();
}

void TestFormatter::cleanupEditor()
{
    mEditor->clear();
    mEditor->close();
}

QString TestFormatter::selectedText() const
{
    qApp->processEvents();
    auto tc = mEditor->textCursor();
    return tc.selectedText();
}

void TestFormatter::selectText(int start, int end)
{
    qApp->processEvents();
    auto tc = mEditor->textCursor();
    tc.setPosition(start);
    QCOMPARE(tc.position(), start);
    tc.setPosition(end, QTextCursor::KeepAnchor);
    QCOMPARE(tc.position(), end);
    mEditor->setTextCursor(tc);
    qApp->processEvents();
}

int TestFormatter::cursorPos() const
{
    qApp->processEvents();
    auto tc = mEditor->textCursor();
    return tc.position();
}

int TestFormatter::setCursorPos(int pos)
{
    qApp->processEvents();
    auto tc = mEditor->textCursor();
    tc.setPosition(pos);
    mEditor->setTextCursor(tc);
    qApp->processEvents();
    return tc.position();
}

void TestFormatter::testCommentInEmptyDocument()
{
    mEditor->onAddComment();
    auto content = mEditor->toPlainText();
    QCOMPARE(content, QString("..\n%1\n").arg(mIndentStr));

    mEditor->onRemoveComment();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString());
}

void TestFormatter::testSingleLineComment()
{
    mEditor->insertPlainText("abc");
    mEditor->onAddComment();
    auto content = mEditor->toPlainText();
    QCOMPARE(content, QString("..\n%1abc\n").arg(mIndentStr));

    mEditor->onRemoveComment();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc"));
}

void TestFormatter::testSingleLineCommentCursorPos_data()
{
    QTest::addColumn<int>("pos");
    QTest::addColumn<QString>("text");

    QTest::newRow("begin") << 0 << QString("abc");
    QTest::newRow("one") << 1 << QString("abc");
    QTest::newRow("second") << 2 << QString("abc");
}

void TestFormatter::testSingleLineCommentCursorPos()
{
    QFETCH(int, pos);
    QFETCH(QString, text);

    mEditor->insertPlainText(text);
    auto ps = setCursorPos(pos);
    QCOMPARE(ps, pos);
    mEditor->onAddComment();
    auto content = mEditor->toPlainText();
    QCOMPARE(content, QString("..\n%1abc\n").arg(mIndentStr));

    ps = cursorPos();
    QCOMPARE(ps, pos + QStringLiteral("..\n%1").arg(mIndentStr).length());

    mEditor->onRemoveComment();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc"));
    ps = cursorPos();
    QCOMPARE(ps, pos);
}

void TestFormatter::testSingleLineCommentSelection_data()
{
    QTest::addColumn<int>("start");
    QTest::addColumn<int>("end");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("expected");

    QTest::newRow("begin") << 0 << 1 << QString("abc") << QString("a");
    QTest::newRow("one") << 1 << 2 << QString("abc") << QString("b");
    QTest::newRow("second") << 2 << 3 << QString("abc") << QString("c");
}

void TestFormatter::testSingleLineCommentSelection()
{
    QFETCH(int, start);
    QFETCH(int, end);
    QFETCH(QString, text);
    QFETCH(QString, expected);

    mEditor->insertPlainText(text);
    selectText(start, end);
    auto st = selectedText();
    QCOMPARE(st, expected);

    mEditor->onAddComment();
    auto content = mEditor->toPlainText();
    QCOMPARE(content, QString("..\n%1abc\n").arg(mIndentStr));

    st = selectedText();
    QCOMPARE(st, expected);

    mEditor->onRemoveComment();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc"));
    st = selectedText();
    QCOMPARE(st, expected);
}

void TestFormatter::testSingleLineHeading()
{
    mEditor->insertPlainText("abc");
    mEditor->onPart();
    auto content = mEditor->toPlainText();
    QCOMPARE(content, QString("###\nabc\n###\n"));

    mEditor->onRemoveSection();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc"));

    mEditor->onChapter();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("***\nabc\n***\n"));

    mEditor->onRemoveSection();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc"));

    mEditor->onSection();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc\n===\n"));

    mEditor->onRemoveSection();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc"));

    mEditor->onParagraphs();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString(R"-(abc
"""
)-"));

    mEditor->onRemoveSection();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc"));
}

void TestFormatter::testSingleLineHeadingCursorPos_data()
{
    QTest::addColumn<int>("pos");
    QTest::addColumn<QString>("text");

    QTest::newRow("begin") << 0 << QString("abc");
    QTest::newRow("one") << 1 << QString("abc");
    QTest::newRow("second") << 2 << QString("abc");
}

void TestFormatter::testSingleLineHeadingCursorPos()
{
    QFETCH(int, pos);
    QFETCH(QString, text);

    mEditor->insertPlainText(text);
    auto ps = setCursorPos(pos);
    QCOMPARE(ps, pos);
    mEditor->onPart();
    auto content = mEditor->toPlainText();
    QCOMPARE(content, QString("###\nabc\n###\n"));

    ps = cursorPos();
    QCOMPARE(ps, pos + QStringLiteral("###\n").length());

    mEditor->onRemoveSection();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc"));
    ps = cursorPos();
    QCOMPARE(ps, pos);

    mEditor->onSection();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc\n===\n"));

    ps = cursorPos();
    QCOMPARE(ps, pos);
}

void TestFormatter::testSingleLineHeadingSelection_data()
{
    QTest::addColumn<int>("start");
    QTest::addColumn<int>("end");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("expected");

    QTest::newRow("begin") << 0 << 1 << QString("abc") << QString("a");
    QTest::newRow("one") << 1 << 2 << QString("abc") << QString("b");
    QTest::newRow("second") << 2 << 3 << QString("abc") << QString("c");
}

void TestFormatter::testSingleLineHeadingSelection()
{
    QFETCH(int, start);
    QFETCH(int, end);
    QFETCH(QString, text);
    QFETCH(QString, expected);

    mEditor->insertPlainText(text);
    selectText(start, end);
    auto st = selectedText();
    QCOMPARE(st, expected);

    mEditor->onPart();
    auto content = mEditor->toPlainText();
    QCOMPARE(content, QString("###\nabc\n###\n"));

    st = selectedText();
    QCOMPARE(st, expected);

    mEditor->onRemoveSection();
    content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc"));
    st = selectedText();
    QCOMPARE(st, expected);
}

void TestFormatter::testInsertTextAtBlockStart_data()
{
    QTest::addColumn<int>("start");
    QTest::addColumn<int>("end");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("selected_text");
    QTest::addColumn<QString>("insert");
    QTest::addColumn<QString>("text_after_insert");
    QTest::addColumn<QString>("selected_text_after_insert");

    QTest::newRow("tab_no_text") << 0 << 0 << QString("") << QString("") << QString("\t")
                                 << QString("\t") << QString("");
    QTest::newRow("tab_no_selection_text") << 0 << 0 << QString("abc") << QString("")
                                           << QString("\t") << QString("\tabc") << QString("");
    QTest::newRow("tab_selection_text") << 0 << 1 << QString("abc") << QString("a") << QString("\t")
                                        << QString("\tabc") << QString("a");
    QTest::newRow("tab_selection_text") << 1 << 2 << QString("abc") << QString("b") << QString("\t")
                                        << QString("\tabc") << QString("b");

    QTest::newRow("space_no_text") << 0 << 0 << QString("") << QString("") << QString("    ")
                                   << QString("    ") << QString("");
    QTest::newRow("space_no_selection_text") << 0 << 0 << QString("abc") << QString("")
                                             << QString("    ") << QString("    abc") << QString("");
    QTest::newRow("space_selection_text a") << 0 << 1 << QString("abc") << QString("a")
                                            << QString("    ") << QString("    abc") << QString("a");
    QTest::newRow("space_selection_text b") << 1 << 2 << QString("abc") << QString("b")
                                            << QString("    ") << QString("    abc") << QString("b");

    QTest::newRow("space_no_selection_text_multiline")
        << 0 << 0 << QString("abc\ndef\ngeh") << QString("") << QString("    ")
        << QString("    abc\ndef\ngeh") << QString("");

    QTest::newRow("space_selection_text_multiline")
        << 0 << 9 << QString("abc\ndef\ngeh") << QString("abc\u2029def\u2029g") << QString("    ")
        << QString("    abc\n    def\n    geh") << QString("abc\u2029    def\u2029    g");
}

void TestFormatter::testInsertTextAtBlockStart()
{
    QFETCH(int, start);
    QFETCH(int, end);
    QFETCH(QString, text);
    QFETCH(QString, selected_text);
    QFETCH(QString, insert);
    QFETCH(QString, text_after_insert);
    QFETCH(QString, selected_text_after_insert);

    mEditor->insertPlainText(text);
    selectText(start, end);
    auto st = selectedText();
    QCOMPARE(st, selected_text);

    mEditor->formatter().insertTextAtBlockStart(mEditor, insert);
    auto content = mEditor->toPlainText();
    QCOMPARE(content, text_after_insert);

    st = selectedText();
    QCOMPARE(st, selected_text_after_insert);

    mEditor->formatter().removeTextAtBlockStart(mEditor, insert);
    content = mEditor->toPlainText();
    QCOMPARE(content, text);

    st = selectedText();
    QCOMPARE(st, selected_text);
}

void TestFormatter::testRemoveTextAtBlockStart_data()
{
    QTest::addColumn<int>("start");
    QTest::addColumn<int>("end");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("selected_text");
    QTest::addColumn<QString>("remove");
    QTest::addColumn<QString>("text_after_remove");
    QTest::addColumn<QString>("selected_text_after_remove");

    QTest::newRow("space_selection_text_a") << 4 << 5 << QString("    abc") << QString("a")
                                            << QString("    ") << QString("abc") << QString("a");

    QTest::newRow("remove_not_exisiting")
        << 0 << 0 << QString("") << QString("") << QString("    ") << QString("") << QString("");

    QTest::newRow("remove_spaces") << 0 << 0 << QString("    ") << QString("") << QString("    ")
                                   << QString("") << QString("");

    QTest::newRow("space_no_selection_text") << 0 << 0 << QString("    abc") << QString("")
                                             << QString("    ") << QString("abc") << QString("");
    QTest::newRow("space_selection_text_b") << 5 << 6 << QString("    abc") << QString("b")
                                            << QString("    ") << QString("abc") << QString("b");

    QTest::newRow("space_no_selection_text_multiline_same_indent")
        << 0 << 0 << QString("    abc\n    def\n    geh") << QString("") << QString("    ")
        << QString("abc\n    def\n    geh") << QString("");

    QTest::newRow("space_selection_text_multiline_same_indent")
        << 0 << 23 << QString("    abc\n    def\n    geh")
        << QString("    abc\u2029    def\u2029    geh") << QString("    ")
        << QString("abc\ndef\ngeh") << QString("abc\u2029def\u2029geh");

    QTest::newRow("space_no_selection_text_multiline_different_indent")
        << 0 << 0 << QString("        abc\n    def\n    geh") << QString("") << QString("    ")
        << QString("    abc\n    def\n    geh") << QString("");

    QTest::newRow("space_selection_text_multiline_different_indent")
        << 0 << 27 << QString("        abc\n    def\n    geh")
        << QString("        abc\u2029    def\u2029    geh") << QString("    ")
        << QString("    abc\ndef\ngeh") << QString("    abc\u2029def\u2029geh");

    QTest::newRow("space_no_selection_text_multiline_different_indent_lesser")
        << 0 << 0 << QString("    abc\n  def\n    geh") << QString("") << QString("    ")
        << QString("abc\n  def\n    geh") << QString("");

    QTest::newRow("space_selection_text_multiline_different_indent_lesser")
        << 0 << 21 << QString("    abc\n  def\n    geh")
        << QString("    abc\u2029  def\u2029    geh") << QString("    ")
        << QString("    abc\n  def\n    geh") << QString("    abc\u2029  def\u2029    geh");
}

void TestFormatter::testRemoveTextAtBlockStart()
{
    QFETCH(int, start);
    QFETCH(int, end);
    QFETCH(QString, text);
    QFETCH(QString, selected_text);
    QFETCH(QString, remove);
    QFETCH(QString, text_after_remove);
    QFETCH(QString, selected_text_after_remove);

    mEditor->insertPlainText(text);
    selectText(start, end);
    auto st = selectedText();
    QCOMPARE(st, selected_text);

    mEditor->formatter().removeTextAtBlockStart(mEditor, remove);
    auto content = mEditor->toPlainText();
    QCOMPARE(content, text_after_remove);

    st = selectedText();
    QCOMPARE(st, selected_text_after_remove);
}

void TestFormatter::testInsertLineText_data()
{
    QTest::addColumn<int>("start");
    QTest::addColumn<int>("end");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("selected_text");
    QTest::addColumn<QString>("text_after_insert");
    QTest::addColumn<QString>("selected_text_after_insert");

    QTest::newRow("no_text") << 0 << 0 << QString("") << QString("") << QString("1. ") << QString("");
    QTest::newRow("no_selection_text")
        << 0 << 0 << QString("abc") << QString("") << QString("1. abc") << QString("");
    QTest::newRow("selection_text_a")
        << 0 << 1 << QString("abc") << QString("a") << QString("1. abc") << QString("a");
    QTest::newRow("selection_text_b")
        << 1 << 2 << QString("abc") << QString("b") << QString("1. abc") << QString("b");

    QTest::newRow("no_selection_text_multiline") << 0 << 0 << QString("abc\ndef\ngeh") << QString("")
                                                 << QString("1. abc\ndef\ngeh") << QString("");

    QTest::newRow("space_selection_text_multiline")
        << 0 << 9 << QString("abc\ndef\ngeh") << QString("abc\u2029def\u2029g")
        << QString("1. abc\n2. def\n3. geh") << QString("abc\u20292. def\u20293. g");
}

void TestFormatter::testInsertLineText()
{
    QFETCH(int, start);
    QFETCH(int, end);
    QFETCH(QString, text);
    QFETCH(QString, selected_text);
    QFETCH(QString, text_after_insert);
    QFETCH(QString, selected_text_after_insert);

    mEditor->insertPlainText(text);
    selectText(start, end);
    auto st = selectedText();
    QCOMPARE(st, selected_text);

    mEditor->formatter().insertLineTextAtBlockStart(mEditor, QString("%1. "), 1);
    auto content = mEditor->toPlainText();
    QCOMPARE(content, text_after_insert);

    st = selectedText();
    QCOMPARE(st, selected_text_after_insert);

    mEditor->formatter().removeLineTextAtBlockStart(mEditor, QString("%1. "));
    content = mEditor->toPlainText();
    QCOMPARE(content, text);

    st = selectedText();
    QCOMPARE(st, selected_text);
}

void TestFormatter::testSingleLineIndent_data()
{
    QTest::addColumn<int>("pos");
    QTest::addColumn<QString>("text");

    QTest::newRow("begin") << 0 << QString("abc");
    QTest::newRow("one") << 1 << QString("abc");
    QTest::newRow("second") << 2 << QString("abc");
}

void TestFormatter::testSingleLineIndent()
{
    QFETCH(int, pos);
    QFETCH(QString, text);

    mEditor->insertPlainText(text);
    auto ps = setCursorPos(pos);
    QCOMPARE(ps, pos);
    QTest::keyPress(mEditor, Qt::Key_Tab);
    auto content = mEditor->toPlainText();
    QCOMPARE(content, mIndentStr + text);

    ps = cursorPos();
    QCOMPARE(ps, pos + mIndentStr.length());

    QTest::keyPress(mEditor, Qt::Key_Backtab);
    content = mEditor->toPlainText();
    QCOMPARE(content, text);
    ps = cursorPos();
    QCOMPARE(ps, pos);
    mEditor->clear();
}

void TestFormatter::testSingleLineSelectIndent_data()
{
    QTest::addColumn<int>("pos");
    QTest::addColumn<int>("end");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("selection");

    QTest::newRow("begin") << 0 << 1 << QString("abc") << QString("a");
    QTest::newRow("one") << 1 << 2 << QString("abc") << QString("b");
    QTest::newRow("second") << 2 << 3 << QString("abc") << QString("c");
}

void TestFormatter::testSingleLineSelectIndent()
{
    QFETCH(int, pos);
    QFETCH(int, end);
    QFETCH(QString, text);
    QFETCH(QString, selection);

    mEditor->insertPlainText(text);
    selectText(pos, end);
    auto ps = cursorPos();
    QCOMPARE(ps, end);
    QCOMPARE(selectedText(), selection);
    QTest::keyPress(mEditor, Qt::Key_Tab);
    auto content = mEditor->toPlainText();
    QCOMPARE(content, mIndentStr + text);

    ps = cursorPos();
    QCOMPARE(ps, end + mIndentStr.length());

    QTest::keyPress(mEditor, Qt::Key_Backtab);
    content = mEditor->toPlainText();
    QCOMPARE(content, text);
    ps = cursorPos();
    QCOMPARE(selectedText(), selection);
    QCOMPARE(ps, end);
    mEditor->clear();
}

void TestFormatter::testBlockIndent_data()
{
    QTest::addColumn<int>("pos");
    QTest::addColumn<int>("end");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("selection");
    QTest::addColumn<QString>("expected_tabbed");
    QTest::addColumn<QString>("expected_untabbed");

    QTest::newRow("begin") << 0 << 1 << QString("abc\ndef") << QString("a")
                           << QString("%1abc\ndef").arg(mIndentStr) << QString("abc\ndef");

    //    QTest::newRow("space_no_selection_text_multiline")
    //        << 0 << 0 << QString("abc\ndef\ngeh") << QString("") << QString(" ")
    //        << QString("    abc\ndef\ngeh") << QString("");

    //    QTest::newRow("space_selection_text_multiline")
    //        << 0 << 9 << QString("abc\ndef\ngeh") <<
    //        QString("abc\u2029def\u2029g") << QString("    ")
    //        << QString("    abc\n    def\n    geh") << QString("abc\u2029
    //        def\u2029    g");
}

void TestFormatter::testBlockIndent()
{
    QFETCH(int, pos);
    QFETCH(int, end);
    QFETCH(QString, text);
    QFETCH(QString, selection);
    QFETCH(QString, expected_tabbed);
    QFETCH(QString, expected_untabbed);

    mEditor->insertPlainText(text);
    selectText(pos, end);
    auto ps = cursorPos();
    QCOMPARE(ps, end);
    QCOMPARE(selectedText(), selection);
    QTest::keyPress(mEditor, Qt::Key_Tab);
    auto content = mEditor->toPlainText();
    QCOMPARE(content, expected_tabbed);

    ps = cursorPos();
    QCOMPARE(ps, end + mIndentStr.length());

    QTest::keyPress(mEditor, Qt::Key_Backtab);
    content = mEditor->toPlainText();
    QCOMPARE(content, expected_untabbed);
    ps = cursorPos();
    QCOMPARE(selectedText(), selection);
    QCOMPARE(ps, end);
    mEditor->clear();
}

void TestFormatter::testFormatBoldItalic_data()
{
    QTest::addColumn<QString>("text");

    QTest::newRow("bold start") << QString("**bold**");
    QTest::newRow("italic start") << QString("*italic*");
    QTest::newRow("bold end") << QString("abc **bold**");
    QTest::newRow("italic end") << QString("abc  *italic*");
    QTest::newRow("mixed") << QString("this **bold** *italic*");
    QTest::newRow("mixed inverse") << QString("this *italic* **bold**");
    QTest::newRow("mixed start") << QString("**bold** *italic*");
    QTest::newRow("mixed inverse start") << QString("*italic* **bold**");
}

void TestFormatter::testFormatBoldItalic()
{
    QFETCH(QString, text);

    mEditor->insertPlainText(text);
    qApp->processEvents();
}

void TestFormatter::testFormatDirective_data()
{
    QTest::addColumn<QString>("text");

    QTest::newRow("single directive") << QString(".. index::");
    QTest::newRow("single directive with space") << QString(".. index:: ");
    QTest::newRow("directive with arg") << QString(".. image:: test.png");
    QTest::newRow("todo directive") << QString(".. todo::");
    QTest::newRow("code-block directive") << QString(".. code-block::");
    QTest::newRow("code-block with arg directive") << QString(".. code-block:: bash");
}

void TestFormatter::testFormatDirective()
{
    QFETCH(QString, text);

    mEditor->insertPlainText(text);
    qApp->processEvents();
}

void TestFormatter::testFormatIndent_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QList<int>>("foldings");

    QTest::newRow("code-directive") << QString(R"-(
.. code:: python

    abc

normal text
)-") << (QList<int>() << 0 << 0 << 1 << 1 << 1 << 0 << 0);

    QTest::newRow("commentBlock") << QString(R"-(
..
    this is a comment
  
    multiple lines

normal text
)-") << (QList<int>() << 0 << 0 << 0 << 1 << 1 << 1 << 0 << 0);

    QTest::newRow("inline comment with other stuff")
        << QString(".. comment\nother text\n") << (QList<int>() << 0 << 0 << 0);
    QTest::newRow("directive") << QString(".. index::") << (QList<int>() << 0);
    QTest::newRow("newline directive") << QString("\n\n.. index::") << (QList<int>() << 0 << 0 << 0);
    QTest::newRow("newline directive with content") << QString(R"-(

.. todo::

    content
)-") << ((QList<int>() << 0 << 0 << 0 << 1 << 1 << 1));
    QTest::newRow("inline comment") << QString(".. comment") << (QList<int>() << 0);
}

void TestFormatter::testFormatIndent()
{
    QFETCH(QString, text);
    QFETCH(QList<int>, foldings);

    mEditor->insertPlainText(text);
    qApp->processEvents();

    QCOMPARE(foldings.length(), mEditor->blockCount());
    for (auto line = 0; line < mEditor->blockCount(); line++) {
        auto userData = static_cast<TextEditor::TextBlockUserData *>(
            mEditor->document()->findBlockByNumber(line).userData());
        if (userData) {
            auto folding = userData->foldingIndent();
            QVERIFY2(foldings[line] == folding,
                     qPrintable(QString("expected folding %1 does not match %2 in line %3")
                                    .arg(foldings[line])
                                    .arg(folding)
                                    .arg(line)));
        }
    }
}
