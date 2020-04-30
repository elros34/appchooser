TEMPLATE = lib

CONFIG += plugin c++11 link_pkgconfig

PKGCONFIG += contentaction5 mlite5

QT += qml quick xml

TARGET = $$qtLibraryTarget($$TARGET)
uri = AppChooser.Settings

SOURCES += \
    src/mimehandler.cpp \
    src/mimeitem.cpp \
    src/appchoosersettingsplugin.cpp \
    src/mimefilter.cpp

MOC_DIR = .moc
OBJECTS_DIR = .obj

DISTFILES += \
    qmldir \
    appchooser-settings.json \
    qml/Settings.qml

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
    src/mimeitem.h \
    src/appchoosersettingsplugin.h \
    src/mimefilter.h

qmlSettings.files = qml/Settings.qml
qmlSettings.path = /usr/share/appchooser/qml/settings/
INSTALLS += qmlSettings

settingsEntry.files = appchooser-settings.json
settingsEntry.path = /usr/share/jolla-settings/entries/
INSTALLS += settingsEntry

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}




