#pragma once

#include <texteditor/textdocument.h>
#include <QObject>
namespace qtcreator::plugin::sphinx {

class EditorDocument : public TextEditor::TextDocument
{
    Q_OBJECT
public:
    EditorDocument();
};

} // namespace qtcreator::plugin::sphinx
