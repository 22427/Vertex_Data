
QT       += core

QT       -= gui

TARGET = vd
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/vd.cpp

CONFIG += c++11

INCLUDEPATH += ./src ./


DESTDIR = bin
OBJECTS_DIR = obj

HEADERS += \
    src/vd.h

