#-------------------------------------------------
#
# Project created by QtCreator 2016-02-22T11:29:40
#
#-------------------------------------------------

QT       += network widgets xml

#QT       -= gui

TARGET = UBXLogin
TEMPLATE = lib

DEFINES += UBXLOGIN_LIBRARY

CONFIG += precompile_header
PRECOMPILED_HEADER += $$PWD/stdafx.h

INCLUDEPATH += \
    $$PWD/../zxLib \
    $$PWD/../Common



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib
DEPENDPATH += $$PWD/../zxLib

macx:{
    LIBS += -L/usr/lib -liconv
}

TRANSLATIONS += ../languages/UBXLogin_zh_CN.ts \
        ../languages/UBXLogin_en.ts \
        ../languages/UBXLogin_zh_HU.ts \

SOURCES += ubxlogin.cpp \
    stdafx.cpp \
    ../Common/MD5.cpp \
    ../Common/Util.cpp \
    dialogloginpage.cpp \
    dialogusermessageedit.cpp \
    ../Common/QtHttpOper.cpp \
    loginlibmgr.cpp \
    ../Common/QtHttpOperSync.cpp \
    ../Common/UBXPublic.cpp \
#    ../Common/Json/json_internalarray.inl \
#    ../Common/Json/json_internalmap.inl \
    ../Common/Json/json_reader.cpp \
    ../Common/Json/json_value.cpp \
#    ../Common/Json/json_valueiterator.inl \
    ../Common/Json/json_writer.cpp \
    widgetlogin.cpp \
    widgetregister.cpp \
    widgetfindpassword.cpp \
    widgetresetpassword.cpp \
    subwidgetpfindpassword.cpp \
    subwidgetphasfindpassword.cpp \
    subwidgetmfindpassword.cpp \
    subwidgetmhasfindpassword.cpp \
    ../Common/simplecrypt.cpp \
    ../Common/widget/mycombobox.cpp \
    ../Common/cbasehttpopersync.cpp

HEADERS += ubxlogin.h\
        ubxlogin_global.h \
    ILibLogin.h \
    stdafx.h \
    ../Common/MD5.h \
    ../Common/Util.h \
    ../zxLib/zxLib.h \
    dialogloginpage.h \
    dialogusermessageedit.h \
    ../Common/QtHttpOper.h \
    loginlibmgr.h \
    ../Common/QtHttpOperSync.h \
    ../Common/UBXPublic.h \
    ../Common/Json/json_internalarray.inl \
    ../Common/Json/json_internalmap.inl \
    ../Common/Json/json_valueiterator.inl \
    ../Common/Json/autolink.h \
    ../Common/Json/config.h \
    ../Common/Json/features.h \
    ../Common/Json/forwards.h \
    ../Common/Json/json_batchallocator.h \
    ../Common/Json/json.h \
    ../Common/Json/reader.h \
    ../Common/Json/value.h \
    ../Common/Json/writer.h \
    widgetlogin.h \
    widgetregister.h \
    widgetfindpassword.h \
    widgetresetpassword.h \
    subwidgetpfindpassword.h \
    subwidgetphasfindpassword.h \
    subwidgetmfindpassword.h \
    subwidgetmhasfindpassword.h\
    loginconst.h \
    ../Common/simplecrypt.h \
    ../Common/widget/mycombobox.h \
    ../Common/cbasehttpopersync.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    dialogloginpage.ui \
    dialogusermessageedit.ui \
    widgetlogin.ui \
    widgetregister.ui \
    widgetfindpassword.ui \
    widgetresetpassword.ui \
    subwidgetpfindpassword.ui \
    subwidgetphasfindpassword.ui \
    subwidgetmfindpassword.ui \
    subwidgetmhasfindpassword.ui

DISTFILES += \
    ../Common/Json/sconscript \
    res/images/bg.png \
    res/images/icon_choice.png

RESOURCES += \
    ubxlogin.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/release/ -lUBXBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXBase/debug/ -lUBXBase
else:unix: LIBS += -L$$OUT_PWD/../UBXBase/ -lUBXBase

INCLUDEPATH += $$PWD/../UBXBase
DEPENDPATH += $$PWD/../UBXBase
