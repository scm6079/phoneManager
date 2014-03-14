#-------------------------------------------------
#
# Project created by QtCreator 2014-03-08T00:21:13
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = phoneManager
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    phonemanager.cpp \
    buttonmonitorthread.cpp \
    targa.cpp \
    displaymanager.cpp

HEADERS += \
    phonemanager.h \
    buttonmonitorthread.h \
    targa.h \
    displaymanager.h

LIBS += -lpifacedigital -lmcp23s17
