#-------------------------------------------------
#
# Project created by QtCreator 2016-06-28T14:28:51
#
#-------------------------------------------------

QT       += sql concurrent testlib

QT       -= gui

TARGET = tst_testinfodatatest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

SOURCES += tst_testinfodatatest.cpp \
    infotestdata.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../infodata/release/ -linfodata
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../infodata/debug/ -linfodata
else:unix:!macx: LIBS += -L$$OUT_PWD/../infodata/ -linfodata

INCLUDEPATH += $$PWD/../infodata
DEPENDPATH += $$PWD/../infodata

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../infodata/release/libinfodata.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../infodata/debug/libinfodata.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../infodata/release/infodata.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../infodata/debug/infodata.lib
else:unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../infodata/libinfodata.a

HEADERS += \
    infotestdata.h
