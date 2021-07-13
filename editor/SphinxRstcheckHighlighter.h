#pragma once

#include "SphinxWidgetHelpers.h"
#include <texteditor/semantichighlighter.h>
#include <utils/fileutils.h>

#include <QElapsedTimer>
//#include <QFuture>
//#include <QFutureWatcher>
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

class ReSTCheckHighLighter : public QObject
{
    Q_OBJECT
public:
    ReSTCheckHighLighter();
    ~ReSTCheckHighLighter();

    static ReSTCheckHighLighter *instance();

    bool run(TextEditor::TextDocument *document);

private:
    void initReSTCheckProcess();
    void finishReSTCheckHighlight();
    void processReSTCheckOutput();
    Marks::Range lineColumnLengthToRange(int line, int column, int length);
    std::unique_ptr<QTemporaryFile> logFilePath() const;

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

    QHash<Utils::FilePath, Marks::Diagnostics> mDiagnostics;
    std::vector<TextMark *> mTextMarks;

    QElapsedTimer mTimer;
    QString mStartSeq = "%<$<!<";
    QString mEndSeq = ">!>$>%";
    std::unique_ptr<QTemporaryFile> mLogFile = nullptr;
};
} // namespace qtc::plugin::sphinx
