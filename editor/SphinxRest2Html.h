#pragma once

#include "SphinxWidgetHelpers.h"
#include <texteditor/semantichighlighter.h>
#include <utils/fileutils.h>

#include <QElapsedTimer>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QPointer>
#include <QTemporaryFile>

#include <functional>
#include <memory>

QT_FORWARD_DECLARE_CLASS(QProcess)

namespace TextEditor {
class TextDocument;
}

namespace qtc::plugin::sphinx {

class TextMark;

//typedef TextEditor::HighlightingResult Offense;
//typedef QVector<TextEditor::HighlightingResult> Offenses;

class ReST2Html : public QObject
{
    Q_OBJECT
public:
    ReST2Html();
    ~ReST2Html();

    static ReST2Html *instance();

    bool run(TextEditor::TextDocument *document);

signals:
    void htmlChanged(const QString &);

private:
    class Range
    {
    public:
        int line = 0;
        int pos = 0;
        int length = 0;

        Range() = default;
        Range(int pos, int length)
            : pos(pos)
            , length(length)
        {}
        Range(int line, int pos, int length)
            : line(line)
            , pos(pos)
            , length(length)
        {}

        // Not really equal, since the length attribute is ignored.
        bool operator==(const Range &other) const
        {
            const int value = other.pos;
            return value >= pos && value < (pos + length);
        }

        bool operator<(const Range &other) const
        {
            const int value = other.pos;
            return pos < value && (pos + length) < value;
        }
    };

    void initReST2HtmlProcess(const QString &);
    void restartReST2HtmlProcess(const QString &);
    void closeReST2HtmlProcess();
    void finishReST2HtmlOutput();
    void processReST2HtmlOutput(const QString &buffer);
    void processReST2HtmlErrors(const QString &buffer);
    /*Offenses*/ void processReST2HtmlErrorOutput(const QString &buffer);
    Range lineColumnLengthToRange(int line, int column, int length);
    std::unique_ptr<QTemporaryFile> logFilePath() const;

    bool mReST2HtmlFound = false;
    bool mBusy = false;
    QProcess *mReST2HtmlProcess = nullptr;
    QString mReST2HtmlScript;
    QTemporaryFile mTemporaryFile;
    QString mPythonFilePath;
    QString mOutputBuffer;
    QString mErrorBuffer;

    int mStartRevision = 0;
    QPointer<TextEditor::TextDocument> mDocument;
    QHash<int, QTextCharFormat> mExtraFormats;

    QHash<Utils::FilePath, Marks::Diagnostics> mDiagnostics;
    std::vector<TextMark *> mTextMarks;

    QElapsedTimer mTimer;
    QString mStartSeq = "%<$<!<";
    QString mEndSeq = ">!>$>%";
    QString mOutHtml;
    std::unique_ptr<QTemporaryFile> mLogFile = nullptr;
    QList<QString> mMessageFilter;
};
} // namespace qtc::plugin::sphinx
