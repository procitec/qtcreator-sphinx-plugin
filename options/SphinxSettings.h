#pragma once

#include <QtCore/QString>

namespace qtc::plugin::sphinx {

namespace SettingsIds {
const QString IndentSize = "IndentSize";
const QString CustomHighlighter = "CustomHighlighter";
const QString PythonFilePath = "PythonFilePath";
const QString ReSTCheckHighlighter = "ReSTCheckHighlighter";
const QString RST2HTML = "rst2html";
}; // namespace SettingsIds

class Settings
{
public:
    Settings();

    void load();
    void save();

    int indentSize() const { return mIndentSize; }
    bool useCustomHighlighter() const { return mUseCustomHighlighter; }
    void setUseCustomHighlighter(bool use) { mUseCustomHighlighter = use; }

    QString pythonFilePath() const { return mPythonFilePath; }
    void setPythonFilePath(const QString &filePath) { mPythonFilePath = filePath; }

    bool useReSTCheckHighlighter() const { return mUseReSTCheckHighlighter; }
    void setUseReSTCheckHighlighter(bool use) { mUseReSTCheckHighlighter = use; }

    bool useRST2HTML() const { return mUseRST2HTML; }
    void setUseRST2HTML(bool use) { mUseRST2HTML = use; }

    bool operator==(const Settings &other) const;
    bool operator!=(const Settings &other) const;

private:
    bool mUseCustomHighlighter = true;
    bool mUseReSTCheckHighlighter = false;
    bool mUseRST2HTML = false;

    QString mPythonFilePath;
    int mIndentSize = 4;
};

inline bool Settings::operator==(const Settings &rhs) const
{
    return (mUseCustomHighlighter == rhs.mUseCustomHighlighter) && (mIndentSize == rhs.mIndentSize)
           && (mPythonFilePath == rhs.mPythonFilePath)
           && (mUseReSTCheckHighlighter == rhs.mUseReSTCheckHighlighter)
           && (mUseRST2HTML == rhs.mUseRST2HTML);
}

inline bool Settings::operator!=(const Settings &other) const
{
    return !(*this == other);
}
} // namespace qtc::plugin::sphinx
