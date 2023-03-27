#-------------------------------------------------
#
# Project created by QtCreator 2015-11-03T17:33:27
#
#-------------------------------------------------

QT       += core gui xml opengl multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32:{
    CONFIG(release, debug|release){
         QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"
    }
}


TARGET = AlphaRobot
TEMPLATE = app

DEFINES += USEVERSION
DEFINES += USEXML

win32:{
#    DEFINES += USE_VLD_DEBUG
}

CONFIG += precompile_header
PRECOMPILED_HEADER += $$PWD/stdafx.h

INCLUDEPATH += \
    $$PWD/../zxLib \
    $$PWD/../zxLib/widget \
    $$PWD/../UBXRobotController \
    $$PWD/../UBXRobotController/Robot \
    $$PWD/../Common \

INCLUDEPATH += $$PWD/../qtsingleapplication/src
include($$PWD/../qtsingleapplication/src/qtsingleapplication.pri)

SOURCES += \
        stdafx.cpp \
        $$PWD/../Common/Util.cpp \
        $$PWD/../Common/UBXPublic.cpp  \
        main.cpp \
    dialogabout.cpp \
    titlebar.cpp \
    mainwindowwidget.cpp \
    ../Common/moviebutton.cpp \
    dialogsetting.cpp \
    languagesettingwidget.cpp \
    ../Common/CRC32.cpp \
    filessaveasui.cpp \
    ../Common/htsparser.cpp \
    ../Common/filedirhandle.cpp \
    cmyalpha.cpp \
    robotcontrolwidget.cpp \
    ../Common/cactmixtureparse.cpp \
    ../Common/cdataoperate.cpp \
    ../Common/cxmlhandle.cpp \
    robottypesettingwdg.cpp \
    crobotproducttypewdg.cpp \
    setting/cwifiinfowidget.cpp \
    ../Common/clineedit.cpp

HEADERS  += \
    stdafx.h \
    $$PWD/../trd/mpg123/include/intsym.h \
    $$PWD/../zxLib/zxLib.h \
    $$PWD/../Common/Util.h \
    $$PWD/../Common/UBXPublic.h \
    $$PWD/../UBXRobotController/UBXRobotController.h \
    dialogabout.h \
    titlebar.h \
    mainwindowwidget.h \
    ../Common/moviebutton.h \
    dialogsetting.h \
    languagesettingwidget.h \
    alpharobotconst.h \
    ../Common/CRC32.h \
    ../Common/singleton.h \
    filessaveasui.h \
    ../Common/htsparser.h \
    ../Common/filedirhandle.h \
    cmyalpha.h \
    robotcontrolwidget.h \
    ../Common/cactmixtureparse.h \
    ../Common/cdataoperate.h \
    ../Common/cxmlhandle.h \
    robottypesettingwdg.h \
    crobotproducttypewdg.h \
    setting/cwifiinfowidget.h \
    ../Common/clineedit.h


FORMS    += \
    dialogabout.ui \
    titlebar.ui \
    mainwindowwidget.ui \
    dialogsetting.ui \
    languagesettingwidget.ui \
    filessaveasui.ui \
    cmyalpha.ui \
    robotcontrolwidget.ui \
    robottypesettingwdg.ui \
    crobotproducttypewdg.ui \
    setting/cwifiinfowidget.ui

TRANSLATIONS += ../languages/AlphaRobot_zh_CN.ts \
        ../languages/AlphaRobot_en.ts \
        ../languages/AlphaRobot_zh_HU.ts


RESOURCES += \
    res.qrc

macx:{
    ICON = $$PWD/../res/AlphaRobot.icns
}else:win32:{
    #    RC_ICONS = $$PWD/../res/AlphaRobot.ico
    RC_FILE = $$PWD/../res/AlphaRobot.rc
}

QMAKE_INFO_PLIST = ../res/AlphaRobotInfo.plist

macx:{
    enLanguage.files = ../languages/AlphaRobot_en.qm
    enLanguage.path = Contents/Resources
    QMAKE_BUNDLE_DATA += enLanguage

    zhLanguage.files = ../languages/AlphaRobot_zh_CN.qm
    zhLanguage.path = Contents/Resources
    QMAKE_BUNDLE_DATA += zhLanguage

    huLanguage.files = ../languages/AlphaRobot_zh_HU.qm
    huLanguage.path = Contents/Resources
    QMAKE_BUNDLE_DATA += huLanguage
}

