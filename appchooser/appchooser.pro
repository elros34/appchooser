# The name of your application
TARGET = appchooser

CONFIG += sailfishapp

QT += dbus

PKGCONFIG += contentaction5 mlite5

MOC_DIR = .moc
OBJECTS_DIR = .obj

SOURCES += \
    src/main.cpp \
    src/appchooser.cpp \
    src/actionitem.cpp \
    src/actionsmodel.cpp \
    src/appchooserfilter.cpp

DISTFILES += qml/appchooser.qml \
    appchooser.desktop \
    qml/pages/MainView.qml \
    dbus/com.appchooser.service

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
#CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
#TRANSLATIONS += translations/appchooser-de.ts

HEADERS += \
    src/appchooser.h \
    src/actionitem.h \
    src/actionsmodel.h \
    src/appchooserfilter.h

dbus.files = dbus/com.appchooser.service
dbus.path = /usr/share/dbus-1/services/
INSTALLS += dbus
