#-------------------------------------------------
#
# Project created by QtCreator 2015-11-25T17:47:04
#
#-------------------------------------------------

QT       += widgets opengl xml

TARGET = UBXRobot3DView
TEMPLATE = lib

DEFINES += UBXROBOT3DVIEW_LIBRARY
DEFINES += FREEGLUT_STATIC
DEFINES += USEXML

CONFIG += precompile_header
PRECOMPILED_HEADER += $$PWD/stdafx.h

#LIBS += gcc


SOURCES += \
    stdafx.cpp \
    IUBXRobot3DView.cpp \
    ../Common/UBXPublic.cpp \
    ../Common/Util.cpp \
    ../Common/CRC32.cpp \
    ubxrobot3dwidget.cpp \
    ubxrobot3ddisplay.cpp \
    ubxrobot3dcontainer.cpp \
    ubxrobot3dcontrolwidget.cpp \
    ubt3dcontroler.cpp

HEADERS +=\
    stdafx.h \
    ../Common/UBXPublic.h \
    ../Common/Util.h \
    ../Common/CRC32.h \
    ubxrobot3dview_global.h \
    IUBXRobot3DView.h \    
    ubxrobot3ddisplay.h \
    ubxrobot3dwidget.h \
    ubxrobot3dcontainer.h \
    ubxrobot3dcontrolwidget.h \
    ubxrobotcontrolinterface.h \
    $$PWD/../UBXRobotController/UBXRobotController.h \
    ../zxLib/zxLib.h \
    ubt3dcontroler.h


macx:{
LIBS += -L$$OUT_PWD/../UBXRobotController -lUBXRobotController
}else:win32:{
CONFIG(debug, debug|release){
    LIBS += -L$$OUT_PWD/../UBXRobotController/Debug -lUBXRobotController
}else{
    LIBS += -L$$OUT_PWD/../UBXRobotController/Release -lUBXRobotController
}
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib \
                                  $$PWD/../zxLib/widget
DEPENDPATH += $$PWD/../zxLib

INCLUDEPATH += \
    $$PWD/../zxLib \
    $$PWD/../UBXRobotController \
    $$PWD/../UBXRobotController/Robot \
    $$PWD/../Common \
    $$PWD/../trd/freeglut/include \
    $$PWD/../trd/glew/include \
    $$PWD/../trd/fbxsdk/include

macx:{
    LIBS += -L$$PWD/../trd/fbxsdk/lib/osx -lfbxsdk
    PRE_TARGETDEPS += $$PWD/../trd/fbxsdk/lib/osx/libfbxsdk.a

    LIBS += -L$$PWD/../trd/glew/lib/osx -lGLEW64
    PRE_TARGETDEPS += $$PWD/../trd/glew/lib/osx/libGLEW64.a
}else:win32:{
    CONFIG(debug, debug|release){
        LIBS += -L$$PWD/../trd/fbxsdk/lib/win32/debug -llibfbxsdk-md
        PRE_TARGETDEPS += $$PWD/../trd/fbxsdk/lib/win32/debug/libfbxsdk-md.lib

        #LIBS += -L$$PWD/../trd/glew/lib/win32/debug -lglew32sd
        #PRE_TARGETDEPS += $$PWD/../trd/glew/lib/win32/debug/glew32sd.lib
    }else{
        LIBS += -L$$PWD/../trd/fbxsdk/lib/win32/release -llibfbxsdk-md
        PRE_TARGETDEPS += $$PWD/../trd/fbxsdk/lib/win32/release/libfbxsdk-md.lib

        #LIBS += -L$$PWD/../trd/glew/lib/win32/release -lglew32s
        #PRE_TARGETDEPS += $$PWD/../trd/glew/lib/win32/release/glew32s.lib
    }

    DEPENDPATH += $$PWD/../trd/freeglut/lib/win32
    LIBS += -L$$PWD/../trd/freeglut/lib/win32 -lfreeglut_static
    PRE_TARGETDEPS += $$PWD/../trd/freeglut/lib/win32/freeglut_static.lib

    DEPENDPATH += $$PWD/../trd/glew/lib/win32
    LIBS += -L$$PWD/../trd/glew/lib/win32 -lglew32
}




FORMS += \
    ubxrobot3dcontrol.ui \
    ubxrobot3dcontrolwidget.ui

TRANSLATIONS += ../languages/UBXRobot3DView_zh_CN.ts \
        ../languages/UBXRobot3DView_en.ts \
        ../languages/UBXRobot3DView_zh_HU.ts \

macx:
{
    defineTest(copyToDestdir) {
        files = $$1
        DDIR = $$2

        for(FILE, files) {
            #DDIR = $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/MacOS/

            # Replace slashes in paths with backslashes for Windows
            #win32:FILE ~= s,/,\\,g
            #win32:DDIR ~= s,/,\\,g

            QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
        }

        export(QMAKE_POST_LINK)
    }
    #copyToDestdir(libUBXRobot3DView.1.0.0.dylib,$$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/MacOS/)
    #copyToDestdir($$PWD/../languages/UBXRobot3DView_en.qm,$$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources/)
    #copyToDestdir($$PWD/../languages/UBXRobot3DView_zh_CN.qm,$$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources/)
}

RESOURCES += \
    res.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../LibFBX/release/ -lLibFBX
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../LibFBX/debug/ -lLibFBX
else:unix: LIBS += -L$$OUT_PWD/../LibFBX/ -lLibFBX

INCLUDEPATH += $$PWD/../LibFBX
DEPENDPATH += $$PWD/../LibFBX

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../LibFBX/release/libLibFBX.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../LibFBX/debug/libLibFBX.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../LibFBX/release/LibFBX.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../LibFBX/debug/LibFBX.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../LibFBX/libLibFBX.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/release/ -lUBXBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/debug/ -lUBXBase
else:unix: LIBS += -L$$OUT_PWD/../UBXBase/ -lUBXBase

INCLUDEPATH += $$PWD/../UBXBase
DEPENDPATH += $$PWD/../UBXBase