INCLUDEPATH += $$PWD/../trd/mpg123/include
INCLUDEPATH += $$PWD/../trd/sndfile/include

win32:{
    CONFIG(debug, debug|release){
        LIBS += -L$$PWD/../trd/VisualLeakDetector/lib/Win32 -lvld -lvld
        INCLUDEPATH += $$PWD/../trd/VisualLeakDetector/include
    }
}

macx:{    
    LIBS += -L$$PWD/../trd/mpg123/lib/osx -lmpg123.0
    PRE_TARGETDEPS += $$PWD/../trd/mpg123/lib/osx/libmpg123.a

    LIBS += -L$$PWD/../trd/sndfile/lib/osx -lsndfile.1
}else:win32:{    
    CONFIG(debug, debug|release){
        LIBS += -L$$PWD/../trd/mpg123/lib/win32/debug -llibmpg123
        PRE_TARGETDEPS += $$PWD/../trd/mpg123/lib/win32/debug/libmpg123.lib
    }else{
        LIBS += -L$$PWD/../trd/mpg123/lib/win32/release -llibmpg123
        PRE_TARGETDEPS += $$PWD/../trd/mpg123/lib/win32/release/libmpg123.lib
    }

    LIBS += -L$$PWD/../trd/sndfile/lib/win32 -llibsndfile-1

    CONFIG(debug, debug|release){
        QMAKE_POST_LINK += \
            copy /y  \"$$PWD\..\trd\sndfile\lib\win32\*.dll\"      \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$PWD\..\trd\glew\lib\win32\*.dll\"       \"$$OUT_PWD\debug\*.dll\"
    }else{
        QMAKE_POST_LINK += \
            copy /y  \"$$PWD\..\trd\sndfile\lib\win32\*.dll\"      \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$PWD\..\trd\glew\lib\win32\*.dll\"       \"$$OUT_PWD\release\*.dll\"
    }

    LIBS += -L$$PWD/../libs -lversion
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../LibTreeNavigation/release/ -lLibTreeNavigation
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../LibTreeNavigation/debug/ -lLibTreeNavigation
else:unix: LIBS += -L$$OUT_PWD/../LibTreeNavigation/ -lLibTreeNavigation

INCLUDEPATH += $$PWD/../LibTreeNavigation
DEPENDPATH += $$PWD/../LibTreeNavigation

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../LibTreeNavigation/release/libLibTreeNavigation.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../LibTreeNavigation/debug/libLibTreeNavigation.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../LibTreeNavigation/release/LibTreeNavigation.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../LibTreeNavigation/debug/LibTreeNavigation.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../LibTreeNavigation/libLibTreeNavigation.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXMotionEditor/release/ -lUBXMotionEditor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXMotionEditor/debug/ -lUBXMotionEditor
else:unix: LIBS += -L$$OUT_PWD/../UBXMotionEditor/ -lUBXMotionEditor

INCLUDEPATH += $$PWD/../UBXMotionEditor
DEPENDPATH += $$PWD/../UBXMotionEditor

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXUndoRedoManager/release/ -lUBXUndoRedoManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXUndoRedoManager/debug/ -lUBXUndoRedoManager
else:unix: LIBS += -L$$OUT_PWD/../UBXUndoRedoManager/ -lUBXUndoRedoManager

INCLUDEPATH += $$PWD/../UBXUndoRedoManager
DEPENDPATH += $$PWD/../UBXUndoRedoManager

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXMotionEditor/release/libUBXMotionEditor.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXMotionEditor/debug/libUBXMotionEditor.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXMotionEditor/release/UBXMotionEditor.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXMotionEditor/debug/UBXMotionEditor.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../UBXMotionEditor/libUBXMotionEditor.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/release/ -lUBXBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/debug/ -lUBXBase
else:unix: LIBS += -L$$OUT_PWD/../UBXBase/ -lUBXBase

INCLUDEPATH += $$PWD/../UBXBase
DEPENDPATH += $$PWD/../UBXBase

#win32-g++:CONFIG(release, debug|release): LIBS += $$OUT_PWD/../UBXBase/release/libUBXBase.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/debug/libUBXBase.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/release/UBXBase.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/debug/UBXBase.lib
#else:unix: PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/libUBXBase.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXRobot3DView/release/ -lUBXRobot3DView
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXRobot3DView/debug/ -lUBXRobot3DView
else:unix: LIBS += -L$$OUT_PWD/../UBXRobot3DView/ -lUBXRobot3DView

INCLUDEPATH += $$PWD/../UBXRobot3DView
DEPENDPATH += $$PWD/../UBXRobot3DView

macx:{
    QMAKE_POST_LINK += \
        cp -a  $$OUT_PWD/../UBXRobot3DView/libUBXRobot3DView.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXRobot3DView.1.dylib && \
        install_name_tool -change libUBXRobot3DView.1.dylib @executable_path/libUBXRobot3DView.1.dylib $${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET}
}else:win32:{
    CONFIG(debug, debug|release){
        QMAKE_POST_LINK += && \
            copy /y  \"$$OUT_PWD\..\UBXRobot3DView\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\"
    }else{
        QMAKE_POST_LINK += && \
            copy /y  \"$$OUT_PWD\..\UBXRobot3DView\release\*.dll\" \"$$OUT_PWD\release\*.dll\"
    }
}

# UBXPlayer Start
INCLUDEPATH += $$PWD/../UBXPlayer
macx:{
    LIBS += -L$$OUT_PWD/../UBXPlayer -lUBXPlayer
}else:win32:{
    CONFIG(debug, debug|release){
        DEPENDPATH += $$OUT_PWD/../UBXPlayer/debug
        LIBS += -L$$OUT_PWD/../UBXPlayer/debug -lUBXPlayer
    }else{
        DEPENDPATH += $$OUT_PWD/../UBXPlayer/release
        LIBS += -L$$OUT_PWD/../UBXPlayer/release -lUBXPlayer
    }
}
#UBXPlayer End

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../mp3proj/release/ -lmp3proj
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../mp3proj/debug/ -lmp3proj
else:unix: LIBS += -L$$OUT_PWD/../mp3proj/ -lmp3proj

INCLUDEPATH += $$PWD/../mp3proj
DEPENDPATH += $$PWD/../mp3proj

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../mp3proj/release/libmp3proj.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../mp3proj/debug/libmp3proj.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../mp3proj/release/mp3proj.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../mp3proj/debug/mp3proj.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../mp3proj/libmp3proj.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXLogin/release/ -lUBXLogin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXLogin/debug/ -lUBXLogin
else:macx: LIBS += -L$$OUT_PWD/../UBXLogin/ -lUBXLogin

INCLUDEPATH += $$PWD/../UBXLogin
DEPENDPATH += $$PWD/../UBXLogin

macx:{
    QMAKE_POST_LINK += && \
        cp -a  $$OUT_PWD/../UBXLogin/libUBXLogin.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXLogin.1.dylib && \
        install_name_tool -change libUBXLogin.1.dylib @executable_path/libUBXLogin.1.dylib $${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET}  && \
        install_name_tool -change libzxLib.1.dylib @executable_path/libzxLib.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXLogin.1.dylib
}else:win32:{
    CONFIG(debug, debug|release){
        QMAKE_POST_LINK += && \
            copy /y  \"$$OUT_PWD\..\UBXLogin\Debug\*.dll\"          \"$$OUT_PWD\debug\*.dll\"

    }else{
        QMAKE_POST_LINK += && \
            copy /y  \"$$OUT_PWD\..\UBXLogin\Release\*.dll\"          \"$$OUT_PWD\release\*.dll\"
    }
}

macx:{
    QMAKE_POST_LINK += && \
        cp -a  $$OUT_PWD/../zxLib/libzxLib.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libzxLib.1.dylib  && \
        cp -a  $$OUT_PWD/../UBXRobotController/libUBXRobotController.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXRobotController.1.dylib && \
        cp -a  $$PWD/../trd/fbxsdk/lib/osx/libfbxsdk.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libfbxsdk.dylib && \
        cp -a  $$PWD/../res/Robot $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents  && \
        install_name_tool -change libzxLib.1.dylib @executable_path/libzxLib.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET}
}else:win32:{
    CONFIG(debug, debug|release){
        QMAKE_POST_LINK += && \
            copy /y  \"$$OUT_PWD\..\zxLib\debug\*.dll\"          \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$OUT_PWD\..\UBXRobotController\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            xcopy /y /s /e /i  \"$$PWD\..\res\Robot\"                \"$$OUT_PWD\debug\Robot\"

    }else{
        QMAKE_POST_LINK += && \
            copy /y  \"$$OUT_PWD\..\zxLib\release\*.dll\"          \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$OUT_PWD\..\UBXRobotController\release\*.dll\" \"$$OUT_PWD\release\*.dll\" && \
            xcopy /y /s /e /i  \"$$PWD\..\res\Robot\"                  \"$$OUT_PWD\release\Robot\"
    }
}


