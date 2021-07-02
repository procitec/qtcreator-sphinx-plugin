#include "SphinxRest2Html.h"
#include "../options/SphinxSettings.h"
#include "../qtcreator-sphinx-pluginconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/messagemanager.h>
#include <utils/temporarydirectory.h>
#include <utils/temporaryfile.h>

#include <texteditor/semantichighlighter.h>
#include <texteditor/textdocument.h>
#include <texteditor/textmark.h>

#include <QtConcurrent/QtConcurrent>
#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtCore/QProcess>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>

Q_LOGGING_CATEGORY(log_rst2html, "qtc.sphinx.rst2html");

namespace qtc::plugin::sphinx {

static ReST2Html *theRest2HtmlInstance = nullptr;

class ReST2HtmlFucture : public QFutureInterface<TextEditor::HighlightingResult>, public QObject
{
public:
    explicit ReST2HtmlFucture(const Offenses &offenses) { reportResults(offenses); }
};

class TextMark : public TextEditor::TextMark
{
public:
    static Utils::Theme::Color colorForSeverity(int severity)
    {
        switch (severity) {
        case 1:
            return Utils::Theme::TextColorNormal;
        case 2:
            return Utils::Theme::CodeModel_Warning_TextMarkColor;
        case 3:
            return Utils::Theme::CodeModel_Error_TextMarkColor;
        case 4:
            return Utils::Theme::CodeModel_Error_TextMarkColor;
        default:
            return Utils::Theme::TextColorNormal;
        }
    }
    TextMark(const Utils::FilePath &fileName, int line, int severity, const QString &text)
        : TextEditor::TextMark(fileName, line, "rest2html")
    {
        setColor(colorForSeverity(severity));
        setPriority(TextEditor::TextMark::Priority(severity));
        setLineAnnotation(text);
        QString tooltip = text;
        setToolTip(tooltip);
    }
};

ReST2Html::ReST2Html()
{
    theRest2HtmlInstance = this;
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
        mReST2HtmlScript = Core::ICore::resourcePath().toString()
                           + QLatin1String("/sphinx/rst2html/rest_to_html.py");
        mReST2HtmlFound = QFileInfo(mReST2HtmlScript).exists();
    } else {
        mPythonFilePath.clear();
        mReST2HtmlFound = false;
    }

    if (mReST2HtmlFound) {
        qCInfo(log_rst2html) << "found rst2html script in " << mPythonFilePath << " "
                             << mReST2HtmlScript;
    } else {
        qCWarning(log_rst2html) << "not executable or wrong configured rst2html script call:"
                                << mPythonFilePath << " " << mReST2HtmlScript;
    }

    mLogFile = logFilePath();
}

ReST2Html::~ReST2Html()
{
    if (!mReST2HtmlProcess)
        return;
    closeReST2HtmlProcess();
}

ReST2Html *ReST2Html::instance()
{
    return theRest2HtmlInstance;
}

// return false if we are busy, true if everything is ok (or ReST2Html wasn't found)
bool ReST2Html::run(TextEditor::TextDocument *document /*, const QString &fileNameTip*/)
{
    if (!document) {
        return true;
    }

    bool filePathValid = !document->filePath().isEmpty();
    if (!filePathValid) {
        return true;
    }
    const QString filePath = QFileInfo(document->filePath().toString()).absoluteDir().absolutePath();

    if (!mReST2HtmlProcess) {
        initReST2HtmlProcess(filePath);
    } else if (mReST2HtmlProcess->workingDirectory() != filePath) {
        // run the process in context of the file path for parsing ".. include:: correctly"
        restartReST2HtmlProcess(filePath);
    }

    if (mBusy || mReST2HtmlProcess->state() == QProcess::Starting)
        return false;
    if (!mReST2HtmlFound || !(mReST2HtmlProcess->state() == QProcess::Running))
        return true;

    QByteArray data = document->plainText().toUtf8();
    if (!data.isEmpty()) {
        mBusy = true;
        mStartRevision = document->document()->revision();

        mTimer.start();
        mDocument = document;

        mReST2HtmlProcess->write(mStartSeq.toUtf8().constData());
        mReST2HtmlProcess->write(data.constData(), data.length());
        mReST2HtmlProcess->write(mEndSeq.toUtf8().constData());
        mReST2HtmlProcess->write("\n");
    }
    return true;
}

std::unique_ptr<QTemporaryFile> ReST2Html::logFilePath() const
{
    auto relResource = QString(qtc::plugin::sphinx::Constants::SnippetGroupId).toLower();

    const QString fileTemplate = QLatin1String("/rst2html") + QLatin1String("-XXXXXX.log");

    auto temporary_dir = Utils::TemporaryDirectory(relResource);
    temporary_dir.setAutoRemove(false);

    auto temporaryFile = std::make_unique<QTemporaryFile>(temporary_dir.path() + fileTemplate);
    temporaryFile->open();
    temporaryFile->setAutoRemove(false);

    qCInfo(log_rst2html()) << "using temporary file" << temporaryFile->fileName()
                           << "for rst2html logs";

    return temporaryFile;
}

