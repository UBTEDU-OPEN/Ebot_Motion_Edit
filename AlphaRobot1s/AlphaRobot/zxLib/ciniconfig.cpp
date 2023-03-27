/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：IniConfig.h
* 创建时间：2016/11/15
* 文件标识：
* 文件摘要：IniConfig参数设置
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：2016/11/15
* 版本摘要：
*/

#include "ciniconfig.h"


#include <QSettings>

#include "configs.h"


const QString SETTING_SPLIT_CHART = "/";

//Config Node
const QString CFG_NODE_NAME_CONFIG = "Config";  //动作表路径配置节名
const QString CFG_KEY_NAME_ACT_PATH = "ActPath";  //动作表路径配置键名
const QString CFG_KEY_NAME_SERVER = "Server";  //服务器地址配置键名
const QString CFG_KEY_NAME_SERVER_UPDATE_ROBOT = "ServerUpdateRobot";  //服务器地址配置键名, only using for update robot ALPHA1E

const QString CFG_KEY_NAME_BIG_FRAME_DELAY = "BigFrameDelay";
const QString CFG_KEY_NAME_SMALL_FRAME_DELAY = "SmallFrameDelay";


//UserInfo Node
const QString CFG_NODE_NAME_USER_INFO = "UserInfo";  //用户信息节名
const QString CFG_KEY_NAME_USER_LOGIN_TYPE_PHONE_LOGIN = "PhoneLogin"; // 手机登录方式
const QString CFG_KEY_NAME_USER_LOGIN_TYPE_MAIL_LOGIN = "MailLogin"; // 邮箱登录方式
const QString CFG_KEY_NAME_COUNTRY_CODE = "CountryCode";  // 国家码
const QString CFG_KEY_NAME_USER_NAME = "UserName";  //户名键名
const QString CFG_KEY_NAME_PASSWORD = "Password"; // 密码
const QString CFG_KEY_NAME_USER_TYPE = "UserType";  //用户类型键名
const QString CFG_KEY_NAME_IS_SAVE = "IsSave";    //是否保存键名

const QString CFG_KEY_MUSIC_PATH = "ImportMusicPath"; // 导入音乐文件路径
const QString CFG_KEY_PROJ_PATH = "OpenProjectPath";  // 打开工程路径


//node of [LanguageSetting]
const QString CFG_NODE_NAME_LANGUAGE_SETTING = "LanguageSetting";  //LanguageSetting节名
const QString CFG_KEY_NAME_LANGUAGE = "Language";  //键名

//node of [RobotTypeSetting]
const QString CFG_NODE_NAME_ROBOT_TYPE_SETTING = "RobotTypeSetting";  //RobotTypeSetting节名
const QString CFG_KEY_NAME_ROBOT_TYPE = "RobotType";  //键名


CIniConfig::CIniConfig()
{
    m_strConfigIniFilePath = CConfigs::getIniConfigFile();////动作表配置文件路径
}

CIniConfig::~CIniConfig()
{

}

bool CIniConfig::setValue(const QString &strFile, const QString &strKey, const QVariant &varValue)
{
    QSettings settings(strFile, QSettings::IniFormat);
    settings.setValue(strKey, varValue);

    return true;
}

bool CIniConfig::getValue(const QString &strFile, const QString &strKey, int &nValue)
{
    QVariant var = CIniConfig::getValue(strFile, strKey);

    bool bOk = false;
    nValue = var.toInt(&bOk);

    return bOk;
}

bool CIniConfig::getValue(const QString &strFile, const QString &strKey, double &dValue)
{
    QVariant var = CIniConfig::getValue(strFile, strKey);

    bool bOk = false;
    dValue = var.toDouble(&bOk);

    return bOk;
}

bool CIniConfig::getValue(const QString &strFile, const QString &strKey, QString &strValue)
{
    QVariant var = CIniConfig::getValue(strFile, strKey);
    strValue = var.toString();

    return (!strValue.isEmpty());
}

