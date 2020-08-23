
#include <texteditor/syntaxhighlighter.h>
#include <QtCore/QObject>
#include <QtCore/QRegularExpression>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCursor>

namespace qtcreator::plugin::sphinx {
class Highlighter : public TextEditor::SyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(QObject *parent = nullptr);

    virtual ~Highlighter() = default;

public:
    QTextCharFormat tst_format(int pos) const { return format(pos); };

protected:
    void highlightBlock(const QString &text) override;

private:
    enum BLOCKSTATE {
        NONE = 0,
        NEWLINE = 0x00010000,
        BLOCK_BEGIN = 0x00100000,
        COMMENT = 0x00000001,
        LIST = 0x00000002,
        DIRECTIVE = 0x00000004,
        DIRECTIVEOPTION = 0x00000008,
        DIRECTIVECONTENT = 0x00000010,
        TITLE = 0x00000020,
        COMMENT_CONTENT = 0x00000040,
    };

    QTextCharFormat mCommentFormat;
    QTextCharFormat mDirectiveFormat;
    QTextCharFormat mDirectiveOptionFormat;
    QTextCharFormat mTitleFormat;
    QTextCharFormat mInlineFormat;
    QTextCharFormat mBoldFormat;
    QTextCharFormat mItalicFormat;
    QTextCharFormat mCodeFormat;
    QTextCharFormat mTodoFormat;
    QTextCharFormat mDefaultFormat;
    QTextCharFormat mTickFormat;

    QRegularExpression mDirectiveRegEx;
    QRegularExpression mDirectiveOptionRegEx;
    QRegularExpression mTitleRegEx;
    QRegularExpression mBoldRegEx;
    QRegularExpression mItalicRegEx;
    QRegularExpression mCodeRegEx;
    QRegularExpression mRoleRegEx;
    QRegularExpression mRoleContentRegEx;
};
} // namespace qtcreator::plugin::sphinx
