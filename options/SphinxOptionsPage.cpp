#include "SphinxOptionsPage.h"
#include "../qtcreator-sphinx-pluginconstants.h"
#include "SphinxOptionsWidget.h"

namespace qtcreator::plugin::sphinx {
OptionsPage::OptionsPage(Settings *settings, QObject *parent)
    : IOptionsPage(parent)
    , mSettings(settings)
{
    setId(Constants::SettingsGeneralId); // page ID
    setDisplayName(tr("General"));       // page label in tabs
    setCategory("Sphinx");               // category ID
    setDisplayCategory("Sphinx");        // category label in options - set once
    auto prefix = QIcon::themeName().contains("dark", Qt::CaseInsensitive) ? QStringLiteral("/dark")
                                                                           : QStringLiteral("");
    setCategoryIcon(Utils::Icon((QString(":%1/sphinx.png").arg(prefix)))); // category icon - set once
}

QWidget *OptionsPage::widget()
{
    if (nullptr == mWidget)
        mWidget = new OptionsWidget(mSettings);

    return mWidget;
}

void OptionsPage::apply()
{
    const Settings newSettings = mWidget->settings();

    if (newSettings != *mSettings) {
        *mSettings = newSettings;
        mSettings->save();
    }
}

void OptionsPage::finish()
{
    delete mWidget;
    mWidget = nullptr;
}

} // namespace qtcreator::plugin::sphinx
