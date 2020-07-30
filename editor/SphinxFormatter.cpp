#include "SphinxFormatter.h"
#include "SphinxEditorWidget.h"

#include <texteditor/texteditor.h>
#include <QtCore/QSettings>
#include <QtGui/QTextBlock>

#include <cassert>

namespace qtcreator::plugin::sphinx {
Formatter::Formatter() {}

//void Formatter::onActiveEditor( TextEditIfc* editor )
//{
//  if ( editor )
//  {
//    disconnect( static_cast<QWidget*>( editor ), 0, this, 0 );
//  }
//  editor = editor;

//  if ( editor )
//  {
//    connect( static_cast<QWidget*>( editor ), SIGNAL( insertTextAtBlockStart( const QString& ) ), this,
//             SLOT( onInsertTextAtBlockStart( const QString& ) ) );
//    connect( static_cast<QWidget*>( editor ), SIGNAL( removeTextAtBlockStart( const QString& ) ), this,
//             SLOT( onRemoveTextAtBlockStart( const QString& ) ) );
//  }
//}

void Formatter::insertAt(EditorWidget *editor,
                         const QString &text,
                         QTextCursor::SelectionType selectionType)
{
    assert(!text.isEmpty());
    assert(editor);

    if (text.isEmpty() || !editor) {
        return;
    }

    auto tc = editor->textCursor();

    tc.beginEditBlock();
    auto pos = tc.position();

    switch (selectionType) {
    case QTextCursor::Document:
        tc.movePosition(QTextCursor::Start);
        break;
    case QTextCursor::LineUnderCursor:
        tc.movePosition(QTextCursor::StartOfLine);
        break;

    case QTextCursor::BlockUnderCursor:
        tc.movePosition(QTextCursor::StartOfBlock);
        break;
    case QTextCursor::WordUnderCursor:
        tc.movePosition(QTextCursor::StartOfWord);
        break;
    }

    tc.insertText(text);
    tc.setPosition(pos + text.length());

    tc.endEditBlock();
}

void Formatter::removeAt(EditorWidget *editor,
                         const QString &text,
                         QTextCursor::SelectionType selectionType)
{
    assert(!text.isEmpty());
    assert(editor);

    if (text.isEmpty() || !editor) {
        return;
    }

    auto tc = editor->textCursor();

    tc.beginEditBlock();
    auto pos = tc.position();

    if (pos >= text.length()) {
        switch (selectionType) {
        case QTextCursor::Document:
            tc.movePosition(QTextCursor::Start);
            break;
        case QTextCursor::LineUnderCursor:
            tc.movePosition(QTextCursor::StartOfLine);
            break;

        case QTextCursor::BlockUnderCursor:
            tc.movePosition(QTextCursor::StartOfBlock);
            break;
        case QTextCursor::WordUnderCursor:
            tc.movePosition(QTextCursor::StartOfWord);
            break;
        }

        auto selStart = tc.hasSelection() ? tc.selectionStart() : -1;
        auto selEnd = tc.hasSelection() ? tc.selectionEnd() : -1;
        tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, text.length());
        tc.removeSelectedText();
        tc.setPosition((pos - text.length()));
        if (-1 < selStart) {
            tc.setPosition(selStart - text.length());
            tc.movePosition(QTextCursor::Right,
                            QTextCursor::KeepAnchor,
                            selEnd - text.length() - selStart + 1);
        }
    }

    tc.endEditBlock();
}

int Formatter::blockNumberOfPos(QTextCursor &tc, int pos) const
{
    auto curr = tc.position();
    tc.setPosition(pos);

    auto block = tc.blockNumber();
    tc.setPosition(curr);
    return block;
}

void Formatter::insertBeforeBlock(EditorWidget *editor,
                                  const QString &text,
                                  bool indentBlock,
                                  bool wrapBlockWithSpacing)
{
    assert(!text.isEmpty());
    assert(editor);

    if (text.isEmpty() || !editor) {
        return;
    }

    auto tc = editor->textCursor();

    tc.beginEditBlock();

    auto curInfo = currentCursorAndSel(tc);

    auto selOffset = 0;

    tc.setPosition(curInfo.start());
    tc.movePosition(QTextCursor::StartOfBlock);
    if (0 < tc.blockNumber() && wrapBlockWithSpacing) {
        tc.insertBlock();
        selOffset++;
    }
    tc.insertText(text);
    selOffset += text.length();
    tc.insertBlock();
    selOffset++;

    if (indentBlock) {
        auto indent = QString().fill(' ', editor->indentSize());
        selOffset = insertTextAtBlockStart(curInfo.start(), curInfo.end(), tc, indent, selOffset);
    }

    if (wrapBlockWithSpacing) {
        tc.insertBlock();
    }
    editor->setTextCursor(tc);

    restorCursorAndSel(tc, curInfo, selOffset);

    tc.endEditBlock();
    editor->setTextCursor(tc);
}

