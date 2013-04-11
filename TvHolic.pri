# Version definition
TVHOLIC_VERSION=0.1

# Define your own build rules
exists(Subsidiary.pri):include(Subsidiary.pri)

CONFIG += debug_and_release

DEFINES += "TVHOLIC_VERSION=$$TVHOLIC_VERSION"

win32:DEFINES   += "_WIN_=1"
mac:DEFINES     += "_MAC_=1"
linux-*:DEFINES += "_LIN_=1"

contains(QT_ARCH, x86_64):DEFINES += "_AMD64_"
else::DEFINES += "_X86_"

!isEmpty(BUILD_NAME) {
     BUILDDIR = $$lower($$join(BUILD_NAME, "", $$LEVEL/build/))
     DESTDIR = $$LEVEL/build
     QMAKE_LIBDIR += $$DESTDIR
     MOC_DIR = $$join(TARGET, "", $$BUILDDIR/mocs/)
     RCC_DIR = $$join(TARGET, "", $$BUILDDIR/resources/)
     OBJECTS_DIR = $$join(TARGET, "", $$BUILDDIR/obj/)
     UI_DIR = $$BUILDDIR/ui
}

INCLUDEDIR += $UI_DIR

CONFIG(debug, debug|release) {
   CONFIG += console

   # Define macros DEBUG for Unix
   unix: !contains( DEFINES, DEBUG ): DEFINES += DEBUG
}
