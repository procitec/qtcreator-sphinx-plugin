#pragma once

#include "qtcreator-sphinx-plugin_global.h"

#include <tuple>
#include <QtWidgets/QAbstractScrollArea>

namespace qtc::plugin::sphinx {

namespace SphinxWidgetHelpers {

std::tuple<int, int> scrollBarPos(const QAbstractScrollArea *);
void setScrollBarPos(const QAbstractScrollArea *, const std::tuple<int, int> &pos);

}; // namespace SphinxWidgetHelpers
} // namespace qtc::plugin::sphinx
