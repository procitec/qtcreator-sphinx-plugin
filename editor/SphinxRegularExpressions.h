#pragma once

#include <QtCore/QRegularExpression>

namespace SphinxRegularExpressions {

const QRegularExpression DirectiveRegEx("^\\.\\.\\s+[\\w\\-_]+::\\s*");
const QRegularExpression DirectiveCaptureRegEx("^\\.\\.\\s+([\\w\\-_]+)::\\s*");
const QRegularExpression DirectiveOptionRegEx("\\s+:[A-Za-z-]+:");
const QRegularExpression TitleRegEx("^[=|\\*|#|\\^|\\-]+");
const QRegularExpression BoldRegEx("[\\s+]\\*{2,}\\w+\\*{2,}|^\\*{2,}\\w+\\*{2,}");
const QRegularExpression ItalicRegEx("[\\s+]\\*{1,1}\\w+\\*{1,1}|^\\*{1,1}\\w+\\*{1,1}");
const QRegularExpression CodeRegEx("[\\s+]`{2,2}[^`]+`{2,2}");
const QRegularExpression RoleRegEx("\\s+:\\w+:");
const QRegularExpression RoleContentRegEx(":`[^`]+`");
}; // namespace SphinxRegularExpressions
