#include "SphinxEditorFactory.h"
#include "../qtcreator-sphinx-pluginconstants.h"
#include "SphinxCompletionAssist.h"
#include "SphinxDocument.h"
#include "SphinxEditor.h"
#include "SphinxEditorWidget.h"
#include "SphinxHighlighter.h"
#include "SphinxIndenter.h"

#include "../options/SphinxSettings.h"

#include <coreplugin/icore.h>

#include <texteditor/texteditoractionhandler.h>
#include <texteditor/texteditorsettings.h>

#include <QCoreApplication>

namespace qtc::plugin::sphinx {

EditorFactory::EditorFactory()
{
    setId(Constants::EditorId);
    setDisplayName(qApp->translate("OpenWith::Editors", Constants::EditorDisplayName));
    addMimeType(Constants::MimeType);

    QSettings *s = Core::ICore::settings();
    s->beginGroup(Constants::SettingsGeneralId);
    bool useInternalHighlighter = s->value(SettingsIds::CustomHighlighter, QVariant(true)).toBool();
    s->endGroup();

    setDocumentCreator([]() { return new EditorDocument; });
    setIndenterCreator([](QTextDocument *doc) { return new Indenter(doc); });
    setEditorWidgetCreator([]() { return new EditorWidget; });
    setEditorCreator([]() { return new Editor; });
    //    setAutoCompleterCreator([]() { return new AutoCompleter; });
    setCompletionAssistProvider(new CompletionAssistProvider);

    setSyntaxHighlighterCreator([]() {
        QSettings *s = Core::ICore::settings();
        s->beginGroup(Constants::SettingsGeneralId);
        bool useInternalHighlighter = s->value(SettingsIds::CustomHighlighter, QVariant(true)).toBool();
        s->endGroup();

        return (useInternalHighlighter) ? new Highlighter : new TextEditor::SyntaxHighlighter;
    });
    setUseGenericHighlighter(!useInternalHighlighter);

    Utils::CommentDefinition cmnt(".. ");

    setCommentDefinition(cmnt);
    setCodeFoldingSupported(true);
    setMarksVisible(true);

    setEditorActionHandlers(TextEditor::TextEditorActionHandler::Format
                            | TextEditor::TextEditorActionHandler::UnCommentSelection
                            | TextEditor::TextEditorActionHandler::UnCollapseAll);
}

// this is called by settings dialog for texteditor (snippets and so on)
void EditorFactory::decorateEditor(TextEditor::TextEditorWidget *editor)
{
    if (TextEditor::TextDocument *document = editor->textDocument()) {
        document->setIndenter(new Indenter(document->document()));
        QSettings *s = Core::ICore::settings();
        s->beginGroup(Constants::SettingsGeneralId);
        bool useInternalHighlighter = s->value(SettingsIds::CustomHighlighter, QVariant(true)).toBool();
        s->endGroup();
        if (useInternalHighlighter) {
            document->setSyntaxHighlighter(new Highlighter);
        } else {
            editor->setupGenericHighlighter();
        }
    }
}

} // namespace qtc::plugin::sphinx
