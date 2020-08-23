#pragma once

#include <QObject>

#include <projectexplorer/customparser.h>

namespace qtcreator::plugin::sphinx {
class CustomParser : public QObject
{
    Q_OBJECT
public:
    CustomParser();

private:
    ProjectExplorer::CustomParserSettings mIssueWithLineNumber;
    ProjectExplorer::CustomParserSettings mIssueWithoutLineNumber;
};
} // namespace qtcreator::plugin::sphinx
