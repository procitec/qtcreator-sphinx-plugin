#include "SphinxEditorWidget.h"
#include "../options/SphinxSettings.h"
#include "../qtcreator-sphinx-plugin-projectsettings.h"
#include "../qtcreator-sphinx-pluginconstants.h"
#include "SphinxFormatActions.h"
#include "SphinxRest2Html.h"
#include "SphinxRstcheckHighlighter.h"

#include "coreplugin/icore.h"
#include "coreplugin/rightpane.h"
#include "projectexplorer/project.h"
#include "projectexplorer/session.h"

namespace qtc::plugin::sphinx {

const int RSTCHECK_UPDATE_INTERVAL = 300;
const int PREVIEW_UPDATE_INTERVAL = 300;

EditorWidget::EditorWidget()
    : TextEditor::TextEditorWidget()
{
    setLanguageSettingsId(Constants::SettingsGeneralId);
    readSettings();

    connectActions();
    addToolBar();

    mUpdateRstCheckTimer.setSingleShot(true);
    mUpdateRstCheckTimer.setInterval(RSTCHECK_UPDATE_INTERVAL);
    connect(&mUpdateRstCheckTimer, &QTimer::timeout, this, [this] {
        if (mReSTCheckUpdatePending)
            updateRstCheck();
    });

    mPreviewTimer.setSingleShot(true);
    mPreviewTimer.setInterval(PREVIEW_UPDATE_INTERVAL);
    connect(&mPreviewTimer, &QTimer::timeout, this, [this] {
        if (mPreviewPending) {
            updatePreview();
        }
    });

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &EditorWidget::onCustomContextMenu);
}
void EditorWidget::finalizeInitialization()
{
    connect(document(), &QTextDocument::contentsChanged, this, &EditorWidget::scheduleRstCheckUpdate);
    connect(document(), &QTextDocument::contentsChanged, this, &EditorWidget::schedulePreview);
}

void EditorWidget::onCustomContextMenu(const QPoint &pos)
{
    QMenu menu(this);

    this->appendStandardContextMenuActions(&menu);

    menu.addSeparator();
    if (mRightPane) {
        menu.addAction(mShowRightPaneAction);
    }
    if (!mRightPane->html().isEnabled()) {
        menu.addAction(mUrlAction);
    }

    menu.exec(this->mapToGlobal(pos));
}

void EditorWidget::unCommentSelection()
{
    // todo determine in comment or not
    // than call formatter action
}
EditorWidget::~EditorWidget()
{
    saveFileSettings();
}

void EditorWidget::aboutToOpen(const QString &fileName, const QString &realFileName)
{
    Q_UNUSED(fileName)
    mRealFileName = realFileName;
    readFileSettings();
}

void EditorWidget::connectActions()
{
    const auto &formatActions = mFormatActions.formatActions();
    connect(formatActions[FormatActions::BOLD], SIGNAL(triggered(bool)), this, SLOT(onBold()));
    connect(formatActions[FormatActions::ITALIC], SIGNAL(triggered(bool)), this, SLOT(onItalic()));
    connect(formatActions[FormatActions::CODE], SIGNAL(triggered(bool)), this, SLOT(onCode()));

    const auto &spaceActions = mFormatActions.spaceActions();
    //    connect(spaceActions[FormatActions::DOUBLE_PARAGRAPH],
    //            SIGNAL(triggered(bool)),
    //            this,
    //            SLOT(onDoubleParaSpacing()));
    //    connect(spaceActions[FormatActions::NORMAL_PARAGRAPH],
    //            SIGNAL(triggered(bool)),
    //            this,
    //            SLOT(onNormalParaSpacing()));
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

    mUrlAction = new QAction(tr("link to preview url"), this);
    connect(mUrlAction, &QAction::triggered, this, &EditorWidget::onUrlAction);
    mShowRightPaneAction = new QAction(tr("toggle preview"), this);
    connect(mShowRightPaneAction, &QAction::triggered, this, &EditorWidget::onToggleRightPane);
    connect(ReST2Html::instance(), &ReST2Html::htmlChanged, this, &EditorWidget::onPreviewHtmlChanged);
}

void EditorWidget::onUrlAction()
{
    onShowRightPane(true);
    mRightPane->setCurrentTab(RightPaneWidget::PAGE_HTML);
    mRightPane->html().updateView();
}

void EditorWidget::onToggleRightPane()
{
    if (mRightPane) {
        onShowRightPane(!mRightPane->isVisible());
        if (mUsePreview && mRightPane->isVisible()) {
            mRightPane->setCurrentTab(RightPaneWidget::PAGE_PREVIEW);
            mRightPane->preview().updateView();
        }
    }
}

