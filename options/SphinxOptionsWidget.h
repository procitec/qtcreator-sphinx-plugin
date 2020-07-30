#pragma once

#include "SphinxSettings.h"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

namespace qtcreator::plugin::sphinx {
class OptionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OptionsWidget(const Settings *settings);

    Settings settings() const;

private:
    QCheckBox mUseCustomHighlighter;
    QCheckBox mUseReSTCheckHighlighter;
    QLineEdit mReSTCheckFilePath;
};
} // namespace qtcreator::plugin::sphinx
