# -------------------------------------------------
# Project created by QtCreator 2010-10-03T00:00:33
# -------------------------------------------------
TARGET = brisamediaserver
TEMPLATE = app
CONFIG += QT \
    BRISA
CONFIG += qt
QT += core \
    network \
    xml
BRISA += upnp \
    core \
    utils
SOURCES += main.cpp \
    brisamediaserver.cpp \
    connectionmanager.cpp \
    contentdirectory.cpp \
    mediaserver.cpp \
    pluginmanager.cpp \
    brisamediaserverabstractplugin.cpp \
    DIDL/didlobject.cpp \
    DIDL/resource.cpp \
    DIDL/searchclass.cpp \
    DIDL/container.cpp \
    DIDL/item.cpp \
    DIDL/videoitem.cpp \
    DIDL/createclass.cpp \
    DIDL/audioitem.cpp \
    DIDL/genre.cpp
HEADERS += brisamediaserver.h \
    connectionmanager.h \
    contentdirectory.h \
    mediaserver.h \
    pluginmanager.h \
    brisamediaserverabstractplugin.h \
    DIDL/didlobject.h \
    DIDL/resource.h \
    DIDL/searchclass.h \
    DIDL/container.h \
    DIDL/item.h \
    DIDL/videoitem.h \
    DIDL/createclass.h \
    DIDL/audioitem.h \
    DIDL/genre.h
OTHER_FILES += ContentDirectory1.xml \
    ConnectionManager1.xml
FORMS += 
