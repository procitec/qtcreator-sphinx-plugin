#include "SphinxRstcheckHighlighter.h"
#include "../options/SphinxSettings.h"
#include "../qtcreator-sphinx-pluginconstants.h"
#include "SphinxWidgetHelpers.h"

#include <coreplugin/icore.h>
#include <coreplugin/messagemanager.h>
#include <texteditor/semantichighlighter.h>
#include <texteditor/textdocument.h>

#include <QtConcurrent/QtConcurrent>
#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtCore/QProcess>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>

Q_LOGGING_CATEGORY(log_rstcheck, "qtc.sphinx.rstcheck");

namespace qtc::plugin::sphinx {

static ReSTCheckHighLighter *theReSTCheckHighlighterInstance = nullptr;

//class ReSTCheckFucture : public QFutureInterface<TextEditor::HighlightingResult>, public QObject
//{
//public:
//    explicit ReSTCheckFucture(const Offenses &offenses) { reportResults(offenses); }
//};

ReSTCheckHighLighter::ReSTCheckHighLighter()
{
    theReSTCheckHighlighterInstance = this;
    QTextCharFormat format;
    format.setUnderlineColor(Qt::darkGreen);
    format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    mExtraFormats[0] = format;
    format.setUnderlineColor(Qt::darkYellow);
    mExtraFormats[1] = format;
    format.setUnderlineColor(Qt::red);
    mExtraFormats[2] = format;

    QSettings *s = Core::ICore::settings();
    s->beginGroup(Constants::SettingsGeneralId);
    QString pythonFilePath = s->value(SettingsIds::PythonFilePath, QVariant("")).toString();
    s->endGroup();

    QFileInfo info(pythonFilePath);
    if (info.exists() && info.isExecutable()) {
        mPythonFilePath = info.absoluteFilePath();
        mReSTCheckScript = Core::ICore::resourcePath().toString()
                           + QLatin1String("/sphinx/rstcheck/rest_check.py");
        mReSTCheckFound = QFileInfo(mReSTCheckScript).exists();
    } else {
        mPythonFilePath.clear();
        mReSTCheckFound = false;
    }

    mLogFile = logFilePath();
}

ReSTCheckHighLighter::~ReSTCheckHighLighter()
{
    if (mReSTCheckProcess) {
        mReSTCheckProcess->closeWriteChannel();
        mReSTCheckProcess->waitForFinished(3000);
    }

    delete mReSTCheckProcess;
    mReSTCheckProcess = nullptr;
    removeMarks();
}

ReSTCheckHighLighter *ReSTCheckHighLighter::instance()
{
    return theReSTCheckHighlighterInstance;
}

// return false if we are busy, true if everything is ok (or rstcheck wasn't found)
bool ReSTCheckHighLighter::run(TextEditor::TextDocument *document)
{
    if (!document) {
        return true;
    }

    if (!mReSTCheckProcess) {
        initReSTCheckProcess();
    }

    if (mBusy || mReSTCheckProcess->state() == QProcess::Starting)
        return false;
    if (!mReSTCheckFound)
        return true;

    QByteArray data = document->plainText().toUtf8();
    if (!data.isEmpty()) {
        mBusy = true;
        mStartRevision = document->document()->revision();

        mTimer.start();
        mDocument = document;

        mReSTCheckProcess->write(mStartSeq.toUtf8().constData());
        mReSTCheckProcess->write(data.constData(), data.length());
        mReSTCheckProcess->write(mEndSeq.toUtf8().constData());
        mReSTCheckProcess->write("\n");
    }
    return true;
}

void ReSTCheckHighLighter::initReSTCheckProcess()
{
    mReSTCheckProcess = new QProcess;
    void (QProcess::*signal)(int, QProcess::ExitStatus) = &QProcess::finished;
    QObject::connect(mReSTCheckProcess, signal, [&](int status, QProcess::ExitStatus /*exitStatus*/) {
        if (status) {
            Core::MessageManager::instance()->writeFlashing(
                QString::fromUtf8(mReSTCheckProcess->readAllStandardError().trimmed()));
            mReSTCheckFound = false;
        }
    });

    QObject::connect(mReSTCheckProcess, &QProcess::readyReadStandardOutput, [&]() {
        mOutputBuffer.append(QString::fromUtf8(mReSTCheckProcess->readAllStandardOutput()));
        if (mOutputBuffer.endsWith(mEndSeq + "\n")) {
            finishReSTCheckHighlight();
        }
    });

    mReSTCheckProcess->start(mPythonFilePath,
                             {mReSTCheckScript, mStartSeq, mEndSeq, mLogFile->fileName()},
                             QIODevice::ReadWrite | QIODevice::Text);
}

void ReSTCheckHighLighter::finishReSTCheckHighlight()
{
    QTextDocument *doc = mDocument ? mDocument->document() : nullptr;
    if (!doc || mStartRevision != doc->revision()) {
        mBusy = false;
        return;
    }

    /*Offenses offenses = */ processReSTCheckOutput();
    const Utils::FilePath filePath = mDocument->filePath();
    if (!filePath.isEmpty()) {
        for (auto &diag : mDiagnostics[filePath]) {
            diag.textMark = std::make_shared<Marks::TextMark>(filePath,
                                                              diag.line,
                                                              diag.severity,
                                                              diag.message);
            mDocument->addMark(diag.textMark.get());
        }
    }
    //    ReSTCheckFucture ReSTCheckFucture(offenses);
    //    TextEditor::SemanticHighlighter::incrementalApplyExtraAdditionalFormats(
    //        mDocument->syntaxHighlighter(), ReSTCheckFucture.future(), 0, offenses.count(), mExtraFormats);
    //    TextEditor::SemanticHighlighter::clearExtraAdditionalFormatsUntilEnd(mDocument->syntaxHighlighter(),
    //                                                                         ReSTCheckFucture.future());

    mBusy = false;
}

static int kindOfSeverity(const QStringView &severity)
{
    int kind = 0;

    QRegularExpression sevRegEx = QRegularExpression("\\((INFO|WARNING|ERROR|SEVERE)/\\d+\\).*");
    QRegularExpressionMatch match = sevRegEx.match(severity);
    if (match.hasMatch()) {
        QString matched = match.captured(1).trimmed();
        if (matched == QLatin1String("INFO")) {
            kind = 1;
        } else if (matched == QLatin1String("WARNING")) {
            kind = 2;
        } else if (matched == QLatin1String("ERROR")) {
            kind = 3;
        } else if (matched == QLatin1String("SEVERE")) {
            kind = 4;
        }
    }
    return kind;
}

std::unique_ptr<QTemporaryFile> ReSTCheckHighLighter::logFilePath() const
{
    auto temporary_dir = WidgetHelpers::log_directory();
    const QString fileTemplate = QLatin1String("/rstcheck") + QLatin1String("-XXXXXX.log");

    auto temporaryFile = std::make_unique<QTemporaryFile>(temporary_dir + fileTemplate);
    temporaryFile->open();
    temporaryFile->setAutoRemove(false);

    qCInfo(log_rstcheck()) << "using temporary file" << temporaryFile->fileName()
                           << "for rstcheck logs";

    return temporaryFile;
}
void ReSTCheckHighLighter::removeMarks()
{
    if (mDocument && !mDocument->filePath().isEmpty()) {
        for (auto &diag : mDiagnostics[mDocument->filePath()]) {
            if (diag.textMark) {
                mDocument->removeMark(diag.textMark.get());
                mDocument->removeMarkFromMarksCache(diag.textMark.get());
            }
        }
    }
    mDiagnostics.clear();
}

void ReSTCheckHighLighter::processReSTCheckOutput()
{
    removeMarks();

    auto &diags = mDiagnostics[mDocument->filePath()];

    auto lines = mOutputBuffer.split('\n');
    for (const auto &line : lines) {
        if (line == mEndSeq)
            break;
        auto fields = line.split(':');
        if (fields.size() < 2)
            continue;
        int lineN = fields[0].toInt();
        int kind = kindOfSeverity(fields[1]);
        int column = -1;
        //fields[2].toInt();
        //int length = -1;
        //fields[3].toInt();
        //result << TextEditor::HighlightingResult(uint(lineN), uint(column), uint(length), kind);
        qCInfo(log_rstcheck) << "adding offense at " << lineN << column << kind;

        QRegularExpression messageRegEx = QRegularExpression(
            "\\((INFO|WARNING|ERROR|SEVERE)/\\d+\\)(.*)");
        QRegularExpressionMatch match = messageRegEx.match(fields[1]);
        QString message;
        if (match.hasMatch()) {
            message = match.captured(2).trimmed();
        }
        diags[lineN] = (Marks::Diagnostic{lineN, kind, message, nullptr});
    }

    qCInfo(log_rstcheck) << "found diagnostic issues with rstcheck" << diags.size() << " in file "
                         << qPrintable(mDocument->filePath().toString());

    mOutputBuffer.clear();
}

Marks::Range ReSTCheckHighLighter::lineColumnLengthToRange(int line, int column, int length)
{
    const QTextBlock block = mDocument->document()->findBlockByLineNumber(line - 1);
    const int pos = block.position() + column;
    return Marks::Range(line, pos, length);
}

} // namespace qtc::plugin::sphinx
