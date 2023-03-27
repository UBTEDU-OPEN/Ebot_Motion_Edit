#include "clanguagemgr.h"

#include <QDir>
#include <QLibraryInfo>
#include <QTranslator>


CLanguageMgr* CLanguageMgr::getInstance()
{
    static CLanguageMgr lang(NULL);
    return &lang;
}

CLanguageMgr::CLanguageMgr(QObject *parent) : QObject(parent)
{
    m_pApplication = NULL;
}

CLanguageMgr::~CLanguageMgr()
{

}

QApplication *CLanguageMgr::getApplication() const
{
    return m_pApplication;
}

void CLanguageMgr::setApplication(QApplication *pApplication)
{
    m_pApplication = pApplication;
}

bool CLanguageMgr::setLanguage(QString strLocalLanguageDir, QString strLanguageKeyword)
{
    if(!m_pApplication || strLocalLanguageDir.isEmpty() || strLanguageKeyword.isEmpty())
    {
        return false;
    }

    //add by hels for 多语言加载
    //QString strLanguageKeyword = CIniConfig::getInstance().getValueLanguage();
    static QTranslator translatorAlpha, transTreeNavigation, transLocalAct,
            transMotionEditor,translatorUBXRobot3DView, translatorMP3,
            translatorActionLib, translatorLogin, translatorUpdateModule,
            translatorUBXPlayer, translatorZxLib;


    setLanguageKeyword(strLanguageKeyword);

    //主工程翻译文件
    QString tsPath = strLocalLanguageDir + "/AlphaRobot_" + strLanguageKeyword;
    translatorAlpha.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&translatorAlpha);

    //zxLib翻译
    tsPath = strLocalLanguageDir + "/zxLib_" + strLanguageKeyword;
    translatorZxLib.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&translatorZxLib);

    //TreeNavi翻译
    tsPath = strLocalLanguageDir + "/LibTreeNavigation_" + strLanguageKeyword;
    transTreeNavigation.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&transTreeNavigation);

    //LocalAct翻译
    tsPath = strLocalLanguageDir + "/LocalActSyncWidget_" + strLanguageKeyword;
    transLocalAct.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&transLocalAct);

    //MotionEditor翻译
    tsPath = strLocalLanguageDir + "/UBXMotionEditor_" + strLanguageKeyword;
    transMotionEditor.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&transMotionEditor);

    //3DView翻译文件
    tsPath = strLocalLanguageDir + "/UBXRobot3DView_" + strLanguageKeyword;
    translatorUBXRobot3DView.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&translatorUBXRobot3DView);

    //MP3模块翻译文件
    tsPath = strLocalLanguageDir + "/mp3proj_" + strLanguageKeyword;
    translatorMP3.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&translatorMP3);

    // 播放器模块
    tsPath = strLocalLanguageDir + "/UBXPlayer_" + strLanguageKeyword;
    translatorUBXPlayer.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&translatorUBXPlayer);

    //动作库模块翻译
    tsPath = strLocalLanguageDir + "/UBXActionLib_" + strLanguageKeyword;
    translatorActionLib.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&translatorActionLib);

    //登录模块翻译
    tsPath = strLocalLanguageDir + "/UBXLogin_" + strLanguageKeyword;
    translatorLogin.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&translatorLogin);

    //更新模块翻译
    tsPath = strLocalLanguageDir + "/UBXUpdateModule_" + strLanguageKeyword;
    translatorUpdateModule.load(QDir::toNativeSeparators(tsPath));
    m_pApplication->installTranslator(&translatorUpdateModule);

    //加载Qt自带翻译
    static QTranslator translatorQt;
    QString locale = QLocale::system().name();
    const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    const QString &qtTrFile = QLatin1String("qt_") + locale;

    if (translatorQt.load(qtTrFile, qtTrPath) ||
            translatorQt.load(qtTrFile, QDir::toNativeSeparators(
                                  QString(strLocalLanguageDir + "/"))))
    {
        m_pApplication->installTranslator(&translatorQt);
        m_pApplication->setProperty("qtc_locale", locale);
    }

    return true;
}

QString CLanguageMgr::getNetLanguageKeyword(const QString &strLanguageKeyword)
{
    if(strLanguageKeyword == LANGUAGE_CHINESE_POSTFIX)
    {
        return "CN";
    }
    return "EN";
}

QString CLanguageMgr::getLanguageKeyword() const
{
    return m_strLanguageKeyword;
}

void CLanguageMgr::setLanguageKeyword(const QString &strLanguageKeyword)
{
    m_strLanguageKeyword = strLanguageKeyword;
}

QString CLanguageMgr::getLocalLanguageDir() const
{
    return m_strLocalLanguageDir;
}

void CLanguageMgr::setLocalLanguageDir(const QString &strLocalLanguageDir)
{
    m_strLocalLanguageDir = strLocalLanguageDir;
}
