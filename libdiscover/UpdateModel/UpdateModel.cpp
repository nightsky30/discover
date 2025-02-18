/***************************************************************************
 *   Copyright © 2011 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "UpdateModel.h"

// Qt includes
#include <QFont>
#include <QTimer>
#include "libdiscover_debug.h"

// KDE includes
#include <KFormat>
#include <KLocalizedString>

// Own includes
#include "UpdateItem.h"
#include <resources/AbstractResource.h>
#include <resources/ResourcesUpdatesModel.h>
#include <resources/ResourcesModel.h>

UpdateModel::UpdateModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_updateSizeTimer(new QTimer(this))
    , m_updates(nullptr)
{
    connect(ResourcesModel::global(), &ResourcesModel::fetchingChanged, this, &UpdateModel::activityChanged);
    connect(ResourcesModel::global(), &ResourcesModel::updatesCountChanged, this, &UpdateModel::activityChanged);
    connect(ResourcesModel::global(), &ResourcesModel::resourceDataChanged, this, &UpdateModel::resourceDataChanged);
    connect(this, &UpdateModel::toUpdateChanged, this, &UpdateModel::updateSizeChanged);

    m_updateSizeTimer->setInterval(100);
    m_updateSizeTimer->setSingleShot(true);
    connect(m_updateSizeTimer, &QTimer::timeout, this, &UpdateModel::updateSizeChanged);
}

UpdateModel::~UpdateModel()
{
    qDeleteAll(m_updateItems);
    m_updateItems.clear();
}

QHash<int,QByteArray> UpdateModel::roleNames() const
{
    return QAbstractItemModel::roleNames().unite({
        { Qt::CheckStateRole, "checked" },
        { ResourceProgressRole, "resourceProgress" },
        { ResourceStateRole, "resourceState" },
        { ResourceRole, "resource" },
        { SizeRole, "size" },
        { SectionRole, "section" },
        { ChangelogRole, "changelog" },
        { InstalledVersionRole, "installedVersion" },
        { AvailableVersionRole, "availableVersion" }
    } );
}

void UpdateModel::setBackend(ResourcesUpdatesModel* updates)
{
    if (m_updates) {
        disconnect(m_updates, nullptr, this, nullptr);
    }

    m_updates = updates;

    connect(m_updates, &ResourcesUpdatesModel::progressingChanged, this, &UpdateModel::activityChanged);
    connect(m_updates, &ResourcesUpdatesModel::resourceProgressed, this, &UpdateModel::resourceHasProgressed);

    activityChanged();
}

void UpdateModel::resourceHasProgressed(AbstractResource* res, qreal progress, AbstractBackendUpdater::State state)
{
    UpdateItem* item = itemFromResource(res);
    if (!item)
        return;
    item->setProgress(progress);
    item->setState(state);

    const QModelIndex idx = indexFromItem(item);
    Q_EMIT dataChanged(idx, idx, { ResourceProgressRole, ResourceStateRole, SectionResourceProgressRole });
}

void UpdateModel::activityChanged()
{
    if (m_updates) {
        if (!m_updates->isProgressing()) {
            m_updates->prepare();
            setResources(m_updates->toUpdate());

            for(auto item : qAsConst(m_updateItems)) {
                item->setProgress(0);
            }
        } else
            setResources(m_updates->toUpdate());
    }
}

QVariant UpdateModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    UpdateItem *item = itemFromIndex(index);

    switch (role) {
    case Qt::DisplayRole:
        return item->name();
    case Qt::DecorationRole:
        return item->icon();
    case Qt::CheckStateRole:
        return item->checked();
    case InstalledVersionRole:
        return item->installedVersion();
    case AvailableVersionRole:
        return item->availableVersion();
    case SizeRole:
        return KFormat().formatByteSize(item->size());
    case ResourceRole:
        return QVariant::fromValue<QObject*>(item->resource());
    case ResourceProgressRole:
        return item->progress();
    case ResourceStateRole:
        return item->state();
    case ChangelogRole:
        return item->changelog();
    case SectionRole: {
        static const QString appUpdatesSection = i18nc("@item:inlistbox", "Application Updates");
        static const QString systemUpdateSection = i18nc("@item:inlistbox", "System Updates");
        static const QString addonsSection = i18nc("@item:inlistbox", "Addons");
        switch(item->resource()->type()) {
            case AbstractResource::Application: return appUpdatesSection;
            case AbstractResource::Technical: return systemUpdateSection;
            case AbstractResource::Addon: return addonsSection;
        }
        Q_UNREACHABLE();
    }
    case SectionResourceProgressRole:
        return (100-item->progress()) + (101 * item->resource()->type());
    default:
        break;
    }

    return QVariant();
}

void UpdateModel::checkResources(const QList<AbstractResource*>& resource, bool checked)
{
    if(checked)
        m_updates->addResources(resource);
    else
        m_updates->removeResources(resource);
}

Qt::ItemFlags UpdateModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return nullptr;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int UpdateModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? m_updateItems.count() : 0;
}

bool UpdateModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole) {
        UpdateItem *item = itemFromIndex(idx);
        const bool newValue = value.toInt() == Qt::Checked;
        const QList<AbstractResource *> apps = { item->app() };

        checkResources(apps, newValue);
        Q_ASSERT(idx.data(Qt::CheckStateRole) == value);

        //When un/checking some backends will decide to add or remove a bunch of packages, so refresh it all
        auto m = idx.model();
        Q_EMIT dataChanged(m->index(0, 0), m->index(m->rowCount()-1, 0), { Qt::CheckStateRole });
        Q_EMIT toUpdateChanged();

        return true;
    }

    return false;
}

void UpdateModel::fetchUpdateDetails(int row)
{
    UpdateItem *item = itemFromIndex(index(row, 0));
    Q_ASSERT(item);
    if (!item) return;

    item->app()->fetchUpdateDetails();
}

void UpdateModel::integrateChangelog(const QString &changelog)
{
    auto app = qobject_cast<AbstractResource*>(sender());
    Q_ASSERT(app);
    auto item = itemFromResource(app);
    if (!item)
        return;

    item->setChangelog(changelog);

    const QModelIndex idx = indexFromItem(item);
    Q_ASSERT(idx.isValid());
    emit dataChanged(idx, idx, { ChangelogRole });
}

void UpdateModel::setResources(const QList<AbstractResource*>& resources)
{
    if (resources == m_resources) {
        return;
    }
    m_resources = resources;

    beginResetModel();
    qDeleteAll(m_updateItems);
    m_updateItems.clear();

    QVector<UpdateItem*> appItems, systemItems, addonItems;
    foreach(AbstractResource* res, resources) {
        connect(res, &AbstractResource::changelogFetched, this, &UpdateModel::integrateChangelog, Qt::UniqueConnection);

        UpdateItem *updateItem = new UpdateItem(res);

        switch(res->type()) {
            case AbstractResource::Technical:
                systemItems += updateItem;
                break;
            case AbstractResource::Application:
                appItems += updateItem;
                break;
            case AbstractResource::Addon:
                addonItems += updateItem;
                break;
        }
    }
    const auto sortUpdateItems = [](UpdateItem *a, UpdateItem *b) { return a->name() < b->name(); };
    std::sort(appItems.begin(), appItems.end(), sortUpdateItems);
    std::sort(systemItems.begin(), systemItems.end(), sortUpdateItems);
    std::sort(addonItems.begin(), addonItems.end(), sortUpdateItems);
    m_updateItems = (QVector<UpdateItem*>() << appItems << addonItems << systemItems);
    endResetModel();

    Q_EMIT hasUpdatesChanged(!resources.isEmpty());
    Q_EMIT toUpdateChanged();
}

bool UpdateModel::hasUpdates() const
{
    return rowCount() > 0;
}

ResourcesUpdatesModel* UpdateModel::backend() const
{
    return m_updates;
}

int UpdateModel::toUpdateCount() const
{
    int ret = 0;
    QSet<QString> packages;
    foreach (UpdateItem* item, m_updateItems) {
        const auto packageName = item->resource()->packageName();
        if (packages.contains(packageName)) {
            continue;
        }
        packages.insert(packageName);
        ret += item->checked() != Qt::Unchecked ? 1 : 0;
    }
    return ret;
}

int UpdateModel::totalUpdatesCount() const
{
    int ret = 0;
    QSet<QString> packages;
    foreach (UpdateItem* item, m_updateItems) {
        const auto packageName = item->resource()->packageName();
        if (packages.contains(packageName)) {
            continue;
        }
        packages.insert(packageName);
        ret += 1;
    }
    return ret;
}

UpdateItem * UpdateModel::itemFromResource(AbstractResource* res)
{
    foreach (UpdateItem* item, m_updateItems) {
        if (item->app() == res)
            return item;
    }
    return nullptr;
}

QString UpdateModel::updateSize() const
{
    return KFormat().formatByteSize(m_updates->updateSize());
}

QModelIndex UpdateModel::indexFromItem(UpdateItem* item) const
{
    return index(m_updateItems.indexOf(item), 0, {});
}

UpdateItem * UpdateModel::itemFromIndex(const QModelIndex& index) const
{
    return m_updateItems[index.row()];
}

void UpdateModel::resourceDataChanged(AbstractResource* res, const QVector<QByteArray>& properties)
{
    auto item = itemFromResource(res);
    if (!item)
        return;

    const auto index = indexFromItem(item);
    if (properties.contains("state"))
        Q_EMIT dataChanged(index, index, {SizeRole, AvailableVersionRole});
    else if (properties.contains("size")) {
        Q_EMIT dataChanged(index, index, {SizeRole});
        m_updateSizeTimer->start();
    }
}

void UpdateModel::checkAll()
{
    for(int i=0, c=rowCount(); i<c; ++i)
        if(index(i,0).data(Qt::CheckStateRole) != Qt::Checked)
            setData(index(i,0), Qt::Checked, Qt::CheckStateRole);
}

void UpdateModel::uncheckAll()
{
    for(int i=0, c=rowCount(); i<c; ++i)
        if(index(i,0).data(Qt::CheckStateRole) != Qt::Unchecked)
            setData(index(i,0), Qt::Unchecked, Qt::CheckStateRole);
}
