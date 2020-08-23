#include "SphinxCodeModel.h"
#include "../qtcreator-sphinx-pluginconstants.h"

#include <coreplugin/icore.h>

#include <QtCore/QDebug>
#include <QtCore/QXmlStreamReader>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>

namespace qtcreator::plugin::sphinx {

static CodeModel *theInstance = nullptr;

CodeModel::CodeModel()
{
    theInstance = this;
    qWarning() << "start reading code model";
    mModelDir.setPath(Core::ICore::resourcePath() + "/sphinx/model");
    if (mModelDir.exists() && mModelDir.isReadable()) {
        mModelDir.setNameFilters(QStringList() << "*.xml");
        auto modelFiles = mModelDir.entryInfoList(QDir::Files | QDir::Readable);
        qWarning() << "found model files: " << modelFiles.length();

        QFile file(mModelDir.absolutePath() + "/model.xsd");
        file.open(QIODevice::ReadOnly);
        if (file.isOpen()) {
            QXmlSchema schema;
            bool loaded = schema.load(&file, QUrl::fromLocalFile(file.fileName()));
            if (loaded) {
                qDebug() << "sucessfully loaded schema" << schema.documentUri();
            }
            file.close();

            for (const auto &file : modelFiles) {
                if (verifyXML(file.absoluteFilePath(), schema)) {
                    readXML(file.absoluteFilePath(), file.baseName());
                }
            }
        }
    }
}

CodeModel::~CodeModel()
{
    theInstance = nullptr;
}

CodeModel *CodeModel::instance()
{
    assert(theInstance);
    return theInstance;
}

bool CodeModel::verifyXML(const QString &fileName, const QXmlSchema &schema) const
{
    bool ok = false;

    if (schema.isValid()) {
        QFile file(fileName);
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            QXmlSchemaValidator validator(schema);
            if (validator.validate(&file, QUrl::fromLocalFile(file.fileName()))) {
                ok = true;
            }
            file.close();
        }
    }

    return ok;
}

void CodeModel::readXML(const QString &fileName, const QString &snippetId)
{
    QFile file(fileName);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(&file);
        if (xml.readNextStartElement()) {
            if (xml.name() == "model") {
                while (xml.readNextStartElement()) {
                    if (xml.name() == "directives") {
                        while (xml.readNextStartElement()) {
                            if (xml.name() == "directive") {
                                const QXmlStreamAttributes &atts = xml.attributes();
                                assert(atts.hasAttribute("name"));
                                const QString &id = atts.value("name").toString();

                                Directive directive;
                                directive.mName = id;
                                directive.mGroupId = snippetId;

                                if (atts.hasAttribute("args")) {
                                    directive.mArgs = atts.value("args").toString();
                                }
                                if (atts.hasAttribute("has_content")) {
                                    directive.mHasContent = atts.value("has_content").toString().toLower()
                                                                    == "true"
                                                                ? true
                                                                : false;
                                }
                                qWarning() << fileName << " found directive " << id
                                           << xml.lineNumber() << xml.columnNumber();

                                //                                        const QString &groupId = atts.value(kGroup).toString();
                                //                                        const QString &trigger = atts.value(kTrigger).toString();
                                //                                        if (!groupId.isEmpty()
                                //                                            && (snippetId.isEmpty() || snippetId == id)) {
                                //                                            TextEditor::Snippet snippet(groupId, id);
                                //                                            snippet.setTrigger(trigger);
                                //                                            snippet.setComplement(QCoreApplication::translate(
                                //                                                "TextEditor::Internal::Snippets",
                                //                                                atts.value(kComplement).toString().toLatin1(),
                                //                                                atts.value(kId).toString().toLatin1()));
                                //                                            snippet.setIsRemoved(
                                //                                                toBool(atts.value(kRemoved).toString()));
                                //                                            snippet.setIsModified(
                                //                                                toBool(atts.value(kModified).toString()));

                                //                                            QString content;
                                //                                            while (!xml.atEnd()) {
                                //                                                xml.readNext();
                                //                                                if (xml.isCharacters()) {
                                //                                                    content += xml.text();
                                //                                                } else if (xml.isEndElement()) {
                                //                                                    snippet.setContent(content);
                                //                                                    snippets.append(snippet);
                                //                                                    break;
                                //                                                }
                                //                                            }

                                //                                        if (!snippetId.isEmpty())
                                //                                            break;
                                //                                    }
                                while (xml.readNextStartElement()) {
                                    if (xml.name() == "options") {
                                        while (xml.readNextStartElement()) {
                                            if (xml.name() == "option") {
                                                const QXmlStreamAttributes &atts = xml.attributes();
                                                const QString &name = atts.value("name").toString();
                                                const QString &type = atts.value("type").toString();
                                                DirectiveOption option;
                                                option.mName = name;
                                                option.mTypes = type;
                                                qWarning()
                                                    << fileName << " found options " << name << type
                                                    << xml.lineNumber() << xml.columnNumber();

                                                while (xml.readNextStartElement()) {
                                                    if (xml.name() == "description") {
                                                        auto desc = xml.readElementText();
                                                        qWarning() << fileName << " found description "
                                                                   << desc << xml.lineNumber()
                                                                   << xml.columnNumber();
                                                        option.mDescription = desc;
                                                    } else {
                                                        xml.skipCurrentElement();
                                                    }
                                                }
                                                directive.mOptions.append(option);

                                            } else {
                                                xml.skipCurrentElement();
                                            }
                                        }
                                    } else if (xml.name() == "description") {
                                        auto desc = xml.readElementText();
                                        qWarning() << fileName << " found description " << desc
                                                   << xml.lineNumber() << xml.columnNumber();
                                        directive.mDescription = desc;
                                    } else if (xml.name() == "content") {
                                        QString content;
                                        while (!xml.atEnd()) {
                                            xml.readNext();
                                            if (xml.isCDATA()) {
                                                content += xml.text();
                                                break;
                                            } else if (xml.isEndElement()) {
                                                break;
                                            }
                                        }
                                        qWarning() << fileName << " found content" << content
                                                   << xml.lineNumber() << xml.columnNumber();
                                        directive.mContent = content;
                                    } else {
                                        xml.skipCurrentElement();
                                    }
                                }
                                mData.mDirectives.append(directive);
                            } else {
                                xml.skipCurrentElement();
                            }
                        }
                    } else if (xml.name() == "roles") {
                    } else {
                        xml.skipCurrentElement();
                    }
                }
            } else {
                xml.skipCurrentElement();
            }
        }
        if (xml.hasError())
            qWarning() << fileName << xml.errorString() << xml.lineNumber() << xml.columnNumber();
        file.close();
    }
}

