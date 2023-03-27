/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：main.cpp
* 创建时间：2015/12/04 14:00
* 文件标识：
* 文件摘要：主函数入口
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2015/12/04 14:00
* 版本摘要：
*/
#include <QTextCodec>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDir>
#include <QSettings>
#include <QtSingleApplication>

#include "UBXPublic.h"
#include "mainwindowwidget.h"
#include "../zxLib/configs.h"
#include "../zxLib/ciniconfig.h"
#include "../UBXBase/clanguagemgr.h"
#include "../UBXBase/UBXBase.h"



#ifdef USE_VLD_DEBUG
    #ifdef Q_OS_WIN
        #include "vld.h"
    #endif
#endif

///////////////////////////////////////////////////test start//////////////////////////////
//// windows下用于记录core.dump文件，方便定位程序崩溃问题
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef  Q_OS_WIN
    #include <windows.h>

    #pragma once

    #include <DbgHelp.h>
    #include <stdlib.h>
    #include "tchar.h"
    #pragma comment(lib, "dbghelp.lib")

    #ifndef _M_IX86
    #error "The following code only works for x86!"
    #endif

    inline BOOL IsDataSectionNeeded(const WCHAR* pModuleName)
    {
        if(pModuleName == 0)
        {
            return FALSE;
        }

        WCHAR szFileName[_MAX_FNAME] = L"";
        _wsplitpath(pModuleName, NULL, NULL, szFileName, NULL);

        if(wcsicmp(szFileName, L"ntdll") == 0)
            return TRUE;

        return FALSE;
    }

    inline BOOL CALLBACK MiniDumpCallback(PVOID                            pParam,
                                          const PMINIDUMP_CALLBACK_INPUT   pInput,
                                          PMINIDUMP_CALLBACK_OUTPUT        pOutput)
    {
        if(pInput == 0 || pOutput == 0)
            return FALSE;

        switch(pInput->CallbackType)
        {
        case ModuleCallback:
            if(pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
                if(!IsDataSectionNeeded(pInput->Module.FullPath))
                    pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
        case IncludeModuleCallback:
        case IncludeThreadCallback:
        case ThreadCallback:
        case ThreadExCallback:
            return TRUE;
        default:;
        }

        return FALSE;
    }

    inline void CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR strFileName)
    {
        HANDLE hFile = CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
        {
            MINIDUMP_EXCEPTION_INFORMATION mdei;
            mdei.ThreadId           = GetCurrentThreadId();
            mdei.ExceptionPointers  = pep;
            mdei.ClientPointers     = NULL;

            MINIDUMP_CALLBACK_INFORMATION mci;
            mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
            mci.CallbackParam       = 0;

            ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, (pep != 0) ? &mdei : 0, NULL, &mci);

            CloseHandle(hFile);
        }
    }

    LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
    {
        CreateMiniDump(pExceptionInfo, L"core.dmp");

        return EXCEPTION_EXECUTE_HANDLER;
    }

    // 此函数一旦成功调用，之后对 SetUnhandledExceptionFilter 的调用将无效
    void DisableSetUnhandledExceptionFilter()
    {
        void* addr = (void*)GetProcAddress(LoadLibrary(L"kernel32.dll"), "SetUnhandledExceptionFilter");

        if (addr)
        {
            unsigned char code[16];
            int size = 0;

            code[size++] = 0x33;
            code[size++] = 0xC0;
            code[size++] = 0xC2;
            code[size++] = 0x04;
            code[size++] = 0x00;

            DWORD dwOldFlag, dwTempFlag;
            VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
            WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
            VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
        }
    }

    void InitMinDump()
    {
        //注册异常处理函数
        SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

        //使SetUnhandledExceptionFilter
        DisableSetUnhandledExceptionFilter();
    }
#endif
////////////////////////////////////////////////////////////////////////test end/////////////////////////

class AlphaApplication : public QtSingleApplication
{
public:
    AlphaApplication(const QString &id, int &argc, char **argv)
        :QtSingleApplication(id, argc, argv)
    {
        m_pMainWidget = NULL;
    }

public:
    bool notify(QObject *receiver, QEvent *e)
    {
        if (e->type() == QEvent::MouseMove)
        {
            MainWindowWidget* pWidget = dynamic_cast<MainWindowWidget*>(receiver);
            if(!pWidget && m_pMainWidget)
            {
                m_pMainWidget->UnsetCursor();
            }
        }
        return QApplication::notify(receiver, e);
    }
public:
    MainWindowWidget* m_pMainWidget;
};

