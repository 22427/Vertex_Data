TEMPLATE = app
#CONFIG   += dll
#CONFIG += staticlib
#DEFINES  += BUILDING_DLL

TARGET = vd


CONFIG   -= qt
CONFIG   -= app_bundle



QT       -= core
QT       -= gui




SOURCES += \
    src/main.cpp \
    src/vd.cpp \
    src/vd_mesh.cpp \
    src/vd_mini.cpp

CONFIG += c++11

INCLUDEPATH += ./include


DESTDIR = bin
OBJECTS_DIR = obj

HEADERS += \
	include/vd.h \
	include/vd_mesh.h \
	include/vd_mini.h \
	include/vd_dll.h

MAKEFILE = Makefile_QT
