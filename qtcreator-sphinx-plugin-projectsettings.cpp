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

#include "qtcreator-sphinx-plugin-projectsettings.h"
#include <projectexplorer/session.h>

#include <utils/algorithm.h>
#include <utils/qtcassert.h>

namespace qtc::plugin::sphinx {
namespace Internal {

static const char SETTINGS_KEY_MAIN[] = "Sphinx";
static const char SETTINGS_PREFIX[] = "Sphinx.";
static const char SETTINGS_KEY_LINKED_PREVIEW[] = "Sphinx.LinkedPreviews";
static const char SETTINGS_KEY_LINKED_PREVIEW_FILEPATH[] = "Sphinx.LinkedPreview.FilePath";
static const char SETTINGS_KEY_LINKED_PREVIEW_URL[] = "Sphinx.LinkedPreview.Url";

ProjectSettings::ProjectSettings(ProjectExplorer::Project *project)
    : m_project(project)
{
    load();
    connect(project, &ProjectExplorer::Project::settingsLoaded, this, &ProjectSettings::load);
    connect(project, &ProjectExplorer::Project::aboutToSaveSettings, this, &ProjectSettings::store);
}

ProjectSettings::~ProjectSettings()
{
    store();
}

void ProjectSettings::addLinkedPreview(const LinkedPreview &linkedPreview)
{
    mLinkedPreviews[linkedPreview.filePath] = linkedPreview.previewUrl;
    emit linkedPreviewsChanged();
}

void ProjectSettings::removeLinkedPreview(const LinkedPreview &linkedPreview)
{
    if (mLinkedPreviews.contains(linkedPreview.filePath)) {
        mLinkedPreviews.remove(linkedPreview.filePath);
        emit linkedPreviewsChanged();
    }
}

void ProjectSettings::removeAllLinkedPreviews()
{
    mLinkedPreviews.clear();
    emit linkedPreviewsChanged();
}

void ProjectSettings::load()
{
    // Load map
    QVariantMap map = m_project->namedSettings(SETTINGS_KEY_MAIN).toMap();

    const QVariantMap linkedPreviews = map.value(SETTINGS_KEY_LINKED_PREVIEW).toMap();

    for (auto iter = linkedPreviews.begin(); iter != linkedPreviews.end(); ++iter) {
        const QString fp = iter.key();
        if (fp.isEmpty())
            continue;
        const QString url = iter.value().toString();
        if (url.isEmpty())
            continue;
        Utils::FilePath fullPath = Utils::FilePath::fromString(fp);
        if (fullPath.toFileInfo().isRelative())
            fullPath = m_project->projectDirectory().pathAppended(fp);
        if (!fullPath.exists())
            continue;

        if (url.isEmpty())
            continue;

        mLinkedPreviews[Utils::FilePath::fromString(fp)] = url;
    }
    emit linkedPreviewsChanged();
}

void ProjectSettings::store()
{
    QVariantMap map;

    QVariantMap linkedPreviewMap;
    for (auto iter = mLinkedPreviews.begin(); iter != mLinkedPreviews.end(); ++iter) {
        linkedPreviewMap.insert(iter.key().toString(), iter.value());
    }
    map.insert(SETTINGS_KEY_LINKED_PREVIEW, linkedPreviewMap);
    m_project->setNamedSettings(SETTINGS_KEY_MAIN, map);
}

ProjectSettings::ProjectSettingsPtr ProjectSettings::getSettings(ProjectExplorer::Project *project)
{
    const QString key = "ProjectSettings";
    QVariant v = project->extraData(key);
    if (v.isNull()) {
        v = QVariant::fromValue(ProjectSettingsPtr{new ProjectSettings(project)});
        project->setExtraData(key, v);
    }
    return v.value<QSharedPointer<ProjectSettings>>();
}

} // namespace Internal
} // namespace qtc::plugin::sphinx
