#include "tst_completion.h"

#include "../editor/SphinxHighlighter.h"
#include "../qtcreator-sphinx-pluginconstants.h"

#include "coreplugin/coreplugin.h"
#include "texteditor/codeassist/completionassistprovider.h"
#include "texteditor/syntaxhighlighter.h"
#include <texteditor/textdocumentlayout.h>

#include <QtCore/QCoreApplication>
#include <QtGui/QTextDocumentFragment>
#include <QtTest/QtTest>

constexpr int COMPLETION_TIMEOUT = 150;
constexpr int KEYPRESS_TIMEOUT = 20;

TestCompletion::TestCompletion() {}

TestCompletion::~TestCompletion() {}

void TestCompletion::initTestCase() {}

void TestCompletion::cleanupTestCase()
{
    cleanupEditor();
}

void TestCompletion::init()
{
    createEditor();
}

void TestCompletion::cleanup()
{
    cleanupEditor();
}

void TestCompletion::createEditor()
{
    mIEditor = Core::EditorManager::openEditorWithContents(qtc::plugin::sphinx::Constants::EditorId);
    mEditor = static_cast<qtc::plugin::sphinx::EditorWidget *>(mIEditor->widget());
    QVERIFY(TextEditor::TabSettings::SpacesOnlyTabPolicy
            == mEditor->textDocument()->tabSettings().m_tabPolicy);

    mEditor->show();
    qApp->processEvents();
}

void TestCompletion::cleanupEditor()
{
    mEditor->close();
}

void TestCompletion::testSimpleDirective_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("completion");

    QTest::newRow("code-block") << QString(".. code-") << QString(R"-(.. code-block:: language

    

)-");

    QTest::newRow("code-block second line")
        << QString("normaltext\n\n.. code-") << QString(R"-(normaltext

.. code-block:: language

    

)-");
}

// TODO Add test for replacement directives like
// .. |date| date::
// .. |rest| replace:: long text
// .. _Python: http://.....

void TestCompletion::testSimpleDirective()
{
    QFETCH(QString, text);
    QFETCH(QString, completion);

    mEditor->insertPlainText(text);
    qApp->processEvents();
    QVERIFY(mEditor->autoCompleter());
    auto tc = mEditor->textCursor();
    mEditor->invokeAssist(TextEditor::Completion, nullptr);
    QTest::qWait(COMPLETION_TIMEOUT);
    QTest::keyPress(qApp->focusWidget(), Qt::Key_Return);
    QTest::qWait(KEYPRESS_TIMEOUT);
    auto completed = mEditor->toPlainText();
    QCOMPARE(completed, completion);
}

void TestCompletion::testSimpleDirectiveOption_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("completion");

    QTest::newRow("image option height") << QString(R"-(.. image:: filename.png
   :h)-") << QString(R"-(.. image:: filename.png
   :height: length
)-");

    QTest::newRow("image option second options") << QString(R"-(.. image:: filename.png
   :height: 100
   :w)-") << QString(R"-(.. image:: filename.png
   :height: 100
   :width: length or percentage of the current line width
)-");
}

void TestCompletion::testSimpleDirectiveOption()
{
    QFETCH(QString, text);
    QFETCH(QString, completion);

    mEditor->insertPlainText(text);
    qApp->processEvents();
    QVERIFY(mEditor->autoCompleter());
    auto tc = mEditor->textCursor();
    mEditor->invokeAssist(TextEditor::Completion, nullptr);
    QTest::qWait(COMPLETION_TIMEOUT);
    QTest::keyPress(qApp->focusWidget(), Qt::Key_Return);
    QTest::qWait(KEYPRESS_TIMEOUT);
    auto completed = mEditor->toPlainText();
    QCOMPARE(completed, completion);
}

void TestCompletion::testSimpleRole_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("completion");
    QTest::addColumn<bool>("shouldComplete");

    QTest::newRow("inline :ref:") << QString("inline :r") << QString("inline :ref:``") << true;
    QTest::newRow("start of line :ref:") << QString("\n:r") << QString("\n:ref:``") << true;
    QTest::newRow("invalid:ref:") << QString("inline:r") << QString("inline:r") << false;
}

void TestCompletion::testSimpleRole()
{
    QFETCH(QString, text);
    QFETCH(QString, completion);
    QFETCH(bool, shouldComplete);

    mEditor->insertPlainText(text);
    qApp->processEvents();
    QVERIFY(mEditor->autoCompleter());
    auto tc = mEditor->textCursor();
    mEditor->invokeAssist(TextEditor::Completion, nullptr);
    QTest::qWait(COMPLETION_TIMEOUT);
    if (shouldComplete) {
        QTest::keyPress(qApp->focusWidget(), Qt::Key_Return);
        QTest::qWait(KEYPRESS_TIMEOUT);
    }
    auto completed = mEditor->toPlainText();
    QCOMPARE(completed, completion);
}

void TestCompletion::testSimpleSnippetDirective_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("completion");

    QTest::newRow("code-block") << QString("__code") << QString(R"-(.. code-block::

    code

)-");
}

void TestCompletion::testSimpleSnippetRole_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("completion");

    QTest::newRow("inline :ref:") << QString("inline _r") << QString("inline :ref:`label`");
}

void TestCompletion::testSimpleSnippetDirective()
{
    QFETCH(QString, text);
    QFETCH(QString, completion);

    mEditor->insertPlainText(text);
    qApp->processEvents();
    QVERIFY(mEditor->autoCompleter());
    auto tc = mEditor->textCursor();
    mEditor->invokeAssist(TextEditor::Completion, nullptr);
    QTest::qWait(COMPLETION_TIMEOUT);
    QTest::keyPress(qApp->focusWidget(), Qt::Key_Return);
    QTest::qWait(KEYPRESS_TIMEOUT);
    auto completed = mEditor->toPlainText();
    QCOMPARE(completed, completion);
}

void TestCompletion::testSimpleSnippetRole()
{
    QFETCH(QString, text);
    QFETCH(QString, completion);

    mEditor->insertPlainText(text);
    qApp->processEvents();
    QVERIFY(mEditor->autoCompleter());
    auto tc = mEditor->textCursor();
    mEditor->invokeAssist(TextEditor::Completion, nullptr);
    QTest::qWait(COMPLETION_TIMEOUT);
    QTest::keyPress(qApp->focusWidget(), Qt::Key_Return);
    QTest::qWait(KEYPRESS_TIMEOUT);
    auto completed = mEditor->toPlainText();
    QCOMPARE(completed, completion);
}