bool CIniConfig::getValue(const QString &strFile, const QString &strKey, bool &bValue)
{
    QVariant var = CIniConfig::getValue(strFile, strKey);

    bValue = var.toBool();
    return bValue;
}

QString CIniConfig::getKey(const QString &strKeyPre, const QString &strKeySuffix)
{
    if(strKeyPre.endsWith(SETTING_SPLIT_CHART))
    {
        return strKeyPre + strKeySuffix;
    }

    return strKeyPre + SETTING_SPLIT_CHART + strKeySuffix;
}

QString CIniConfig::getKey(const QString &strKeyPre, const QString &strKeyMid, const QString &strKeySuffix)
{
    QString strTemp = CIniConfig::getKey(strKeyPre, strKeyMid);
    return CIniConfig::getKey(strTemp, strKeySuffix);
}

QVariant CIniConfig::getValue(const QString &strFile, const QString &strKey)
{
    QSettings settings(strFile, QSettings::IniFormat);
    return settings.value(strKey);
}


QString CIniConfig::getKeyNodeConfig()
{
    return CFG_NODE_NAME_CONFIG;
}

QString CIniConfig::getKeyNodeUserInfo()
{
    return CFG_NODE_NAME_USER_INFO;
}

QString CIniConfig::getKeyNodeLanguageSetting()
{
    return CFG_NODE_NAME_LANGUAGE_SETTING;
}

QString CIniConfig::getKeyLanguage()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeLanguageSetting(),
                              CFG_KEY_NAME_LANGUAGE);
}

QString CIniConfig::getKeyNodeRobotTypeSetting()
{
    return CFG_NODE_NAME_ROBOT_TYPE_SETTING;
}

QString CIniConfig::getKeyRobotType()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeRobotTypeSetting(),
                              CFG_KEY_NAME_ROBOT_TYPE);
}

QString CIniConfig::getKeyActPath()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeConfig(),
                                        CFG_KEY_NAME_ACT_PATH);
}

QString CIniConfig::getKeyServer()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeConfig(),
                              CFG_KEY_NAME_SERVER);
}

//robot update 1e
QString CIniConfig::getKeyServerRobotUpdate()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeConfig(),
                              CFG_KEY_NAME_SERVER_UPDATE_ROBOT);
}

QString CIniConfig::getKeyBigFrameDelay()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeConfig(),
                                        CFG_KEY_NAME_BIG_FRAME_DELAY);
}

QString CIniConfig::getKeySmallFrameDelay()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeConfig(),
                                        CFG_KEY_NAME_SMALL_FRAME_DELAY);
}

QString CIniConfig::getKeyML()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeUserInfo(),
                              CFG_KEY_NAME_USER_LOGIN_TYPE_MAIL_LOGIN);
}

QString CIniConfig::getKeyPL()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeUserInfo(),
                              CFG_KEY_NAME_USER_LOGIN_TYPE_PHONE_LOGIN);
}

QString CIniConfig::getKeyMLUserName()
{
    return CIniConfig::getKey(CIniConfig::getKeyML(),
                              CFG_KEY_NAME_USER_NAME);
}

QString CIniConfig::getKeyMLPassword()
{
    return CIniConfig::getKey(CIniConfig::getKeyML(),
                              CFG_KEY_NAME_PASSWORD);
}

QString CIniConfig::getKeyMLUserType()
{
    return CIniConfig::getKey(CIniConfig::getKeyML(),
                              CFG_KEY_NAME_USER_TYPE);
}

QString CIniConfig::getKeyMLIsSave()
{
    return CIniConfig::getKey(CIniConfig::getKeyML(),
                              CFG_KEY_NAME_IS_SAVE);
}

QString CIniConfig::getKeyPLCountryCode()
{
    return CIniConfig::getKey(CIniConfig::getKeyPL(),
                              CFG_KEY_NAME_COUNTRY_CODE);
}

QString CIniConfig::getKeyPLUserName()
{
    return CIniConfig::getKey(CIniConfig::getKeyPL(),
                              CFG_KEY_NAME_USER_NAME);
}

