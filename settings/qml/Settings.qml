import QtQuick 2.0
import Sailfish.Silica 1.0
import AppChooser.Settings 1.0

Page {
    id: page

    property Item searchField: listView.headerItem
    focus: !searchField.focus

    Keys.onPressed: {
        searchField.text += event.text
        searchField.forceActiveFocus()
    }

    SilicaListView {
        id: listView
        anchors.fill: parent
        currentIndex: -1

        header: SearchField {
            id: _searchField
            width: parent.width
            placeholderText: "Search MIMEs"
            focusOutBehavior: FocusBehavior.KeepFocus
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase

            onTextChanged: {
                mimeFilter.setFilterWildcard(text)
            }
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
                text: "Set visible to 'Ask'"
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

        model: mimeFilter

        delegate: ListItem {
            contentHeight: label.height

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
                textFormat: Text.StyledText
                text: model.mime + "<font color=\"" +
                      Theme.primaryColor + "\"> " + model.actionPretty + "</font>"
            }

            onClicked: {
                openMenu()
            }

            menu: ContextMenu {
                MenuItem {
                    text: "Reset to default"
                    visible: !model.empty
                    onClicked: {
                        mimeHandler.reset(index)
                    }
                }
                MenuItem {
                    text: "Set to 'Ask'"
                    visible: model.action !== "appchooser"
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

    MimeHandler {
        id: mimeHandler
    }

    MimeFilter {
        id: mimeFilter
        sourceModel: mimeHandler
        showEmptyMimes: listView.showEmptyMimes
    }
}
