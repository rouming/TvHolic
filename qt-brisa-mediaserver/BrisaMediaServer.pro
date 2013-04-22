TARGET = brisamediaserver
TEMPLATE = app

CONFIG += debug_and_release

QT += core network xml

include($$PWD/../qt-brisa/brisa.pri)

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
