#pragma once

#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/snippets/snippetassistcollector.h>

#include <QIcon>

namespace qtcreator::plugin::sphinx {

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
    TextEditor::IAssistProposal *perform(const TextEditor::AssistInterface *interface) override;

private:
    TextEditor::SnippetAssistCollector m_SphinxSnippetCollector;
};

} // namespace qtcreator::plugin::sphinx
