#include "SphinxOutputParser.h"
#include "../qtcreator-sphinx-pluginconstants.h"
#include <projectexplorer/customparser.h>
#include <projectexplorer/projectexplorer.h>

namespace qtc::plugin::sphinx {
CustomParser::CustomParser()
{
    mIssueWithoutLineNumber.displayName = tr("Sphinx Issue w/o line");
    mIssueWithoutLineNumber.id = (QString(Constants::CustomParserId) + QString(".IssueWithoutLine"))
                                     .toLatin1()
                                     .constData();
    mIssueWithoutLineNumber.warning.setPattern(R"-(([\S]+):\s+WARNING:\s+(.*))-");

    mIssueWithLineNumber.displayName = tr("Sphinx Issue w line");
    mIssueWithLineNumber.id = (QString(Constants::CustomParserId) + QString(".IssueWithLine"))
                                  .toLatin1()
                                  .constData();
    mIssueWithLineNumber.warning.setPattern(R"-(([\S]+):(\\d+):\s+WARNING:\s+(.*))-");

    //    QTest::newRow( "warning unkown directive" ) << "
    //                                              << static_cast<int>( SphinxParser::Info::WARNING ) << "~tmp/sphinx-test/index.rst" << 13
    //                                              << "Unknown directive type \"uml\". .. uml::";

    mIssueWithoutLineNumber.warning.setFileNameCap(1);
    mIssueWithoutLineNumber.warning.setLineNumberCap(100);
    mIssueWithoutLineNumber.warning.setMessageCap(2);
    mIssueWithoutLineNumber.warning.setChannel(
        ProjectExplorer::CustomParserExpression::CustomParserChannel::ParseBothChannels);
    mIssueWithLineNumber.warning.setExample(
        "~/tmp/sphinx-test/free.rst: WARNING: document isn't included in any toctree\n");
    mIssueWithoutLineNumber.error = mIssueWithoutLineNumber.warning;
    mIssueWithoutLineNumber.error.setPattern(R"-(([\S]+):\s+ERROR:\s+(.*))-");

    mIssueWithLineNumber.warning.setFileNameCap(1);
    mIssueWithLineNumber.warning.setLineNumberCap(2);
    mIssueWithLineNumber.warning.setMessageCap(3);
    mIssueWithLineNumber.warning.setChannel(
        ProjectExplorer::CustomParserExpression::CustomParserChannel::ParseBothChannels);
    mIssueWithLineNumber.warning.setExample(
        "~tmp/sphinx-test/index.rst:13: WARNING: Unknown directive type \"uml\". .. uml::\n");
    mIssueWithLineNumber.error = mIssueWithoutLineNumber.warning;
    mIssueWithLineNumber.error.setPattern(R"-(([\S]+):(\\d+):\s+ERROR:\s+(.*))-");

    QList<ProjectExplorer::CustomParserSettings>
        settings = ProjectExplorer::ProjectExplorerPlugin::customParsers();
    bool foundIssueWithoutLineNumber = false;
    bool foundIssueWithLineNumber = false;

    for (auto setting : settings) {
        if (setting.id == mIssueWithoutLineNumber.id) {
            foundIssueWithoutLineNumber = true;
        }
        if (setting.id == mIssueWithLineNumber.id) {
            foundIssueWithLineNumber = true;
        }
    }
    if (!foundIssueWithoutLineNumber) {
        ProjectExplorer::ProjectExplorerPlugin::addCustomParser(mIssueWithoutLineNumber);
    }
    if (!foundIssueWithLineNumber) {
        ProjectExplorer::ProjectExplorerPlugin::addCustomParser(mIssueWithLineNumber);
    }
}
} // namespace qtc::plugin::sphinx
