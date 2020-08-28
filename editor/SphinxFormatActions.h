#pragma once

#include <QtCore/QList>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolBar>

namespace qtc::plugin::sphinx {

class FormatActions : public QWidget
{
    Q_OBJECT
public:
    FormatActions();
    ~FormatActions() = default;

    enum FORMAT { BOLD, ITALIC, CODE, LAST_FORMAT };

    enum SPACINGS {
        //        DOUBLE_PARAGRAPH,
        //        NORMAL_PARAGRAPH,
        INCREASE_INDENT,
        DECREASE_INDENT,
        LAST_SPACING
    };

    enum LISTS {
        BULLETED,
        AUTONUMBERED,
        NUMBERED,
        BLOCKQUOTE,
        //    TERM,
        LAST_LIST
    };

    enum COMMENTS { COMMENTS, UNCOMMENT, LAST_COMMENT };

    enum SECTIONS {
        DEFAULT,
        PART,
        CHAPTER,
        SECTIONS,
        SUBSECTION,
        SUBSUBSECTION,
        PARAGRAPHS,
        LAST_SECTION
    };

    enum BLOCKS { LITERAL, LAST_BLOCK };

    enum LINKS {
        WEBLINK_INLINE, // `Link text <https://domain.invalid/>`_
        WEBLINK_DEF,    // This is a paragraph that contains `a link`_.
                        //    .. _a link: https://domain.invalid/
    };

    const QVector<QAction *> &formatActions() const { return mFormat; }
    const QVector<QAction *> &spaceActions() const { return mSpacing; }
    const QVector<QAction *> &listActions() const { return mList; }
    const QVector<QAction *> &commentActions() const { return mComment; }
    const QVector<QAction *> &sectionActions() const { return mSection; }
    void toMenu(QMenu *menu) const;
    void toToolBar(QToolBar *);

private:
    QString toDarkName(const QAction *action) const;
    QVector<QAction *> mFormat;
    QVector<QAction *> mSpacing;
    QVector<QAction *> mComment;
    QVector<QAction *> mList;
    QVector<QAction *> mSection;
    QVector<QAction *> mBlock;
    QVector<QAction *> mLink;
};
} // namespace qtc::plugin::sphinx