Formatter::CursorInfo Formatter::currentCursorAndSel(const QTextCursor &tc) const
{
    auto start = tc.hasSelection() ? tc.selectionStart() : tc.position();
    auto end = tc.hasSelection() ? tc.selectionEnd() : tc.position();
    bool hasSelection = tc.hasSelection();
    return CursorInfo(start, end, hasSelection);
}

void Formatter::restorCursorAndSel(QTextCursor &tc, const CursorInfo &inf, int offset)
{
    if (inf.hasSelection()) {
        tc.setPosition(inf.start() + offset);
        tc.setPosition(inf.end() + offset, QTextCursor::KeepAnchor);
    } else {
        tc.setPosition(inf.start() + offset);
    }
}

void Formatter::removeBeforeBlock(EditorWidget *editor,
                                  const QString &text,
                                  bool unindentBlock,
                                  bool unwrapBlockSpacing)
{
    assert(!text.isEmpty());
    assert(editor);

    if (text.isEmpty() || !editor) {
        return;
    }

    auto tc = editor->textCursor();

    tc.beginEditBlock();

    auto curInf = currentCursorAndSel(tc);

    auto selOffset = text.length();

    tc.setPosition(curInf.start());
    tc.movePosition(QTextCursor::StartOfBlock);

    // find the first occurence of text in this or previous blocks until an complete empty line occurs
    bool doBreak = false;
    bool prepareToBreak = false;
    while (!doBreak) {
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        if (tc.selectedText() == text) {
            doBreak = true;
        } else if (tc.selectedText().length() == 0) {
            prepareToBreak ? doBreak = true : prepareToBreak = true;
            tc.movePosition(QTextCursor::PreviousBlock);
        } else {
            prepareToBreak = tc.selectedText().isEmpty() ? prepareToBreak : false;
            if (prepareToBreak) {
                doBreak = true;
            } else {
                doBreak = !tc.movePosition(QTextCursor::PreviousBlock);
            }
        }
    }

    if (tc.hasSelection()) {
        selOffset -= removeBlock(tc);
        if (unwrapBlockSpacing && isBlockEmpty(tc)) {
            selOffset -= removeBlock(tc);
        }
    }

    if (unindentBlock) {
        auto indent = QString().fill(' ', editor->indentSize());
        doBreak = false;
        bool prepareToBreak = false;
        while (!doBreak) {
            tc.select(QTextCursor::BlockUnderCursor);

            if (tc.selectedText().startsWith(QChar(0x2029) + indent)
                || tc.selectedText().startsWith(indent)) {
                tc.movePosition(QTextCursor::StartOfBlock);
                tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, editor->indentSize());
                selOffset -= tc.selectedText().length();
                tc.removeSelectedText();
                tc.movePosition(QTextCursor::NextBlock);
                prepareToBreak = false;
            } else if (tc.selectedText().isEmpty()) // empty line
            {
                prepareToBreak ? doBreak = true : prepareToBreak = true;
                tc.movePosition(QTextCursor::NextBlock);
            } else {
                if (prepareToBreak) {
                    doBreak = true;
                } else {
                    doBreak = !tc.movePosition(QTextCursor::NextBlock);
                }
            }
        }
    }

    if (unwrapBlockSpacing) {
        tc.movePosition(QTextCursor::PreviousBlock);
        if (!isBlockEmpty(tc)) {
            tc.movePosition(QTextCursor::NextBlock);
        }

        if (isBlockEmpty(tc)) {
            selOffset -= removeBlock(tc);
        }
    }

    editor->setTextCursor(tc);

    restorCursorAndSel(tc, curInf, selOffset);

    tc.endEditBlock();
    editor->setTextCursor(tc);
}

int Formatter::removeBlock(QTextCursor &tc)
{
    int offset = 0;
    tc.select(QTextCursor::BlockUnderCursor);
    offset += tc.selectedText().length();
    tc.removeSelectedText();
    tc.movePosition(QTextCursor::StartOfBlock);
    if (tc.atEnd()) {
        if (!tc.atStart()) {
            tc.deletePreviousChar();
            offset += 2;
        }
    } else {
        tc.deleteChar();
        offset += 1;
        if (tc.atEnd()) // due to end of document tag??
        {
            offset += 1;
        }
    }
    return offset;
}

bool Formatter::isBlockEmpty(QTextCursor &tc)
{
    tc.select(QTextCursor::BlockUnderCursor);
    return (tc.selectedText().isEmpty());
}

