#pragma once

#include <QObject>

#include <projectexplorer/customparser.h>

#ifdef WITH_OUTPUT_PARSER
namespace qtc::plugin::sphinx {
class CustomParser : public QObject
{
    Q_OBJECT
public:
    CustomParser();

private:
    ProjectExplorer::CustomParserSettings mIssueWithLineNumber;
    ProjectExplorer::CustomParserSettings mIssueWithoutLineNumber;
};
} // namespace qtc::plugin::sphinx
#endif