QString CIniConfig::getKeyPLPassword()
{
    return CIniConfig::getKey(CIniConfig::getKeyPL(),
                              CFG_KEY_NAME_PASSWORD);
}

QString CIniConfig::getKeyPLUserType()
{
    return CIniConfig::getKey(CIniConfig::getKeyPL(),
                              CFG_KEY_NAME_USER_TYPE);
}

QString CIniConfig::getKeyPLIsSave()
{
    return CIniConfig::getKey(CIniConfig::getKeyPL(),
                              CFG_KEY_NAME_IS_SAVE);
}

QString CIniConfig::getKeyFilePath(emFilePathType eType)
{
    QString strKey = "";
    switch (eType) {
    case eFilePathImportMusic:
    {
        strKey = getKeyMusicPath();
        break;
    }
    case eFilePathOpenProject:
    {
        strKey = getKeyOpenProjPath();
        break;
    }
    default:
        break;
    }

    return strKey;
}

QString CIniConfig::getKeyMusicPath()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeUserInfo(),
                              CFG_KEY_MUSIC_PATH);
}

QString CIniConfig::getKeyOpenProjPath()
{
    return CIniConfig::getKey(CIniConfig::getKeyNodeUserInfo(),
                              CFG_KEY_PROJ_PATH);
}

bool CIniConfig::getValueActPath(QString &strValue)
{
    QString strKey = CIniConfig::getKeyActPath();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, strValue);
}

void CIniConfig::setValueActPath(const QString &strValue)
{
    QString strKey = CIniConfig::getKeyActPath();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strValue);
}

bool CIniConfig::getValueServer(QString &strValue)
{
    QString strKey = CIniConfig::getKeyServer();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, strValue);
}

void CIniConfig::setValueServer(const QString &strValue)
{
    QString strKey = CIniConfig::getKeyServer();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strValue);
}
//robot 1e
bool CIniConfig::getValueServerRobotUpate(QString &strValue)
{
    QString strKey = CIniConfig::getKeyServerRobotUpdate();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, strValue);
}
//robot 1e
void CIniConfig::setValueServerRobotUpate(QString &strValue)
{
    QString strKey = CIniConfig::getKeyServerRobotUpdate();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strValue);
}

bool CIniConfig::getValueBigFrameDelay(int &nValue)
{
    QString strKey = CIniConfig::getKeyBigFrameDelay();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, nValue);
}

void CIniConfig::setValueBigFrameDelay(const int &nValue)
{
    QString strKey = CIniConfig::getKeyBigFrameDelay();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, nValue);
}

bool CIniConfig::getValueSmallFrameDelay(int &nValue)
{
    QString strKey = CIniConfig::getKeySmallFrameDelay();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, nValue);
}

void CIniConfig::setValueSmallFrameDelay(const int &nValue)
{
    QString strKey = CIniConfig::getKeySmallFrameDelay();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, nValue);
}

bool CIniConfig::getValueMLUserName(QString &strValue)
{
    const QString strKey = CIniConfig::getKeyMLUserName();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, strValue);
}

void CIniConfig::setValueMLUserName(const QString &strValue)
{
    const QString strKey = CIniConfig::getKeyMLUserName();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strValue);
}

bool CIniConfig::getValueMLPassword(QString &strValue)
{
    QString strKey = CIniConfig::getKeyMLPassword();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, strValue);
}

void CIniConfig::setValueMLPassword(const QString &strValue)
{
    const QString strKey = CIniConfig::getKeyMLPassword();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strValue);
}

bool CIniConfig::getValueMLUserType(int &nValue)
{
    QString strKey = CIniConfig::getKeyMLUserType();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, nValue);
}

void CIniConfig::setValueMLUserType(const int &nValue)
{
    const QString strKey = CIniConfig::getKeyMLUserType();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, nValue);
}

bool CIniConfig::getValueMLIsSave(bool &bSave)
{
    QString strKey = CIniConfig::getKeyMLIsSave();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, bSave);
}

