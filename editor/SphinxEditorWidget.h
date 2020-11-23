#pragma once

#include "SphinxDocument.h"
#include "SphinxFormatActions.h"
#include "SphinxFormatter.h"
#include "SphinxRightPaneWidget.h"
#include <texteditor/tabsettings.h>
#include <texteditor/textdocument.h>
#include <texteditor/texteditor.h>
#include <utils/uncommentselection.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>

namespace qtc::plugin::sphinx {
class EditorWidget : public TextEditor::TextEditorWidget
{
    Q_OBJECT
public:
    EditorWidget();
    virtual ~EditorWidget();

    void unCommentSelection() override;
    void aboutToOpen(const QString &fileName, const QString &realFileName) override;

    void increaseIndent() { mAutoIndent++; }
    void decreaseIndent() { mAutoIndent = (0 < mAutoIndent) ? mAutoIndent - 1 : 0; }
    //    void doubleParagraphSpacing() { mParagraphSpacing = 2; }
    //    void normalParagraphSpacing() { mParagraphSpacing = 1; }
    int indent() const { return mAutoIndent * indentSize(); }
    int indentSize() const
    {
        return static_cast<EditorDocument *>(this->textDocument())->tabSettings().m_indentSize;
    }
    int spacing() const { return mParagraphSpacing; }
    Formatter &formatter() { return mFormatter; }

public Q_SLOTS:
    // simple inline formats
    void onBold();
    void onItalic();
    void onCode();

    // spacing
    void onIncreaseIndent();
    void onDecreaseIndent();
    //    void onNormalParaSpacing();
    //    void onDoubleParaSpacing();

    void onBulletedList();
    void onAutoNumberedList();
    void onNumberedList();
    void onBlockQuote();

    void onAddComment();
    void onRemoveComment();

    void onPart();
    void onChapter();
    void onSection();
    void onSubSection();
    void onSubSubSection();
    void onParagraphs();
    void onRemoveSection();

protected:
    void finalizeInitialization() override;
    void keyPressEvent(QKeyEvent *e) override;
    void showEvent(QShowEvent *) override;
    void hideEvent(QHideEvent *) override;

private Q_SLOTS:
    void onCustomContextMenu(const QPoint &pos);
    void onUrlAction();
    void onToggleRightPane();
    void onLivePreviewHtmlChanged(const QString &);

private:
    void addToolBar();

    void readSettings();
    void readFileSettings();
    void saveFileSettings();
    void connectActions();
    void handleTabKeyRemove();
    void handleTabKeyInsert();
    void scheduleRstCheckUpdate();
    void updateRstCheck();
    void scheduleLivePreview();
    void onShowPreview(bool show);
    void updateLivePreview();

    int mAutoIndent = 0;
    bool mInsertSpaceForTab = true;
    int mParagraphSpacing = 1;
    QToolBar *mToolbar = nullptr;
    FormatActions mFormatActions;
    Formatter mFormatter;

    QString mParts;
    QString mChapters;

    bool mPartsOverline = true;
    bool mChaptersOverline = true;

    QString mSections;
    QString mSubSections;
    QString mSubSubSections;
    QString mParagraphs;

    bool mSectionsOverline = false;
    bool mSubSectionsOverline = false;
    bool mSubSubSectionsOverline = false;
    bool mParagraphsOverline = false;

    QTimer mUpdateRstCheckTimer;
    bool mReSTCheckUpdatePending = false;
    bool mUseReSTCheckHighlighter = false;

    QTimer mLivePreviewTimer;
    bool mLivePreviewPending = false;
    bool mUseLivePreview = false;

    QString mRealFileName;
    RightPaneWidget *mRightPane = nullptr;

    QAction *mUrlAction = nullptr;
    QAction *mShowRightPaneAction = nullptr;
};
} // namespace qtc::plugin::sphinx
