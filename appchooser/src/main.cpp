#include <QtQuick>
#include <sailfishapp.h>
#include "appchooser.h"
#include "appchooserfilter.h"
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
#ifdef QT_DEBUG
    QLoggingCategory::setFilterRules("appchooser.debug=true");
#endif

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    qmlRegisterType<AppChooser>("AppChooser", 1, 0, "AppChooser");
    qmlRegisterType<AppChooserFilter>("AppChooser", 1, 0, "AppChooserFilter");
    QQmlApplicationEngine engine;
    engine.load(SailfishApp::pathToMainQml());

    return app->exec();
}
