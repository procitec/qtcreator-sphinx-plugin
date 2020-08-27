#pragma once

#include <texteditor/texteditor.h>

#include <QObject>

namespace qtc::plugin::sphinx {

class Editor : public TextEditor::BaseTextEditor
{
    Q_OBJECT
public:
    Editor();
};
} // namespace qtc::plugin::sphinx
