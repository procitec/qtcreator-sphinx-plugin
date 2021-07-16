#include "SphinxWidgetHelpers.h"
#include "../qtcreator-sphinx-pluginconstants.h"
#include "utils/temporarydirectory.h"

#include <QtWidgets/QScrollBar>

namespace qtc::plugin::sphinx {
namespace WidgetHelpers {

std::unique_ptr<Utils::TemporaryDirectory> mTempDir;

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

QString log_directory()
{
    if (!mTempDir) {
        auto relResource = QString(qtc::plugin::sphinx::Constants::SnippetGroupId).toLower();
        mTempDir = std::make_unique<Utils::TemporaryDirectory>(relResource);
        mTempDir->setAutoRemove(false);
    }

    return mTempDir->path().toString();
}

} // namespace WidgetHelpers
namespace Marks {

Utils::Theme::Color TextMark::colorForSeverity(int severity)
{
    switch (severity) {
    case 1:
        return Utils::Theme::TextColorNormal;
    case 2:
        return Utils::Theme::CodeModel_Warning_TextMarkColor;
    case 3:
        return Utils::Theme::CodeModel_Error_TextMarkColor;
    case 4:
        return Utils::Theme::CodeModel_Error_TextMarkColor;
    default:
        return Utils::Theme::TextColorNormal;
    }
}

TextMark::TextMark(const Utils::FilePath &fileName, int line, int severity, const QString &text)
    : TextEditor::TextMark(fileName, line, "rstcheck")
{
    setColor(colorForSeverity(severity));
    setPriority(TextEditor::TextMark::Priority(severity));
    setLineAnnotation(text);
    QString tooltip = text;
    setToolTip(tooltip);
}

Range::Range(int pos, int length)
    : pos(pos)
    , length(length)
{}

Range::Range(int line, int pos, int length)
    : line(line)
    , pos(pos)
    , length(length)
{}

// Not really equal, since the length attribute is ignored.
bool Range::operator==(const Range &other) const
{
    const int value = other.pos;
    return value >= pos && value < (pos + length);
}

bool Range::operator<(const Range &other) const
{
    const int value = other.pos;
    return pos < value && (pos + length) < value;
}
}; // namespace Marks

} // namespace qtc::plugin::sphinx
