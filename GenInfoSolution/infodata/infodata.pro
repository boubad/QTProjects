#-------------------------------------------------
#
# Project created by QtCreator 2016-06-28T11:27:09
#
#-------------------------------------------------

QT       += sql concurrent

QT       -= gui

TARGET = infodata
TEMPLATE = lib
CONFIG += staticlib
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

SOURCES += infodata.cpp \
    qmatricedata.cpp \
    qdistancemap.cpp

HEADERS += infodata.h \
    qmatricedata.h \
    qdistancemap.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
