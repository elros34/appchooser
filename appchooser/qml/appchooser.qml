import QtQuick 2.6
import Sailfish.Silica 1.0
import Sailfish.Lipstick 1.0
import QtQuick.Window 2.1
import AppChooser 1.0
import Nemo.DBus 2.0
import "pages"

SystemDialog {
    id: window
    allowedOrientations: __silica_applicationwindow_instance.defaultAllowedOrientations
    visible: false
    category: SystemDialog.Dialog
    contentHeight: mainView.height

    property bool isPortrait: orientation & (Orientation.Portrait | Orientation.PortraitInverted)

    onVisibleChanged: {
        if (visible) {
            mainView.forceActiveFocus()
        } else  {
            appChooser.clear()
            mainView.scrollToTop()
            mainView.searchField.text = ""
            mainView.searchField.visible = false
        }
    }

    MainView {
        id: mainView
        clip: true

        Behavior on height { NumberAnimation { duration: 200} }
    }

    AppChooser {
        id: appChooser
        onShowWindow: window.activate()
        onHideWindow: window.dismiss()
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

        function display_status_ind(status) {
            if (status === "off")
                window.dismiss()
        }
    }

    AppChooserFilter {
        id: appChooserFilter
        sourceModel: appChooser
        dedicatedAppsMode: appChooser.dedicatedAppsMode
    }
}