INCLUDEPATH += $$PWD/../LocalActSyncWidget
macx:{
    LIBS += -L$$OUT_PWD/../LocalActSyncWidget -lLocalActSyncWidget
}else:win32:{
    CONFIG(debug, debug|release){
        DEPENDPATH += $$OUT_PWD/../LocalActSyncWidget/debug
        LIBS += -L$$OUT_PWD/../LocalActSyncWidget/debug -lLocalActSyncWidget
    }else{
        DEPENDPATH += $$OUT_PWD/../LocalActSyncWidget/release
        LIBS += -L$$OUT_PWD/../LocalActSyncWidget/release -lLocalActSyncWidget
    }
}

INCLUDEPATH += $$PWD/../UBXActionLib
macx:{
    LIBS += -L$$OUT_PWD/../UBXActionLib -lUBXActionLib
}else:win32:{
    CONFIG(debug, debug|release){
        DEPENDPATH += $$OUT_PWD/../UBXActionLib/debug
        LIBS += -L$$OUT_PWD/../UBXActionLib/debug -lUBXActionLib
    }else{
        DEPENDPATH += $$OUT_PWD/../UBXActionLib/release
        LIBS += -L$$OUT_PWD/../UBXActionLib/release -lUBXActionLib
    }
}

INCLUDEPATH += $$PWD/../UBXUpdateModule
macx:{
    LIBS += -L$$OUT_PWD/../UBXUpdateModule -lUBXUpdateModule
}else:win32:{
    CONFIG(debug, debug|release){
        DEPENDPATH += $$OUT_PWD/../UBXUpdateModule/debug
        LIBS += -L$$OUT_PWD/../UBXUpdateModule/debug -lUBXUpdateModule
    }else{
        DEPENDPATH += $$OUT_PWD/../UBXUpdateModule/release
        LIBS += -L$$OUT_PWD/../UBXUpdateModule/release -lUBXUpdateModule
    }
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib
DEPENDPATH += $$PWD/../zxLib

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXRobotController/release/ -lUBXRobotController
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXRobotController/debug/ -lUBXRobotController
else:unix: LIBS += -L$$OUT_PWD/../UBXRobotController/ -lUBXRobotController

INCLUDEPATH += $$PWD/../UBXRobotController
DEPENDPATH += $$PWD/../UBXRobotController


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../trd/qiniu/lib/win32/release -lqiniu
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../trd/qiniu/lib/win32/debug -lqiniu
#else:unix: LIBS += -L$$PWD/../trd/qiniu/lib/osx -lqiniu

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../trd/qiniu/lib/win32/release/libqiniu.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../trd/qiniu/lib/win32/debug/libqiniu.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../trd/qiniu/lib/win32/release/qiniu.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../trd/qiniu/lib/win32/debug/qiniu.lib
#else:unix: PRE_TARGETDEPS += $$PWD/../trd/qiniu/lib/osx/libqiniu.a

#INCLUDEPATH += $$PWD/../trd/qiniu/include


macx:{
    QMAKE_POST_LINK += && \
        cp -a  $$OUT_PWD/../LocalActSyncWidget/libLocalActSyncWidget.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libLocalActSyncWidget.1.dylib && \
        install_name_tool -change libUBXRobotController.1.dylib @executable_path/libUBXRobotController.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET} && \
        install_name_tool -change libLocalActSyncWidget.1.dylib @executable_path/libLocalActSyncWidget.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET} && \
        install_name_tool -change libzxLib.1.dylib @executable_path/libzxLib.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libLocalActSyncWidget.1.dylib && \
        install_name_tool -change libUBXRobotController.1.dylib @executable_path/libUBXRobotController.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libLocalActSyncWidget.1.dylib && \
        install_name_tool -change libzxLib.1.dylib @executable_path/libzxLib.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXRobot3DView.1.dylib && \
        install_name_tool -change libUBXRobotController.1.dylib @executable_path/libUBXRobotController.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXRobot3DView.1.dylib  && \
        cp -a  $$OUT_PWD/../UBXActionLib/libUBXActionLib.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXActionLib.1.dylib && \
        install_name_tool -change libUBXActionLib.1.dylib @executable_path/libUBXActionLib.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET} && \
        install_name_tool -change libzxLib.1.dylib @executable_path/libzxLib.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXActionLib.1.dylib && \
        cp -a  $$PWD/../trd/quazip/lib/osx/libquazip.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libquazip.1.dylib && \
        install_name_tool -change libquazip.1.dylib @executable_path/libquazip.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXActionLib.1.dylib && \
