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
  void insertTextAtBlockStart(EditorWidget *editor, const QString &);

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
  bool isBlockEmpty(QTextCursor &);
  int removeBlock(QTextCursor &);
  CursorInfo currentCursorAndSel(const QTextCursor &) const;
  void restorCursorAndSel(QTextCursor &, const CursorInfo &, int offset);
  int removeTextAtBlockStart(
      int start, int end, QTextCursor &, const QString &text, int selectionOffset);
  int insertTextAtBlockStart(
      int start, int end, QTextCursor &, const QString &text, int selectionOffset);

  private:
  class CursorInfo
  {
  public:
    CursorInfo( int start, int end, bool hasSelection )
      : mInfo( std::make_tuple( start, end, hasSelection ) )
    {
    }
    int  start() const { return std::get<0>( mInfo ); }
    int  end() const { return std::get<1>( mInfo ); }
    bool hasSelection() const { return std::get<2>( mInfo ); }

  private:
    CursorInfo();
    std::tuple<int, int, bool> mInfo;
  };

  //  TextEditIfc* mEditor = nullptr;
};
} // namespace qtcreator::plugin::sphinx
