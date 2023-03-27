#-------------------------------------------------
#
# Project created by QtCreator 2016-01-11T14:35:21
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LocalActSyncWidget
TEMPLATE = lib

DEFINES += LOCALACTSYNCWIDGET_EXPORTS
DEFINES += USEXML

CONFIG += precompile_header
PRECOMPILED_HEADER += $$PWD/stdafx.h

INCLUDEPATH += $$PWD/../zxLib \
    $$PWD/../UBXRobotController \
    $$PWD/../UBXRobotController/Robot \
    $$PWD/../Common



macx:{
LIBS += -L$$OUT_PWD/../UBXRobotController -lUBXRobotController
}else:win32:{
CONFIG(debug, debug|release){
    LIBS += -L$$OUT_PWD/../UBXRobotController/Debug -lUBXRobotController
}else{
    LIBS += -L$$OUT_PWD/../UBXRobotController/Release -lUBXRobotController
}
}

SOURCES += stdafx.cpp \
        $$PWD/../Common/Util.cpp \
        $$PWD/../Common/UBXPublic.cpp \
        $$PWD/../Common/QCheckboxHeaderView.cpp \
    ../Common/CRC32.cpp \
    LocalActSyncWidget.cpp \
    ../Common/filedirhandle.cpp

HEADERS  += stdafx.h \
        $$PWD/../UBXRobotController/UBXRobotController.h \
        $$PWD/../Common/Util.h \
        $$PWD/../Common/UBXPublic.h \
        $$PWD/../Common/QCheckboxHeaderView.h \
    ../zxLib/zxLib.h \
    ../Common/CRC32.h \
    LocalActSyncWidget.h \
    ilocalactsyncwidget.h \
    ../Common/filedirhandle.h \
    localactsyncconst.h

FORMS    += \
    LocalActSyncWidget.ui

TRANSLATIONS += ../languages/LocalActSyncWidget_zh_CN.ts \
        ../languages/LocalActSyncWidget_en.ts \
        ../languages/LocalActSyncWidget_zh_HU.ts \


RESOURCES += \
    localactsyncwidget.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib
DEPENDPATH += $$PWD/../zxLib

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/release/ -lUBXBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/debug/ -lUBXBase
else:unix: LIBS += -L$$OUT_PWD/../UBXBase/ -lUBXBase

INCLUDEPATH += $$PWD/../UBXBase
DEPENDPATH += $$PWD/../UBXBase

win32: LIBS += -L$$PWD/../trd/ubtlib/lib/ -lUbtlib

INCLUDEPATH += $$PWD/../trd/ubtlib/include
DEPENDPATH += $$PWD/../trd/ubtlib/include
