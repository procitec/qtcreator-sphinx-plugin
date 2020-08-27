#pragma once

#include <coreplugin/dialogs/ioptionspage.h>

#include "SphinxSettings.h"

namespace qtc::plugin::sphinx {
class OptionsWidget;
class OptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    OptionsPage(Settings *settings, QObject *parent = nullptr);

    QWidget *widget() override;

    void apply() override;
    void finish() override;

private:
    Settings *mSettings = nullptr;

    OptionsWidget *mWidget = nullptr;
};
} // namespace qtc::plugin::sphinx
