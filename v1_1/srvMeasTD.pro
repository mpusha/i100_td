#-------------------------------------------------
#
# Project created by QtCreator
# Designer Sergey Sytov 2019
#-------------------------------------------------
# 1.00 begin version
# 1.10 12.02.20 move hw calls in other thread

QT       += core
QT       += network
QT       -= gui

VERSION = 1.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
MOC_DIR = ../moc
OBJECTS_DIR = ../obj
include (../config.pro)
DESTDIR = ../$${CURRENT_BUILD}


TEMPLATE  = app
CONFIG   += console
CONFIG   += app
CONFIG   -= app_bundle
CONFIG   += exception


SOURCES += main.cpp \
    hw.cpp \
    srvBehav.cpp \
    summa.c

HEADERS += \
    hw.h \
    srvBehav.h \
    summa.h \
    request_dev.h


