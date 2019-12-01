/*
 *   Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.5
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import "navigation.js" as Navigation
import org.kde.discover.app 1.0
import org.kde.discover 2.0
import org.kde.kirigami 2.4 as Kirigami

DiscoverPage {
    id: page
    readonly property var model: appsModel
    property alias category: appsModel.filteredCategory
    property alias sortRole: appsModel.sortRole
    property alias sortOrder: appsModel.sortOrder
    property alias originFilter: appsModel.originFilter
    property alias mimeTypeFilter: appsModel.mimeTypeFilter
    property alias stateFilter: appsModel.stateFilter
    property alias extending: appsModel.extending
    property alias search: appsModel.search
    property alias resourcesUrl: appsModel.resourcesUrl
    property alias isBusy: appsModel.isBusy
    property alias allBackends: appsModel.allBackends
    property alias count: apps.count
    property alias listHeader: apps.header
    property alias listHeaderPositioning: apps.headerPositioning
    property alias sortProperty: saveChanges.property
    property bool compact: page.width < 550 || !applicationWindow().wideScreen
    property bool showRating: true

    property bool canNavigate: true
    readonly property alias subcategories: appsModel.subcategories

    function escapeHtml(unsafe) {
        return unsafe
            .replace(/&/g, "&amp;")
            .replace(/</g, "&lt;")
            .replace(/>/g, "&gt;")
            .replace(/"/g, "&quot;")
            .replace(/'/g, "&#039;");
    }

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
    }

    title: search.length>0 ? i18n("Search: %1", escapeHtml(search))
         : category ? category.name : ""

    signal clearSearch()

    supportsRefreshing: true
    onRefreshingChanged: if (refreshing) {
        appsModel.invalidateFilter()
        refreshing = false
    }

    ActionGroup {
        id: sortGroup
        exclusive: true
    }

    Binding {
        id: saveChanges
        target: DiscoverSettings
        property: "appsListPageSorting"
        value: appsModel.sortRole
    }

    contextualActions: [
        Kirigami.Action {
            visible: !appsModel.sortByRelevancy
            text: i18n("Sort: %1", sortGroup.checkedAction.text)
            Action {
                ActionGroup.group: sortGroup
                text: i18n("Name")
                onTriggered: {
                    appsModel.sortRole = ResourcesProxyModel.NameRole
                }
                checkable: true
                checked: appsModel.sortRole === ResourcesProxyModel.NameRole
            }
            Action {
                ActionGroup.group: sortGroup
                text: i18n("Rating")
                onTriggered: {
                    appsModel.sortRole = ResourcesProxyModel.SortableRatingRole
                }
                checkable: true
                checked: appsModel.sortRole === ResourcesProxyModel.SortableRatingRole
            }
            Action {
                ActionGroup.group: sortGroup
                text: i18n("Size")
                onTriggered: {
                    appsModel.sortRole = ResourcesProxyModel.SizeRole
                }
                checkable: true
                checked: appsModel.sortRole === ResourcesProxyModel.SizeRole
            }
            Action {
                ActionGroup.group: sortGroup
                text: i18n("Release Date")
                onTriggered: {
                    appsModel.sortRole = ResourcesProxyModel.ReleaseDateRole
                }
                checkable: true
                checked: appsModel.sortRole === ResourcesProxyModel.ReleaseDateRole
            }
        }
    ]

    Kirigami.CardsListView {
        id: apps

        section.delegate: Label {
            text: section
            anchors {
                right: parent.right
            }
        }

        model: ResourcesProxyModel {
            id: appsModel
            sortRole: DiscoverSettings.appsListPageSorting
            sortOrder: sortRole === ResourcesProxyModel.SortableRatingRole || sortRole === ResourcesProxyModel.ReleaseDateRole ? Qt.DescendingOrder : Qt.AscendingOrder

            onBusyChanged: if (isBusy) {
                apps.currentIndex = -1
            }
        }
        currentIndex: -1
        delegate: ApplicationDelegate {
            application: model.application
            compact: !applicationWindow().wideScreen
            showRating: page.showRating
        }

        Label {
            anchors.centerIn: parent
            opacity: apps.count == 0 && !appsModel.isBusy ? 0.3 : 0
            Behavior on opacity { PropertyAnimation { duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad } }
            text: i18n("Sorry, nothing found...")
        }

        footer: BusyIndicator {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: appsModel.isBusy && apps.atYEnd
            running: visible
            opacity: visible ? 1 : 0
            Behavior on opacity {
                PropertyAnimation { duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad }
            }
            Label {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.top
                }
                text: i18n("Still looking...")
            }
        }
    }
}
