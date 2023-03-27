#-------------------------------------------------
#
# Project created by QtCreator 2016-02-19T10:45:26
#
#-------------------------------------------------

QT       += core gui network widgets xml  multimedia multimediawidgets

TARGET = UBXActionLib
TEMPLATE = lib

DEFINES += UBXACTIONLIB_LIBRARY
DEFINES += QUAZIP_ENABLE
DEFINES += MARKUP_STL
DEFINES += USEVERSION
DEFINES += USEQINIU
DEFINES += USEXML

CONFIG += precompile_header
PRECOMPILED_HEADER += $$PWD/stdafx.h

INCLUDEPATH += $$PWD/../zxLib \
    $$PWD/../Common \
    $$PWD/../trd/quazip/include \
    $$PWD/../trd/zlib/include \
    $$PWD/../UBXLogin

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxLib/release/ -lzxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxLib/debug/ -lzxLib
else:unix: LIBS += -L$$OUT_PWD/../zxLib/ -lzxLib

INCLUDEPATH += $$PWD/../zxLib
DEPENDPATH += $$PWD/../zxLib

####播放器库#####
macx:{
LIBS += -L$$OUT_PWD/../UBXPlayer -lUBXPlayer
}else:win32:{
CONFIG(debug, debug|release){
    LIBS += -L$$OUT_PWD/../UBXPlayer/Debug -lUBXPlayer
}else{
    LIBS += -L$$OUT_PWD/../UBXPlayer/Release -lUBXPlayer
}
}
INCLUDEPATH += $$PWD/../UBXPlayer
DEPENDPATH += $$PWD/../UBXPlayer
####播放器库#####

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../trd/qiniu/lib/win32/release -lqiniu
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../trd/qiniu/lib/win32/debug -lqiniu
else:unix: LIBS += -L$$PWD/../trd/qiniu/lib/osx -lqiniu

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../trd/qiniu/lib/win32/release/libqiniu.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../trd/qiniu/lib/win32/debug/libqiniu.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../trd/qiniu/lib/win32/release/qiniu.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../trd/qiniu/lib/win32/debug/qiniu.lib
else:unix: PRE_TARGETDEPS += $$PWD/../trd/qiniu/lib/osx/libqiniu.a

INCLUDEPATH += $$PWD/../trd/qiniu/include \
                    $$PWD/../Common/b64 \
                    $$PWD/../Common/cJSON

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../trd/openssl/lib/win32/release -lqiniu
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../trd/qiniu/lib/win32/debug -lqiniu
macx:{
    LIBS += -L$$PWD/../trd/openssl/lib/osx -lssl
    LIBS += -L$$PWD/../trd/openssl/lib/osx -lcrypto
    INCLUDEPATH += $$PWD/../trd/openssl/include

    LIBS += -L/usr/lib -lcurl
    INCLUDEPATH += /usr/include

}

macx:{
    LIBS += -L/usr/lib -liconv
}else:win32:{
    LIBS += -L$$PWD/../trd/zlib/lib -lzdll
    LIBS += -L$$PWD/../libs -lversion
}

SOURCES += \
    ../Common/QtHttpOper.cpp \
    ../Common/QtHttpOperSync.cpp \
    ../Common/Markup.cpp \
    ../Common/UBXPublic.cpp \
    ../Common/CRC32.cpp \
    #../Common/unzip.cpp \
    ../Common/Util.cpp \
    actionlibwidget.cpp \
    actupdatemangr.cpp \
    #../Common/zip.cpp    
    ../Common/Json/json_reader.cpp \
    ../Common/Json/json_value.cpp \
    ../Common/Json/json_writer.cpp \
    pictureflow.cpp \
    downloaditemdelegate.cpp \
    homepageitempreview.cpp \
    commentitemdelegate.cpp \
    actiondetailwidget.cpp \
    actionlistspage.cpp \
    actionlistitempreview.cpp \
    httptransfer.cpp \
    downloadwidget.cpp \
    httpdownload.cpp \
    favoritespage.cpp \
    widgetupload.cpp \
    widgetuploadaddaction.cpp \
    widgetuploadaddcover.cpp \
    mybasewidget.cpp \
    myuploadpage.cpp \
    iactionlistitempreview.cpp \
    myuploadlistitempreview.cpp \
    actionrecommendpage.cpp \
    ../Common/regexp.cpp \
    ../Common/textedit.cpp \
    ../Common/clineedit.cpp \
    ../Common/widget/cimagelabel.cpp \
    ../Common/widget/checkboxlabel.cpp \
    ../Common/filedirhandle.cpp \
    ../Common/htsparser.cpp \
    datamodel/userrankitem.cpp \
    userrankitemdeletegate.cpp \
    datamodel/usercommentitem.cpp \
    actioncommentwidget.cpp \
    ../Common/actitemdatahandle.cpp \
    datamodel/businesspromotedata.cpp \
    businesspromotewidget.cpp \
    actionyoumaylikewidget.cpp \
    cpromotepopupwidget.cpp \
    cactionlibdatapool.cpp \
    cqiniuupoadhandle.cpp \
    ../Common/cfont.cpp \
    ccommenttreeview.cpp \
    ../Common/cdataoperate.cpp \
    ../Common/cbasehttpopersync.cpp \
    ../Common/cstringhandle.cpp


