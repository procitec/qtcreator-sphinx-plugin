#include "SphinxEditor.h"

#include "../qtcreator-sphinx-pluginconstants.h"

namespace qtc::plugin::sphinx {
Editor::Editor()
{
    setObjectName("SphinxEditor");
    addContext(Constants::LangSphinx);
}
} // namespace qtc::plugin::sphinx
