#pragma once
#include <texteditor/textindenter.h>
#include <QStringList>

namespace qtcreator::plugin::sphinx {

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
    }

    void reindent(const QTextCursor &cursor,
                  const TextEditor::TabSettings &tabSettings,
                  int cursorPositionInEditor = -1) override
    {
        Q_UNUSED(cursor)
        Q_UNUSED(tabSettings)
        Q_UNUSED(cursorPositionInEditor)
    }
};

} // namespace qtcreator::plugin::sphinx
