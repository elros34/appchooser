#include <QtQuick>
#include <sailfishapp.h>
#include <qpa/qplatformnativeinterface.h>
#include "appchooser.h"
#include "appchooserfilter.h"

int main(int argc, char *argv[])
{
    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();
    qmlRegisterType<AppChooser>("AppChooser", 1, 0, "AppChooser");
    qmlRegisterType<AppChooserFilter>("AppChooser", 1, 0, "AppChooserFilter");
    view->setSource(SailfishApp::pathToMainQml());
    // Thanks to CODeRUS for dialog type window
    view->create();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    native->setWindowProperty(view->handle(), "CATEGORY", "dialog"); // dialog, silica, notification
    native->setWindowProperty(view->handle(), "BACKGROUND_VISIBLE", false);

    return app->exec();
}
