#include "SphinxRest2Html.h"
#include "../options/SphinxSettings.h"
#include "../qtcreator-sphinx-pluginconstants.h"
#include "SphinxWidgetHelpers.h"

#include <coreplugin/icore.h>
#include <coreplugin/messagemanager.h>
#include <utils/temporarydirectory.h>

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

//class ReST2HtmlFuture : public QFutureInterface<TextEditor::HighlightingResult>, public QObject
//{
//public:
//    explicit ReST2HtmlFuture(const Offenses &offenses) { reportResults(offenses); }
//};

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
    mMessageFilter = {"Unknown directive"};
}

ReST2Html::~ReST2Html()
{
    closeReST2HtmlProcess();
    removeMarks();
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
        QVariantMap content;
        content["source"] = data.constData();
        content["source_path"] = document->filePath().toString();
        QJsonDocument json;
        json.setObject(QJsonObject::fromVariantMap(content));
        mReST2HtmlProcess->write(json.toJson().constData(), json.toJson().length());
        mReST2HtmlProcess->write(mEndSeq.toUtf8().constData());
        mReST2HtmlProcess->write("\n");
    }
    return true;
}

std::unique_ptr<QTemporaryFile> ReST2Html::logFilePath() const
{
    const QString fileTemplate = QLatin1String("/rst2html") + QLatin1String("-XXXXXX.log");

    auto temporary_dir = WidgetHelpers::log_directory();

    auto temporaryFile = std::make_unique<QTemporaryFile>(temporary_dir + fileTemplate);
    temporaryFile->open();
    temporaryFile->setAutoRemove(false);

    qCInfo(log_rst2html()) << "using temporary file" << temporaryFile->fileName()
                           << "for rst2html logs";

    return temporaryFile;
}

