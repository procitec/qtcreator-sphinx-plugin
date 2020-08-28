#include "SphinxFormatActions.h"

#include <QtWidgets/QComboBox>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>

namespace qtc::plugin::sphinx {

FormatActions::FormatActions()
    : mFormat(LAST_FORMAT)
    , mSpacing(LAST_SPACING)
    , mComment(LAST_COMMENT)
    , mList(LAST_LIST)
    , mSection(LAST_SECTION)
{
    QString actionID = "Sphinx Editor.Action.";
    QAction *action;
    action = new QAction(tr("Bold"), this);
    action->setObjectName(QString("%1.Bold").arg(actionID));
    action->setIcon(QIcon::fromTheme("format-text-bold"));
    mFormat[FORMAT::BOLD] = action;

    action = new QAction(tr("Italic"), this);
    action->setObjectName(QString("%1.Italic").arg(actionID));
    action->setIcon(QIcon::fromTheme("format-text-italic"));
    mFormat[FORMAT::ITALIC] = action;

    action = new QAction(tr("Code"), this);
    action->setObjectName(QString("%1.Code").arg(actionID));
    action->setIcon(QIcon::fromTheme("format-text-code"));
    mFormat[FORMAT::CODE] = action;

    //    action = new QAction(tr("Double Paragraph Spacing"), this);
    //    action->setObjectName(QString("%1.Double Paragraph Spacing").arg(actionID));
    //    action->setIcon(QIcon::fromTheme("format-line-spacing-double"));
    //    mSpacing[SPACING::DOUBLE_PARAGRAPH] = action;

    //    action = new QAction(tr("Normal Paragraph Spacing"), this);
    //    action->setObjectName(QString("%1.Normal Paragraph Spacing").arg(actionID));
    //    action->setIcon(QIcon::fromTheme("format-line-spacing-normal"));
    //    mSpacing[SPACING::NORMAL_PARAGRAPH] = action;

    action = new QAction(tr("Increase Indent"), this);
    action->setObjectName(QString("%1.Increase Indent").arg(actionID));
    action->setIcon(QIcon::fromTheme("format-indent-more"));
    mSpacing[SPACINGS::INCREASE_INDENT] = action;

    action = new QAction(tr("Decrease Indent"), this);
    action->setObjectName(QString("%1.Decrease Indent").arg(actionID));
    action->setIcon(QIcon::fromTheme("format-indent-less"));
    mSpacing[SPACINGS::DECREASE_INDENT] = action;

    action = new QAction(tr("Bulleted List"), this);
    action->setObjectName(QString("%1.Bulleted List").arg(actionID));
    action->setIcon(QIcon::fromTheme("format-list-unordered"));
    mList[LISTS::BULLETED] = action;

    action = new QAction(tr("Numbered List"), this);
    action->setObjectName(QString("%1.Numbered List").arg(actionID));
    action->setIcon(QIcon::fromTheme("format-list-ordered"));
    mList[LISTS::NUMBERED] = action;

    action = new QAction(tr("Auto Numbered List"), this);
    action->setObjectName(QString("%1.Auto Numbered List").arg(actionID));
    action->setIcon(QIcon::fromTheme("format-list-ordered"));
    mList[LISTS::AUTONUMBERED] = action;

    action = new QAction(tr("Block Quote"), this);
    action->setObjectName(QString("%1.Block Quote").arg(actionID));
    action->setIcon(QIcon::fromTheme("format-text-blockquote"));
    mList[LISTS::BLOCKQUOTE] = action;

    action = new QAction(tr("Comment selection"), this);
    action->setObjectName(QString("%1.Comment selection").arg(actionID));
    action->setIcon(QIcon::fromTheme("edit-comment"));
    mComment[COMMENTS::COMMENT] = action;

    action = new QAction(tr("Uncomment selection"), this);
    action->setObjectName(QString("%1.Uncomment selection").arg(actionID));
    action->setIcon(QIcon::fromTheme("delete-comment"));
    mComment[COMMENTS::UNCOMMENT] = action;

    action = new QAction(tr("Default"), this);
    action->setObjectName(QString("%1.Default").arg(actionID));
    mSection[SECTIONS::DEFAULT] = action;
    action = new QAction(tr("Part"), this);
    action->setObjectName(QString("%1.Part").arg(actionID));
    mSection[SECTIONS::PART] = action;
    action = new QAction(tr("Chapter"), this);
    action->setObjectName(QString("%1.Chapter").arg(actionID));
    mSection[SECTIONS::CHAPTER] = action;
    action = new QAction(tr("Section"), this);
    action->setObjectName(QString("%1.Section").arg(actionID));
    mSection[SECTIONS::SECTION] = action;
    action = new QAction(tr("SubSection"), this);
    action->setObjectName(QString("%1.SubSection").arg(actionID));
    mSection[SECTIONS::SUBSECTION] = action;
    action = new QAction(tr("SubSubSection"), this);
    action->setObjectName(QString("%1.SubSubSection").arg(actionID));
    mSection[SECTIONS::SUBSUBSECTION] = action;
    action = new QAction(tr("Paragraphs"), this);
    action->setObjectName(QString("%1.Paragraphs").arg(actionID));
    mSection[SECTIONS::PARAGRAPHS] = action;
}

void FormatActions::toMenu(QMenu *menu) const
{
    auto *textMenu = menu->addMenu(tr("Text"));

    for (int idx = 0; idx < FORMAT::LAST_FORMAT; idx++) {
        textMenu->addAction(mFormat[idx]);
    }

    auto *spacingMenu = menu->addMenu(tr("Spacing"));

    for (int idx = 0; idx < SPACINGS::LAST_SPACING; idx++) {
        spacingMenu->addAction(mSpacing[idx]);
    }

    auto *listsMenu = menu->addMenu(tr("Lists"));

    for (int idx = 0; idx < LISTS::LAST_LIST; idx++) {
        listsMenu->addAction(mList[idx]);
    }

    auto *sectionsMenu = menu->addMenu(tr("Sections"));
    for (int idx = 0; idx < SECTIONS::LAST_SECTION; idx++) {
        sectionsMenu->addAction(mSection[idx]);
    }

    menu->addSeparator();
    for (int idx = 0; idx < COMMENTS::LAST_COMMENT; idx++) {
        menu->addAction(mComment[idx]);
    }
    menu->addSeparator();
}

QString FormatActions::toDarkName(const QAction *action) const
{
    auto name = action->icon().name();
    if (!name.isEmpty()) {
        name = ":/dark/" + name;
    }
    return name;
}
void FormatActions::toToolBar(QToolBar *fmtToolBar)
{
    for (int idx = 0; idx < FORMAT::LAST_FORMAT; idx++) {
        auto *action = mFormat[idx];
        action->setIcon(QIcon(toDarkName(action)));
        fmtToolBar->addAction(action);
    }
    fmtToolBar->addSeparator();

    for (int idx = 0; idx < COMMENTS::LAST_COMMENT; idx++) {
        auto *action = mComment[idx];
        action->setIcon(QIcon(toDarkName(action)));
        fmtToolBar->addAction(action);
    }

    fmtToolBar->addSeparator();
    for (int idx = 0; idx < SPACINGS::LAST_SPACING; idx++) {
        auto *action = mSpacing[idx];
        action->setIcon(QIcon(toDarkName(action)));
        fmtToolBar->addAction(action);
    }

    fmtToolBar->addSeparator();
    for (int idx = 0; idx < LISTS::LAST_LIST; idx++) {
        auto *action = mList[idx];
        action->setIcon(QIcon(toDarkName(action)));
        fmtToolBar->addAction(action);
    }

    auto *sectionBtn = new QToolButton(this);
    sectionBtn->setText(tr("Sections"));
    sectionBtn->setPopupMode(QToolButton::InstantPopup);
    QMenu *sectionMenu = new QMenu(sectionBtn);
    for (int idx = 0; idx < SECTIONS::LAST_SECTION; idx++) {
        sectionMenu->addAction(mSection[idx]);
    }
    sectionBtn->setMenu(sectionMenu);

    fmtToolBar->addWidget(sectionBtn);
}
} // namespace qtc::plugin::sphinx
