#include "SphinxHighlighter.h"
#include <texteditor/colorscheme.h>
#include <texteditor/fontsettings.h>
#include <texteditor/textdocumentlayout.h>
#include <utils/theme/theme.h>

#include <cassert>
#include <QtCore/QDebug>
#include <QtCore/QRegularExpression>
#include <QtGui/QTextDocument>

namespace qtc::plugin::sphinx {

Highlighter::Highlighter(QObject *parent)
    : TextEditor::SyntaxHighlighter(parent)
    , mDirectiveRegEx("^\\.\\.\\s+[\\w\\-_]+::\\s*")
    , mDirectiveOptionRegEx("\\s+:[A-Za-z-]+:")
    , mTitleRegEx("^[=|\\*|#|\\^|\\-]+")
    , mBoldRegEx("[\\s+]\\*{2,}\\w+\\*{2,}|^\\*{2,}\\w+\\*{2,}")
    , mItalicRegEx("[\\s+]\\*{1,1}\\w+\\*{1,1}|^\\*{1,1}\\w+\\*{1,1}")
    , mCodeRegEx("[\\s+]`{2,2}[^`]+`{2,2}")
    , mRoleRegEx("\\s+:\\w+:")
    , mRoleContentRegEx(":`[^`]+`")
{
    auto colorScheme = fontSettings().colorScheme();

    mDefaultFormat = QTextCharFormat();
    mCommentFormat.setForeground(colorScheme.formatFor(TextEditor::TextStyle::C_COMMENT).foreground());

    mDirectiveFormat.setForeground(colorScheme.formatFor(TextEditor::TextStyle::C_TYPE).foreground());
    mDirectiveOptionFormat.setForeground(
        colorScheme.formatFor(TextEditor::TextStyle::C_LABEL).foreground());

    mTitleFormat.setFontWeight(QFont::Bold);
    mTitleFormat.setForeground(
        colorScheme.formatFor(TextEditor::TextStyle::C_DIFF_CONTEXT_LINE).foreground());
    mTitleFormat.setBackground(
        colorScheme.formatFor(TextEditor::TextStyle::C_DIFF_CONTEXT_LINE).background());

    mInlineFormat.setForeground(
        colorScheme.formatFor(TextEditor::TextStyle::C_VIRTUAL_METHOD).foreground());

    mBoldFormat.setFontWeight(QFont::Bold);
    mItalicFormat.setFontItalic(true);

    mTickFormat.setForeground(
        colorScheme.formatFor(TextEditor::TextStyle::C_PRIMITIVE_TYPE).foreground());

    mCodeFormat = mTickFormat;

    mTodoFormat = mDirectiveFormat;
    mTodoFormat.setBackground(
        colorScheme.formatFor(TextEditor::TextStyle::C_DIFF_SOURCE_LINE).background());
    mTodoFormat.setForeground(
        colorScheme.formatFor(TextEditor::TextStyle::C_DIFF_SOURCE_LINE).foreground());
}

void Highlighter::highlightBlock(const QString &text)
{
    setCurrentBlockState(NONE);
    auto prevBlockState = (previousBlockState() == -1) ? NONE : previousBlockState();

    auto foldingIndent = 0;

    if (currentBlock().previous().isValid()) {
        foldingIndent = TextEditor::TextDocumentLayout::foldingIndent(currentBlock().previous());
    }

    if (prevBlockState & BLOCK_BEGIN) {
        foldingIndent++;
        prevBlockState &= ~BLOCK_BEGIN;
    }

    TextEditor::TextDocumentLayout::userData(currentBlock())->setFoldingStartIncluded(false);

    if (text.startsWith("..") && (0 != text.indexOf(mDirectiveRegEx))) {
        // beginn of a comment
        setFormat(0, text.length(), mCommentFormat);
        setCurrentBlockState(COMMENT);
    } else if ((prevBlockState & COMMENT && (text.isEmpty() || text.startsWith(" ")))) {
        // now depends on previous content
        if (currentBlock().previous().isValid()) {
            auto comment = currentBlock().previous().text();
            bool validComment = false;
            if (0 == comment.indexOf(QRegularExpression("^\\.\\.\\s*$"))) {
                if (text.trimmed().isEmpty()) {
                    setCurrentBlockState(NONE);
                    foldingIndent = 0;
                } else {
                    validComment = true;
                }
            } else if (0 == comment.indexOf(QRegularExpression("^\\.\\.\\s+\\w+"))) {
                validComment = true;
            }
            if (validComment) {
                setCurrentBlockState(COMMENT_CONTENT | BLOCK_BEGIN);
                setFormat(0, text.length(), mCommentFormat);
            }
        }
    } else if (0 == text.indexOf(QRegularExpression(mDirectiveRegEx))) {
        if (text.contains("todo")) {
            setFormat(0, text.length(), mTodoFormat);
        } else {
            setFormat(0, text.length(), mDirectiveFormat);
        }
        foldingIndent = 0;
        setCurrentBlockState(DIRECTIVE | BLOCK_BEGIN);

    } else if ((0 == text.indexOf(mTitleRegEx) && text.count(text.at(0)) == text.length())
               || (currentBlockState() & TITLE)) {
        setCurrentBlockState(TITLE);
        setFormat(0, text.length(), mTitleFormat);
        foldingIndent = 0;

    } else if (!text.isEmpty()
               && ((prevBlockState & DIRECTIVE) || (prevBlockState & DIRECTIVEOPTION)
                   || (prevBlockState & DIRECTIVECONTENT))) {
        QRegularExpression expression(mDirectiveOptionRegEx);
        QRegularExpressionMatchIterator i = expression.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), mDirectiveOptionFormat);
            setCurrentBlockState(DIRECTIVEOPTION);
        }
    } else if ((prevBlockState & COMMENT_CONTENT) && (text.isEmpty() || text.startsWith(" "))) {
        setCurrentBlockState(COMMENT_CONTENT);
        setFormat(0, text.length(), mCommentFormat);

    }

    else if (text.isEmpty()) {
    } else if (!text.isEmpty()) {
        if (0 == text.indexOf(QRegularExpression("^\\w+"))) {
            foldingIndent = 0;
            setCurrentBlockState(NONE);
        }

        // now we do inline matching
        QRegularExpressionMatchIterator i = mBoldRegEx.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), mBoldFormat);
        }

        i = mItalicRegEx.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), mItalicFormat);
        }

        i = mRoleRegEx.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), mInlineFormat);
        }

        i = mCodeRegEx.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), mCodeFormat);
        }

        i = mRoleContentRegEx.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart() + 1, match.capturedLength() - 1, mTickFormat);
        }
    }

    foldingIndent = (0 > foldingIndent) ? 0 : foldingIndent;

    TextEditor::TextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);
}
} // namespace qtc::plugin::sphinx
