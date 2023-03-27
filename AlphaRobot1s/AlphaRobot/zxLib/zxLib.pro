#-------------------------------------------------
#
# Project created by QtCreator 2015-12-24T11:02:10
#
#-------------------------------------------------
QT       += core multimedia multimediawidgets
QT       += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zxLib
TEMPLATE = lib

DEFINES += ZXLIB_EXPORTS

CONFIG += precompile_header
PRECOMPILED_HEADER += stdafx.h

macx:{
LIBS += -lpthread
}else:win32:{
LIBS += -L$$PWD/../trd/pthread/lib/win32 -lpthreadVC2
LIBS += -L$$PWD/../libs -lAdvAPI32
}

macx:{
LIBS += -framework IOKit -framework CoreFoundation
}

SOURCES += \
    globaldef.cpp \
    stdafx.cpp \
    zxEventNotify.cpp \
    zxLib.cpp \
    zxMemPool.cpp \
    zxMemPoolEx.cpp \
    zxObserverBase.cpp \
    zxObserverBaseEx.cpp \
    zxObserverSubjectBase.cpp \
    zxObserverSubjectBaseEx.cpp \
    zxSingletonMangr.cpp \
    zxThreadPool.cpp \
    zxCriticalSection.cpp \
    alpharobotlog.cpp \
    configs.cpp \
    widget/messagebox.cpp \
    widget/mytitlebar.cpp \
    widget/ubxwidget.cpp \
    widget/phonenumregexp.cpp \
    widget/mailregexp.cpp \
    ../Common/regexp.cpp \
    widget/QRoundProgressBar.cpp \
    widget/batteryicon.cpp \
    ../Common/filedirhandle.cpp \
    widget/ubxdialog.cpp \
    widget/ubxmultipage.cpp \
    timermanager.cpp \
    widget/menu.cpp \
    inotify.cpp \
    widget/ccommentwidget.cpp \
    widget/videocapture.cpp \
    widget/progressdialog.cpp \
    widget/ctablewidget.cpp \
    cubxudiskmanager.cpp \
    ciniconfig.cpp \
    widget/cubxintvalidator.cpp \
    widget/ubxpushbutton.cpp \
    widget/cdialogcontainer.cpp \
    widget/cdockwidget.cpp \
    widget/ctablewidgetitem.cpp


HEADERS += \
    globaldef.h \
    stdafx.h \
    zxCirCleQueueT.h \
    zxEventNotify.h \
    zxLib.h \
    zxMemPool.h \
    zxMemPoolEx.h \
    zxObserverBase.h \
    zxObserverBaseEx.h \
    zxObserverSubjectBase.h \
    zxObserverSubjectBaseEx.h \
    zxSingletonMangr.h \
    zxSingletonT.h \
    zxSqLinkT.h \
    zxThreadPool.h \
    zxCriticalSection.h \
    alpharobotlog.h \
    configs.h \
    widget/messagebox.h \
    widget/mytitlebar.h \
    widget/ubxwidget.h \
    widget/phonenumregexp.h \
    widget/mailregexp.h \
    ../Common/regexp.h \
    widget/QRoundProgressBar.h \
    widget/batteryicon.h \
    ../Common/filedirhandle.h \
    widget/ubxdialog.h \
    widget/ubxmultipage.h \
    widget/ubxwidgetconst.h \
    timermanager.h \
    widget/menu.h \
    inotify.h \
    widget/ccommentwidget.h \
    zxlibconst.h \
    widget/videocapture.h \
    widget/progressdialog.h \
    widget/ctablewidget.h \
    cubxudiskmanager.h \
    ciniconfig.h \
    widget/cubxintvalidator.h \
    czxqueue.h \
    widget/ubxpushbutton.h \
    widget/cdialogcontainer.h \
    widget/cdockwidget.h \
    widget/ctablewidgetitem.h

macx:{
SOURCES += myusbdevice.cpp
HEADERS += myusbdevice.h
}

#macx:{
#    QMAKE_POST_LINK +=  \
#        rm -rf $$PWD/../../include && \
#        mkdir -p $$PWD/../../include && \
#        cp -a $$PWD/../../src/zxLib/*.h $$PWD/../../include && \
#        rm -f $$PWD/../../include/stdafx.h && \
#        rm -f $$PWD/../../include/targetver.h

#    CONFIG(debug, debug|release){
#        QMAKE_POST_LINK +=  && \
#            rm -rf $$PWD/../../bin/Debug && \
#            mkdir -p $$PWD/../../bin/Debug && \
#            cp -a $$OUT_PWD/*.dylib $$PWD/../../bin/Debug
#    }else{
#        QMAKE_POST_LINK +=  && \
#            rm -rf $$PWD/../../bin/Release && \
#            mkdir -p $$PWD/../../bin/Release && \
#            cp -a $$OUT_PWD/*.dylib $$PWD/../../bin/Release
#    }
#}else:win32:{
#    QMAKE_POST_LINK +=  \
#        if exist \"$$PWD\..\..\include\" (rmdir /s /q \"$$PWD\..\..\include\") && \
#        mkdir \"$$PWD\..\..\include\" && \
#        xcopy /y /s /e /i \"$$PWD\..\..\src\zxLib\*.h\" \"$$PWD\..\..\include\" && \
#        del \"$$PWD\..\..\include\stdafx.h\" && \
#        del \"$$PWD\..\..\include\targetver.h\"
#    CONFIG(debug, debug|release){
#        QMAKE_POST_LINK +=  && \
#            if exist \"$$PWD\..\..\bin\Debug\" (rmdir /s /q \"$$PWD\..\..\bin\Debug\") && \
#            mkdir \"$$PWD\..\..\bin\Debug\" && \
#            xcopy /y /s /e /i \"$$OUT_PWD\Debug\*.dll\" \"$$PWD\..\..\bin\Debug\" && \
#            xcopy /y /s /e /i \"$$OUT_PWD\Debug\*.lib\" \"$$PWD\..\..\bin\Debug\"
#    }else{
#        QMAKE_POST_LINK +=  && \
#            if exist \"$$PWD\..\..\bin\Release\" (rmdir /s /q \"$$PWD\..\..\bin\Release\") && \
#            mkdir \"$$PWD\..\..\bin\Release\" && \
#            xcopy /y /s /e /i \"$$OUT_PWD\Release\*.dll\" \"$$PWD\..\..\bin\Release\" && \
#            xcopy /y /s /e /i \"$$OUT_PWD\Release\*.lib\" \"$$PWD\..\..\bin\Release\"
#    }
#}

TRANSLATIONS += ../languages/zxLib_zh_CN.ts \
        ../languages/zxLib_en.ts \
        ../languages/zxLib_zh_HU.ts \


FORMS += \
    widget/mytitlebar.ui \
    widget/ubxmultipage.ui \
    widget/ccommentwidget.ui \
    widget/cdialogcontainer.ui

RESOURCES += \
    zxlib.qrc \
