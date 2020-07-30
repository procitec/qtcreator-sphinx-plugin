#pragma once
#include <coreplugin/icore.h>
#include <texteditor/icodestylepreferencesfactory.h>

namespace qtcreator::plugin::sphinx {

class CodeStylePreferencesFactory : public TextEditor::ICodeStylePreferencesFactory
{
    Q_DECLARE_TR_FUNCTIONS(CodeStylePreferencesFactory)
public:
    Core::Id languageId() override;
    QString displayName() override;
    TextEditor::ICodeStylePreferences *createCodeStyle() const override;
    QWidget *createEditor(TextEditor::ICodeStylePreferences *, QWidget *parent) const override;
    TextEditor::Indenter *createIndenter(QTextDocument *doc) const override;
    QString snippetProviderGroupId() const override;
    QString previewText() const override;
};

} // namespace qtcreator::plugin::sphinx
