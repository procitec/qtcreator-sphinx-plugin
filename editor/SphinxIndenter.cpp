#include "SphinxIndenter.h"

#include <texteditor/tabsettings.h>
#include <QDebug>
#include <QRegularExpression>

namespace qtcreator::plugin::sphinx {

Indenter::Indenter(QTextDocument *doc) : TextEditor::TextIndenter(doc) {}
} // namespace qtcreator::plugin::sphinx
