TEMPLATE = subdirs

LEVEL = .

!include($$LEVEL/TvHolic.pri):error("Can't load TvHolic.pri")

SUBDIRS += src

# build must be last:
CONFIG += ordered
