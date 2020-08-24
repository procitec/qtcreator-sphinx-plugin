#include "SphinxCompletionAssist.h"
#include "../qtcreator-sphinx-pluginconstants.h"
#include "SphinxCodeModel.h"

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
    const int blockPos = interface->textDocument()->findBlock(startPosition).position();
    QString context;
    int linePos = line.lastIndexOf(QRegExp(R"-(\s)-"));
    if (0 < linePos) {
        linePos++; // the letter afther the space
        context = line.mid(linePos, startPosition);
    } else {
        context = line;
        linePos = 0;
    }

    QString myTyping = interface->textAt(startPosition, interface->position() - startPosition);
    const QString fileName = interface->fileName();

    QList<TextEditor::AssistProposalItemInterface *> snippetProposal;

    for (auto *snippet : m_SphinxSnippetCollector.collect()) {
        if (snippet->text().startsWith(context)) { // this should match all directives
            snippetProposal += snippet;
        }
    }

    if (snippetProposal.empty()) {
        // load from code model, search for directives
        linePos = line.lastIndexOf(QRegExp(R"-(^\s*\.{2}\s+)-"));
        if (0 <= linePos) {
            linePos = line.lastIndexOf(QRegExp(R"-(\s)-"));
            linePos++;
            context = line.mid(linePos, startPosition);

            for (const auto &snippet : CodeModel::instance()->collectDirectives()) {
                if (snippet.trigger().startsWith(context)) {
                    auto item = new TextEditor::AssistProposalItem;
                    item->setText(snippet.trigger() + QLatin1Char(' ') + snippet.complement());
                    item->setData(snippet.content());
                    item->setDetail(snippet.generateTip());
                    //                item->setIcon(icon);
                    //                item->setOrder(order);
                    snippetProposal += item;
                }
            }
        }
    }

    if (snippetProposal.empty()) {
        // load from code model, search for roles
        linePos = line.lastIndexOf(QRegExp(R"-(^\s*:|\s+:)-"));
        if (0 <= linePos) {
            linePos = line.lastIndexOf(QRegExp(R"-(:)-"));
            linePos++;
            context = line.mid(linePos, startPosition);

            for (const auto &snippet : CodeModel::instance()->collectRoles()) {
                if (snippet.trigger().startsWith(context)) {
                    auto item = new TextEditor::AssistProposalItem;
                    item->setText(snippet.trigger() + QLatin1Char(' ') + snippet.complement());
                    item->setData(snippet.content());
                    item->setDetail(snippet.generateTip());
                    //                item->setIcon(icon);
                    //                item->setOrder(order);
                    snippetProposal += item;
                }
            }
        }
    }

    if (snippetProposal.empty()) {
        return nullptr;
    }

    TextEditor::GenericProposalModelPtr model(new TextEditor::GenericProposalModel);

    if (!snippetProposal.isEmpty()) {
        model->loadContent(snippetProposal);
        proposal = new TextEditor::GenericProposal(blockPos + linePos, model);
    }
    return proposal;
}

} // namespace qtcreator::plugin::sphinx