void EditorWidget::onPreviewHtmlChanged(const QString &html)
{
    if (!html.isEmpty() && mRightPane->isVisible()) {
        onShowRightPane(true);
        //mRightPane->setCurrentTab(RightPaneWidget::PAGE_PREVIEW);
        mRightPane->setTabEnabled(RightPaneWidget::PAGE_PREVIEW, true);
        mRightPane->preview().setHtml(html);
    }
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
    s->beginGroup(Constants::SettingsGeneralId);
    mUseReSTCheckHighlighter = s->value(SettingsIds::ReSTCheckHighlighter,
                                        QVariant(mUseReSTCheckHighlighter))
                                   .toBool();
    mUsePreview = s->value(SettingsIds::RST2HTML, QVariant(mUsePreview)).toBool();
    s->endGroup();

    if (mUsePreview) {
        if (!mRightPane) {
            mRightPane = new RightPaneWidget(this);
        }
    }

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

void EditorWidget::readFileSettings()
{
    auto *project = ProjectExplorer::SessionManager::projectForFile(textDocument()->filePath());
    if (!project) {
        project = ProjectExplorer::SessionManager::startupProject();
    }
    if (project) {
        const auto projectSettings = Internal::ProjectSettings::getSettings(project);
        auto linkedPreviews = projectSettings->linkedPreviews();
        for (auto iter = linkedPreviews.begin(); iter != linkedPreviews.end(); ++iter) {
            auto filePath = Utils::FilePath::fromString(mRealFileName);
            if (filePath == iter.key()) {
                if (!iter.value().isEmpty()) {
                    if (!mRightPane) {
                        mRightPane = new RightPaneWidget(this);
                    }
                    onShowRightPane(true);
                    mRightPane->html().setHtmlFile(iter.value());
                }
            }
        }
    }
}

void EditorWidget::saveFileSettings()
{
    if (mRightPane) {
        auto *project = ProjectExplorer::SessionManager::projectForFile(textDocument()->filePath());
        if (!project) {
            project = ProjectExplorer::SessionManager::startupProject();
        }
        if (project) {
            const auto projectSettings = Internal::ProjectSettings::getSettings(project);
            auto url = mRightPane->html().htmlFile();
            auto filePath = textDocument()->filePath();
            if (!url.isEmpty() && filePath.exists()) {
                projectSettings->addLinkedPreview(Internal::LinkedPreview(filePath, url));
            }
        }
    }
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

//void EditorWidget::onDoubleParaSpacing()
//{
//    // editor->doubleParagraphSpacing();
//}

//void EditorWidget::onNormalParaSpacing()
//{
//    //  assert(editor);
//    //  editor->normalParagraphSpacing();
//}

void EditorWidget::onIncreaseIndent()
{
    mFormatter.insertTextAtBlockStart(this, QString().fill(' ', indentSize()));
}

void EditorWidget::onDecreaseIndent()
{
    mFormatter.removeTextAtBlockStart(this, QString().fill(' ', indentSize()));
}

void EditorWidget::onBulletedList()
{
    mFormatter.insertTextAtBlockStart(this, QStringLiteral("* "));
}

void EditorWidget::onAutoNumberedList()
{
    mFormatter.insertTextAtBlockStart(this, QStringLiteral("#. "));
}

void EditorWidget::onNumberedList()
{
    mFormatter.insertLineTextAtBlockStart(this, QStringLiteral("%1. "));
}

void EditorWidget::onBlockQuote()
{
    mFormatter.insertTextAtBlockStart(this, QString("| "));
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
            TextEditorWidget::keyPressEvent(e);
        }

        forwardToBase = false;
        //textDocumentPtr()->autoIndent(textCursor());
        if (0 < mAutoIndent) {
            auto tc = textCursor();
            tc.insertText(QString().fill(' ', indent()));
        }
    }

    if (forwardToBase) {
        TextEditorWidget::keyPressEvent(e);
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

void EditorWidget::schedulePreview()
{
    mPreviewPending = mPreviewTimer.isActive();
    if (mPreviewPending)
        return;

    mPreviewPending = false;
    updatePreview();
    mPreviewTimer.start();
}

void EditorWidget::updatePreview()
{
    if (mUsePreview && !ReST2Html::instance()->run(textDocument(), mRealFileName)) {
        mPreviewPending = true;
        mPreviewTimer.start();
    }
}

void EditorWidget::showEvent(QShowEvent *e)
{
    onShowRightPane(true);
    TextEditorWidget::showEvent(e);
}

void EditorWidget::hideEvent(QHideEvent *e)
{
    onShowRightPane(false);
    TextEditorWidget::hideEvent(e);
}

void EditorWidget::onShowRightPane(bool show)
{
    if (mRightPane) {
        if (show) {
            mRightPane->show();
            Core::RightPaneWidget::instance()->setWidget(mRightPane);
            Core::RightPaneWidget::instance()->setShown(true);
        } else {
            mRightPane->hide();
            Core::RightPaneWidget::instance()->setWidget(nullptr);
            Core::RightPaneWidget::instance()->setShown(false);
        }
    }
}

} // namespace qtc::plugin::sphinx
