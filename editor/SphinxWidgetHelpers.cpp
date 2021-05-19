#include "SphinxWidgetHelpers.h"

#include <QtWidgets/QScrollBar>
namespace qtc::plugin::sphinx {
namespace SphinxWidgetHelpers {
std::tuple<int, int> scrollBarPos(const QAbstractScrollArea *w)
{
    std::tuple<int, int> pos = {-1, -1};
    if (w) {
        // preserve scroll position
        auto vPos = -1;
        auto hPos = -1;
        auto vBar = w->verticalScrollBar();
        auto hBar = w->horizontalScrollBar();
        if (vBar) {
            vPos = vBar->sliderPosition();
        }
        if (hBar) {
            hPos = hBar->sliderPosition();
        }
        pos = {vPos, hPos};
    }
    return pos;
}

void setScrollBarPos(const QAbstractScrollArea *w, const std::tuple<int, int> &pos)
{
    if (w) {
        auto vPos = std::get<0>(pos);
        auto hPos = std::get<1>(pos);
        auto vBar = w->verticalScrollBar();
        auto hBar = w->horizontalScrollBar();

        if (0 < vPos && vBar) {
            vBar->setSliderPosition(vPos);
        }
        if (0 < hPos && hBar) {
            hBar->setSliderPosition(hPos);
        }
    }
}

} // namespace SphinxWidgetHelpers
} // namespace qtc::plugin::sphinx
