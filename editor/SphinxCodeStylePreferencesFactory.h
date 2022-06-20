#pragma once
#include <texteditor/icodestylepreferencesfactory.h>
#include <utils/id.h>

namespace qtc::plugin::sphinx {

class CodeStylePreferencesFactory : public TextEditor::ICodeStylePreferencesFactory
{
    Q_DECLARE_TR_FUNCTIONS(CodeStylePreferencesFactory)
public:
    Utils::Id languageId() override;
    QString displayName() override;
    TextEditor::ICodeStylePreferences *createCodeStyle() const override;
    QWidget *createEditor(TextEditor::ICodeStylePreferences *, ProjectExplorer::Project *project = nullptr, QWidget *parent =nullptr) const override;
    TextEditor::Indenter *createIndenter(QTextDocument *doc) const override;
    QString snippetProviderGroupId() const override;
    QString previewText() const override;
};

} // namespace qtc::plugin::sphinx
