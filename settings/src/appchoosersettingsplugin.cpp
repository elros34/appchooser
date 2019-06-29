#include "appchoosersettingsplugin.h"
#include "mimehandler.h"
#include "mimefilter.h"

#include <qqml.h>

void AppChooserSettingsPlugin::registerTypes(const char *uri)
{
    // @uri AppChooser.Settings
    qmlRegisterType<MimeHandler>(uri, 1, 0, "MimeHandler");
    qmlRegisterType<MimeFilter>(uri, 1, 0, "MimeFilter");
}
