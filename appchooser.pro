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

contentaction.files = contentaction/video-url.xml contentaction/telegram.xml
contentaction.path = /usr/share/contentaction/
INSTALLS += contentaction

# TODO clean .moc and .obj dirs


