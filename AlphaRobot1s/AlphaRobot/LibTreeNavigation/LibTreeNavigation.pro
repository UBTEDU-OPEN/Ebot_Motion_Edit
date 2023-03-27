#-------------------------------------------------
#
# Project created by QtCreator 2015-11-25T17:44:56
#
#-------------------------------------------------

QT       += widgets xml

QT       -= gui

TARGET = LibTreeNavigation
TEMPLATE = lib
CONFIG += staticlib


SOURCES += \
    ubxtreewidget.cpp \
    stable.cpp \
    custreewidget.cpp \
    deftreewidget.cpp \
    ctemplateacttreewdg.cpp

HEADERS += ILibTreeNavigation.h \
    ubxtreewidget.h \
    stable.h \
    custreewidget.h \
    deftreewidget.h \
    navigationconst.h \
    ctemplateacttreewdg.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    ubxtreewidget.ui

TRANSLATIONS += ../languages/LibTreeNavigation_zh_CN.ts \
        ../languages/LibTreeNavigation_en.ts \
        ../languages/LibTreeNavigation_zh_HU.ts \

macx: LIBS += -L$$OUT_PWD/../UBXBase -lUBXBase
else:win32: CONFIG(debug, debug|release){
        LIBS += -L$$OUT_PWD/../UBXBase/debug -lUBXBase
    }else{
        LIBS += -L$$OUT_PWD/../UBXBase/release -lUBXBase
    }

INCLUDEPATH += $$PWD/../Common
INCLUDEPATH += $$PWD/../UBXBase
DEPENDPATH += $$PWD/../UBXBase

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib
DEPENDPATH += $$PWD/../zxLib

#macx: PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/libUBXBase.a
#else:win32: CONFIG(debug, debug|release){
#        PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/debug/UBXBase.lib
#    }else{
#        PRE_TARGETDEPS += $$OUT_PWD/../UBXBase/release/UBXBase.lib
#    }

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXUndoRedoManager/release/ -lUBXUndoRedoManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXUndoRedoManager/debug/ -lUBXUndoRedoManager
else:unix: LIBS += -L$$OUT_PWD/../UBXUndoRedoManager/ -lUBXUndoRedoManager

INCLUDEPATH += $$PWD/../UBXUndoRedoManager
DEPENDPATH += $$PWD/../UBXUndoRedoManager

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXMotionEditor/release/ -lUBXMotionEditor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXMotionEditor/debug/ -lUBXMotionEditor
else:unix: LIBS += -L$$OUT_PWD/../UBXMotionEditor/ -lUBXMotionEditor

INCLUDEPATH += $$PWD/../UBXMotionEditor
DEPENDPATH += $$PWD/../UBXMotionEditor

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXMotionEditor/release/libUBXMotionEditor.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXMotionEditor/debug/libUBXMotionEditor.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXMotionEditor/release/UBXMotionEditor.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../UBXMotionEditor/debug/UBXMotionEditor.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../UBXMotionEditor/libUBXMotionEditor.a

RESOURCES += \
    treenavigation.qrc

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

QMAKE_POST_LINK += \
    rcc -binary $$PWD/treenavigation.qrc -o $$PWD/../res/rcc/treenavigation.rcc
