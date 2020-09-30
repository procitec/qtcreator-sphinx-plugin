#include "SphinxCodeStylePreferencesFactory.h"
#include "../qtcreator-sphinx-pluginconstants.h"
#include "SphinxIndenter.h"

#include <texteditor/simplecodestylepreferences.h>
#include <texteditor/snippets/snippetprovider.h>
#include <utils/id.h>

#include <extensionsystem/pluginmanager.h>

#include <QLabel>

namespace qtc::plugin::sphinx {

Utils::Id CodeStylePreferencesFactory::languageId()
{
    return Constants::SettingsGeneralId;
}

QString CodeStylePreferencesFactory::displayName()
{
    return QStringLiteral("Sphinx");
}

TextEditor::ICodeStylePreferences *CodeStylePreferencesFactory::createCodeStyle() const
{
    return new TextEditor::SimpleCodeStylePreferences();
}

QWidget *CodeStylePreferencesFactory::createEditor(TextEditor::ICodeStylePreferences *,
                                                   QWidget *parent) const
{
    return new QLabel(tr("Not yet implemented"), parent);
}

TextEditor::Indenter *CodeStylePreferencesFactory::createIndenter(QTextDocument *doc) const
{
    return new Indenter(doc);
}

QString CodeStylePreferencesFactory::snippetProviderGroupId() const
{
    return Constants::SnippetGroupId;
}

QString CodeStylePreferencesFactory::previewText() const
{
    return QStringLiteral(".. todo::\n"
                          "    this is a directive\n"
                          "\n"
                          ".. image:: image.png\n"
                          "    :width: 100\n"
                          "\n"
                          "* this is a list"
                          "\n"
                          "    * sublist item\n"
                          "    * sublist item\n"
                          "\n"
                          "* list continued\n"
                          "\n");
}

} // namespace qtc::plugin::sphinx
