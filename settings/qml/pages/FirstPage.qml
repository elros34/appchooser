import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    SilicaListView {
        id: listView
        anchors.fill: parent

        header: PageHeader {
            title: "Mimes"
        }

        PullDownMenu {
//            MenuItem {
//                text: "Refresh"
//                onClicked: {
//                    mimeHandler.refresh() // useless because buggy contentaction
//                }
//            }
            MenuItem {
                text: "Reset all"
                onClicked: {
                    mimeHandler.resetAll()
                }
            }
            MenuItem {
                text: "Set visible to AppChooser"
                onClicked: {
                    if (listView.showEmptyMimes)
                        mimeHandler.setAllToAppChooser()
                    else
                        mimeHandler.setVisibleToAppChooser()
                }
            }
            MenuItem {
                text: listView.showEmptyMimes ? "Hide unconfigured" : "Show unconfigured"
                onClicked: {
                    listView.showEmptyMimes = !listView.showEmptyMimes
                }
            }
        }

        property bool showEmptyMimes: false

        model: mimeHandler

        delegate: ListItem {
            contentHeight: visible ? label.height : 0
            visible: listView.showEmptyMimes ? true : !model.empty

            Label {
                id: label
                anchors {
                    left: parent.left
                    leftMargin: Theme.paddingMedium
                    right: parent.right
                    rightMargin: Theme.paddingMedium
                    verticalCenter: parent.verticalCenter
                }
                color: Theme.secondaryHighlightColor
                wrapMode: Text.WrapAnywhere
                text: model.mime + "<font color=\"" +
                      Theme.primaryColor + "\"> " + model.actionPretty + "</font>"
            }

            onClicked: {
                openMenu()
            }

            menu: ContextMenu {
                MenuItem {
                    text: "Reset to default"
                    onClicked: {
                        mimeHandler.reset(index)
                    }
                }
                MenuItem {
                    text: "Set to \"AppChooser\""
                    onClicked: {
                        mimeHandler.setToAppChooser(index)
                    }
                }
            }
        }

        VerticalScrollDecorator {
            flickable: listView
        }

        footer: Item {
            width: 1
            height: Theme.paddingMedium
        }
    }
}
