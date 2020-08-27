/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include <projectexplorer/project.h>

#include <utils/fileutils.h>

namespace qtc::plugin::sphinx {
namespace Internal {

class LinkedPreview
{
public:
    LinkedPreview(const Utils::FilePath &editorPath, const QString &url)
        : filePath(editorPath)
        , previewUrl(url)
    {}

    Utils::FilePath filePath; // Relative for files in project, absolute otherwise.
    QString previewUrl;
};

inline bool operator==(const LinkedPreview &lpv1, const LinkedPreview &lpv2)
{
    return lpv1.filePath == lpv2.filePath && lpv1.previewUrl == lpv2.previewUrl;
}

using LinkedPreviewContainer = QMap<Utils::FilePath, QString>;

class ProjectSettings : public QObject
{
    Q_OBJECT

public:
    ProjectSettings(ProjectExplorer::Project *project);
    ~ProjectSettings() override;

    const LinkedPreviewContainer &linkedPreviews() const { return mLinkedPreviews; }
    void addLinkedPreview(const LinkedPreview &linkedPreview);
    void removeLinkedPreview(const LinkedPreview &linkedPreview);
    void removeAllLinkedPreviews();

    using ProjectSettingsPtr = QSharedPointer<ProjectSettings>;
    static ProjectSettingsPtr getSettings(ProjectExplorer::Project *project);

signals:
    void linkedPreviewsChanged();

private:
    void load();
    void store();

    ProjectExplorer::Project *m_project;
    LinkedPreviewContainer mLinkedPreviews;
};

} // namespace Internal
} // namespace qtc::plugin::sphinx

Q_DECLARE_METATYPE(QSharedPointer<qtc::plugin::sphinx::Internal::ProjectSettings>)
