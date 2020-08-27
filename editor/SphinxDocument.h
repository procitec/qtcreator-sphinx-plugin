#pragma once

#include <texteditor/textdocument.h>
#include <QObject>
namespace qtc::plugin::sphinx {

class EditorDocument : public TextEditor::TextDocument
{
    Q_OBJECT
public:
    EditorDocument();
};

} // namespace qtc::plugin::sphinx
