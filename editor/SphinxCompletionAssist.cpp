#include "SphinxCompletionAssist.h"
#include "../qtcreator-sphinx-pluginconstants.h"
#include "SphinxCodeModel.h"
#include "SphinxRegularExpressions.h"

#include <texteditor/codeassist/assistinterface.h>
#include <texteditor/codeassist/assistproposalitem.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/codeassist/genericproposalmodel.h>

#include <QDebug>
#include <QTextBlock>
#include <QTextDocument>
#include <QtConcurrent/QtConcurrent>
#include <QtCore/QFutureInterface>
#include <QtCore/QRegularExpression>

namespace qtc::plugin::sphinx {

TextEditor::IAssistProcessor *CompletionAssistProvider::createProcessor() const
{
    return new CompletionAssistProcessor;
}

bool CompletionAssistProvider::isActivationCharSequence(const QString &sequence) const
{
    return sequence.startsWith(".. ") || (0 == sequence.indexOf(QRegularExpression("^\\s+:")));
}

static QStringList createProposal(QFutureWatcher<QStringList> *future,
                                  const QString &text,
                                  const QString &wordUnderCursor)
{
    const QRegularExpression wordRE("([a-zA-Z_][a-zA-Z0-9_]{2,})");

    QSet<QString> words;
    QRegularExpressionMatchIterator it = wordRE.globalMatch(text);
    int wordUnderCursorFound = 0;
    while (it.hasNext()) {
        if (future->isCanceled())
            return {};
        QRegularExpressionMatch match = it.next();
        const QString &word = match.captured();
        if (word == wordUnderCursor) {
            // Only add the word under cursor if it
            // already appears elsewhere in the text
            if (++wordUnderCursorFound < 2)
                continue;
        }

        if (!words.contains(word))
            words.insert(word);
    }

    return words.values();
}

CompletionAssistProcessor::CompletionAssistProcessor()
    : m_SphinxSnippetCollector(Constants::SnippetGroupId, QIcon())
    , mDocumentWords()
{}

CompletionAssistProcessor::~CompletionAssistProcessor()
{
    cancel();
}

void CompletionAssistProcessor::cancel()
{
    if (isWatcherRunning()) {
        mWordProcessor.cancel();
    }
}

TextEditor::IAssistProposal *CompletionAssistProcessor::perform(const TextEditor::AssistInterface *interface)
{
    TextEditor::IAssistProposal *proposal = nullptr;
    QList<TextEditor::AssistProposalItemInterface *> snippetProposal = {};

    int startPosition = interface->position();
    const QString line = interface->textDocument()->findBlock(startPosition).text();
    const int blockPos = interface->textDocument()->findBlock(startPosition).position();

    auto linePos = line.lastIndexOf(QRegExp(R"-(\s)-"));
    QString context;

    if (0 < linePos) {
        linePos++; // the letter after the space
        context = line.mid(linePos, startPosition);
    } else {
        context = line;
        linePos = 0;
    }

    if (interface->reason() == TextEditor::IdleEditor) {
        if (!isWatcherRunning()) {
            const QString text = interface->textDocument()->toPlainText();
            // get the word unter cursor
            auto wordUnderCursor = line.mid(linePos, startPosition);

            QObject::connect(&mWordProcessor, &QFutureWatcher<QStringList>::finished, [this]() {
                mDocumentWords = mWordProcessor.result();
            });

            // Start the computation.
            QFuture<QStringList> future = QtConcurrent::run(createProposal,
                                                            &mWordProcessor,
                                                            text,
                                                            wordUnderCursor);
            mWordProcessor.setFuture(future);
            //return nullptr;
        }
    }

    // QString myTyping = interface->textAt(startPosition, interface->position() - startPosition);
    //auto fileName = interface->filePath();

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
        // load from code model, search for directive options
        linePos = line.lastIndexOf(QRegExp(R"-(^\s+:)-"));
        if (0 <= linePos) {
            // this could be a role or an option. We have to look if we are in context of a directive
            for (auto block = interface->textDocument()->findBlock(startPosition).blockNumber() - 1;
                 block >= 0;
                 block--) {
                auto l = interface->textDocument()->findBlockByNumber(block).text();
                if (0 == l.indexOf(SphinxRegularExpressions::DirectiveRegEx)) {
                    // found directive, complete the options
                    // determine directive name
                    auto match = SphinxRegularExpressions::DirectiveCaptureRegEx.match(l);
                    if (match.hasMatch()) {
                        auto directive = match.captured(1);

                        linePos = line.lastIndexOf(QRegExp(R"-(:)-"));
                        linePos++;
                        context = line.mid(linePos, startPosition);

                        for (const auto &snippet :
                             CodeModel::instance()->collectDirectiveOptions(directive)) {
                            if (snippet.trigger().startsWith(context)) {
                                auto item = new TextEditor::AssistProposalItem;
                                item->setText(snippet.trigger() + QLatin1Char(' ')
                                              + snippet.complement());
                                item->setData(snippet.content());
                                item->setDetail(snippet.generateTip());
                                //                item->setIcon(icon);
                                //                item->setOrder(order);
                                snippetProposal += item;
                            }
                        }
                    }

                    break;
                } else if (-1 == l.indexOf(SphinxRegularExpressions::DirectiveOptionRegEx))
                    break;
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
        linePos = line.lastIndexOf(QRegExp(R"-(\s)-"));

        if (0 < linePos) {
            linePos++; // the letter after the space
            context = line.mid(linePos, startPosition);
            //linePos = startPosition;
        } else {
            context = line;
            linePos = 0;
        }

        // load from this document words
        if (!context.isEmpty()) {
            for (const auto &word : mDocumentWords) {
                if (word.startsWith(context)) { // this should match all directives
                    auto item = new TextEditor::AssistProposalItem;
                    item->setText(word);
                    item->setData(word);
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
} // namespace qtc::plugin::sphinx

} // namespace qtc::plugin::sphinx
