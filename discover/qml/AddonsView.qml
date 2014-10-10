import QtQuick 2.1
import QtQuick.Controls 1.1
import org.kde.muon 1.0
import org.kde.kquickcontrolsaddons 2.0

Column
{
    id: addonsView
    property alias application: addonsModel.application
    property bool isInstalling: false
    property alias isEmpty: addonsModel.isEmpty
    enabled: !addonsView.isInstalling
    visible: !addonsView.isEmpty

    Repeater
    {
        model: ApplicationAddonsModel { id: addonsModel }
        
        delegate: Item {
            height: (description.height + name.height)*1.2
            width: addonsView.width
            Row {
                id: componentsRow
                height: parent.height
                spacing: 10
                CheckBox {
                    enabled: !addonsView.isInstalling
                    anchors.verticalCenter: parent.verticalCenter
                    checked: model.checked
                    onClicked: addonsModel.changeState(display, checked)
                }
                QIconItem {
                    icon: "applications-other"
                    height: parent.height*0.9
                    width: height
                    smooth: true
                    opacity: addonsView.isInstalling ? 0.3 : 1
                }
            }
            Label {
                id: name
                anchors {
                    top: parent.top
                    left: componentsRow.right
                    right: parent.right
                }
                elide: Text.ElideRight
                text: display
            }
            Label {
                id: description
                anchors {
                    bottom: parent.bottom
                    left: componentsRow.right
                    right: parent.right
                }
                elide: Text.ElideRight
                font.italic: true
                text: toolTip
            }
        }
    }
    
    Row {
        enabled: addonsModel.hasChanges && !addonsView.isInstalling
        spacing: 5
        
        Button {
            height: parent.enabled ? implicitHeight : 0
            visible: height!=0
            iconName: "dialog-ok"
            text: i18n("Apply Changes")
            onClicked: addonsModel.applyChanges()
            Behavior on height { NumberAnimation { duration: 100 } }
        }
        Button {
            height: parent.enabled ? implicitHeight : 0
            visible: height!=0
            iconName: "document-revert"
            text: i18n("Discard")
            onClicked: addonsModel.discardChanges()
            Behavior on height { NumberAnimation { duration: 100 } }
        }
    }
}