void ReST2Html::initReST2HtmlProcess(const QString &workingDirectoy)
{
    if (!mReST2HtmlProcess) {
        mReST2HtmlProcess = new QProcess;
        mReST2HtmlProcess->setWorkingDirectory(workingDirectoy);
        void (QProcess::*signal)(int, QProcess::ExitStatus) = &QProcess::finished;
        QObject::connect(mReST2HtmlProcess, signal, [&](int status, QProcess::ExitStatus /*exitStatus*/) {
            if (status) {
                Core::MessageManager::instance()->writeFlashing(
                    QString("rst2html exited with error: %1\n").arg(status)
                    + QString::fromUtf8(mReST2HtmlProcess->readAllStandardError().trimmed()));
            }
        });

        QObject::connect(mReST2HtmlProcess, &QProcess::readyReadStandardOutput, [&]() {
            mOutputBuffer.append(QString::fromUtf8(mReST2HtmlProcess->readAllStandardOutput()));
            if (mOutputBuffer.endsWith(mEndSeq + "\n")) {
                finishReST2HtmlHighlight();
            }
        });

        mReST2HtmlProcess->start(mPythonFilePath,
                                 {mReST2HtmlScript, mStartSeq, mEndSeq, mLogFile->fileName()},
                                 QIODevice::ReadWrite | QIODevice::Text);
    }
}

void ReST2Html::closeReST2HtmlProcess()
{
    if (mReST2HtmlProcess) {
        //mReST2HtmlProcess->write(mEndSeq.toUtf8().constData());
        mReST2HtmlProcess->closeWriteChannel();
        mReST2HtmlProcess->waitForBytesWritten();
        mReST2HtmlProcess->waitForFinished(300);
        if (mReST2HtmlProcess->state() == QProcess::NotRunning) {
            delete mReST2HtmlProcess;
            mReST2HtmlProcess = nullptr;
        } else {
            qCWarning(log_rst2html()) << "process could not be stopped gracefully";
        }
    }
}

void ReST2Html::restartReST2HtmlProcess(const QString &workingDirectory)
{
    qCInfo(log_rst2html()) << "restart html process during new workingDirectory:" << workingDirectory;
    closeReST2HtmlProcess();
    initReST2HtmlProcess(workingDirectory);
}

void ReST2Html::finishReST2HtmlHighlight()
{
    QTextDocument *doc = mDocument ? mDocument->document() : nullptr;
    if (!doc || mStartRevision != doc->revision()) {
        mBusy = false;
        return;
    }

    bool isHtml = (mOutputBuffer.startsWith("<!DOCTYPE html>")) ? true : false;
    // normal output or error output?
    if (!isHtml) {
        qCInfo(log_rst2html()) << "received error output in rst2html" << mOutputBuffer;
        Offenses offenses = processReST2HtmlErrorOutput();
        const Utils::FilePath filePath = mDocument->filePath();
        for (Diagnostic &diag : mDiagnostics[filePath]) {
            diag.textMark = std::make_shared<TextMark>(filePath, diag.line, diag.severity, diag.message);
            mDocument->addMark(diag.textMark.get());
        }
        ReST2HtmlFucture ReST2HtmlFucture(offenses);
        TextEditor::SemanticHighlighter::incrementalApplyExtraAdditionalFormats(
            mDocument->syntaxHighlighter(), ReST2HtmlFucture.future(), 0, offenses.count(), mExtraFormats);
        TextEditor::SemanticHighlighter::clearExtraAdditionalFormatsUntilEnd(
            mDocument->syntaxHighlighter(), ReST2HtmlFucture.future());
    } else {
        processReST2HtmlOutput();
    }

    mBusy = false;
}

static int kindOfSeverity(const QStringRef &severity)
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

Offenses ReST2Html::processReST2HtmlErrorOutput()
{
    //        SystemMessage: <string>:13: (SEVERE/4) Problems with "include" directive path:
    //        InputError: [Errno 2] No such file or directory: 'test.rst'.

    Offenses result;
    Diagnostics &diag = mDiagnostics[mDocument->filePath()] = Diagnostics();

    mOutHtml.clear();

    const QVector<QStringRef> lines = mOutputBuffer.splitRef('\n');
    for (const QStringRef &line : lines) {
        if (line == mEndSeq)
            break;

        QVector<QStringRef> fields = line.split(':');
        if (fields.size() < 2)
            continue;
        int lineN = fields[0].toInt();
        int kind = kindOfSeverity(fields[1]);
        int column = -1;
        //fields[2].toInt();
        int length = -1;
        //fields[3].toInt();
        result << TextEditor::HighlightingResult(uint(lineN), uint(column), uint(length), kind);

        QRegularExpression messageRegEx = QRegularExpression(
            "\\((INFO|WARNING|ERROR|SEVERE)/\\d+\\)(.*)");
        QRegularExpressionMatch match = messageRegEx.match(fields[1]);
        QString message;
        if (match.hasMatch()) {
            message = match.captured(2).trimmed();
        }
        diag.push_back(Diagnostic{lineN, kind, message, nullptr});
    }

    mOutputBuffer.clear();

    return result;
}

void ReST2Html::processReST2HtmlOutput()
{
    mOutHtml.clear();
    const QVector<QStringRef> lines = mOutputBuffer.splitRef('\n');
    for (const QStringRef &line : lines) {
        if (line == mEndSeq)
            break;

        mOutHtml.append(line + "\n");
    }

    emit htmlChanged(mOutHtml);

    mOutputBuffer.clear();
}

qtc::plugin::sphinx::ReST2Html::Range ReST2Html::lineColumnLengthToRange(int line, int column, int length)
{
    const QTextBlock block = mDocument->document()->findBlockByLineNumber(line - 1);
    const int pos = block.position() + column;
    return Range(line, pos, length);
}

} // namespace qtc::plugin::sphinx
