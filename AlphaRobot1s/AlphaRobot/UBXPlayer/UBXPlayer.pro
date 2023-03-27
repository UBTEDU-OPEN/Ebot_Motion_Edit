#-------------------------------------------------
#
# Project created by QtCreator 2016-08-22T16:59:47
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UBXPlayer
TEMPLATE = lib

DEFINES += UBXPLAYER_LIBRARY

CONFIG += precompile_header

SOURCES += \
    ubxvideoplayer.cpp \
    videoplayercontrlwidget.cpp \
    ../Common/UBXPublic.cpp \
    ubxslider.cpp \
    ubxvideooutputitem.cpp \
    ../Common/QtHttpOperSync.cpp \
    ../Common/cbasehttpopersync.cpp \
    ../Common/cdownloadpic.cpp

HEADERS +=\
    ubxplayer_global.h \
    ubxvideoplayer.h \
    ubxplayerconst.h \
    videoplayercontrlwidget.h \
    ../Common/UBXPublic.h \
    ubxslider.h \
    ubxvideooutputitem.h \
    ../Common/QtHttpOperSync.h \
    ../Common/cbasehttpopersync.h \
    ../Common/cdownloadpic.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib
DEPENDPATH += $$PWD/../zxLib

INCLUDEPATH += $$PWD/../Common

TRANSLATIONS += ../languages/UBXPlayer_zh_CN.ts \
        ../languages/UBXPlayer_en.ts \
        ../languages/UBXPlayer_zh_HU.ts \

FORMS += \
    ubxvideoplayer.ui \
    videoplayercontrlwidget.ui

RESOURCES += \
    res.qrc

