#pragma once

#include <texteditor/texteditor.h>

#include <QObject>

namespace qtcreator::plugin::sphinx {

class Editor : public TextEditor::BaseTextEditor
{
    Q_OBJECT
public:
    Editor();
};
} // namespace qtcreator::plugin::sphinx
