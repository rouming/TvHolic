#
# BRisa main project file
# 
# Do not edit this file.
# Use the configure script to specify compile options.
#

TEMPLATE = subdirs
DESTDIR = lib

#check Qt version
lessThan(QT_MAJOR_VERSION, 4) | lessThan(QT_MINOR_VERSION, 6) {
   error(BRisa requires Qt 4.6.2 or newer but Qt $$[QT_VERSION] was detected.)
}

contains(BRISA_MODULES, core) {
   message(building core module)
   sub_core.subdir = src/core
   SUBDIRS += sub_core
}

contains(BRISA_MODULES, utils) {
   message(building utils module)
   sub_utils.subdir = src/utils
   SUBDIRS += sub_utils
}

contains(BRISA_MODULES, upnp) {
   message(building upnp module)
   sub_upnp.subdir = src/upnp
   SUBDIRS += sub_upnp
}

#SUBDIRS += examples

features.path = $$BRISA_INSTALL_FEATURES
win32:features.files = $$BRISA_SOURCE_TREE/features/win32/brisa.prf $$BRISA_BUILD_TREE/features/win32/brisavars.prf
unix:features.files = $$BRISA_SOURCE_TREE/deploy/qt/brisa.prf $$BRISA_BUILD_TREE/features/brisavars.prf
INSTALLS += features

style.CONFIG = recursive
style.recurse = $$SUBDIRS
style.recurse_target = astyle
QMAKE_EXTRA_TARGETS += style

sub-examples.commands += cd examples && $(QMAKE) $$BRISA_SOURCE_TREE/examples/examples.pro && $(MAKE)
QMAKE_EXTRA_TARGETS += sub-examples

#sub-tests.commands += cd tests && $(QMAKE) $$BRISA_SOURCE_TREE/tests/tests.pro && $(MAKE)
#QMAKE_EXTRA_TARGETS += sub-tests

#runtests.depends += sub-tests
#runtests.commands += cd tests && $(MAKE) test
#QMAKE_EXTRA_TARGETS += runtests

unix {
    cov_zerocounters.CONFIG += recursive
    cov_zerocounters.recurse = $$SUBDIRS
    cov_zerocounters.recurse -= sub_designer
    cov_zerocounters.recurse_target = zerocounters
    QMAKE_EXTRA_TARGETS += cov_zerocounters

    cov_capture.CONFIG += recursive
    cov_capture.recurse = $$SUBDIRS
    cov_capture.recurse_target = capture
    QMAKE_EXTRA_TARGETS += cov_capture

    cov_genhtml.CONFIG += recursive
    cov_genhtml.recurse = $$SUBDIRS
    cov_genhtml.recurse_target = genhtml
    QMAKE_EXTRA_TARGETS += cov_genhtml

    coverage.depends += first cov_zerocounters runtests cov_capture cov_genhtml
    QMAKE_EXTRA_TARGETS += coverage
}
