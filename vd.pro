TEMPLATE = lib

TARGET = vd
CONFIG   += dll

QT       += core

QT       -= gui


CONFIG   -= app_bundle


SOURCES += \
    src/main.cpp \
    src/vd.cpp \
    src/vd_mesh.cpp \
    src/vd_mini.cpp

CONFIG += c++11

INCLUDEPATH += ./src ./


DESTDIR = lib
OBJECTS_DIR = obj

HEADERS += \
    src/vd.h \
    src/vd_mesh.h \
    src/vd_mini.h \
    src/vd_dll.h

