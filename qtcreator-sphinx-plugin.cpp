#include "qtcreator-sphinx-plugin.h"
#include "qtcreator-sphinx-pluginconstants.h"

#include "editor/SphinxCodeStylePreferencesFactory.h"
#include "editor/SphinxEditor.h"
#include "editor/SphinxEditorFactory.h"
#include "editor/SphinxEditorWidget.h"

#include "tests/tst_completion.h"
#include "tests/tst_formatter.h"

#include <texteditor/codestylepool.h>
#include <texteditor/simplecodestylepreferences.h>
#include <texteditor/snippets/snippetprovider.h>
#include <texteditor/texteditorsettings.h>

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>

#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QtCore/QTranslator>
#include <QtTest/QTest>

#ifdef WITH_TESTS
QTEST_APPLESS_MAIN(qtc::plugin::sphinx::Plugin)
#endif

namespace qtc::plugin::sphinx {
namespace Internal {
PluginPrivate::PluginPrivate() {}

} // namespace Internal
Plugin::Plugin()
{
    // Create your members
}

Plugin::~Plugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
    delete d;
    d = nullptr;
}

bool Plugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    initializeToolsSettings();

    d = new Internal::PluginPrivate();
    createActions();

    TextEditor::SnippetProvider::registerGroup(Constants::SnippetGroupId,
                                               tr("Sphinx", "SnippetProvider"),
                                               &EditorFactory::decorateEditor);

    const QString locale = Core::ICore::userInterfaceLanguage();
    if (!locale.isEmpty()) {
        auto qtr = new QTranslator(this);
        const QString creatorTrPath = Core::ICore::resourcePath("translations").toString();
        const QString trFile = QLatin1String(Constants::SnippetGroupId) + "_" + locale;
        if (qtr->load(trFile, creatorTrPath)) {
            QCoreApplication::installTranslator(qtr);
        }
    }
    return true;
}

void Plugin::initializeToolsSettings()
{
    mSettings.load();
    // code style factory
    auto factory = new CodeStylePreferencesFactory;
    TextEditor::TextEditorSettings::registerCodeStyleFactory(factory);

    // code style pool
    auto pool = new TextEditor::CodeStylePool(factory, this);
    TextEditor::TextEditorSettings::registerCodeStylePool(Constants::SettingsGeneralId, pool);

    // global code style settings
    auto globalCodeStyle = new TextEditor::SimpleCodeStylePreferences(this);
    globalCodeStyle->setDelegatingPool(pool);
    globalCodeStyle->setDisplayName(tr("Global", "Settings"));
    globalCodeStyle->setId("SphinxGlobal");
    pool->addCodeStyle(globalCodeStyle);
    TextEditor::TextEditorSettings::registerCodeStyle(Constants::SettingsGeneralId, globalCodeStyle);

    // built-in settings
    // Sphinx style
    auto sphinxyCodeStyle = new TextEditor::SimpleCodeStylePreferences;
    sphinxyCodeStyle->setId("sphinx");
    sphinxyCodeStyle->setDisplayName(tr("Sphinx"));
    sphinxyCodeStyle->setReadOnly(true);
    TextEditor::TabSettings tabSettings;
    tabSettings.m_tabPolicy = TextEditor::TabSettings::SpacesOnlyTabPolicy;
    tabSettings.m_tabSize = mSettings.indentSize();
    tabSettings.m_indentSize = mSettings.indentSize();
    tabSettings.m_continuationAlignBehavior = TextEditor::TabSettings::ContinuationAlignWithIndent;
    sphinxyCodeStyle->setTabSettings(tabSettings);
    pool->addCodeStyle(sphinxyCodeStyle);

    // default delegate for global preferences
    globalCodeStyle->setCurrentDelegate(sphinxyCodeStyle);

    pool->loadCustomCodeStyles();

    // load global settings (after built-in settings are added to the pool)
    globalCodeStyle->fromSettings(Constants::SettingsGeneralId, Core::ICore::settings());

    // mimetypes to be handled
    TextEditor::TextEditorSettings::registerMimeTypeForLanguageId(Constants::MimeType,
                                                                  Constants::SettingsGeneralId);

    mOptionsPage = new OptionsPage(&mSettings, this);
}

void Plugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag Plugin::aboutToShutdown()
{
    mSettings.save();
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

QVector<QObject *> Plugin::createTestObjects() const
{
#ifdef WITH_TESTS
    QVector<QObject *> tests;
    tests << new TestFormatter << new TestCompletion;
    return tests;
#else
    return {};
#endif
}
void Plugin::createActions()
{
    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("Sphinx Editor"));
    // auto action = new QAction(tr("Sphinx Editor Action"), this);

    Core::Command *cmd = nullptr;
    QAction *action = nullptr;

    const auto &formatActions = mFormatActions.formatActions();

    for (int idx = 0; idx < FormatActions::FORMAT::LAST_FORMAT; idx++) {
        action = formatActions[idx];
        cmd = Core::ActionManager::registerAction(action,
                                                  qPrintable(action->objectName()),
                                                  Core::Context(Core::Constants::C_EDIT_MODE));
        cmd->setDefaultKeySequence(action->shortcut());
        connect(action, &QAction::triggered, this, &Plugin::triggerAction);
        menu->addAction(cmd);
    }
    menu->addSeparator();

    const auto &commentActions = mFormatActions.commentActions();
    for (int idx = 0; idx < FormatActions::COMMENTS::LAST_COMMENT; idx++) {
        action = commentActions[idx];
        cmd = Core::ActionManager::registerAction(action,
                                                  qPrintable(action->objectName()),
                                                  Core::Context(Core::Constants::C_EDIT_MODE));
        cmd->setDefaultKeySequence(action->shortcut());
        connect(action, &QAction::triggered, this, &Plugin::triggerAction);
        menu->addAction(cmd);
    }

    menu->addSeparator();

    //    const auto &spaceActions = mFormatActions.spaceActions();
    const auto &sectionActions = mFormatActions.sectionActions();
    for (int idx = 0; idx < FormatActions::SECTIONS::LAST_SECTION; idx++) {
        action = sectionActions[idx];
        cmd = Core::ActionManager::registerAction(action,
                                                  qPrintable(action->objectName()),
                                                  Core::Context(Core::Constants::C_EDIT_MODE));
        cmd->setDefaultKeySequence(action->shortcut());
        connect(action, &QAction::triggered, this, &Plugin::triggerAction);
        menu->addAction(cmd);
    }

    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);
}

void Plugin::triggerAction()
{
    auto id = Core::EditorManager::currentDocument()->id();
    if (id == Constants::EditorId) {
        auto *editor = static_cast<Editor *>(Core::EditorManager::currentEditor());
        if (editor) {
            auto *editorWidget = static_cast<EditorWidget *>(editor->widget());
            if (editorWidget) {
                auto objName = sender()->objectName();
                QString actionID = "Sphinx Editor.Action.";

                if (objName == actionID + ".Bold") {
                    editorWidget->onBold();
                }
            }
        }
    }
}
} // namespace qtc::plugin::sphinx
