#-------------------------------------------------
#
# Project created by QtCreator 2015-12-03T13:43:23
#
#-------------------------------------------------

QT       += widgets xml

TARGET = UBXMotionEditor
TEMPLATE = lib
CONFIG += staticlib
DEFINES += USEXML

INCLUDEPATH += \
        $$PWD/../Common \
        $$PWD/../zxLib

SOURCES += \
    UBXMotionEditorImpl.cpp \
    UBXWaveAnalyze.cpp \
    UBXMotionEditor.cpp \
    LayerUI/MotionDataPlay.cpp \
    stable.cpp \
    OptionsMotionWidget.cpp \
    LayerUI/UILayerCommonImpl.cpp \
    LayerUI/UIMotionLayer.cpp \
    UIMainLayerLayout.cpp \
    UIMainLayerLayoutToolsWidget.cpp \
    UBXMotionEditorLayout.cpp \
    UILayerManager.cpp \
    UILayerOptions.cpp \
    LayerUI/UIMusicLayer.cpp \
    LayerUI/UITimeLayer.cpp \
    UIDialogBlockDescription.cpp \
    dialogcommandbox.cpp \
    ../Common/UBXPublic.cpp \
    ../Common/Util.cpp \
    ../Common/cstringhandle.cpp \
    ../Common/csorthandle.cpp \
    LayerUI/cnumautochange.cpp
HEADERS += \
    stable.h \
    UBXMotionEditorImpl.h \
    UBXWaveAnalyze.h \
    UBXMotionEditor.h \
    OptionsMotionWidget.h \
    UIMainLayerLayout.h \
    UIMainLayerLayoutToolsWidget.h\
    LayerUI/ILayerManager.h \
    LayerUI/ILayerOptionHandler.h \
    LayerUI/MotionDataPlay.h \
    LayerUI/UILayerCommonImpl.h \
    LayerUI/UIMotionLayer.h \
    UBXMotionEditorLayout.h \
    LayerUI/ILayerUI.h \
    UILayerManager.h \
    LayerUI/ILayerOption.h \
    UILayerOptions.h \
    LayerUI/UIMusicLayer.h \
    LayerUI/UITimeLayer.h \
    UIDialogBlockDescription.h \
    dialogcommandbox.h \
    LayerUI/IActionLayerManager.h \
    ../Common/UBXPublic.h \
    ../Common/Util.h \
    ../Common/robotDefine.h \
    ../Common/cstringhandle.h \
    ../Common/csorthandle.h \
    LayerUI/cnumautochange.h

RESOURCES += \
    res.qrc


unix { \
    target.path = /usr/lib
    INSTALLS += target
}

CONFIG+=precompile_header
PRECOMPILED_HEADER=stable.h

FORMS += \
    UIOptionMotionWidget.ui \
    UIMainLayerLayout.ui \
    UIMainLayerLayoutToolsWidget.ui \
    UBXMotionEditorLayout.ui \
    UILayerManager.ui \
    UILayerOptions.ui \
    UIDialogBlockDescription.ui \
    dialogcommandbox.ui

TRANSLATIONS += ../languages/UBXMotionEditor_zh_CN.ts \
        ../languages/UBXMotionEditor_en.ts \
        ../languages/UBXMotionEditor_zh_HU.ts \

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/release/ -lUBXBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/debug/ -lUBXBase
else:unix: LIBS += -L$$OUT_PWD/../UBXBase/ -lUBXBase

INCLUDEPATH += $$PWD/../UBXBase
DEPENDPATH += $$PWD/../UBXBase

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/release/libUBXBase.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/debug/libUBXBase.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/release/UBXBase.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/debug/UBXBase.lib
#else:unix: PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/libUBXBase.a

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXRobot3DView/release/ -lUBXRobot3DView
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXRobot3DView/debug/ -lUBXRobot3DView
else:unix: LIBS += -L$$OUT_PWD/../UBXRobot3DView/ -lUBXRobot3DView

INCLUDEPATH += $$PWD/../UBXRobot3DView
DEPENDPATH += $$PWD/../UBXRobot3DView

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXUndoRedoManager/release/ -lUBXUndoRedoManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXUndoRedoManager/debug/ -lUBXUndoRedoManager
else:unix: LIBS += -L$$OUT_PWD/../UBXUndoRedoManager/ -lUBXUndoRedoManager

INCLUDEPATH += $$PWD/../UBXUndoRedoManager
DEPENDPATH += $$PWD/../UBXUndoRedoManager


QMAKE_POST_LINK += \
    rcc -binary $$PWD/res.qrc -o $$PWD/../res/rcc/UBXMotionEditor.rcc
