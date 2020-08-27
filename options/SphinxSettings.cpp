#include "SphinxSettings.h"

#include "../qtcreator-sphinx-pluginconstants.h"

#include <coreplugin/icore.h>

namespace qtc::plugin::sphinx {
Settings::Settings() {}

void Settings::load()
{
    QSettings *s = Core::ICore::settings();

    s->beginGroup(Constants::SettingsGeneralId);
    mUseCustomHighlighter = s->value(SettingsIds::CustomHighlighter, QVariant(true)).toBool();
    mIndentSize = s->value(SettingsIds::IndentSize, QVariant(4)).toInt();
    mUseReSTCheckHighlighter = s->value(SettingsIds::ReSTCheckHighlighter, QVariant(false)).toBool();
    mPythonFilePath = s->value(SettingsIds::PythonFilePath, QVariant()).toString();
    s->endGroup();
}

void Settings::save()
{
    QSettings *s = Core::ICore::settings();

    s->beginGroup(Constants::SettingsGeneralId);
    s->setValue(SettingsIds::CustomHighlighter, mUseCustomHighlighter);
    s->setValue(SettingsIds::IndentSize, mIndentSize);
    s->setValue(SettingsIds::ReSTCheckHighlighter, mUseReSTCheckHighlighter);
    s->setValue(SettingsIds::PythonFilePath, mPythonFilePath);
    s->endGroup();
}
} // namespace qtc::plugin::sphinx
