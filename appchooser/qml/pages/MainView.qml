import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: mainView
    property alias flickable: flickable
    property alias searchField: searchField

    focus: !searchField.focus && window.visible

    Keys.onPressed: {
        // exotic hw keys might generate junk
        if (event.text.length && event.key > 0 && event.key < 16000000) {
            searchField.text += event.text
            searchField.visible = true
            searchField.forceActiveFocus()
        }
    }

    SilicaFlickable {
        id: flickable
        width: parent.width
        height: Math.min(contentHeight, rotateItem.maxMainViewHeight)
        contentHeight: column.height

        PullDownMenu {
            enabled: appChooser.currentMimeType.length
            visible: enabled

            MenuItem {
                enabled: listView.count
                TextSwitch {
                    id: textSwitch
                    text: "Remember choice"
                    width: parent.width
                    checked: appChooser.rememberChoice
                    enabled: parent.enabled
                    _label.color: parent.color
                    leftMargin: (parent.width - _label.contentWidth)/3 //FIXME
                    anchors.verticalCenter: parent.verticalCenter
                }

                onClicked: {
                    appChooser.rememberChoice = !appChooser.rememberChoice
                }
            }

            MenuItem {
                text: appChooser.dedicatedAppsMode ? "More applications" : "Dedicated applications"
                onClicked: {
                    if (appChooser.dedicatedAppsMode)
                        appChooser.busy = true
                }
                onDelayedClick: {
                    appChooser.dedicatedAppsMode = !appChooser.dedicatedAppsMode
                }
            }

            MenuItem {
                text: searchField.visible ? "Hide Search" : "Show Search"
                onClicked: {
                    searchField.visible = !searchField.visible
                }
            }
        }

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
                    text: labelText
                    maximumLineCount: expanded ? undefined : 2
                    textFormat: Text.StyledText
                    truncationMode: TruncationMode.Elide

                    clip: true
                    property bool expanded: false
                    property string labelText: {
                        if (appChooser.currentMimeType.length)
                            return appChooser.launchArgs + " <font color=\"" +
                                Theme.secondaryHighlightColor + "\">(" + appChooser.currentMimeType + ")</font>"
                        else
                           return appChooser.launchArgs
                    }
                }

                menu: ContextMenu {
                    MenuItem {
                        text: "Copy to clipboard"
                        onClicked: {
                            Clipboard.text = appChooser.launchArgs
                        }
                    }
                }
            }

            Item {
                width: parent.width
                height: noAppsLabel.height + Theme.paddingLarge
                visible: !listView.count && !searchField.visible
                Label {
                    id: noAppsLabel
                    anchors {
                        left: parent.left
                        leftMargin: Theme.paddingMedium
                        right: parent.right
                        rightMargin: Theme.paddingMedium
                        verticalCenter: parent.verticalCenter
                    }
                    wrapMode: Text.Wrap
                    text: "No dedicated application found"
                    horizontalAlignment: Text.AlignHCenter
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeLarge
                }
            }

            SearchField {
                id: searchField
                width: parent.width
                visible: false
                focus: visible
                placeholderText: "Search .."
                focusOutBehavior: FocusBehavior.KeepFocus
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase

                onTextChanged: {
                    appChooserFilter.search(text)
                }
            }

            SilicaListView {
                id: listView
                width: parent.width
                height: contentHeight
                currentIndex: -1

                verticalLayoutDirection: ListView.BottomToTop

                model: appChooserFilter

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
                       appChooser.launch(appChooserFilter.indexToSource(index))
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

    BusyIndicator {
        anchors.centerIn: parent
        running: appChooser.busy
        size: BusyIndicatorSize.Large
    }
}