int main(int argc, char *argv[])
{
//    ///////////////////////////////test start///////////////////////
//#ifdef Q_OS_WIN
//    InitMinDump();
//#endif
//    //////////////////////////////test end///////////////////////

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    AlphaApplication a(QDir::currentPath(), argc, argv);
    if (a.isRunning())
    {
        a.sendMessage("raise_window_noop");
        return EXIT_SUCCESS;
    }

    qRegisterMetaType<TypeAngleChanged>("TypeAngleChanged");

    CConfigs::instance();//to creat some dirs, and init some paths

    QString strRobotType;
    CIniConfig::getInstance().getValueRobotType(strRobotType);
    emUbtProductType eProductTyep = UBXBase::toProductType(strRobotType);
    if(eUbtProductUnknown != eProductTyep)
    {
        UBXBase::getInstance()->setCurrentRobotProductType(eProductTyep);
    }
    else
    {
        UBXBase::getInstance()->setToDefaultRobotProductType();
        emUbtProductType eProductTypeDefault = UBXBase::getInstance()->getCurrentRobotProductType();
        QString strDefault =UBXBase::productTypeToStr(eProductTypeDefault);
        CIniConfig::getInstance().setValueRobotType(strDefault);
    }    

    QString strXmlPathName = CConfigs::getLocalRobotPath(MODEL_XML_NAME);
    vector<MOTOR_DATA> vecMotorDataDefaultMaxAngle;
    UBXBase::getInstance()->readMotorDefaultMaxAngle(strXmlPathName, vecMotorDataDefaultMaxAngle);
    UBXBase::getInstance()->setMotorDefaultMaxAngle(vecMotorDataDefaultMaxAngle, eUbtProduct1E);

    g_actupdatemgr = new CActUpdateMangr();
    g_actupdatemgr->SetUserID(-1);

    CreateRobotController(); //创建机器人通信控制器

#if 1
    CLanguageMgr::getInstance()->setApplication((QApplication*)&a);
    CLanguageMgr::getInstance()->setLanguage(CConfigs::getLocalLanguageDir(),
                                             CIniConfig::getInstance().getValueLanguage());
#else
    //add by hels for 多语言加载
    QString strLanguageKeyword = CIniConfig::getInstance().getValueLanguage();
    QTranslator translatorAlpha, transTreeNavigation, transLocalAct,
            transMotionEditor,translatorUBXRobot3DView, translatorMP3,
            translatorActionLib, translatorLogin, translatorUpdateModule,
            translatorUBXPlayer, translatorZxLib;


    //主工程翻译文件
    QString tsPath = CConfigs::getLocalLanguageDir() + "/AlphaRobot_" + strLanguageKeyword;
    translatorAlpha.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&translatorAlpha);

    //zxLib翻译
    tsPath = CConfigs::getLocalLanguageDir() + "/zxLib_" + strLanguageKeyword;
    translatorZxLib.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&translatorZxLib);

    //TreeNavi翻译
    tsPath = CConfigs::getLocalLanguageDir() + "/LibTreeNavigation_" + strLanguageKeyword;
    transTreeNavigation.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&transTreeNavigation);

    //LocalAct翻译
    tsPath = CConfigs::getLocalLanguageDir() + "/LocalActSyncWidget_" + strLanguageKeyword;
    transLocalAct.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&transLocalAct);

    //MotionEditor翻译
    tsPath = CConfigs::getLocalLanguageDir() + "/UBXMotionEditor_" + strLanguageKeyword;
    transMotionEditor.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&transMotionEditor);

    //3DView翻译文件
    tsPath = CConfigs::getLocalLanguageDir() + "/UBXRobot3DView_" + strLanguageKeyword;
    translatorUBXRobot3DView.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&translatorUBXRobot3DView);

    //MP3模块翻译文件
    tsPath = CConfigs::getLocalLanguageDir() + "/mp3proj_" + strLanguageKeyword;
    translatorMP3.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&translatorMP3);

    // 播放器模块
    tsPath = CConfigs::getLocalLanguageDir() + "/UBXPlayer_" + strLanguageKeyword;
    translatorUBXPlayer.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&translatorUBXPlayer);

    //动作库模块翻译
    tsPath = CConfigs::getLocalLanguageDir() + "/UBXActionLib_" + strLanguageKeyword;
    translatorActionLib.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&translatorActionLib);

    //登录模块翻译
    tsPath = CConfigs::getLocalLanguageDir() + "/UBXLogin_" + strLanguageKeyword;
    translatorLogin.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&translatorLogin);

    //更新模块翻译
    tsPath = CConfigs::getLocalLanguageDir() + "/UBXUpdateModule_" + strLanguageKeyword;
    translatorUpdateModule.load(QDir::toNativeSeparators(tsPath));
    a.installTranslator(&translatorUpdateModule);

    //加载Qt自带翻译
    QTranslator translatorQt;
    QString locale = QLocale::system().name();
    const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    const QString &qtTrFile = QLatin1String("qt_") + locale;

    if (translatorQt.load(qtTrFile, qtTrPath) ||
            translatorQt.load(qtTrFile, QDir::toNativeSeparators(
                                  QString(CConfigs::getLocalLanguageDir() + "/"))))
    {
        a.installTranslator(&translatorQt);
        a.setProperty("qtc_locale", locale);
    }
#endif

    a.m_pMainWidget = pMainWindow;
    a.setActivationWindow(pMainWindow);
    pMainWindow->show();

#ifdef Q_OS_WIN
    a.installNativeEventFilter(pMainWindow);
#endif

    int nRet = a.exec();
    SAFE_DELETE(a.m_pMainWidget);  // 释放内存，否则MainWindow析构函数不会调用

    //  没有释放内存 add by hwx
    SAFE_DELETE(g_actupdatemgr);
    //ReleaseRobotController();//释放机器人通信控制器
    return nRet;
}
