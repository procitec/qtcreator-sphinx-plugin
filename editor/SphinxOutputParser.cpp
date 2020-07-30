#include "SphinxOutputParser.h"
#include "Constants.h"
#include <projectexplorer/customparser.h>
#include <projectexplorer/projectexplorer.h>

namespace qtcreator::plugin::sphinx {
CustomParser::CustomParser()
{
    mSettings.displayName = tr("Parser for Sphinx toolchain");
    mSettings.id = Constants::CustomParserId;
    //mSettings.error.setPattern("aaa");
    //    customParserSettings.error.setPattern(
    //        data.value("ProjectExplorer.CustomToolChain.ErrorPattern").toString());
    //    customParserSettings.error.setFileNameCap(
    //        data.value("ProjectExplorer.CustomToolChain.ErrorLineNumberCap").toInt());
    //    customParserSettings.error.setLineNumberCap(
    //        data.value("ProjectExplorer.CustomToolChain.ErrorFileNameCap").toInt());
    //    customParserSettings.error.setMessageCap(
    //        data.value("ProjectExplorer.CustomToolChain.ErrorMessageCap").toInt());
    //    customParserSettings.error.setChannel(
    //        static_cast<Internal::CustomParserExpression::CustomParserChannel>(
    //            data.value("ProjectExplorer.CustomToolChain.ErrorChannel").toInt()));
    //    customParserSettings.error.setExample(
    //        data.value("ProjectExplorer.CustomToolChain.ErrorExample").toString());
    //    customParserSettings.warning.setPattern(
    //        data.value("ProjectExplorer.CustomToolChain.WarningPattern").toString());
    //    customParserSettings.warning.setFileNameCap(
    //        data.value("ProjectExplorer.CustomToolChain.WarningLineNumberCap").toInt());
    //    customParserSettings.warning.setLineNumberCap(
    //        data.value("ProjectExplorer.CustomToolChain.WarningFileNameCap").toInt());
    //    customParserSettings.warning.setMessageCap(
    //        data.value("ProjectExplorer.CustomToolChain.WarningMessageCap").toInt());
    //    customParserSettings.warning.setChannel(
    //        static_cast<Internal::CustomParserExpression::CustomParserChannel>(
    //            data.value("ProjectExplorer.CustomToolChain.WarningChannel").toInt()));
    //    customParserSettings.warning.setExample(
    //        data.value("ProjectExplorer.CustomToolChain.WarningExample").toString());
    //        setOutputParserId(customParserSettings.id);
    //        customParserSettings.displayName = tr("Parser for toolchain %1").arg(displayName());
    QList<ProjectExplorer::Internal::CustomParserSettings>
        settings = ProjectExplorer::ProjectExplorerPlugin::customParsers();
    settings << mSettings;
    ProjectExplorer::ProjectExplorerPlugin::setCustomParsers(settings);
}
} // namespace qtcreator::plugin::sphinx
