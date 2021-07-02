#include "tst_rightpane.h"

#include "../editor/SphinxHighlighter.h"
#include "../qtcreator-sphinx-pluginconstants.h"

#include "coreplugin/coreplugin.h"
#include "coreplugin/rightpane.h"
#include "texteditor/syntaxhighlighter.h"
#include <texteditor/textdocumentlayout.h>

#include <QtCore/QCoreApplication>
#include <QtGui/QTextDocumentFragment>
#include <QtTest/QtTest>

namespace qtc::plugin::sphinx {
// add necessary includes here
TestRightPane::TestRightPane() {}

TestRightPane::~TestRightPane() {}

void TestRightPane::initTestCase() {}

void TestRightPane::cleanupTestCase()
{
    cleanupEditor();
}

void TestRightPane::init()
{
    createEditor();
}

void TestRightPane::cleanup()
{
    cleanupEditor();
}

void TestRightPane::createEditor()
{
    mIEditor = Core::EditorManager::openEditorWithContents(qtc::plugin::sphinx::Constants::EditorId);
    mEditor = static_cast<qtc::plugin::sphinx::EditorWidget *>(mIEditor->widget());
    QVERIFY(TextEditor::TabSettings::SpacesOnlyTabPolicy
            == mEditor->textDocument()->tabSettings().m_tabPolicy);
    mEditor->show();
    qApp->processEvents();
}

void TestRightPane::cleanupEditor()
{
    mEditor->clear();
    mEditor->close();
}

void TestRightPane::testOpenRightPane()
{
    mEditor->insertPlainText("abc");
    auto content = mEditor->toPlainText();
    QCOMPARE(content, QString("abc"));
    mEditor->onShowRightPane(true);
    QTest::qWait(500);
    QVERIFY(Core::RightPaneWidget::instance()->widget());
    QVERIFY(Core::RightPaneWidget::instance()->widget()->isVisible());
    mEditor->onShowRightPane(false);
}
} // namespace qtc::plugin::sphinx
