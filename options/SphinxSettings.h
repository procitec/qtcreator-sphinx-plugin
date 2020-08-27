#pragma once

#include <QtCore/QString>

namespace qtc::plugin::sphinx {

namespace SettingsIds {
const QString CustomHighlighter = "CustomHighlighter";
const QString ReSTCheckHighlighter = "ReSTCheckHighlighter";
const QString PythonFilePath = "PythonFilePath";
const QString IndentSize = "IndentSize";
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

    bool useReSTCheckHighlighter() const { return mUseReSTCheckHighlighter; }
    void setUseReSTCheckHighlighter(bool use) { mUseReSTCheckHighlighter = use; }

    QString pythonFilePath() const { return mPythonFilePath; }
    void setPythonFilePath(const QString &filePath) { mPythonFilePath = filePath; }

    bool operator==(const Settings &other) const;
    bool operator!=(const Settings &other) const;

private:
    bool mUseCustomHighlighter = true;
    bool mUseReSTCheckHighlighter = false;
    QString mPythonFilePath;
    int mIndentSize = 4;
};

inline bool Settings::operator==(const Settings &rhs) const
{
    return (mUseCustomHighlighter == rhs.mUseCustomHighlighter) && (mIndentSize == rhs.mIndentSize)
           && (mUseReSTCheckHighlighter == rhs.mUseReSTCheckHighlighter)
           && (mPythonFilePath == rhs.mPythonFilePath);
}

inline bool Settings::operator!=(const Settings &other) const
{
    return !(*this == other);
}
} // namespace qtc::plugin::sphinx