void Formatter::insertAroundCursor(EditorWidget *editor,
                                   const QString &text,
                                   QTextCursor::SelectionType selecionType)
{
    assert(!text.isEmpty());
    assert(editor);

    if (text.isEmpty() || !editor) {
        return;
    }

    bool removeSelection = false;

    auto tc = editor->textCursor();
    tc.beginEditBlock();

    if (!tc.hasSelection() && selecionType != QTextCursor::Document) {
        tc.select(selecionType);
        removeSelection = true;
    }
    if (tc.hasSelection()) {
        auto pos = tc.position();
        auto startPos = tc.selectionStart();
        auto endPos = tc.selectionEnd();
        assert(pos >= startPos);
        assert(pos <= endPos);
        tc.setPosition(endPos);
        tc.insertText(text);
        tc.setPosition(startPos);
        tc.insertText(text);
        tc.setPosition(pos + text.length());
    } else {
        tc.insertText(text);
        tc.insertText(text);
        tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, text.length());
    }

    if (tc.hasSelection() && removeSelection) {
        tc.clearSelection();
    }

    tc.endEditBlock();

    editor->setTextCursor(tc);
}

void Formatter::insertHeading(EditorWidget *editor, const QChar &text, bool overLine)
{
    assert(!text.isNull());
    assert(editor);

    if (text.isNull() || !editor) {
        return;
    }

    auto tc = editor->textCursor();

    tc.beginEditBlock();

    auto curInfo = currentCursorAndSel(tc);

    auto selOffset = 0;

    tc.setPosition(curInfo.start());
    if (overLine) {
        auto blockLen = tc.block().text().length();
        tc.movePosition(QTextCursor::StartOfBlock);
        if (0 < tc.blockNumber()) {
            tc.insertBlock();
            selOffset++;
        }
        tc.insertText(QString().fill(text, blockLen));
        selOffset += blockLen;
        tc.insertBlock();
        selOffset++;
    }

    tc.setPosition(curInfo.end() + selOffset);
    auto blockLen = tc.block().text().length();
    tc.movePosition(QTextCursor::EndOfBlock);
    tc.insertBlock();
    tc.insertText(QString().fill(text, blockLen));
    tc.insertBlock();

    restorCursorAndSel(tc, curInfo, selOffset);

    tc.endEditBlock();

    editor->setTextCursor(tc);
}

void Formatter::removeHeading(EditorWidget *editor)
{
    assert(editor);

    if (!editor) {
        return;
    }

    auto tc = editor->textCursor();

    tc.beginEditBlock();

    auto curInfo = currentCursorAndSel(tc);

    auto selOffset = 0;

    tc.setPosition(curInfo.start());
    tc.movePosition(QTextCursor::EndOfBlock);
    if (tc.movePosition(QTextCursor::NextBlock)) {
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        auto text = tc.selectedText();

        if (!text.isEmpty()) {
            auto pattern = text.at(0);
            if (text.length() == text.count(pattern)) {
                tc.removeSelectedText();
                tc.deleteChar();
                tc.deletePreviousChar();
            }
        }
        editor->setTextCursor(tc);
        tc = editor->textCursor();
    }

    tc.setPosition(curInfo.start());
    tc.movePosition(QTextCursor::StartOfBlock);
    if (tc.movePosition(QTextCursor::PreviousBlock)) {
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        auto text = tc.selectedText();
        if (!text.isEmpty()) {
            auto pattern = text.at(0);
            if (text.length() == text.count(pattern)) {
                tc.removeSelectedText();
                tc.deleteChar();
                selOffset -= text.length();
                selOffset -= 1;
            }
        }
        editor->setTextCursor(tc);
        tc = editor->textCursor();
    }

    restorCursorAndSel(tc, curInfo, selOffset);
    editor->setTextCursor(tc);
    tc = editor->textCursor();

    tc.endEditBlock();
}

int Formatter::insertTextAtBlockStart(
    int start, int end, QTextCursor &tc, const QString &text, int selectionOffset)
{
    auto blockStart = blockNumberOfPos(tc, start + selectionOffset);
    auto blockEnd = blockNumberOfPos(tc, end + selectionOffset);

    for (auto block = blockStart; block <= blockEnd; block++) {
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.insertText(text);
        selectionOffset += text.length();
        auto nextBlock = tc.movePosition(QTextCursor::NextBlock);
        if (!nextBlock) {
            tc.movePosition(QTextCursor::End);
        }
    }

    return selectionOffset;
}

