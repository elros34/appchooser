#include <QtQuick>
#include <sailfishapp.h>
#include "mimehandler.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<MimeHandler>("MimeHandlerr", 1, 0, "MimeHandler");

    return SailfishApp::main(argc, argv);
}
