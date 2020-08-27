#include "SphinxIndenter.h"

#include <texteditor/tabsettings.h>
#include <QDebug>
#include <QRegularExpression>

namespace qtc::plugin::sphinx {

Indenter::Indenter(QTextDocument *doc) : TextEditor::TextIndenter(doc) {}
} // namespace qtc::plugin::sphinx
