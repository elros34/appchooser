import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0 as SilicaPrivate
import AppChooser 1.0
import Nemo.DBus 2.0
import "pages"

SilicaPrivate.Window {
    id: window
    _allowedOrientations: defaultAllowedOrientations
    visible: false
    property bool isPortrait: orientation & (Orientation.Portrait | Orientation.PortraitInverted)
    property alias __silica_applicationwindow_instance: window

    property bool _dimScreen: false
    property bool _dimmingActive: false

    onVisibleChanged: {
        console.log("visible", visible)
        if (!visible) {
            appChooser.clear()
            mainView.flickable.scrollToTop()
            appChooser.rememberChoice = false
        }
    }

    AppChooser {
        id: appChooser

        onShowWindow: {
            window.activate()
        }
    }

    Item {
        id: rotateItem
        anchors.centerIn: parent
        width: window.isPortrait ? parent.width : parent.height
        height: window.isPortrait ? parent.height : parent.width
        rotation: {
            switch (window.orientation) {
            case Orientation.Portrait:
                return 0
            case Orientation.Landscape:
                return 90
            case Orientation.PortraitInverted:
                return 180
            case Orientation.LandscapeInverted:
                return 270
            }
        }

        property int maxMainViewHeight: height * 4/5
        property int mainViewHeight: Math.min(maxMainViewHeight, mainView.flickable.height)

        SilicaPrivate.Wallpaper {
            width: parent.width
            height: rotateItem.mainViewHeight
            blending: true
            windowRotation: parent.rotation
            backgroundOpacity: colorScheme == Theme.LightOnDark ? 0.3 : _backgroundOpacity
        }

        Item {
            width: parent.width
            height: rotateItem.mainViewHeight
            clip: true

            MainView {
                id: mainView
                anchors.fill: parent
                //opacity: window._dimScreen ? 0.7 : 1
            }
        }
        MouseArea {
            height: parent.height - rotateItem.mainViewHeight
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            onClicked: {
                window.deactivate()
            }
            Rectangle {
                anchors.fill: parent
                color: Theme.rgba("black", 0.3)
            }
        }
    }

    DBusAdaptor {
        service: "com.appchooser"
        iface: "com.appchooser"
        path: "/com/appchooser"
        bus: DBus.SessionBus
        xml: '<interface name="com.appchooser">\n' +
             '  <method name="openWith">\n' +
             '      <arg name="url" type="s"/>\n' +
             '  </method>\n' +
             '</interface>\n'

        function openWith(url) {
            console.log("DBus adaptor openWith: " + url)
            appChooser.openWith(url)
        }
    }

    DBusInterface {
        bus: DBus.SystemBus
        service: "com.nokia.mce"
        path: "/com/nokia/mce/signal"
        iface: "com.nokia.mce.signal"

        signalsEnabled: true

        function display_status_ind(status)
        {
            if (status === "off")
                window.deactivate()
        }
    }
}


