TARGET = TvHolic
QT += gui core network xml
CONFIG += warn_on console

LEVEL = ..

!include($$LEVEL/TvHolic.pri):error("Can't load TvHolic.pri")

TEMPLATE = app

include($$PWD/../qt-brisa/brisa.pri)

HEADERS += \
           TvHolic.h \
           Config.h \
           Global.h \
           Encoder.h \
           PipeProcess.h \
           MencoderTranscoder.h \
           AsIsTranscoder.h \
           ConnectionManagerService.h \
           ContentDirectoryService.h \
           \
           DIDL/didlobject.h \
           DIDL/resource.h \
           DIDL/searchclass.h \
           DIDL/container.h \
           DIDL/item.h \
           DIDL/videoitem.h \
           DIDL/createclass.h \
           DIDL/audioitem.h \
           DIDL/genre.h


SOURCES += \
           TvHolic.cpp \
           Config.cpp \
           Global.cpp \
           Encoder.cpp \
           PipeProcess.cpp \
           MencoderTranscoder.cpp \
           AsIsTranscoder.cpp \
           ConnectionManagerService.cpp \
           ContentDirectoryService.cpp \
           \
           DIDL/didlobject.cpp \
           DIDL/resource.cpp \
           DIDL/searchclass.cpp \
           DIDL/container.cpp \
           DIDL/item.cpp \
           DIDL/videoitem.cpp \
           DIDL/createclass.cpp \
           DIDL/audioitem.cpp \
           DIDL/genre.cpp


RESOURCES += $$LEVEL/TvHolic.qrc