int Formatter::removeTextAtBlockStart(
    int start, int end, QTextCursor &tc, const QString &text, int selectionOffset)
{
    auto blockStart = blockNumberOfPos(tc, start + selectionOffset);
    auto blockEnd = blockNumberOfPos(tc, end + selectionOffset);

    for (auto block = blockStart; block <= blockEnd; block++) {
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, text.length());
        tc.removeSelectedText();
        selectionOffset -= text.length();
        auto nextBlock = tc.movePosition(QTextCursor::NextBlock);
        if (!nextBlock) {
            tc.movePosition(QTextCursor::End);
        }
    }

    return selectionOffset;
}

void Formatter::removeAroundCursor(EditorWidget *editor,
                                   const QString &text,
                                   QTextCursor::SelectionType selectionType)
{
    assert(!text.isEmpty());
    assert(editor);
    Q_UNUSED(selectionType)

    if (text.isEmpty() || !editor) {
        return;
    }
}

//void Formatter::onBold()
//{
//  insertAroundCursor( "**", QTextCursor::WordUnderCursor );
//}

//void Formatter::onItalic()
//{
//  insertAroundCursor( "*", QTextCursor::WordUnderCursor );
//}

//void Formatter::onCode()
//{
//  insertAroundCursor( "``", QTextCursor::WordUnderCursor );
//}

//void Formatter::onDoubleParaSpacing()
//{
//    assert(editor);
//    editor->doubleParagraphSpacing();
//}

//void Formatter::onNormalParaSpacing()
//{
//    assert(editor);
//    editor->normalParagraphSpacing();
//}

//void Formatter::onIncreaseIndent()
//{
//    assert(editor);
//    editor->increaseIndent();
//    insertAt(QString().fill(' ', editor->indent()), QTextCursor::BlockUnderCursor);
//}

//void Formatter::onDecreaseIndent()
//{
//    assert(editor);
//    auto indent = editor->indent();
//    if (0 < indent) {
//        removeAt(QString().fill(' ', editor->indent()), QTextCursor::BlockUnderCursor);
//        editor->decreaseIndent();
//    }
//}

//void Formatter::onBulletedList()
//{
//  // insertBeforeCursor( "* ", QTextCursor::LineUnderCursor );
//}

//void Formatter::onAutoNumberedList()
//{
//  // insertBeforeCursor( "#. ", QTextCursor::LineUnderCursor );
//}

//void Formatter::onNumberedList() {}

//void Formatter::onBlockQuote()
//{
//  // insertBeforeCursor( "| ", QTextCursor::LineUnderCursor );
//}

//void Formatter::onAddComment()
//{
//  insertBeforeBlock( "..", true, true );
//}

//void Formatter::onRemoveComment()
//{
//  removeBeforeBlock( "..", true, true );
//}

//void Formatter::onRemoveSection()
//{
//  removeHeading();
//}

//void Formatter::onPart()
//{
//  insertHeading( mParts.at( 0 ), mPartsOverline );
//}
//void Formatter::onChapter()
//{
//  insertHeading( mChapters.at( 0 ), mPartsOverline );
//}

//void Formatter::onSection()
//{
//  insertHeading( mSections.at( 0 ), mSectionsOverline );
//}

//void Formatter::onSubSection()
//{
//  insertHeading( mSubSections.at( 0 ), mSubSectionsOverline );
//}
//void Formatter::onSubSubSection()
//{
//  insertHeading( mSubSubSections.at( 0 ), mSubSubSectionsOverline );
//}
//void Formatter::onParagraphs()
//{
//  insertHeading( mParagraphs.at( 0 ), mParagraphsOverline );
//}

void Formatter::removeTextAtBlockStart(EditorWidget *editor, const QString &text)
{
    assert(editor);

    if (!editor) {
        return;
    }

    auto tc = editor->textCursor();

    tc.beginEditBlock();

    auto curInfo = currentCursorAndSel(tc);

    auto selOffset = 0;

    tc.setPosition(curInfo.start());

    selOffset = removeTextAtBlockStart(curInfo.start(), curInfo.end(), tc, text, selOffset);

    restorCursorAndSel(tc, curInfo, selOffset);

    tc.endEditBlock();
}

void Formatter::insertTextAtBlockStart(EditorWidget *editor, const QString &text)
{
    assert(editor);

    if (!editor) {
        return;
    }

    auto tc = editor->textCursor();

    tc.beginEditBlock();

    auto curInfo = currentCursorAndSel(tc);

    auto selOffset = 0;

    tc.setPosition(curInfo.start());

    selOffset = insertTextAtBlockStart(curInfo.start(), curInfo.end(), tc, text, selOffset);

    restorCursorAndSel(tc, curInfo, selOffset);

    tc.endEditBlock();
}
} // namespace qtcreator::plugin::sphinx
