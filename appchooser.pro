TEMPLATE = subdirs
SUBDIRS = appchooser/appchooser.pro settings/appchooser-settings.pro
CONFIG = ordered

OTHER_FILES += \
    rpm/appchooser.spec

MOC_DIR = .moc
OBJECTS_DIR = .obj

mimeapps.files = data/mimeapps-appchooser.list
mimeapps.path = /usr/share/appchooser/
INSTALLS += mimeapps

DISTFILES += \
    settings/qml/pages/MimeSelector.qml