void CIniConfig::setValueMLIsSave(const bool &bSave)
{
    const QString strKey = CIniConfig::getKeyMLIsSave();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, bSave);
}

bool CIniConfig::getValuePLCountryCode(QString &strValue)
{
    QString strKey = CIniConfig::getKeyPLCountryCode();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, strValue);
}

void CIniConfig::setValuePLCountryCode(const QString &strValue)
{
    QString strKey = CIniConfig::getKeyPLCountryCode();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strValue);
}

bool CIniConfig::getValuePLUserName(QString &strValue)
{
    QString strKey = CIniConfig::getKeyPLUserName();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, strValue);
}

void CIniConfig::setValuePLUserName(const QString &strValue)
{
    const QString strKey = CIniConfig::getKeyPLUserName();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strValue);
}

bool CIniConfig::getValuePLPassword(QString &strValue)
{
    QString strKey = CIniConfig::getKeyPLPassword();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, strValue);
}

void CIniConfig::setValuePLPassword(const QString &strValue)
{
    const QString strKey = CIniConfig::getKeyPLPassword();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strValue);
}

bool CIniConfig::getValuePLUserType(int &nValue)
{
    QString strKey = CIniConfig::getKeyPLUserType();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, nValue);
}

void CIniConfig::setValuePLUserType(const int &nValue)
{
    const QString strKey = CIniConfig::getKeyPLUserType();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, nValue);
}

bool CIniConfig::getValuePLIsSave(bool &bSave)
{
    QString strKey = CIniConfig::getKeyPLIsSave();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, bSave);
}

void CIniConfig::setValuePLIsSave(const bool &bSave)
{
    const QString strKey = CIniConfig::getKeyPLIsSave();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, bSave);
}

void CIniConfig::setPath(emFilePathType eType, const QString &strPath)
{
    const QString strKey = getKeyFilePath(eType);
    if (strKey.isEmpty())
    {
        return;
    }
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strPath);
}

bool CIniConfig::getPath(emFilePathType eType, QString &strPath)
{
    const QString strKey = getKeyFilePath(eType);
    if (strKey.isEmpty())
    {
        return false;
    }
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, strPath);
}

/**************************************************************************
* 函数名: getUserLanguage
* 功能: 获取用户语言，首先从配置文件获取，没有从配置文件获取到时
*          以系统语言返回
* 参数:
* 返回值: QString: 返回的语言关键字
*             中文为：zh_CN
*             英文为：en
* 时间: 2016/07/17
* 作者: hwx
*/
QString CIniConfig::getValueLanguage()
{
    QString strKey = CIniConfig::getKeyLanguage();
    QString strLanguageKeyword;
    CIniConfig::getValue(m_strConfigIniFilePath, strKey, strLanguageKeyword);

    // 如果配置文件没有记录语言信息，则使用系统语言
    if (strLanguageKeyword.isEmpty())
    {
       strLanguageKeyword = QLocale::system().name();
    }

    return strLanguageKeyword;
}

void CIniConfig::setValueLanguage(const QString &strValue)
{
    QString strKey = CIniConfig::getKeyLanguage();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strValue);
}

bool CIniConfig::getValueRobotType(QString &strValue)
{
    QString strKey = CIniConfig::getKeyRobotType();
    return CIniConfig::getValue(m_strConfigIniFilePath, strKey, strValue);
}

void CIniConfig::setValueRobotType(const QString &strValue)
{
    QString strKey = CIniConfig::getKeyRobotType();
    CIniConfig::setValue(m_strConfigIniFilePath, strKey, strValue);
}

bool CIniConfig::getValueIsLogEnable(bool &bEnable)
{
    QString strKey = CIniConfig::getKeyNodeConfig();
    strKey = CIniConfig::getKey(strKey, LOG_ENABLE_KEYWORD);

    CIniConfig::getValue(m_strConfigIniFilePath, strKey, bEnable);

    return bEnable;
}
