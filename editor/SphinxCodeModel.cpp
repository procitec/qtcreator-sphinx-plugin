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
                Directive directive;
                DirectiveOption directiveOption;
                Role role;
                while (xml.readNext()) {
                    if (xml.atEnd()) {
                        break;
                    } else if (xml.name() == "directive" && xml.isStartElement()) {
                        const QXmlStreamAttributes &atts = xml.attributes();
                        assert(atts.hasAttribute("name"));
                        const QString &id = atts.value("name").toString();

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
                        qWarning() << fileName << " found directive " << id << xml.lineNumber()
                                   << xml.columnNumber();
                    } else if (xml.name() == "directive" && xml.isEndElement()) {
                        mData.mDirectives.append(directive);
                        directive = Directive();
                    } else if (xml.name() == "option" && xml.isStartElement()
                               && !directive.mName.isEmpty()) {
                        const QXmlStreamAttributes &atts = xml.attributes();
                        const QString &name = atts.value("name").toString();
                        const QString &type = atts.value("type").toString();
                        directiveOption.mName = name;
                        directiveOption.mTypes = type;
                        qWarning() << fileName << " found options " << name << type
                                   << xml.lineNumber() << xml.columnNumber();
                    } else if (xml.name() == "option" && xml.isEndElement()) {
                        directive.mOptions.append(directiveOption);
                        directiveOption = DirectiveOption();
                    } else if (xml.name() == "description" && xml.isStartElement()
                               && !directiveOption.mName.isEmpty()) {
                        auto desc = xml.readElementText();
                        qWarning() << fileName << " found directive option description " << desc
                                   << xml.lineNumber() << xml.columnNumber();
                        directiveOption.mDescription = desc;
                    } else if (xml.name() == "description" && xml.isStartElement()
                               && (directiveOption.mName.isEmpty() && !directive.mName.isEmpty())) {
                        auto desc = xml.readElementText();
                        qWarning() << fileName << " found directive description " << desc
                                   << xml.lineNumber() << xml.columnNumber();
                        directive.mDescription = desc;
                    } else if (xml.name() == "content" && xml.isStartElement()
                               && !directive.mName.isEmpty()) {
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

                        qWarning() << fileName << " found directive content" << content
                                   << xml.lineNumber() << xml.columnNumber();
                        directive.mContent = content;
                    } else if (xml.name() == "role" && xml.isStartElement()) {
                        const QXmlStreamAttributes &atts = xml.attributes();
                        assert(atts.hasAttribute("name"));
                        const QString &id = atts.value("name").toString();

                        role.mName = id;
                        role.mGroupId = snippetId;

                        qWarning() << fileName << " found role " << id << xml.lineNumber()
                                   << xml.columnNumber();
                    } else if (xml.name() == "role" && xml.isEndElement()) {
                        mData.mRoles.append(role);
                        role = Role();
                    } else if (xml.name() == "description" && xml.isStartElement()
                               && !role.mName.isEmpty()) {
                        auto desc = xml.readElementText();
                        qWarning() << fileName << " found role description " << desc
                                   << xml.lineNumber() << xml.columnNumber();
                        role.mDescription = desc;
                    }
                }
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
                QString content = directive.mContent.isEmpty() ? QStringLiteral("\n    $$\n")
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
        if (!role.mGroupId.isEmpty()) {
            assert(!role.mName.isEmpty());
            TextEditor::Snippet snippet(
                QString("%1.%2").arg(Constants::SnippetGroupId).arg(role.mGroupId),
                QString("%1_%2").arg(role.mGroupId.toLower()).arg(role.mName.toLower()));
            snippet.setTrigger(role.mName);
            snippet.setComplement("");
            snippet.setIsRemoved(false);
            snippet.setIsModified(false);

            snippet.setContent(QString("%1:`%2`").arg(role.mName).arg("$$"));
            snippets += snippet;
        }
    }

    return snippets;
}

} // namespace qtcreator::plugin::sphinx
