import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: mainView
    property alias flickableHeight: flickable.height

    SilicaFlickable {
        id: flickable
        width: parent.width
        height: Math.min(contentHeight, rotateItem.maxMainViewHeight)
        contentHeight: column.height

        Column {
            id: column
            width: mainView.width
            spacing: Theme.paddingSmall

            Item {
                height: Theme.paddingMedium
                width: 1
            }

            Label {
                width: parent.width - 2*Theme.paddingMedium
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                text: "Open with"
                color: Theme.highlightColor
                font {
                    family: Theme.fontFamilyHeading
                    pixelSize: Theme.fontSizeLarge
                }
            }


            ListItem {
                width: parent.width
                contentHeight: urlLabel.contentHeight
                clip: true
                onClicked: {
                    urlLabel.expanded = !urlLabel.expanded
                }
                Behavior on contentHeight {NumberAnimation { duration: 150}}

                Label {
                    id: urlLabel
                    width: parent.width - 2*Theme.paddingLarge
                    anchors.horizontalCenter: parent.horizontalCenter
                    wrapMode: Text.WrapAnywhere
                    text: appChooser.launchArgs
                    maximumLineCount: expanded ? undefined : 2
                    clip: true
                    property bool expanded: false
                }

                menu: ContextMenu {
                    MenuItem {
                        text: "Copy to clipboard"
                        onClicked: {
                            Clipboard.text = urlLabel.text
                        }
                    }
                }
            }

//            TextSwitch {
//                text: "Remember choice"
//                onCheckedChanged: {

//                }
//            }

            SilicaListView {
                width: parent.width
                height: contentHeight

                model: appChooser

                delegate: ListItem {

                    Item {
                        id: iconItem
                        height: parent.height
                        width: height
                        anchors {
                            left: parent.left
                            leftMargin: Theme.paddingLarge
                        }

                        Image {
                            source: model.icon
                            anchors.centerIn: parent
                            fillMode: Image.PreserveAspectFit
                            asynchronous: true
                            width: parent.width - Theme.paddingMedium
                            height: width
                            sourceSize.width: width
                            sourceSize.height: width
                        }
                    }

                    Label {
                        anchors {
                            left: iconItem.right
                            leftMargin: Theme.paddingLarge
                            verticalCenter: parent.verticalCenter
                        }

                        text: model.name
                    }

                    onClicked: {
                        appChooser.launch(index)
                        window.deactivate()
                    }
                }
            }

            Item {
                height: Theme.paddingMedium
                width: 1
            }
        }

        VerticalScrollDecorator {
            flickable: flickable
        }
    }
}
