#pragma once

#include "qtcreator-sphinx-plugin_global.h"
#include <texteditor/textmark.h>
#include <tuple>
#include <QtCore/QString>
#include <QtWidgets/QAbstractScrollArea>

namespace qtc::plugin::sphinx {

namespace WidgetHelpers {

std::tuple<int, int> scrollBarPos(const QAbstractScrollArea *);
void setScrollBarPos(const QAbstractScrollArea *, const std::tuple<int, int> &pos);
QString log_directory();
}; // namespace WidgetHelpers

namespace Marks {
class Range
{
public:
    int line = 0;
    int pos = 0;
    int length = 0;

    Range() = default;
    Range(int pos, int length);
    Range(int line, int pos, int length);

    // Not really equal, since the length attribute is ignored.
    bool operator==(const Range &other) const;
    bool operator<(const Range &other) const;
};
class TextMark : public TextEditor::TextMark
{
public:
    static Utils::Theme::Color colorForSeverity(int severity);
    TextMark(const Utils::FilePath &fileName, int line, int severity, const QString &text);
};

struct Diagnostic
{
    int line;
    int severity;
    QString message;
    std::shared_ptr<TextMark> textMark;
};

using Diagnostics = QHash<int, Diagnostic>;

} // namespace Marks
} // namespace qtc::plugin::sphinx
