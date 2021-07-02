#pragma once

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

typedef TextEditor::HighlightingResult Offense;
typedef QVector<TextEditor::HighlightingResult> Offenses;

class ReSTCheckHighLighter : public QObject
{
    Q_OBJECT
public:
    ReSTCheckHighLighter();
    ~ReSTCheckHighLighter();

    static ReSTCheckHighLighter *instance();

    bool run(TextEditor::TextDocument *document);

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

    struct Diagnostic
    {
        int line;
        int severity;
        QString message;
        std::shared_ptr<TextMark> textMark;
    };

    using Diagnostics = std::vector<Diagnostic>;

    void initReSTCheckProcess();
    void finishReSTCheckHighlight();
    Offenses processReSTCheckOutput();
    Range lineColumnLengthToRange(int line, int column, int length);

    bool mReSTCheckFound = false;
    bool mBusy = false;
    QProcess *mReSTCheckProcess = nullptr;
    QString mReSTCheckScript;
    QTemporaryFile mTemporaryFile;
    QString mPythonFilePath;
    QString mOutputBuffer;

    int mStartRevision = 0;
    QPointer<TextEditor::TextDocument> mDocument;
    QHash<int, QTextCharFormat> mExtraFormats;

    QHash<Utils::FilePath, Diagnostics> mDiagnostics;
    std::vector<TextMark *> mTextMarks;

    QElapsedTimer mTimer;
    QString mStartSeq = "%<$<!<";
    QString mEndSeq = ">!>$>%";
};
} // namespace qtc::plugin::sphinx
