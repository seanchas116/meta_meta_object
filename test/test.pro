#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T17:59:16
#
#-------------------------------------------------

QT       += core gui qml quick testlib

TARGET = meta_meta_object-test
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += \
    ../metaobject.cpp \
    ../object.cpp \
	testmetaobjectgenerator.cpp \
    ../metaobjectbuilder.cpp \
    test.cpp

HEADERS += \
    ../metaobject.h \
    ../method.h \
    ../object.h \
    ../metaobjectbuilder.h \
    test.h

OTHER_FILES +=

RESOURCES +=
