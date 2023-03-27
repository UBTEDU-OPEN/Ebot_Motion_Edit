#-------------------------------------------------
#
# Project created by QtCreator 2017-01-09T16:23:19
#
#-------------------------------------------------

QT += gui core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UBXUndoRedoManager
TEMPLATE = lib

DEFINES += UBXUNDOREDOMANAGER_LIBRARY

SOURCES += \
    datamodel/ActionBlockData.cpp \
    datamodel/ActionBlockDataManager.cpp \
    datamodel/ActionBlockGroupManager.cpp \
    ioperation.cpp \
    ../Common/UBXPublic.cpp \
    chandlemotiondata.cpp \
    chandlemarkline.cpp \
    datamodel/cmarkline.cpp \
    ubxundoredomanager.cpp \
    chandletemplatefile.cpp \
    chandleint.cpp \
    chandlestring.cpp \
    datamodel/cstringlistcontainer.cpp

HEADERS +=\
    datamodel/ActionBlockData.h \
    datamodel/ActionBlockDataManager.h \
    datamodel/ActionBlockGroupManager.h \
    ubxundoredomanagerconst.h \
    ioperation.h \
    ../Common/UBXPublic.h \
    chandlemotiondata.h \
    chandlemarkline.h \
    datamodel/cmarkline.h \
    ubxundoredomanager.h \
    ubxundoredomanager_global.h \
    chandletemplatefile.h \
    chandleint.h \
    chandlestring.h \
    datamodel/cstringlistcontainer.h

INCLUDEPATH += $$PWD/../Common

macx:{
LIBS += -L$$OUT_PWD/../UBXBase -lUBXBase \
             -L$$OUT_PWD/../zxLib -lzxLib
}else:win32:{
CONFIG(debug, debug|release){
    LIBS += -L$$OUT_PWD/../UBXBase/debug/ -lUBXBase \
                 -L$$OUT_PWD/../zxLib/debug/ -lzxLib
}else{
    LIBS += -L$$OUT_PWD/../UBXBase/release/ -lUBXBase \
                 -L$$OUT_PWD/../zxLib/release/ -lzxLib
}
}

INCLUDEPATH += $$PWD/../UBXBase
DEPENDPATH += $$PWD/../UBXBase

INCLUDEPATH += $$PWD/../zxLib
DEPENDPATH += $$PWD/../zxLib

unix {
    target.path = /usr/lib
    INSTALLS += target
}