void ReST2Html::initReST2HtmlProcess(const QString &workingDirectory)
{
    if (!mReST2HtmlProcess) {
        mReST2HtmlProcess = new QProcess;
        mReST2HtmlProcess->setWorkingDirectory(workingDirectory);
        void (QProcess::*signal)(int, QProcess::ExitStatus) = &QProcess::finished;
        QObject::connect(mReST2HtmlProcess, signal, [&](int status, QProcess::ExitStatus /*exitStatus*/) {
            if (status) {
                Core::MessageManager::instance()->writeFlashing(
                    QString("rst2html exited with error: %1\n").arg(status) + mErrorBuffer);
            }
        });

        QObject::connect(mReST2HtmlProcess, &QProcess::readyReadStandardOutput, [&]() {
            mOutputBuffer.append(QString::fromUtf8(mReST2HtmlProcess->readAllStandardOutput()));
            if (mOutputBuffer.endsWith(mEndSeq + "\n")) {
                finishReST2HtmlOutput();
            }
        });
        QObject::connect(mReST2HtmlProcess, &QProcess::readyReadStandardError, [&]() {
            mErrorBuffer.append(QString::fromUtf8(mReST2HtmlProcess->readAllStandardError()));
        });

        mErrorBuffer.clear();
        mOutputBuffer.clear();

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

void ReST2Html::finishReST2HtmlOutput()
{
    QTextDocument *doc = mDocument ? mDocument->document() : nullptr;
    if (!doc || mStartRevision != doc->revision()) {
        mBusy = false;
        return;
    }

    bool isHtml = (mOutputBuffer.startsWith("<!DOCTYPE html>")) ? true : false;
    if (isHtml) {
        processReST2HtmlOutput(mOutputBuffer);
    }
    if (!mErrorBuffer.isEmpty()) {
        processReST2HtmlErrors(mErrorBuffer);
    }

    mOutputBuffer.clear();
    mErrorBuffer.clear();

    mBusy = false;
}

void ReST2Html::processReST2HtmlErrors(const QString &buffer)
{
    qCInfo(log_rst2html()) << "received error output in rst2html" << buffer;
    /*Offenses offenses = */
    processReST2HtmlErrorOutput(buffer);
    if (mDocument) {
        const Utils::FilePath filePath = mDocument->filePath();
        if (!filePath.isEmpty()) {
            if (mDiagnostics.contains(filePath)) {
                for (auto &diag : mDiagnostics[filePath]) {
                    diag.textMark = std::make_shared<Marks::TextMark>(filePath,
                                                                      diag.line,
                                                                      diag.severity,
                                                                      diag.message);
                    qCInfo(log_rst2html()) << "adding diag mark due to errors";
                    mDocument->addMark(diag.textMark.get());
                }
            }
        } else {
            qCWarning(log_rst2html()) << "current document file path is empty!";
        }
    }
    //    ReST2HtmlFuture ReST2HtmlFuture(offenses);
    //    TextEditor::SemanticHighlighter::incrementalApplyExtraAdditionalFormats(
    //        mDocument->syntaxHighlighter(), ReST2HtmlFuture.future(), 0, offenses.count(), mExtraFormats);
    //    TextEditor::SemanticHighlighter::clearExtraAdditionalFormatsUntilEnd(mDocument->syntaxHighlighter(),
    //                                                                         ReST2HtmlFuture.future());
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

void ReST2Html::removeMarks()
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

void ReST2Html::processReST2HtmlErrorOutput(const QString &buffer)
{
    //        SystemMessage: <string>:13: (SEVERE/4) Problems with "include" directive path:
    //        InputError: [Errno 2] No such file or directory: 'test.rst'.

    removeMarks();

    const QVector<QStringRef> lines = buffer.splitRef('\n');
    for (const QStringRef &line : lines) {
        if (line == mEndSeq)
            break;

        QVector<QStringRef> fields = line.split(':');
        if (fields.size() < 2)
            continue;

        auto filename = fields[0].toString();
        int lineN = fields[1].toInt();
        int kind = kindOfSeverity(fields[2]);
        //fields[2].toInt();
        //int length = -1;
        //fields[3].toInt();
        //result << TextEditor::HighlightingResult(uint(lineN), uint(column), uint(length), kind);

        //<string>:9: (ERROR/3) Content block expected for the \"code\" directive
        QRegularExpression messageRegEx = QRegularExpression(
            "\\((INFO|WARNING|ERROR|SEVERE)/\\d+\\)(.*)");
        QRegularExpressionMatch match = messageRegEx.match(fields[2]);
        QString message;
        if (match.hasMatch()) {
            message = match.captured(2).trimmed();
            for (const auto &f : mMessageFilter) {
                if (message.startsWith(f)) {
                    message.clear();
                    break;
                }
            }
        }
        // filename handling
        if (filename == "<string>" || filename.isEmpty())
            continue;

        if (0 < lineN && 0 < kind && !message.isEmpty()) {
            int column = -1;

            QFileInfo fi = (QFileInfo::exists(filename))
                               ? filename
                               : mReST2HtmlProcess->workingDirectory() + "/" + filename;

            qCInfo(log_rst2html) << "adding diag for " << fi.absoluteFilePath() << " at " << lineN
                                 << column << kind;
            auto &diag = mDiagnostics[Utils::FilePath::fromFileInfo(fi)];
            diag[lineN] = (Marks::Diagnostic{lineN, kind, message, nullptr});
        }
    }

    // return {};
}

void ReST2Html::processReST2HtmlOutput(const QString &buffer)
{
    mOutHtml.clear();
    const QVector<QStringRef> lines = buffer.splitRef('\n');
    for (const QStringRef &line : lines) {
        if (line == mEndSeq)
            break;

        mOutHtml.append(line + "\n");
    }

    emit htmlChanged(mOutHtml, mReST2HtmlProcess->workingDirectory());
}

qtc::plugin::sphinx::ReST2Html::Range ReST2Html::lineColumnLengthToRange(int line, int column, int length)
{
    const QTextBlock block = mDocument->document()->findBlockByLineNumber(line - 1);
    const int pos = block.position() + column;
    return Range(line, pos, length);
}

} // namespace qtc::plugin::sphinx
