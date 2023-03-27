#-------------------------------------------------
#
# Project created by QtCreator 2015-12-04T16:40:18
#
#-------------------------------------------------

QT       += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UBXBase
TEMPLATE = lib
DEFINES += UBXBASE_LIBRARY
CONFIG += precompile_header

SOURCES += stable.cpp \
    UBXBase.cpp \
    UBXCritSec.cpp \
    clanguagemgr.cpp
#    BaseBlock.cpp
#    FlowChartBlock.cpp \
#    PortConnetMap.cpp \
#    PortInfo.cpp \
#    PortInfoManager.cpp \
#    UBXBitrate.cpp \
#    UBXSocket.cpp \
#    XMLreader.cpp \
#    ../Common/UBXPublic.cpp

HEADERS +=stable.h \
    UBXBase.h \
    UBXCritSec.h \
    clanguagemgr.h
#    BaseBlock.h
#    AngleTransform.h \
#    FlowChartBlock.h \
#    PortConnetMap.h \
#    PortInfo.h \
#    PortInfoManager.h \
#    UBXBitrate.h \
#    UBXSocket.h \
#    XMLreader.h \
#    ../Common/UBXPublic.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib \
                                  $$PWD/../zxLib/widget
DEPENDPATH += $$PWD/../zxLib

#PRECOMPILED_HEADER=stable.h
