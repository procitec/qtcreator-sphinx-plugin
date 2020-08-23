#pragma once

#include <QtCore/QDir>

#include "texteditor/snippets/snippet.h"

class QXmlSchema;

namespace qtcreator::plugin::sphinx {

class CodeModel
{
public:
    CodeModel();
    ~CodeModel();

    static CodeModel *instance();
    QList<TextEditor::Snippet> collectDirectives();
    QList<TextEditor::Snippet> collectRoles();

private:
    void readXML(const QString &fileName, const QString &snippetId);

    bool verifyXML(const QString &fileName, const QXmlSchema &schema) const;

    class DirectiveOption
    {
    public:
        QString mName = {};
        QString mTypes = {};
        QString mDescription = {};
    };

    class Directive
    {
    public:
        QString mName = {};
        QString mArgs = {};
        bool mHasContent = false;
        ;
        QString mDescription = {};
        QString mContent = {};
        QString mGroupId = {};
        QList<DirectiveOption> mOptions;
    };

    class Role
    {
    public:
        QString mName;
        QString mDescription;
    };

    class Data
    {
    public:
        QList<Directive> mDirectives;
        QList<Role> mRoles;
    };

    QDir mModelDir;
    Data mData;
};
} // namespace qtcreator::plugin::sphinx
