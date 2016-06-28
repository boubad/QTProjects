#-------------------------------------------------
#
# Project created by QtCreator 2016-04-29T15:12:45
#
#-------------------------------------------------

QT       += network sql concurrent

QT       -= gui

TARGET = QStatData
TEMPLATE = lib
CONFIG += staticlib

SOURCES += qstatdata.cpp \
    dbstathelper.cpp \
    qindiv.cpp \
    qdbindivprovider.cpp \
    qindivcluster.cpp \
    qindivtreeelem.cpp \
    qserialindivprovider.cpp \
    qserialindivprovideractor.cpp \
    qworkindivprovider.cpp

HEADERS += qstatdata.h \
    dbstathelper.h \
    qindiv.h \
    qdbindivprovider.h \
    qindivcluster.h \
    qindivtreeelem.h \
    qserialindivprovider.h \
    qserialindivprovideractor.h \
    qworkindivprovider.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
