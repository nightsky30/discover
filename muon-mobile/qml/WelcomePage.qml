import QtQuick 1.1
import org.kde.plasma.components 0.1
import org.kde.qtextracomponents 0.1
import org.kde.muon 1.0
import "navigation.js" as Navigation

Item {
    function init() {
        breadcrumbsItem.pushItem("go-home")
    }
    
    Page {
        id: mainPage
        
        Information {
            id: info
            height: 200
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            
            dataModel: ListModel {
                ListElement { text: "KAlgebra"; color: "#770033"; icon: "kalgebra"; opacity: 0.2 }
                ListElement { text: "Digikam"; color: "#000088"; icon: "digikam"; opacity: 0.2 }
                ListElement { text: "Plasma"; color: "#003333"; icon: "plasma"; opacity: 0.2 }
            }
            
            delegate: Item {
                    property QtObject modelData
                    
                    Rectangle {
                        anchors.fill: parent
                        radius: 10
                        color: modelData.color
                        opacity: modelData.opacity
                    }
                    
                    Label {
                        anchors.margins: 20
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        text: modelData.text
                        font.pointSize: info.height/3
                    }
                    
                    QIconItem {
                        anchors {
                            margins: 20
                            left: parent.left
                            top: parent.top
                            bottom: parent.bottom
                        }
                        width: height
                        icon: modelData.icon
                    }
            }
        }
        
        ListView {
            id: top1
            clip: true
            anchors {
                margins: 5
                top: info.bottom
                left: parent.left
                bottom: parent.bottom
            }
            width: parent.width/2-10
            header: Label { text: i18n("<b>Popularity Contest</b>") }
            model: ApplicationProxyModel {
                stringSortRole: "popcon"
                sortOrder: Qt.DescendingOrder
                
                Component.onCompleted: sortModel()
            }
            delegate: ListItem {
                        width: top1.width
                        QIconItem {
                            id: iconItem
                            anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                            height: parent.height*0.8
                            width: height
                            icon: model["icon"]
                        }
                        Label {
                            anchors { left: iconItem.right; right: pointsLabel.left; verticalCenter: parent.verticalCenter }
                            text: name
                            elide: Text.ElideRight
                        }
                        Label {
                            anchors { right: parent.right; verticalCenter: parent.verticalCenter }
                            id: pointsLabel
                            text: i18n("points: %1", popcon)
                        }
                        MouseArea { anchors.fill: parent; onClicked: Navigation.openApplication(pageStack, application) }
                    }
        }
        ListView {
            id: top2
            clip: true
            anchors {
                margins: 5
                top: info.bottom
                right: parent.right
                bottom: parent.bottom
            }
            width: parent.width/2-10
            header: Label { text: i18n("<b>Best Ratings</b>") }
            model: ApplicationProxyModel {
                id: ratingsTopModel
                stringSortRole: "ratingPoints"
                sortOrder: Qt.DescendingOrder
            }
            Connections {
                target: app.appBackend.reviewsBackend()
                onRatingsReady: ratingsTopModel.sortModel()
            }
            delegate: ListItem {
                        width: top1.width
                        QIconItem {
                            id: iconItem
                            anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                            height: parent.height*0.8
                            width: height
                            icon: model["icon"]
                        }
                        Label {
                            anchors { left: iconItem.right; right: ratingsItem.left; verticalCenter: parent.verticalCenter }
                            text: name
                            elide: Text.ElideRight
                        }
                        Rating {
                            id: ratingsItem
                            anchors { verticalCenter: parent.verticalCenter; right: parent.right }
                            rating: model.rating
                            height: 10
                        }
                        MouseArea { anchors.fill: parent; onClicked: Navigation.openApplication(pageStack, application) }
                    }
        }
    }
    
    ToolBar {
        id: breadcrumbsItemBar
        anchors {
            top: parent.top
            left: parent.left
            right: pageToolBar.left
            rightMargin: 10
        }
        height: 30
        z: 0
        
        Breadcrumbs {
            id: breadcrumbsItem
            anchors.fill: parent
            onClicked: Navigation.jumpToIndex(pageStack, breadcrumbsItem, idx)
        }
    }
    
    ToolBar {
        id: pageToolBar
        anchors {
            leftMargin: 10
            rightMargin: 10
            top: parent.top
            right: parent.right
        }
        width: visible ? parent.width/4 : 0
        visible: tools!=null
        
        Behavior on width {
            NumberAnimation { duration: 250 }
        }
    }
    
    PageStack
    {
        id: pageStack
        anchors {
            margins: 10
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: breadcrumbsItemBar.bottom
        }
        property alias breadcrumbs: breadcrumbsItem
        
        initialPage: window.state=="loaded" ? mainPage : null
        clip: true
        
        toolBar: pageToolBar
    }
}