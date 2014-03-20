
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = phoneManager
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           phonemanager.cpp \
           buttonmonitorthread.cpp \
           targa.cpp \
           displaymanager.cpp \

HEADERS  += mainwindow.h \
            phonemanager.h \
            buttonmonitorthread.h \
            targa.h \
            displaymanager.h

FORMS    += mainwindow.ui

LIBS += -lpifacedigital -lmcp23s17

macx {
    SOURCES += mockpiface/pifacedigital.c
    HEADERS += mockpiface/pifacedigital.h
    LIBS -= -lpifacedigital -lmcp23s17
    INCLUDEPATH += mockpiface
}
