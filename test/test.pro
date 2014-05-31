#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T17:59:16
#
#-------------------------------------------------

QT       += core gui qml quick testlib core-private

TARGET = meta_meta_object-test
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += \
    ../metaobject.cpp \
    ../object.cpp \
	testmetaobjectgenerator.cpp \
    ../foreignclass.cpp \
    test.cpp

HEADERS += \
    ../metaobject.h \
    ../object.h \
    ../foreignclass.h \
    test.h \
    ../common.h

OTHER_FILES +=

RESOURCES +=
