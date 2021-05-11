#pragma once

#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/snippets/snippetassistcollector.h>

#include <QtCore/QFutureWatcher>
#include <QtGui/QIcon>

namespace qtc::plugin::sphinx {

class CodeModel;

class CompletionAssistProvider : public TextEditor::CompletionAssistProvider
{
public:
    TextEditor::IAssistProcessor *createProcessor() const override;

    int activationCharSequenceLength() const override { return 1; }
    bool isActivationCharSequence(const QString &sequence) const override;
};

class CompletionAssistProcessor : public TextEditor::IAssistProcessor
{
public:
    CompletionAssistProcessor();
    virtual ~CompletionAssistProcessor();
    TextEditor::IAssistProposal *perform(const TextEditor::AssistInterface *interface) override;
    void cancel() final;

private:
    bool isWatcherRunning() const { return mWordProcessor.isRunning(); }

private:
    TextEditor::SnippetAssistCollector m_SphinxSnippetCollector;
    QStringList mDocumentWords;
    QFutureWatcher<QStringList> mWordProcessor;
};

} // namespace qtc::plugin::sphinx
