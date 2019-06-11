import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"
import MimeHandlerr 1.0

ApplicationWindow
{
    initialPage: Component { FirstPage { } }
    cover: undefined //Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
    _defaultPageOrientations: allowedOrientations

    MimeHandler {
        id: mimeHandler
    }
}