HEADERS +=\
        ubxactionlib_global.h \
    ../Common/AesParser.h \
    ../Common/QtHttpOper.h \
    ../Common/QtHttpOperSync.h \
    ../Common/Markup.h \
    ../Common/UBXPublic.h \ \
    ../Common/CRC32.h \
    #../Common/unzip.h \
    ../Common/Util.h \
    #../Common/zip.h \
    ../zxLib/zxLib.h \
    actionlibwidget.h \
    actupdatemangr.h \    
    ../Common/Json/autolink.h \
    ../Common/Json/config.h \
    ../Common/Json/features.h \
    ../Common/Json/forwards.h \
    ../Common/Json/json.h \
    ../Common/Json/json_batchallocator.h \
    ../Common/Json/reader.h \
    ../Common/Json/value.h \
    ../Common/Json/writer.h \
#    ../trd/quazip/include/crypt.h \
#    ../trd/quazip/include/ioapi.h \
#    ../trd/quazip/include/JlCompress.h \
#    ../trd/quazip/include/quaadler32.h \
#    ../trd/quazip/include/quachecksum32.h \
#    ../trd/quazip/include/quacrc32.h \
#    ../trd/quazip/include/quagzipfile.h \
#    ../trd/quazip/include/quaziodevice.h \
    ../trd/quazip/include/quazip.h \
#    ../trd/quazip/include/quazip_global.h \
#    ../trd/quazip/include/quazipdir.h \
#    ../trd/quazip/include/quazipfile.h \
#    ../trd/quazip/include/quazipfileinfo.h \
#    ../trd/quazip/include/quazipnewinfo.h \
#    ../trd/quazip/include/unzip.h \
#    ../trd/quazip/include/zip.h \
    pictureflow.h \
    ubxactionlibconstants.h \
    downloaditemdelegate.h \
    homepageitempreview.h \
    commentitemdelegate.h \
    actiondetailwidget.h \
    actionlistspage.h \
    actionlistitempreview.h \
    httptransfer.h \
    downloadwidget.h \
    httpdownload.h \
    favoritespage.h \
    widgetupload.h \
    widgetuploadaddaction.h \
    widgetuploadaddcover.h \
    mybasewidget.h \
    myuploadpage.h \
    iactionlistitempreview.h \
    myuploadlistitempreview.h \
    actionrecommendpage.h \
    ../Common/regexp.h \
    ../Common/textedit.h \
    ../Common/clineedit.h \
    ../Common/widget/cimagelabel.h \
    ../Common/widget/checkboxlabel.h \
    ../Common/filedirhandle.h \
    ../Common/htsparser.h \
    datamodel/userrankitem.h \
    userrankitemdeletegate.h \
    datamodel/usercommentitem.h \
    actioncommentwidget.h \
    ../Common/actitemdatahandle.h \
    datamodel/businesspromotedata.h \
    businesspromotewidget.h \
    actionyoumaylikewidget.h \
    cpromotepopupwidget.h \
    cactionlibdatapool.h \
    cqiniuupoadhandle.h \
    ../Common/cfont.h \
    ccommenttreeview.h \
    ../Common/cdataoperate.h \
    ../Common/cbasehttpopersync.h \
    ../Common/cstringhandle.h

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}

FORMS += \
    actionlibwidget.ui \
    homepageitempreview.ui \
    actiondetailwidget.ui \
    actionlistspage.ui \
    actionlistitempreview.ui \
    downloadwidget.ui \
    favoritespage.ui \
    widgetupload.ui \
    widgetuploadaddaction.ui \
    widgetuploadaddcover.ui \
    mybasewidget.ui \
    myuploadpage.ui \
    actionrecommendpage.ui \
    actioncommentwidget.ui \
    businesspromotewidget.ui \
    actionyoumaylikewidget.ui \
    cpromotepopupwidget.ui

TRANSLATIONS += ../languages/UBXActionLib_zh_CN.ts \
        ../languages/UBXActionLib_en.ts \
        ../languages/UBXActionLib_zh_HU.ts \

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../trd/quazip/lib/win32/release/ -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../trd/quazip/lib/win32/debug/ -lquazip
else:unix: LIBS += -L$$PWD/../trd/quazip/lib/osx/ -lquazip.1.0.0

INCLUDEPATH += $$PWD/../trd/quazip/include
DEPENDPATH += $$PWD/../trd/quazip/include

RESOURCES += \
    ubxactionlib.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../UBXLogin/release/ -lUBXLogin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../UBXLogin/debug/ -lUBXLogin
else:unix: LIBS += -L$$OUT_PWD/../UBXLogin/ -lUBXLogin

INCLUDEPATH += $$PWD/../UBXLogin
DEPENDPATH += $$PWD/../UBXLogin

win32: LIBS += -L$$PWD/../trd/ubtlib/lib/ -lUbtlib

INCLUDEPATH += $$PWD/../trd/ubtlib/include
DEPENDPATH += $$PWD/../trd/ubtlib/include
