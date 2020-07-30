#include "SphinxEditorWidget.h"
#include "../options/SphinxSettings.h"
#include "Constants.h"
#include "SphinxFormatActions.h"
#include "SphinxRstcheckHighlighter.h"

#include "coreplugin/icore.h"

namespace qtcreator::plugin::sphinx {

const int RSTCHECK_UPDATE_INTERVAL = 300;

EditorWidget::EditorWidget()
    : TextEditor::TextEditorWidget()
{
    setLanguageSettingsId(Constants::SettingsId);
    readSettings();

    connectActions();
    addToolBar();

    mUpdateRstCheckTimer.setSingleShot(true);
    mUpdateRstCheckTimer.setInterval(RSTCHECK_UPDATE_INTERVAL);
    connect(&mUpdateRstCheckTimer, &QTimer::timeout, this, [this] {
        if (mReSTCheckUpdatePending)
            updateRstCheck();
    });
}
void EditorWidget::finalizeInitialization()
{
    connect(document(), &QTextDocument::contentsChanged, this, &EditorWidget::scheduleRstCheckUpdate);
}

void EditorWidget::unCommentSelection()
{
    // todo determine in comment or not
    // than call formatter action
}
EditorWidget::~EditorWidget() {}

void EditorWidget::aboutToOpen(const QString &fileName, const QString &realFileName)
{
    Q_UNUSED(fileName)
    mRealFileName = realFileName;
}

void EditorWidget::connectActions()
{
    const auto &formatActions = mFormatActions.formatActions();
    connect(formatActions[FormatActions::BOLD], SIGNAL(triggered(bool)), this, SLOT(onBold()));
    connect(formatActions[FormatActions::ITALIC], SIGNAL(triggered(bool)), this, SLOT(onItalic()));
    connect(formatActions[FormatActions::CODE], SIGNAL(triggered(bool)), this, SLOT(onCode()));

    const auto &spaceActions = mFormatActions.spaceActions();
    connect(spaceActions[FormatActions::DOUBLE_PARAGRAPH],
            SIGNAL(triggered(bool)),
            this,
            SLOT(onDoubleParaSpacing()));
    connect(spaceActions[FormatActions::NORMAL_PARAGRAPH],
            SIGNAL(triggered(bool)),
            this,
            SLOT(onNormalParaSpacing()));
    connect(spaceActions[FormatActions::INCREASE_INDENT],
            SIGNAL(triggered(bool)),
            this,
            SLOT(onIncreaseIndent()));
    connect(spaceActions[FormatActions::DECREASE_INDENT],
            SIGNAL(triggered(bool)),
            this,
            SLOT(onDecreaseIndent()));

    const auto &listActions = mFormatActions.listActions();

    connect(listActions[FormatActions::BULLETED], SIGNAL(triggered(bool)), this, SLOT(onBulletedList()));
    connect(listActions[FormatActions::NUMBERED], SIGNAL(triggered(bool)), this, SLOT(onNumberedList()));
    connect(listActions[FormatActions::AUTONUMBERED],
            SIGNAL(triggered(bool)),
            this,
            SLOT(onAutoNumberedList()));
    connect(listActions[FormatActions::BLOCKQUOTE], SIGNAL(triggered(bool)), this, SLOT(onBlockQuote()));

    const auto &commentActions = mFormatActions.commentActions();

    connect(commentActions[FormatActions::COMMENT], SIGNAL(triggered(bool)), this, SLOT(onAddComment()));
    connect(commentActions[FormatActions::UNCOMMENT],
            SIGNAL(triggered(bool)),
            this,
            SLOT(onRemoveComment()));

    const auto &sectionActions = mFormatActions.sectionActions();
    connect(sectionActions[FormatActions::DEFAULT],
            SIGNAL(triggered(bool)),
            this,
            SLOT(onRemoveSection()));
    connect(sectionActions[FormatActions::PART], SIGNAL(triggered(bool)), this, SLOT(onPart()));
    connect(sectionActions[FormatActions::CHAPTER], SIGNAL(triggered(bool)), this, SLOT(onChapter()));
    connect(sectionActions[FormatActions::SECTION], SIGNAL(triggered(bool)), this, SLOT(onSection()));
    connect(sectionActions[FormatActions::SUBSECTION],
            SIGNAL(triggered(bool)),
            this,
            SLOT(onSubSection()));
    connect(sectionActions[FormatActions::SUBSUBSECTION],
            SIGNAL(triggered(bool)),
            this,
            SLOT(onSubSubSection()));
    connect(sectionActions[FormatActions::PARAGRAPHS],
            SIGNAL(triggered(bool)),
            this,
            SLOT(onParagraphs()));
}

void EditorWidget::addToolBar()
{
    if (!mToolbar) {
        mToolbar = new QToolBar(this);
        mFormatActions.toToolBar(mToolbar);
        insertExtraToolBarWidget(Left, mToolbar);
    }
}
void EditorWidget::readSettings()
{
    QSettings *s = Core::ICore::settings();
    s->beginGroup(Constants::SettingsId);
    mUseReSTCheckHighlighter = s->value(SettingsIds::ReSTCheckHighlighter, QVariant(true)).toBool();
    s->endGroup();

    //    parts\char="#"
    //    parts\overline=true
    //    chapters\char="*"
    //    chapters\overline=true
    //    sections\char="="
    //    subsections\char="-"
    //    subsubsections\char="^"
    //    paragraphs\char="""

    mParts = "#";
    mPartsOverline = true;
    mChapters = "*";
    mChaptersOverline = true;
    mSections = "=";
    mSectionsOverline = false;
    mSubSections = "-";
    mSubSectionsOverline = false;
    mSubSubSections = "^";
    mSubSubSectionsOverline = false;
    mParagraphs = QString(R"-(")-");
    mParagraphsOverline = false;
}

void EditorWidget::onBold()
{
    mFormatter.insertAroundCursor(this, "**", QTextCursor::WordUnderCursor);
}

void EditorWidget::onItalic()
{
    mFormatter.insertAroundCursor(this, "*", QTextCursor::WordUnderCursor);
}

void EditorWidget::onCode()
{
    mFormatter.insertAroundCursor(this, "``", QTextCursor::WordUnderCursor);
}

void EditorWidget::onDoubleParaSpacing()
{
    // editor->doubleParagraphSpacing();
}

void EditorWidget::onNormalParaSpacing()
{
    //  assert(editor);
    //  editor->normalParagraphSpacing();
}

void EditorWidget::onIncreaseIndent()
{
    //   assert(editor);
    //    editor->increaseIndent();
    //    insertAt(QString().fill(' ', editor->indent()),
    //    QTextCursor::BlockUnderCursor);
}

void EditorWidget::onDecreaseIndent()
{
    //    assert(editor);
    //    auto indent = editor->indent();
    //    if (0 < indent) {
    //        removeAt(QString().fill(' ', editor->indent()),
    //        QTextCursor::BlockUnderCursor); editor->decreaseIndent();
    //    }
}

void EditorWidget::onBulletedList()
{
    // mFormatter.insertBeforeCursor(this, "* ", QTextCursor::LineUnderCursor);
}

void EditorWidget::onAutoNumberedList()
{
    // mFormatter.insertBeforeCursor(this, "#. ", QTextCursor::LineUnderCursor);
}

void EditorWidget::onNumberedList() {}

void EditorWidget::onBlockQuote()
{
    // mFormatter.insertBeforeCursor(this, "| ", QTextCursor::LineUnderCursor);
}

void EditorWidget::onAddComment()
{
    mFormatter.insertBeforeBlock(this, "..", true, true);
}

void EditorWidget::onRemoveComment()
{
    mFormatter.removeBeforeBlock(this, "..", true, true);
}

void EditorWidget::onRemoveSection()
{
    mFormatter.removeHeading(this);
}

void EditorWidget::onPart()
{
    mFormatter.insertHeading(this, mParts.at(0), mPartsOverline);
}
void EditorWidget::onChapter()
{
    mFormatter.insertHeading(this, mChapters.at(0), mPartsOverline);
}

void EditorWidget::onSection()
{
    mFormatter.insertHeading(this, mSections.at(0), mSectionsOverline);
}

void EditorWidget::onSubSection()
{
    mFormatter.insertHeading(this, mSubSections.at(0), mSubSectionsOverline);
}
void EditorWidget::onSubSubSection()
{
    mFormatter.insertHeading(this, mSubSubSections.at(0), mSubSubSectionsOverline);
}
void EditorWidget::onParagraphs()
{
    mFormatter.insertHeading(this, mParagraphs.at(0), mParagraphsOverline);
}

void EditorWidget::handleTabKeyInsert()
{
    auto text = QString().fill(' ', indentSize());
    mFormatter.insertTextAtBlockStart(this, text);
}

void EditorWidget::handleTabKeyRemove()
{
    auto text = QString().fill(' ', indentSize());
    mFormatter.removeTextAtBlockStart(this, text);
}

void EditorWidget::keyPressEvent(QKeyEvent *e)
{
    auto k = e->key();
    bool forwardToBase = true;

    if (k == Qt::Key_Tab) {
        handleTabKeyInsert();
        forwardToBase = false;
    } else if (k == Qt::Key_Backtab) {
        handleTabKeyRemove();
        forwardToBase = false;
    } else if (k == Qt::Key_Return || k == Qt::Key_Enter) {
        for (auto line = 0; line < spacing(); line++) {
            QPlainTextEdit::keyPressEvent(e);
        }

        forwardToBase = false;
        if (0 < mAutoIndent) {
            auto tc = textCursor();
            tc.insertText(QString().fill(' ', indent()));
        }
    }

    if (forwardToBase) {
        QPlainTextEdit::keyPressEvent(e);
    }
}

void EditorWidget::scheduleRstCheckUpdate()
{
    mReSTCheckUpdatePending = mUpdateRstCheckTimer.isActive();
    if (mReSTCheckUpdatePending)
        return;

    mReSTCheckUpdatePending = false;
    updateRstCheck();
    mUpdateRstCheckTimer.start();
}

void EditorWidget::updateRstCheck()
{
    if (mUseReSTCheckHighlighter
        && !ReSTCheckHighLighter::instance()->run(textDocument(), mRealFileName)) {
        mReSTCheckUpdatePending = true;
        mUpdateRstCheckTimer.start();
    }
}

} // namespace qtcreator::plugin::sphinx
