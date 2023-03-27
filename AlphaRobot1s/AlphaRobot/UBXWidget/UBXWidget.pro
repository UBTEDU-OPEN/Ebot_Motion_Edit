#-------------------------------------------------
#
# Project created by QtCreator 2016-07-02T11:12:34
#
#-------------------------------------------------
QT       += widgets

TARGET = UBXWidget
TEMPLATE = lib

DEFINES += UBXWIDGET_LIBRARY

CONFIG += precompile_header
#PRECOMPILED_HEADER += $$PWD/stdafx.h

INCLUDEPATH += \
    $$PWD/../zxLib \
    $$PWD/../Common

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib
DEPENDPATH += $$PWD/../zxLib

macx:{
    LIBS += -L/usr/lib -liconv
}

TRANSLATIONS +=


unix {
    target.path = /usr/lib
    INSTALLS += target
}
