#include "SphinxEditor.h"

#include "../qtcreator-sphinx-pluginconstants.h"

namespace qtc::plugin::sphinx {
Editor::Editor()
{
    addContext(Constants::LangSphinx);
}
} // namespace qtc::plugin::sphinx
