#-------------------------------------------------
#
# Project created by QtCreator 2016-04-29T15:14:54
#
#-------------------------------------------------

QT       += network sql testlib

QT       -= gui

TARGET = tst_dteststatdatatest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_dteststatdatatest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QStatData/release/ -lQStatData
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QStatData/debug/ -lQStatData
else:unix: LIBS += -L$$OUT_PWD/../QStatData/ -lQStatData

INCLUDEPATH += $$PWD/../QStatData
DEPENDPATH += $$PWD/../QStatData

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QStatData/release/libQStatData.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QStatData/debug/libQStatData.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QStatData/release/QStatData.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QStatData/debug/QStatData.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../QStatData/libQStatData.a

HEADERS +=
