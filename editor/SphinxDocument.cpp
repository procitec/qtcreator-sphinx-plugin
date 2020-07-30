#include "SphinxDocument.h"
#include "../qtcreator-sphinx-pluginconstants.h"

namespace qtcreator::plugin::sphinx {
EditorDocument::EditorDocument() : TextEditor::TextDocument()
{
    setId(Constants::EditorId);
}

} // namespace qtcreator::plugin::sphinx
