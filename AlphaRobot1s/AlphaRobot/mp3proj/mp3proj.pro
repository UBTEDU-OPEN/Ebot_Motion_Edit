#-------------------------------------------------
#
# Project created by QtCreator 2015-12-07T15:00:42
#
#-------------------------------------------------

QT       += core gui multimedia xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mp3proj
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += \
    $$PWD/../zxLib \
    $$PWD/../Common

INCLUDEPATH += $$PWD/../trd/mpg123/include
INCLUDEPATH += $$PWD/../trd/sndfile/include

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

    LIBS += -L$$PWD/../trd/sndfile/lib/win32 -lsndfile
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib \
                                  $$PWD/../zxLib/widget
DEPENDPATH += $$PWD/../zxLib

CONFIG += precompile_header
PRECOMPILED_HEADER += $$PWD/stdafx.h

SOURCES +=\
    stdafx.cpp \
    $$PWD/../Common/Util.cpp \
    $$PWD/../Common/UBXPublic.cpp \    
    mp3widget.cpp \
    wave.cpp \
    mp3container.cpp \
    mp3player.cpp \
    actionlayer.cpp \
    wavelayer.cpp \
    mainwidget.cpp \
    cmarklineadm.cpp \
    crectselectdrawer.cpp \
    MiniBpm.cpp \
    cimportmp3progress.cpp

HEADERS  += \
    stdafx.h \
    $$PWD/../trd/mpg123/include/intsym.h \
    $$PWD/../Common/Util.h \
    $$PWD/../Common/UBXPublic.h \
    ../zxLib/zxLib.h \
    mp3widget.h \
    wave.h \
    mp3projconstants.h \
    mp3container.h \
    mp3player.h \
    actionlayer.h \
    wavelayer.h \
    mainwidget.h \
    cmarklineadm.h \
    crectselectdrawer.h \
    MiniBpm.h \
    cimportmp3progress.h


FORMS    += \
    mp3widget.ui \
    mp3container.ui \
    mainwidget.ui \
    cimportmp3progress.ui


TRANSLATIONS += ../languages/mp3proj_zh_CN.ts \
        ../languages/mp3proj_en.ts \
        ../languages/mp3proj_zh_HU.ts \

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
    #copyToDestdir($$PWD/../languages/mp3proj_en.qm,$$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources/)
    #copyToDestdir($$PWD/../languages/mp3proj_zh_CN.qm,$$OUT_PWD/../AlphaRobot/AlphaRobot.app/Contents/Resources/)
}

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

RESOURCES += \
    res.qrc

QMAKE_POST_LINK += \
    rcc -binary $$PWD/res.qrc -o $$PWD/../res/rcc/mp3proj.rcc

