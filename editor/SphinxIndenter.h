#pragma once
#include <texteditor/textindenter.h>
#include <QStringList>

namespace qtc::plugin::sphinx {

class Indenter : public TextEditor::TextIndenter
{
public:
    explicit Indenter(QTextDocument *doc);
    bool isElectricCharacter(const QChar &) const override { return false; }
    void indentBlock(const QTextBlock &block,
                     const QChar &typedChar,
                     const TextEditor::TabSettings &settings,
                     int cursorPositionInEditor = -1) override
    {
        Q_UNUSED(block)
        Q_UNUSED(typedChar)
        Q_UNUSED(settings)
        Q_UNUSED(cursorPositionInEditor)
        //  TextEditor::TextIndenter::indentBlock(block, typedChar, settings, cursorPositionInEditor);
    }

    void indent(const QTextCursor &cursor,
                const QChar &typedChar,
                const TextEditor::TabSettings &tabSettings,
                int cursorPositionInEditor = -1) override
    {
        Q_UNUSED(cursor)
        Q_UNUSED(typedChar)
        Q_UNUSED(tabSettings)
        Q_UNUSED(cursorPositionInEditor)
        //  TextEditor::TextIndenter::indent(cursor, typedChar, tabSettings, cursorPositionInEditor);
    }

    void reindent(const QTextCursor &cursor,
                  const TextEditor::TabSettings &tabSettings,
                  int cursorPositionInEditor = -1) override
    {
        Q_UNUSED(cursor)
        Q_UNUSED(tabSettings)
        Q_UNUSED(cursorPositionInEditor)
        // TextEditor::TextIndenter::reindent(cursor, tabSettings, cursorPositionInEditor);
    }
};

} // namespace qtc::plugin::sphinx
