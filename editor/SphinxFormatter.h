#pragma once

#include <QtCore/QObject>
#include <QtGui/QTextCursor>

namespace qtcreator::plugin::sphinx {

class EditorWidget;

class Formatter : public QObject
{
    class CursorInfo;
    Q_OBJECT
public:
    Formatter();
    virtual ~Formatter() {}

public Q_SLOTS:
    void removeTextAtBlockStart(EditorWidget *editor, const QString &);
    void removeLineTextAtBlockStart(EditorWidget *editor, const QString &, int lineOffset = 1);
    void insertTextAtBlockStart(EditorWidget *editor, const QString &);
    void insertLineTextAtBlockStart(EditorWidget *editor, const QString &, int lineOffset = 1);

    void insertAroundCursor(EditorWidget *editor, const QString &text, QTextCursor::SelectionType);
    void removeAroundCursor(EditorWidget *editor, const QString &text, QTextCursor::SelectionType);
    void removeAt(EditorWidget *editor, const QString &text, QTextCursor::SelectionType);
    void insertBeforeBlock(EditorWidget *editor,
                           const QString &text,
                           bool indentBlock = true,
                           bool wrapBlockWithSpacing = true);
    void removeBeforeBlock(EditorWidget *editor,
                           const QString &text,
                           bool unindentBlock = true,
                           bool unwrapBlockSpacing = true);
    void insertHeading(EditorWidget *editor, const QChar &, bool overLine);
    void removeHeading(EditorWidget *editor);
    void insertAt(EditorWidget *editor, const QString &text, QTextCursor::SelectionType);

private:
    int blockNumberOfPos(QTextCursor &, int pos) const;
    bool isBlockEmpty(QTextCursor &) const;
    int removeBlock(QTextCursor &) const;
    CursorInfo currentCursorAndSel(const QTextCursor &) const;
    void restoreCursorAndSel(QTextCursor &, const CursorInfo &) const;

    bool checkCompareTextAtBlockStart(QTextCursor &tc,
                                      const CursorInfo &cursorInfo,
                                      const QString &text,
                                      int lineOffset = -1);

    CursorInfo removeTextAtBlockStart(QTextCursor &, const CursorInfo &, const QString &text);
    CursorInfo removeLineTextAtBlockStart(QTextCursor &,
                                          const CursorInfo &,
                                          const QString &text,
                                          int lineOffset);

    CursorInfo insertTextAtBlockStart(QTextCursor &, const CursorInfo &info, const QString &text);
    CursorInfo insertLineTextAtBlockStart(QTextCursor &,
                                          const CursorInfo &info,
                                          const QString &text,
                                          int lineOffset);

private:
    class CursorInfo
    {
    public:
        CursorInfo(int start, int end, bool hasSelection)
            : mInfo(std::make_tuple(start, end, hasSelection))
        {}
        void incr(int len = 1)
        {
            std::get<0>(mInfo) += len;
            std::get<1>(mInfo) += len;
        }

        void decr(int len = 1) { incr(-1 * len); }

        void incrEnd(int len)
        {
            std::get<1>(mInfo) += len;
            if (!hasSelection()) {
                std::get<0>(mInfo) += len;
            }
        }

        void decrEnd(int len) { incrEnd(-1 * len); }

        int start() const { return std::get<0>(mInfo); }
        int end() const { return std::get<1>(mInfo); }
        bool hasSelection() const { return std::get<2>(mInfo); }

    private:
        CursorInfo();
        std::tuple<int, int, bool> mInfo;
    };

    //  TextEditIfc* mEditor = nullptr;
};
} // namespace qtcreator::plugin::sphinx
