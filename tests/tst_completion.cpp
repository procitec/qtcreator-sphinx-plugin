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

// add necessary includes here
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
    mIEditor = Core::EditorManager::openEditorWithContents(
        qtcreator::plugin::sphinx::Constants::EditorId);
    mEditor = static_cast<qtcreator::plugin::sphinx::EditorWidget *>(mIEditor->widget());
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

    QTest::newRow("code-block") << QString(".. code") << QString(R"-(.. code-block::

    code

)-");
}

void TestCompletion::testSimpleRole_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("completion");

    QTest::newRow("inline :ref:") << QString("inline :r") << QString("inline :ref:`label`");
}

void TestCompletion::testSimpleDirective()
{
    QFETCH(QString, text);
    QFETCH(QString, completion);

    mEditor->insertPlainText(text);
    qApp->processEvents();
    QVERIFY(mEditor->autoCompleter());
    auto tc = mEditor->textCursor();
    mEditor->invokeAssist(TextEditor::Completion, nullptr);
    QTest::qWait(20);
    QTest::keyPress(qApp->focusWidget(), Qt::Key_Return);
    QTest::qWait(20);
    auto completed = mEditor->toPlainText();
    QCOMPARE(completed, completion);
}

void TestCompletion::testSimpleRole()
{
    QFETCH(QString, text);
    QFETCH(QString, completion);

    mEditor->insertPlainText(text);
    qApp->processEvents();
    QVERIFY(mEditor->autoCompleter());
    auto tc = mEditor->textCursor();
    mEditor->invokeAssist(TextEditor::Completion, nullptr);
    QTest::qWait(20);
    QTest::keyPress(qApp->focusWidget(), Qt::Key_Return);
    QTest::qWait(20);
    auto completed = mEditor->toPlainText();
    QCOMPARE(completed, completion);
}
