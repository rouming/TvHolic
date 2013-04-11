TARGET = TvHolic
QT += gui core network xml
CONFIG += warn_on console

LEVEL = ..

!include($$LEVEL/TvHolic.pri):error("Can't load TvHolic.pri")

TEMPLATE = app

HEADERS += \
           Config.h \
           Encoder.h \
           HttpSession.h \
           TcpServer.h \
           ThreadPool.h \
           UpnpServer.h \

SOURCES += \
           Main.cpp \
           Config.cpp \
           Encoder.cpp \
           HttpSession.cpp \
           TcpServer.cpp \
           ThreadPool.cpp \
           UpnpServer.cpp \


RESOURCES += $$LEVEL/TvHolic.qrc
