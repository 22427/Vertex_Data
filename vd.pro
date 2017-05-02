TEMPLATE = lib

TARGET = vd

#CONFIG   += dll
CONFIG += staticlib
CONFIG   -= qt
DEFINES  += PT_BUILDING_DLL


QT       -= core
QT       -= gui


CONFIG   -= app_bundle


SOURCES += \
    src/main.cpp \
    src/vd.cpp \
    src/vd_mesh.cpp \
    src/vd_mini.cpp

CONFIG += c++11

INCLUDEPATH += ./include


DESTDIR = lib
OBJECTS_DIR = obj

HEADERS += \
	include/vd.h \
	include/vd_mesh.h \
	include/vd_mini.h \
	include/vd_dll.h

