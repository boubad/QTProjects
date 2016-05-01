#-------------------------------------------------
#
# Project created by QtCreator 2016-04-29T15:12:45
#
#-------------------------------------------------

QT       += network sql

QT       -= gui

TARGET = QStatData
TEMPLATE = lib
CONFIG += staticlib

SOURCES += qstatdata.cpp \
    dbstathelper.cpp \
    qindiv.cpp \
    qdbindivprovider.cpp \
    qindivcluster.cpp \
    qindivtreeelem.cpp

HEADERS += qstatdata.h \
    dbstathelper.h \
    qindiv.h \
    qdbindivprovider.h \
    qindivcluster.h \
    qindivtreeelem.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
