#-------------------------------------------------
#
# Project created by QtCreator 2015-12-24T11:02:10
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UBXRobotController
TEMPLATE = lib

DEFINES += UBXROBOTCONTROLLER_EXPORTS

CONFIG += precompile_header
PRECOMPILED_HEADER += $$PWD/stdafx.h

INCLUDEPATH += $$PWD/../zxLib \
    $$PWD/Robot \
    $$PWD/../Common \
    $$PWD/../trd/hidsdi/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib
DEPENDPATH += $$PWD/../zxLib
win32:{
LIBS += -L$$PWD/../libs -lsetupapi
LIBS += -L$$PWD/../libs -luser32
LIBS += -L$$PWD/../trd/hidsdi/lib -lhid
}

macx:{
LIBS += -framework IOKit -framework CoreFoundation
}

SOURCES += \
    $$PWD/../Common/UBXPublic.cpp \
    $$PWD/Robot/hid.cpp \
    $$PWD/Robot/DataPool.cpp \
    $$PWD/Robot/MyHidClass.cpp \
    $$PWD/Robot/RobotCmd.cpp \
    $$PWD/Robot/RobotController.cpp \
    $$PWD/stdafx.cpp \
    $$PWD/UBXRobotController.cpp \
    $$PWD/../Common/Util.cpp \
    $$PWD/Robot/CRC32.cpp \
    Robot/crobotcmddata.cpp \
    Robot/crobotcommunicatethread.cpp \
    Robot/ccommandtype.cpp \
    Robot/cbluetoothcomandparser.cpp \
    ../Common/cdataoperate.cpp \
    Robot/robotcmdbtprotocol.cpp \
    ../Common/ubtwifiutil.cpp \
    ../Common/cjsonutil.cpp \
    Robot/ccomandparsernewbt.cpp

HEADERS += \
    $$PWD/../zxLib/zxLib.h \
    $$PWD/../Common/UBXPublic.h \
    $$PWD/Robot/hidapi.h \
    $$PWD/Robot/DataPool.h \
    $$PWD/Robot/IMyPortInterface.h \
    $$PWD/Robot/MyHidClass.h \
    $$PWD/Robot/RobotCmd.h \
    $$PWD/Robot/RobotController.h \
    $$PWD/stdafx.h \
    $$PWD/UBXRobotController.h \
    $$PWD/../Common/Util.h \
    $$PWD/Robot/CRC32.h \
    Robot/ubxrobotcontrolconst.h \
    Robot/crobotcmddata.h \
    Robot/crobotcommunicatethread.h \
    Robot/ccommandtype.h \
    Robot/cbluetoothcomandparser.h \
    ../Common/cdataoperate.h \
    Robot/irobotcmd.h \
    Robot/robotcmdbtprotocol.h \
    ../Common/ubtwifiutil.h \
    ../Common/cjsonutil.h \
    Robot/ccomandparsernewbt.h

win32:{
    HEADERS += $$PWD/Robot/SerialPort.h
    SOURCES += $$PWD/Robot/SerialPort.cpp
}

macx:{
    QMAKE_POST_LINK += \
        install_name_tool -change libzxLib.1.dylib @executable_path/libzxLib.1.dylib libUBXRobotController.1.0.0.dylib
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/release/ -lUBXBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/debug/ -lUBXBase
else:unix: LIBS += -L$$OUT_PWD/../UBXBase/ -lUBXBase

INCLUDEPATH += $$PWD/../UBXBase
DEPENDPATH += $$PWD/../UBXBase