QList<TextEditor::Snippet> CodeModel::collectDirectives()
{
    QList<TextEditor::Snippet> snippets;

    for (const auto &directive : mData.mDirectives) {
        //                                        const QString &groupId = atts.value(kGroup).toString();
        //                                        const QString &trigger = atts.value(kTrigger).toString();
        //                                        if (!groupId.isEmpty()
        //                                            && (snippetId.isEmpty() || snippetId == id)) {
        //                                            TextEditor::Snippet snippet(groupId, id);
        //                                            snippet.setTrigger(trigger);
        //                                            snippet.setComplement(QCoreApplication::translate(
        //                                                "TextEditor::Internal::Snippets",
        //                                                atts.value(kComplement).toString().toLatin1(),
        //                                                atts.value(kId).toString().toLatin1()));
        //                                            snippet.setIsRemoved(
        //                                                toBool(atts.value(kRemoved).toString()));
        //                                            snippet.setIsModified(
        //                                                toBool(atts.value(kModified).toString()));

        //                                            QString content;
        //                                                    snippet.setContent(content);
        //                                                    snippets.append(snippet);
        if (!directive.mGroupId.isEmpty()) {
            assert(!directive.mName.isEmpty());
            TextEditor::Snippet snippet(
                QString("%1.%2").arg(Constants::SnippetGroupId).arg(directive.mGroupId),
                QString("%1_%2").arg(directive.mGroupId.toLower()).arg(directive.mName.toLower()));
            snippet.setTrigger(directive.mName);
            snippet.setComplement("");
            snippet.setIsRemoved(false);
            snippet.setIsModified(false);

            QString option = directive.mArgs.isEmpty() ? QString()
                                                       : QString(" $%1$").arg(directive.mArgs);

            if (directive.mHasContent) {
                QString content = directive.mContent.isEmpty() ? QStringLiteral("\n    $content$\n")
                                                               : directive.mContent;
                snippet.setContent(
                    QString("%1::%2\n%3\n").arg(directive.mName).arg(option).arg(content));
            } else {
                snippet.setContent(QString("%1::%2\n\n").arg(directive.mName).arg(option));
            }
            snippets += snippet;
        }
    }

    return snippets;
}

QList<TextEditor::Snippet> CodeModel::collectRoles()
{
    QList<TextEditor::Snippet> snippets;

    for (const auto &role : mData.mRoles) {
    }

    return snippets;
}

} // namespace qtcreator::plugin::sphinx
