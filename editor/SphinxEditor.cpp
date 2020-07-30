#include "SphinxEditor.h"

#include "../qtcreator-sphinx-pluginconstants.h"

namespace qtcreator::plugin::sphinx {
Editor::Editor()
{
    addContext(Constants::LangSphinx);
}
} // namespace qtcreator::plugin::sphinx
