#pragma once

#include "editor/SphinxCodeModel.h"
#include "editor/SphinxEditorFactory.h"
#include "editor/SphinxFormatActions.h"
#include "editor/SphinxOutputParser.h"
#include "editor/SphinxRest2Html.h"
#include "editor/SphinxRstcheckHighlighter.h"
#include "options/SphinxOptionsPage.h"
#include "options/SphinxSettings.h"
#include "qtcreator-sphinx-plugin_global.h"
#include <extensionsystem/iplugin.h>

namespace qtc::plugin::sphinx {
namespace Internal {

class PluginPrivate
{
public:
    PluginPrivate();
    EditorFactory editorFactory;
    ReSTCheckHighLighter highlighter;
    ReST2Html preview;
    CustomParser parser;
    CodeModel codeModel;
};
} // namespace Internal

class Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Sphinx.json")

public:
    Plugin();
    ~Plugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    QVector<QObject *> createTestObjects() const final;
    void initializeToolsSettings();

    Internal::PluginPrivate *d = nullptr;
    void triggerAction();
    void createActions();

    FormatActions mFormatActions;
    Settings mSettings;
    OptionsPage *mOptionsPage;
};

} // namespace qtc::plugin::sphinx
