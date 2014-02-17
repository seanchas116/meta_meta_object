#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T17:59:16
#
#-------------------------------------------------

QT       += core gui qml quick testlib

TARGET = foreign-qobject-test
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app


SOURCES += main.cpp \
    ../metaobject.cpp \
    ../object.cpp \
	testmetaobjectgenerator.cpp \
    ../metaobjectbuilder.cpp

HEADERS += \
    ../metaobject.h \
    ../method.h \
    ../object.h \
	testmetaobjectgenerator.h \
    ../metaobjectbuilder.h

OTHER_FILES += \
    testmetaobjectgenerator.qml

RESOURCES += \
    files.qrc
