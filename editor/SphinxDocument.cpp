#include "SphinxDocument.h"
#include "../qtcreator-sphinx-pluginconstants.h"

namespace qtc::plugin::sphinx {
EditorDocument::EditorDocument() : TextEditor::TextDocument()
{
    setId(Constants::EditorId);
}

} // namespace qtc::plugin::sphinx
