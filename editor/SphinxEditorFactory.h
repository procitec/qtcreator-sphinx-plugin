#pragma once
#include <QObject>

#include <texteditor/texteditor.h>

namespace qtcreator::plugin::sphinx {

class EditorFactory : public TextEditor::TextEditorFactory
{
    Q_OBJECT
public:
    EditorFactory();
    static void decorateEditor(TextEditor::TextEditorWidget *editor);
};

} // namespace qtcreator::plugin::sphinx
