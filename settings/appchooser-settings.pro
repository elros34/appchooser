TARGET = appchooser-settings

CONFIG += sailfishapp

PKGCONFIG += contentaction5 mlite5

INCLUDEPATH += /usr/include/

SOURCES += \
    src/main.cpp \
    src/mimehandler.cpp \
    src/mimeitem.cpp

MOC_DIR = .moc
OBJECTS_DIR = .obj

DISTFILES += qml/appchooser-settings.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    appchooser-settings.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
#CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
#TRANSLATIONS += translations/appchooser-settings-de.ts

HEADERS += \
    src/mimehandler.h \
    src/mimeitem.h
