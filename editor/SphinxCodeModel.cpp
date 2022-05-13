#include "SphinxCodeModel.h"
#include "../qtcreator-sphinx-pluginconstants.h"

#include <coreplugin/icore.h>

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtCore/QXmlStreamReader>
//#include <QtXmlPatterns/QXmlSchemaValidator>

Q_LOGGING_CATEGORY(log_cm, "qtc.sphinx.codemodel");

namespace qtc::plugin::sphinx {

static CodeModel *theInstance = nullptr;

CodeModel::CodeModel()
{
    theInstance = this;
    qCDebug(log_cm) << "start reading code model";
    mModelDir.setPath(Core::ICore::resourcePath().toString() + "/sphinx/model");
    if (mModelDir.exists() && mModelDir.isReadable()) {
        mModelDir.setNameFilters(QStringList() << "*.xml");
        auto modelFiles = mModelDir.entryInfoList(QDir::Files | QDir::Readable);
        qCDebug(log_cm) << "found model files: " << modelFiles.length();

        //        QFileInfo file(mModelDir.absolutePath() + "/model.xsd");
        //        QXmlSchema schema;
        //        bool loaded = schema.load(QUrl::fromLocalFile(file.absoluteFilePath()));
        //        if (loaded) {
        //            qCDebug(log_cm) << "sucessfully loaded schema" << schema.documentUri();
        //        }

        //        assert(schema.isValid());
        //        if (schema.isValid()) {
        for (const auto &model : modelFiles) {
            //            if (verifyXML(model.absoluteFilePath(), schema)) {
            readXML(model.absoluteFilePath(), model.baseName());
            //            }
            //        }
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

//bool CodeModel::verifyXML(const QString &fileName, const QXmlSchema &schema) const
//{
//    bool ok = false;

//    if (schema.isValid()) {
//        QFileInfo file(fileName);
//        if (file.exists()) {
//            QXmlSchemaValidator validator(schema);
//            assert(validator.validate(QUrl::fromLocalFile(file.absoluteFilePath())));
//            if (validator.validate(QUrl::fromLocalFile(file.absoluteFilePath()))) {
//                ok = true;
//                qCDebug(log_cm) << "sucessfully loaded file " << file.absoluteFilePath();
//            }
//        }
//    }

//    return ok;
//}

void CodeModel::readXML(const QString &fileName, const QString &snippetId)
{
    QFile file(fileName);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(&file);
        if (xml.readNextStartElement()) {
            if (xml.name() == QLatin1String("model")) {
                Directive directive;
                DirectiveOption directiveOption;
                Role role;
                while (xml.readNext()) {
                    if (xml.atEnd()) {
                        break;
                    } else if (xml.name() == QLatin1String("directive") && xml.isStartElement()) {
                        const QXmlStreamAttributes &atts = xml.attributes();
                        assert(atts.hasAttribute("name"));
                        const QString &id = atts.value("name").toString();

                        directive.mName = id;
                        directive.mGroupId = snippetId;

                        if (atts.hasAttribute("args")) {
                            directive.mArgs = atts.value("args").toString();
                        }
                        qCDebug(log_cm) << fileName << " found directive " << id << xml.lineNumber()
                                        << xml.columnNumber();
                    } else if (xml.name() == QLatin1String("directive") && xml.isEndElement()) {
                        mData.mDirectives.append(directive);
                        directive = Directive();
                    } else if (xml.name() == QLatin1String("option") && xml.isStartElement()
                               && !directive.mName.isEmpty()) {
                        const QXmlStreamAttributes &atts = xml.attributes();
                        const QString &name = atts.value("name").toString();
                        const QString &type = atts.value("type").toString();
                        directiveOption.mName = name;
                        directiveOption.mTypes = type;
                        qCDebug(log_cm) << fileName << " found options " << name << type
                                        << xml.lineNumber() << xml.columnNumber();
                    } else if (xml.name() == QLatin1String("option") && xml.isEndElement()) {
                        directive.mOptions.append(directiveOption);
                        directiveOption = DirectiveOption();
                    } else if (xml.name() == QLatin1String("description") && xml.isStartElement()
                               && !directiveOption.mName.isEmpty()) {
                        auto desc = xml.readElementText();
                        qCDebug(log_cm) << fileName << " found directive option description "
                                        << desc << xml.lineNumber() << xml.columnNumber();
                        directiveOption.mDescription = desc;
                    } else if (xml.name() == QLatin1String("description") && xml.isStartElement()
                               && (directiveOption.mName.isEmpty() && !directive.mName.isEmpty())) {
                        auto desc = xml.readElementText();
                        qCDebug(log_cm) << fileName << " found directive description " << desc
                                        << xml.lineNumber() << xml.columnNumber();
                        directive.mDescription = desc;
                    } else if (xml.name() == QLatin1String("content") && xml.isStartElement()
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

                        qCDebug(log_cm) << fileName << " found directive content" << content
                                        << xml.lineNumber() << xml.columnNumber();
                        directive.mContent = content;
                    } else if (xml.name() == QLatin1String("explanation") && xml.isStartElement()
                               && !directive.mName.isEmpty()) {
                        QString explanation;
                        while (!xml.atEnd()) {
                            xml.readNext();
                            if (xml.isCDATA()) {
                                explanation += xml.text();
                                break;
                            } else if (xml.isEndElement()) {
                                break;
                            }
                        }

                        qCDebug(log_cm) << fileName << " found directive explanation" << explanation
                                        << xml.lineNumber() << xml.columnNumber();
                        directive.mContent = explanation;
                    } else if (xml.name() == QLatin1String("role") && xml.isStartElement()) {
                        const QXmlStreamAttributes &atts = xml.attributes();
                        assert(atts.hasAttribute("name"));
                        const QString &id = atts.value("name").toString();

                        role.mName = id;
                        role.mGroupId = snippetId;

                        qCDebug(log_cm) << fileName << " found role " << id << xml.lineNumber()
                                        << xml.columnNumber();
                    } else if (xml.name() == QLatin1String("role") && xml.isEndElement()) {
                        mData.mRoles.append(role);
                        role = Role();
                    } else if (xml.name() == QLatin1String("description") && xml.isStartElement()
                               && !role.mName.isEmpty()) {
                        auto desc = xml.readElementText();
                        qCDebug(log_cm) << fileName << " found role description " << desc
                                        << xml.lineNumber() << xml.columnNumber();
                        role.mDescription = desc;
                    }
                }
            }
        }
        if (xml.hasError())
            qCWarning(log_cm) << fileName << xml.errorString() << xml.lineNumber()
                              << xml.columnNumber();
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

            if (!directive.mContent.isEmpty()) {
                snippet.setContent(
                    QString("%1::%2\n%3\n").arg(directive.mName).arg(option).arg(directive.mContent));
            } else {
                snippet.setContent(QString("%1::%2\n\n").arg(directive.mName).arg(option));
            }
            snippets += snippet;
        }
    }

    return snippets;
}

QList<TextEditor::Snippet> CodeModel::collectDirectiveOptions(const QString &directiveName)
{
    QList<TextEditor::Snippet> snippets;

    for (const auto &directive : mData.mDirectives) {
        if (!directive.mGroupId.isEmpty()) {
            assert(!directive.mName.isEmpty());
            if (directive.mName == directiveName) {
                for (const auto &option : directive.mOptions) {
                    TextEditor::Snippet snippet(
                        QString("%1.%2").arg(Constants::SnippetGroupId).arg(directive.mGroupId),
                        QString("%1_%2_%3")
                            .arg(directive.mGroupId.toLower())
                            .arg(directive.mName.toLower())
                            .arg(option.mName.toLower()));
                    snippet.setTrigger(option.mName);
                    snippet.setComplement("");
                    snippet.setIsRemoved(false);
                    snippet.setIsModified(false);

                    QString typeInfo = option.mTypes.isEmpty() ? QString()
                                                               : QString(" $%1$").arg(option.mTypes);

                    snippet.setContent(QString("%1:%2\n").arg(option.mName).arg(typeInfo));
                    snippets += snippet;
                }
            }
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

} // namespace qtc::plugin::sphinx
