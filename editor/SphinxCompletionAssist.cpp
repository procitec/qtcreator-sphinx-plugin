#include "SphinxCompletionAssist.h"
#include "../qtcreator-sphinx-pluginconstants.h"

#include <coreplugin/id.h>
#include <texteditor/codeassist/assistinterface.h>
#include <texteditor/codeassist/assistproposalitem.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/codeassist/genericproposalmodel.h>

#include <QDebug>
#include <QTextBlock>
#include <QTextDocument>
#include <QtCore/QRegularExpression>

namespace qtcreator::plugin::sphinx {

TextEditor::IAssistProcessor *CompletionAssistProvider::createProcessor() const
{
    return new CompletionAssistProcessor;
}

bool CompletionAssistProvider::isActivationCharSequence(const QString &sequence) const
{
    return sequence.startsWith(".. ") || (0 == sequence.indexOf(QRegularExpression("^\\s+:")));
}

CompletionAssistProcessor::CompletionAssistProcessor()
    : m_SphinxSnippetCollector(Constants::SnippetGroupId, QIcon())
{}

TextEditor::IAssistProposal *CompletionAssistProcessor::perform(const TextEditor::AssistInterface *interface)
{
    if (interface->reason() == TextEditor::IdleEditor)
        return 0;

    TextEditor::IAssistProposal *proposal = nullptr;

    int startPosition = interface->position();
    const QString line = interface->textDocument()->findBlock(startPosition).text();
    QString role;
    int linePos = interface->textDocument()->findBlock(startPosition).position();
    int rolePos = line.lastIndexOf(":", (line.length() - (startPosition - linePos)) - 1);
    if (0 < rolePos) {
        role = line.mid(rolePos, startPosition);
    }

    QString myTyping = interface->textAt(startPosition, interface->position() - startPosition);
    const QString fileName = interface->fileName();

    QList<TextEditor::AssistProposalItemInterface *> lineProposals;
    QList<TextEditor::AssistProposalItemInterface *> roleProposals;

    bool hasRole = !role.isEmpty();
    bool hasLine = !line.isEmpty();

    for (auto *snippet : m_SphinxSnippetCollector.collect()) {
        if (hasLine && snippet->text().startsWith(line)) { // this should match all directives
            lineProposals += snippet;
        } else if (hasRole) {
            if (snippet->text().startsWith(role)) {
                roleProposals += snippet;
            }
        }
    }

    if (lineProposals.empty() && roleProposals.isEmpty()) {
        return nullptr;
    }

    TextEditor::GenericProposalModelPtr model(new TextEditor::GenericProposalModel);
    if (!lineProposals.isEmpty()) {
        model->loadContent(lineProposals);
        proposal = new TextEditor::GenericProposal(linePos, model);
    } else if (!roleProposals.isEmpty()) {
        model->loadContent(roleProposals);
        proposal = new TextEditor::GenericProposal(linePos + rolePos, model);
    }

    return proposal;
}

} // namespace qtcreator::plugin::sphinx
