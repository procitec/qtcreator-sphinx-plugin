#include "SphinxRstcheckHighlighter.h"
#include "../options/SphinxSettings.h"
#include "Constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/messagemanager.h>
#include <texteditor/semantichighlighter.h>
#include <texteditor/textdocument.h>
#include <texteditor/textmark.h>

#include <QtConcurrent/QtConcurrent>
#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtCore/QProcess>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>

Q_LOGGING_CATEGORY(log, "qtc.sphinx.rstcheck");

namespace qtcreator::plugin::sphinx {

static ReSTCheckHighLighter *theInstance = nullptr;

class ReSTCheckFucture : public QFutureInterface<TextEditor::HighlightingResult>, public QObject
{
public:
    ReSTCheckFucture(const Offenses &offenses) { reportResults(offenses); }
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
        : TextEditor::TextMark(fileName, line, "rstcheck")
    {
        setColor(colorForSeverity(severity));
        setPriority(TextEditor::TextMark::Priority(severity));
        setLineAnnotation(text);
        QString tooltip = text;
        setToolTip(tooltip);
    }
};

ReSTCheckHighLighter::ReSTCheckHighLighter()
{
    theInstance = this;
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
        mReSTCheckScript = Core::ICore::resourcePath() + "/sphinx/rstcheck/rest_check.py";
        mReSTCheckFound = QFileInfo(mReSTCheckScript).exists();
    } else {
        mPythonFilePath.clear();
        mReSTCheckFound = false;
    }
}

ReSTCheckHighLighter::~ReSTCheckHighLighter()
{
    if (!mReSTCheckProcess)
        return;
    mReSTCheckProcess->closeWriteChannel();
    mReSTCheckProcess->waitForFinished(3000);
    delete mReSTCheckProcess;
}

ReSTCheckHighLighter *ReSTCheckHighLighter::instance()
{
    return theInstance;
}

// return false if we are busy, true if everything is ok (or rstcheck wasn't found)
bool ReSTCheckHighLighter::run(TextEditor::TextDocument *document, const QString &fileNameTip)
{
    if (!mReSTCheckProcess) {
        initReSTCheckProcess();
    }
    if (mBusy || mReSTCheckProcess->state() == QProcess::Starting)
        return false;
    if (!mReSTCheckFound)
        return true;

    mBusy = true;
    mStartRevision = document->document()->revision();

    mTimer.start();
    mDocument = document;

    const QString filePath = document->filePath().isEmpty() ? fileNameTip
                                                            : document->filePath().toString();

    mReSTCheckProcess->write(mStartSeq.toUtf8().constData());
    QByteArray data = document->plainText().toUtf8();
    mReSTCheckProcess->write(data.constData(), data.length());
    mReSTCheckProcess->write(mEndSeq.toUtf8().constData());
    mReSTCheckProcess->write("\n");
    return true;
}

void ReSTCheckHighLighter::initReSTCheckProcess()
{
    mReSTCheckProcess = new QProcess;
    void (QProcess::*signal)(int, QProcess::ExitStatus) = &QProcess::finished;
    QObject::connect(mReSTCheckProcess, signal, [&](int status, QProcess::ExitStatus /*exitStatus*/) {
        if (status) {
            Core::MessageManager::instance()
                ->write(QString::fromUtf8(mReSTCheckProcess->readAllStandardError().trimmed()),
                        Core::MessageManager::ModeSwitch);
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
                             {mReSTCheckScript, mStartSeq, mEndSeq},
                             QIODevice::ReadWrite | QIODevice::Text);
}

void ReSTCheckHighLighter::finishReSTCheckHighlight()
{
    QTextDocument *doc = mDocument ? mDocument->document() : nullptr;
    if (!doc || mStartRevision != doc->revision()) {
        mBusy = false;
        return;
    }

    Offenses offenses = processReSTCheckOutput();
    const Utils::FilePath filePath = mDocument->filePath();
    for (Diagnostic &diag : mDiagnostics[filePath]) {
        diag.textMark = std::make_shared<TextMark>(filePath, diag.line, diag.severity, diag.message);
        mDocument->addMark(diag.textMark.get());
    }
    ReSTCheckFucture ReSTCheckFucture(offenses);
    TextEditor::SemanticHighlighter::incrementalApplyExtraAdditionalFormats(
        mDocument->syntaxHighlighter(), ReSTCheckFucture.future(), 0, offenses.count(), mExtraFormats);
    TextEditor::SemanticHighlighter::clearExtraAdditionalFormatsUntilEnd(mDocument->syntaxHighlighter(),
                                                                         ReSTCheckFucture.future());

    mBusy = false;

#if 0
    qCDebug(log) << "rstcheck in" << mTimer.elapsed() << "ms," << offenses.count()
                 << "offenses found.";
#endif
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

Offenses ReSTCheckHighLighter::processReSTCheckOutput()
{
    Offenses result;
    Diagnostics &diag = mDiagnostics[mDocument->filePath()] = Diagnostics();

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

qtcreator::plugin::sphinx::Range ReSTCheckHighLighter::lineColumnLengthToRange(int line,
                                                                               int column,
                                                                               int length)
{
    const QTextBlock block = mDocument->document()->findBlockByLineNumber(line - 1);
    const int pos = block.position() + column;
    return Range(line, pos, length);
}

} // namespace qtcreator::plugin::sphinx