#        cp -a  $$PWD/../trd/qiniu/lib/osx/libqiniu.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libqiniu.1.dylib && \
#        install_name_tool -change libqiniu.1.dylib @executable_path/libqiniu.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXActionLib.1.dylib && \
        cp -a  $$OUT_PWD/../UBXUpdateModule/libUBXUpdateModule.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXUpdateModule.1.dylib && \
        install_name_tool -change libUBXUpdateModule.1.dylib @executable_path/libUBXUpdateModule.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET} && \
        install_name_tool -change libUBXRobotController.1.dylib @executable_path/libUBXRobotController.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXUpdateModule.1.dylib && \
        install_name_tool -change libzxLib.1.dylib @executable_path/libzxLib.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXUpdateModule.1.dylib && \
        install_name_tool -change libUBXBase.1.dylib @executable_path/libUBXBase.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXUpdateModule.1.dylib && \
        install_name_tool -change libUBXLogin.1.dylib @executable_path/libUBXLogin.1.dylib $${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXActionLib.1.dylib && \
        cp -a $$PWD/../trd/mpg123/lib/osx/libmpg123.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS && \
        install_name_tool -change libmpg123.0.dylib @executable_path/libmpg123.0.dylib $${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET} && \
        cp -a $$PWD/../trd/sndfile/lib/osx/*.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS && \
        install_name_tool -change libsndfile.1.dylib @executable_path/libsndfile.1.dylib $${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET} &&\
        cp -a  $$OUT_PWD/../UBXPlayer/libUBXPlayer.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXPlayer.1.dylib && \
        install_name_tool -change libzxLib.1.dylib @executable_path/libzxLib.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXPlayer.1.dylib && \
        install_name_tool -change libUBXPlayer.1.dylib @executable_path/libUBXPlayer.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS//libUBXActionLib.1.dylib && \
        install_name_tool -change libUBXPlayer.1.dylib @executable_path/libUBXPlayer.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET} && \
        cp -a  $$OUT_PWD/../UBXUndoRedoManager/libUBXUndoRedoManager.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXUndoRedoManager.1.dylib && \
        install_name_tool -change libzxLib.1.dylib @executable_path/libzxLib.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXUndoRedoManager.1.dylib && \
        install_name_tool -change libUBXUndoRedoManager.1.dylib @executable_path/libUBXUndoRedoManager.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET} && \
        cp -a  $$OUT_PWD/../UBXBase/libUBXBase.1.0.0.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXBase.1.dylib && \
        install_name_tool -change libzxLib.1.dylib @executable_path/libzxLib.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXBase.1.dylib && \
        install_name_tool -change libUBXBase.1.dylib @executable_path/libUBXBase.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/$${TARGET} && \
        install_name_tool -change libUBXBase.1.dylib @executable_path/libUBXBase.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXUndoRedoManager.1.dylib && \
        install_name_tool -change libUBXBase.1.dylib @executable_path/libUBXBase.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXRobot3DView.1.dylib && \
        install_name_tool -change libUBXBase.1.dylib @executable_path/libUBXBase.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXRobotController.1.dylib && \
        install_name_tool -change libUBXBase.1.dylib @executable_path/libUBXBase.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXLogin.1.dylib  && \
        install_name_tool -change libUBXBase.1.dylib @executable_path/libUBXBase.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libLocalActSyncWidget.1.dylib
#        install_name_tool -change libUBXBase.1.dylib @executable_path/libUBXBase.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libLibFBX.1.dylib && \
#        install_name_tool -change libUBXBase.1.dylib @executable_path/libUBXBase.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libLibTreeNavigation.1.dylib  && \
#        install_name_tool -change libUBXBase.1.dylib @executable_path/libUBXBase.1.dylib $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/MacOS/libUBXMotionEditor.1.dylib

}else:win32:{
    CONFIG(debug, debug|release){
        QMAKE_POST_LINK += && \
            copy /y  \"$$OUT_PWD\..\UBXBase\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\"  && \
            copy /y  \"$$OUT_PWD\..\LocalActSyncWidget\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\"  && \
            copy /y  \"$$OUT_PWD\..\UBXActionLib\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\"  && \
            copy /y  \"$$OUT_PWD\..\UBXUpdateModule\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\"  && \
            copy /y  \"$$PWD\..\trd\quazip\lib\win32\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$PWD\..\trd\qiniu\lib\win32\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$OUT_PWD\..\UBXPlayer\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$OUT_PWD\..\UBXUndoRedoManager\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\"

    }else{
        QMAKE_POST_LINK += && \
            copy /y  \"$$OUT_PWD\..\UBXBase\release\*.dll\" \"$$OUT_PWD\release\*.dll\"  && \
            copy /y  \"$$OUT_PWD\..\LocalActSyncWidget\release\*.dll\" \"$$OUT_PWD\release\*.dll\"  && \
            copy /y  \"$$OUT_PWD\..\UBXActionLib\release\*.dll\" \"$$OUT_PWD\release\*.dll\"  && \
            copy /y  \"$$OUT_PWD\..\UBXUpdateModule\release\*.dll\" \"$$OUT_PWD\release\*.dll\"  && \
            copy /y  \"$$PWD\..\trd\quazip\lib\win32\release\*.dll\" \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$PWD\..\trd\qiniu\lib\win32\release\*.dll\" \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$OUT_PWD\..\UBXPlayer\release\*.dll\" \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$OUT_PWD\..\UBXUndoRedoManager\release\*.dll\" \"$$OUT_PWD\release\*.dll\"
    }
}

macx:{
    QMAKE_POST_LINK += && \
        mkdir -p $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/res  && \
        mkdir -p $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/defaultTemplate  && \
        cp -r  $$PWD/../res/defaultTemplate/ $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/defaultTemplate && \
        cp -a  $$PWD/../res/image/*.* $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/res && \
        cp -a  $$PWD/../res/image/display $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/res/display && \
        cp -a  $$PWD/../res/*.* $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/res && \
        cp -a  $$PWD/../res/rcc/*.* $$OUT_PWD/$${TARGET}.$$TEMPLATE/Contents/res && \
        cp -a  $$PWD/../languages/qt_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/qt_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/mp3proj_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/mp3proj_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/mp3proj_zh_HU.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXPlayer_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXPlayer_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXPlayer_zh_HU.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/zxLib_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/zxLib_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/zxLib_zh_HU.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXRobot3DView_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXRobot3DView_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXRobot3DView_zh_HU.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXMotionEditor_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXMotionEditor_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXMotionEditor_zh_HU.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/LocalActSyncWidget_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/LocalActSyncWidget_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/LocalActSyncWidget_zh_HU.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/LibTreeNavigation_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/LibTreeNavigation_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/LibTreeNavigation_zh_HU.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXActionLib_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXActionLib_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXActionLib_zh_HU.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXLogin_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXLogin_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXLogin_zh_HU.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXUpdateModule_en.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXUpdateModule_zh_CN.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources && \
        cp -a  $$PWD/../languages/UBXUpdateModule_zh_HU.qm $$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources
}else:win32:{
    CONFIG(debug, debug|release){
        QMAKE_POST_LINK += && \
            xcopy /y /s /e /i  \"$$PWD\..\res\defaultTemplate\*.*\" \"$$OUT_PWD\debug\defaultTemplate\" && \
            xcopy /y /s /e /i  \"$$PWD\..\res\image\*.*\" \"$$OUT_PWD\debug\res\" && \
            xcopy /y /s /e /i  \"$$PWD\..\res\*.ico\" \"$$OUT_PWD\debug\res\" && \
            xcopy /y /s /e /i  \"$$PWD\..\res\*.bin\" \"$$OUT_PWD\debug\res\" && \
            xcopy /y /s /e /i  \"$$PWD\..\res\rcc\*.*\" \"$$OUT_PWD\debug\res\" && \
            xcopy /y /s /e /i  \"$$PWD\..\languages\*.qm\" \"$$OUT_PWD\debug\languages\"

    }else{
        QMAKE_POST_LINK += && \
            xcopy /y /s /e /i  \"$$PWD\..\res\defaultTemplate\*.*\" \"$$OUT_PWD\release\defaultTemplate\" && \
            xcopy /y /s /e /i  \"$$PWD\..\res\image\*.*\" \"$$OUT_PWD\release\res\" && \
            xcopy /y /s /e /i  \"$$PWD\..\res\*.ico\" \"$$OUT_PWD\release\res\" && \
            copy /y  \"$$PWD\..\res\*.bin\" \"$$OUT_PWD\release\res\" && \
            xcopy /y /s /e /i  \"$$PWD\..\res\rcc\*.*\" \"$$OUT_PWD\release\res\" && \
            xcopy /y /s /e /i  \"$$PWD\..\languages\*.qm\" \"$$OUT_PWD\release\languages\"
    }
}

macx:{
    ##package app，putin at the end
    CONFIG(release, debug|release){
        QMAKE_POST_LINK += && \
            macdeployqt $$OUT_PWD/$${TARGET}.$$TEMPLATE -verbose=1
    }
}else:win32:{
    CONFIG(debug, debug|release){
        QMAKE_POST_LINK += && \
            copy /y  \"$$(QTDIR)\bin\qt5cored.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5opengld.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5widgetsd.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5guid.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5xmld.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5multimediad.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\Qt5MultimediaWidgetsd.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5networkd.dll\" \"$$OUT_PWD\debug\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\mediaservice\dsengined.dll\" \"$$OUT_PWD\debug\mediaservice\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\mediaservice\qtmedia_audioengined.dll\" \"$$OUT_PWD\debug\mediaservice\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\mediaservice\wmfengined.dll\" \"$$OUT_PWD\debug\mediaservice\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\platforms\qminimald.dll\" \"$$OUT_PWD\debug\platforms\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\platforms\qoffscreend.dll\" \"$$OUT_PWD\debug\platforms\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\platforms\qwindowsd.dll\" \"$$OUT_PWD\debug\platforms\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\imageformats\qicod.dll\" \"$$OUT_PWD\debug\imageformats\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\imageformats\qjpegd.dll\" \"$$OUT_PWD\debug\imageformats\*.dll\" && \
            copy /y  \"$$PWD\..\..\lib\debug\*.dll\" \"$$OUT_PWD\debug\*.dll\"
    }else{
        QMAKE_POST_LINK += && \
            copy /y  \"$$(QTDIR)\bin\qt5core.dll\" \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5opengl.dll\" \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5widgets.dll\" \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5gui.dll\" \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5xml.dll\" \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5multimedia.dll\" \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\Qt5MultimediaWidgets.dll\" \"$$OUT_PWD\release\*.dll\" && \
            copy /y  \"$$(QTDIR)\bin\qt5network.dll\" \"$$OUT_PWD\release\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\mediaservice\dsengine.dll\" \"$$OUT_PWD\release\mediaservice\*.dll\" && \
            xcopy /y /s /e /i \"$$(QTDIR)\plugins\mediaservice\qtmedia_audioengine.dll\" \"$$OUT_PWD\release\mediaservice\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\mediaservice\wmfengine.dll\" \"$$OUT_PWD\release\mediaservice\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\platforms\qminimal.dll\" \"$$OUT_PWD\release\platforms\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\platforms\qoffscreen.dll\" \"$$OUT_PWD\release\platforms\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\platforms\qwindows.dll\" \"$$OUT_PWD\release\platforms\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\imageformats\qico.dll\" \"$$OUT_PWD\release\imageformats\*.dll\" && \
            xcopy /y /s /e /i  \"$$(QTDIR)\plugins\imageformats\qjpeg.dll\" \"$$OUT_PWD\release\imageformats\*.dll\" && \
            copy /y  \"$$PWD\..\..\lib\release\*.dll\" \"$$OUT_PWD\release\*.dll\"
    }
}

DISTFILES += \
    ../res/AlphaRobot.icns \
    ../res/AlphaRobotInfo.plist

win32: LIBS += -L$$PWD/../trd/ubtlib/lib/ -lUbtlib

INCLUDEPATH += $$PWD/../trd/ubtlib/include
DEPENDPATH += $$PWD/../trd/ubtlib/include
