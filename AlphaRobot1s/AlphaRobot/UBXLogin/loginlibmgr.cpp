/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：loginlibmgr.h
* 创建时间：2016/4/16 8:52
* 文件标识：
* 文件摘要：用户登录函数定义源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/4/16 8:52
* 版本摘要：
*/
#include "StdAfx.h"
#include "loginlibmgr.h"
#include "alpharobotlog.h"
#include "dialogloginpage.h"


#include <QDateTime>

const UINT NOTIFY_MSG_LOGIN_REQUEST = QEvent::registerEventType(); // 用户登录

QString g_strToken;
int g_nUserID = 0;
/*ActionInfo g_ActionInfo;*/

#ifdef WINDOW
const UINT WM_MSG_USER_REGISTER_RESULT = ::RegisterWindowMessage(("WM_MSG_USER_REGISTER_RESULT"));  //用户注册结果消息
#endif
USERINFO  g_userinfo;//全局用户
zxCriticalSection  g_crisec;  //全局临界区
FILEVERINFO g_exefileinfo;  //程序版本
//QString     g_sServer = "http://10.10.1.12:8080";      //服务器地址
QString     m_ZoneCode;     //手机区号
int         m_userIdInfo;   //用户ID
QString     m_userToken;    //用户标签
QString     m_userNameInfo; //用户名
QString     m_userNickName; //用户昵称
QString     m_userGender;   //用户性别

loginLibMgr::loginLibMgr(QWidget *parent)
{
    m_pHttpOper = new QtHttpOperSync(parent);
    //    QObject::connect(m_pHttpOper,SIGNAL(requestResultSignal(bool bRet, char* pData, int nDataLen)), this, SLOT(onRequestResult(bool bRet, char* pData, int nDataLen)));
}

loginLibMgr::~loginLibMgr()
{
    m_pHttpOper->Cancel();
    SAFE_DELETE(m_pHttpOper);
}

/**************************************************************************
* 函数名: GetRequestKey
* 功能: 获取MD5加密密钥
* 参数:
* 	@[in] szTime:	当前系统时间
* 	@[in]	 nTimeLen:  时间字符串长度
* 	@[out]  szKey:		加密密钥
* 	@[in]	 nKeyLen:   密钥长度
* 返回值: 加密成功返回TRUE， 否则返回FALSE
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
bool loginLibMgr::GetRequestKey(char * szTime,int nTimeLen, char* szKey, int nKeyLen)
{
    if (nTimeLen < 15 || nKeyLen <33)
        return false;

    char szRequestKey[512] = {"UBTech832%1293*6"};
    char szMD5Key[512] = {0};
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString strTime = time.toString("yyyyMMddhhmmss"); //设置显示格式
    strcpy(szTime,strTime.toUtf8().data());
    sprintf(szMD5Key,"%s%s",szTime,szRequestKey);

    CMD5 iMD5;
    iMD5.GenerateMD5((unsigned char* )szMD5Key, strlen(szMD5Key));

    strcpy(szKey, iMD5.ToString());

    return true;
}


/**************************************************************************
* 函数名: LoginMail
* 功能: 邮箱登陆
* 参数:
* 	@[in ] sLoginURL: 网络地址，如http://www.test.com/version.xml
* 	@[in]  sUserName: 用户名
* 	@[in ] sPassWord: 密码
* 	@[in ] sGender:   性别
* 	@[in ] sUserID:   ID
* 返回值: 登陆成功返回0， 其余请查看错误码
* 时间: 2016/3/15 9:32
* 作者: 周志平
*/

// http:// 10.10.1.14:8080/ubx/user/login
// {"userName":"lovycici@163.com","userPassword":123456}
//

int loginLibMgr::LoginMail( const QString& sLoginURL, const QString& sUserName, const QString& sPassWord, const QString& sGender, int& sUserID)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    m_RequestTask = PHONENAME;
    do
    {
        char szParam[512] = {0};
        int nProd = -1;
        int nProdType  = -1;
        int appType = ALPHA_APP_TYPE;
        QString  serviceVersion = g_exefileinfo.sFileVer;
        char szTime[512] = {0};
        char szKey[33] = {0};

        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_CONNECT_FAIL;
            break;
        }
        QByteArray TmpArray(szKey);
        sprintf(szParam, "{\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        qDebug()<<szParam<<endl;

        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sLoginURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }


        //qDebug() << (char*)pData;

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {
                    char token[512] = {0};
                    int nIndex = 0;
                    char userName[512] = {0};
                    char userN[512] = {0};
                    char userG[512] = {0};
                    char ccode[512] = {0}; //国家码

                    strcpy(token, jvalue_root["models"][nIndex]["token"].asCString());
                    if (!jvalue_root["models"][nIndex]["userName"].isNull() && jvalue_root["models"][nIndex]["userName"]!="")
                    {
                        strcpy(userN, jvalue_root["models"][nIndex]["userName"].asCString());
                        m_userNickName = Utf8ToUnicode(userN);

                        //                        CreateString_InitWithUTF8Text(m_userNickName, userN);
                    }
                    else
                    {
                        //userG = 0;
                        //m_userNickName = QObject::tr("ubtech");
                        m_userNickName = sUserName;
                    }
                    strcpy(userName, jvalue_root["models"][nIndex]["userEmail"].asCString());
                    if (!jvalue_root["models"][nIndex]["userGender"].isNull())
                    {
                        strcpy(userG, jvalue_root["models"][nIndex]["userGender"].asCString());
                        m_userGender = QString(QLatin1String(userG));
                    }
                    else
                        //userG = 0;
                        m_userGender = "0";
                    g_strToken = QString(QLatin1String(token));
                    m_userNameInfo = QString(QLatin1String(userName));

                    g_nUserID = jvalue_root["models"][nIndex]["userId"].asInt();

                    m_ZoneCode = "";
                    if (!jvalue_root["models"][nIndex]["countryCode"].isNull())
                    {
                        strcpy(ccode, jvalue_root["models"][nIndex]["countryCode"].asCString());
                        m_ZoneCode = QString(QLatin1String(ccode));
                    }

                    int nZoneCode = -1;
                    nZoneCode = m_ZoneCode.toInt();
                    if(nZoneCode < 1)
                    {
                        nZoneCode =  DialogLoginPage::getNationIDByNation(DialogLoginPage::getDefaultNationText());
                        m_ZoneCode = QString::number(nZoneCode);
                    }

                    nRet =REQUEST_SUCCESS;
                    break;
                }
                else
                {
                    char sInfo[512] = {0};
                    strcpy(sInfo, jvalue_root["info"].asCString());
                    nRet = atoi(sInfo);
                }
            }
        }
    } while (0);
    sUserID = g_nUserID;
    m_userIdInfo = sUserID;
    m_userToken = g_strToken;
    SAFE_FREE(pData);
    return nRet;
}

/**************************************************************************
* 函数名: LoginP
* 功能: 手机登陆
* 参数:
* 	@[in ] sLoginURL: 网络地址，如http://www.test.com/version.xml
* 	@[in]  countryCode: 国家码
* 	@[in]  sUserName: 用户名
* 	@[in ] sPassWord: 密码
* 	@[in ] sGender:   性别
** 	@[in ] sUserID:   ID
* 返回值: 登陆成功返回0， 其余请查看错误码
* 时间: 2016/3/15 9:32
* 作者: 周志平
*/
int loginLibMgr::LoginP( const QString& sLoginURL, const QString& countryCode, const QString& sUserName, const QString& sPassWord, const QString& sGender, int& sUserID)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    m_RequestTask = PHONENAME;
    //    USES_CONVERSION;
    do
    {
        char szParam[512] = {0};
        int nProd = -1;
        int nProdType  = -1;
        int appType = ALPHA_APP_TYPE;
        QString  serviceVersion = g_exefileinfo.sFileVer;
        char szTime[512] = {0};
        char szKey[33] = {0};

        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_CONNECT_FAIL;
            break;
        }
        QByteArray TmpArray(szKey);
        sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        qDebug()<<szParam<<endl;
        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sLoginURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }


        //qDebug() << pData;

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {
                    char token[512] = {0};
                    int nIndex = 0;
                    char userName[512] = {0};
                    char userN[512] = {0};
                    char userG[512] = {0};
                    strcpy(token, jvalue_root["models"][nIndex]["token"].asCString());
                    if (!jvalue_root["models"][nIndex]["userName"].isNull() && jvalue_root["models"][nIndex]["userName"]!="")
                    {
                        strcpy(userN, jvalue_root["models"][nIndex]["userName"].asCString());
                        m_userNickName = Utf8ToUnicode(userN);

                        //                        CreateString_InitWithUTF8Text(m_userNickName, userN);
                    }
                    else
                    {
                        //userG = 0;
                        //m_userNickName = QObject::tr("ubtech");
                        m_userNickName = sUserName;
                    }
                    strcpy(userName, jvalue_root["models"][nIndex]["userPhone"].asCString());
                    if (!jvalue_root["models"][nIndex]["userGender"].isNull())
                    {
                        strcpy(userG, jvalue_root["models"][nIndex]["userGender"].asCString());
                        m_userGender = QString(QLatin1String(userG));
                    }
                    else
                        //userG = 0;
                        m_userGender = "0";
                    g_strToken = QString(QLatin1String(token));
                    m_userNameInfo = QString(QLatin1String(userName));

                    g_nUserID = jvalue_root["models"][nIndex]["userId"].asInt();

                    m_ZoneCode = countryCode;
                    nRet =REQUEST_SUCCESS;
                    break;
                }
                else
                {
                    char sInfo[512] = {0};
                    strcpy(sInfo, jvalue_root["info"].asCString());
                    nRet = atoi(sInfo);
                }
            }
        }
    } while (0);
    sUserID = g_nUserID;
    m_userIdInfo = sUserID;
    m_userToken = g_strToken;
    SAFE_FREE(pData);
    return nRet;
}

/**************************************************************************
* 函数名: RegisterMail
* 功能: 用户邮箱注册
* 参数:
* 	@[in ] sLoginURL: 注册地址
* 	@[in] sUserName: 用户名
* 	@[in ] sPassWord: 密码
* 	@[in ] sRegisterType: 注册类型
* 返回值: 注册成功返回0，其余看错误码
* 时间: 2016/03/15 9:32
* 作者: 周志平
*/
int loginLibMgr::RegisterMail(const QString& sRegisterURL, const QString& sUserName, const QString& sPassWord, int sRegisterType)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        int appType = ALPHA_APP_TYPE;
        QString  serviceVersion = g_exefileinfo.sFileVer;
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_CONNECT_FAIL;
            break;
        }
        QByteArray TmpArray(szKey);
        //            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        sprintf(szParam, "{\"userName\":\"%s\",\"type\":%d,\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", sUserName.toUtf8().data(), sRegisterType, sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);

        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sRegisterURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }
        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {
                    char token[512] = {0};
                    int nIndex = 0;
                    char userName[512] = {0};
                    char userN[512] = {0};
                    char userG[512] = {0};
                    strcpy(token, jvalue_root["models"][nIndex]["token"].asCString());
                    //strcpy(userN, jvalue_root["models"][nIndex]["userName"].asCString());
                    strcpy(userName, jvalue_root["models"][nIndex]["userEmail"].asCString());
                    if (!jvalue_root["models"][nIndex]["userGender"].isNull())
                    {
                        strcpy(userG, jvalue_root["models"][nIndex]["userGender"].asCString());
                        m_userGender = Utf8ToUnicode(userG);
                    }
                    else
                    {
                        //userG = 0;
                        m_userGender = "0";
                    }
                    if (!jvalue_root["models"][nIndex]["userName"].isNull() && jvalue_root["models"][nIndex]["userName"]!="")
                    {
                        strcpy(userN, jvalue_root["models"][nIndex]["userName"].asCString());
                        m_userNickName = Utf8ToUnicode(userN);
                        //m_userNickName = A2W(userN);
                    }
                    else
                    {
                        //userG = 0;
                        //m_userNickName = QObject::tr("ubtech");
                        m_userNickName = sUserName;
                    }
                    m_userToken = Utf8ToUnicode(token);
                    m_userNameInfo = Utf8ToUnicode(userName);
                    m_userIdInfo = jvalue_root["models"][nIndex]["userId"].asInt();

                    nRet = REQUEST_SUCCESS;
                    break;
                }
                else
                {
                    char sInfo[512] = {0};
                    strcpy(sInfo, jvalue_root["info"].asCString());
                    nRet = atoi(sInfo);
                }
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return nRet;
}


/**************************************************************************
* 函数名: RegisterP
* 功能: 用户手机注册
* 参数:
* 	@[in ] sLoginURL: 注册地址
* 	@[in] countryCode: 国家码
* 	@[in] sUserName: 用户名
* 	@[in ] sPassWord: 密码
* 	@[in ] sRegisterType: 注册类型
* 返回值: 注册成功返回0，其余看错误码
* 时间: 2016/03/15 9:32
* 作者: 周志平
*/
int loginLibMgr::RegisterP(const QString& sRegisterURL, const QString& countryCode, const QString& sUserName, const QString& sPassWord, int sRegisterType)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        int appType = ALPHA_APP_TYPE;
        QString  serviceVersion = g_exefileinfo.sFileVer;
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_CONNECT_FAIL;
            break;
        }
        QByteArray TmpArray(szKey);
        sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"type\":%d,\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", countryCode.toUtf8().data(), sUserName.toUtf8().data(),sRegisterType, sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        qDebug()<<szParam<<endl;
        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sRegisterURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }
        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {
                    char token[512] = {0};
                    int nIndex = 0;
                    char userName[512] = {0};
                    char userN[512] = {0};
                    char userG[512] = {0};
                    strcpy(token, jvalue_root["models"][nIndex]["token"].asCString());
                    //strcpy(userN, jvalue_root["models"][nIndex]["userName"].asCString());
                    strcpy(userName, jvalue_root["models"][nIndex]["userPhone"].asCString());
                    if (!jvalue_root["models"][nIndex]["userGender"].isNull())
                    {
                        strcpy(userG, jvalue_root["models"][nIndex]["userGender"].asCString());
                        m_userGender = QString(QLatin1String(userG));
                    }
                    else
                    {
                        //userG = 0;
                        m_userGender = "0";
                    }
                    if (!jvalue_root["models"][nIndex]["userName"].isNull() && jvalue_root["models"][nIndex]["userName"]!="")
                    {
                        strcpy(userN, jvalue_root["models"][nIndex]["userName"].asCString());
                        m_userNickName = Utf8ToUnicode(userN);
                    }
                    else
                    {
                        //userG = 0;
                        m_userNickName = QObject::tr("ubtech");
                        m_userNickName = sUserName;
                    }
                    m_userToken = Utf8ToUnicode(token);
                    m_userNameInfo = Utf8ToUnicode(userName);
                    m_userIdInfo = jvalue_root["models"][nIndex]["userId"].asInt();

                    nRet = REQUEST_SUCCESS;
                    break;
                }
                else
                {
                    char sInfo[512] = {0};
                    strcpy(sInfo, jvalue_root["info"].asCString());
                    nRet = atoi(sInfo);
                }
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return nRet;
}

/**************************************************************************
* 函数名:SetUserInfo
* 功能:设置用户信息
* 参数:
* 返回值:
* 时间: 2016/01/01
* 作者: 周志平
**************************************************************************/
int loginLibMgr::SetUserInfo(const QString& sSetUserInfoURL, const QString& sUserName, const QString& sNickName, const QString& sUserGender,const QString& sCountryCode)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        int appType = ALPHA_APP_TYPE;
        QString  serviceVersion = g_exefileinfo.sFileVer;
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_CONNECT_FAIL;
            break;
        }
        // 转换 QString 到 utf-8编码
        char * pBuff = NULL;
        UnicodeToUtf8(m_userNickName, pBuff);
        QByteArray TmpArray(szKey);
        //            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        sprintf(szParam, "{\"token\":\"%s\",\"userId\":%d,\"userName\":\"%s\",\"userGender\":\"%s\",\"userImage\":\"%s\",\"userEmail\":\"%s\",\"countryCode\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", m_userToken.toUtf8().data(), m_userIdInfo, pBuff, sUserGender.toUtf8().data(), "", "", sCountryCode.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);
        SAFE_DELETE(pBuff);

        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sSetUserInfoURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }
        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {
                    int nIndex = 0;
                    char userN[512] = {0};
                    char userG[512] = {0};
                    if (!jvalue_root["models"][nIndex]["userGender"].isNull())
                    {
                        strcpy(userG, jvalue_root["models"][nIndex]["userGender"].asCString());
                        m_userGender = Utf8ToUnicode(userG);
                    }
                    else
                    {
                        //userG = 0;
                        m_userGender = "0";
                    }
                    if (!jvalue_root["models"][nIndex]["userName"].isNull() && jvalue_root["models"][nIndex]["userName"]!="")
                    {
                        strcpy(userN, jvalue_root["models"][nIndex]["userName"].asCString());

                        m_userNickName = Utf8ToUnicode(userN);

                    }
                    else
                    {
                        //userG = 0;
                        //m_userNickName = QObject::tr("ubtech");
                        m_userNickName = sNickName;
                    }

                    nRet = REQUEST_SUCCESS;
                    break;
                }
                else
                {
                    char sInfo[512] = {0};
                    strcpy(sInfo, jvalue_root["info"].asCString());
                    nRet = atoi(sInfo);
                }
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return nRet;
}



/**************************************************************************
* 函数名:SetUserInfoP
* 功能:设置手机用户信息
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
**************************************************************************/
int loginLibMgr::SetUserInfoP(const QString& sSetUserInfoURL, const QString& sUserName, const QString& sNickName, const QString& sUserGender)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        int appType = ALPHA_APP_TYPE;
        QString  serviceVersion = g_exefileinfo.sFileVer;
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_CONNECT_FAIL;
            break;
        }
        // 转换 QString 到 utf-8编码
        QByteArray TmpArray(szKey);

        char * pBuff = NULL;
        UnicodeToUtf8(m_userNickName, pBuff);
        sprintf(szParam, "{\"token\":\"%s\",\"userId\":%d,\"userName\":\"%s\",\"userGender\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", m_userToken.toUtf8().data(), m_userIdInfo, pBuff, sUserGender.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);
        SAFE_DELETE(pBuff);
        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sSetUserInfoURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }

        QString strDebugInfo = QString("url: %1\n parameter: %2\n output: %3").arg(sSetUserInfoURL).arg(szParam).arg(QString((char *)pData));
        AlphaRobotLog::getInstance()->printInfo(strDebugInfo);

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {
                    int nIndex = 0;
                    char userN[512] = {0};
                    char userG[512] = {0};
                    if (!jvalue_root["models"][nIndex]["userGender"].isNull())
                    {
                        strcpy(userG, jvalue_root["models"][nIndex]["userGender"].asCString());
                        m_userGender = Utf8ToUnicode(userG);
                    }
                    else
                    {
                        //userG = 0;
                        m_userGender = "0";
                    }
                    if (!jvalue_root["models"][nIndex]["userName"].isNull() && jvalue_root["models"][nIndex]["userName"]!="")
                    {
                        strcpy(userN, jvalue_root["models"][nIndex]["userName"].asCString());
                        m_userNickName = Utf8ToUnicode(userN);
                    }
                    else
                    {
                        //userG = 0;
                        //m_userNickName = QObject::tr("ubtech");
                        m_userNickName = sNickName;
                    }

                    nRet = REQUEST_SUCCESS;
                    break;
                }
                else
                {
                    char sInfo[512] = {0};
                    strcpy(sInfo, jvalue_root["info"].asCString());
                    nRet = atoi(sInfo);
                }
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return nRet;
}

//bool loginLibMgr::ActionQuery(const QString& sQueryURL, int  nPage, int nPageSize, ActionInfo actionInfo[], int &nActionCount)
//{
//        byte* pData = NULL;
//        qint64 nLen = 0;
//        char sErr[255] = {0};
//        bool bRslt = FALSE;
//        do
//        {
//            char szParam[512] = {0};
//            char szTime[512] = {0};
//            char szKey[33] = {0};
//            int appType = ALPHA_APP_TYPE;
//            QString  serviceVersion = g_exefileinfo.sFileVer;
//            if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
//            {
//                bRslt = FALSE;
//                break;
//            }
//            QByteArray TmpArray(szKey);
//            char sPage[8] = {0};
//            itoa(nPage,sPage,sizeof(sPage));
//            char sPageSize[8] = {0};
//            itoa(nPageSize,sPageSize,sizeof(sPageSize));
////            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
//            sprintf(szParam, "{\"page\":%d,\"pageSize\":%d,\"token\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestTime\":\"%s\",\"requestKey\":\"%s\"}", nPage,nPageSize, g_strToken.toUtf8().data(), appType, serviceVersion.toUtf8().data(), szTime, TmpArray.toUpper().data());
//            if(!g_httpoper.ExecuteRequest(CHttpConnection::HTTP_VERB_POST, sQueryURL, szParam, pData, nLen, sErr))
//            {
//                bRslt = FALSE;
//                break;
//            }
//            Json::Reader jsonreader;
//            Json::Value  jvalue_root;
//            if(jsonreader.parse((char*)pData, jvalue_root))
//            {
//                if(jvalue_root.size() > 0)
//                {
//                    if(jvalue_root["status"].asBool())
//                    {
//                        for (int nIndex = 0; nIndex < nPageSize; nIndex++)
//                        {
//                            if (jvalue_root["models"][nIndex]["actionName"].isNull() )
//                            {
//                                break;
//                            }
//                            strcpy(actionInfo[nIndex].name, jvalue_root["models"][nIndex]["actionName"].asCString());
//                            strcpy(actionInfo[nIndex].url, jvalue_root["models"][nIndex]["actionPath"].asCString());
//                            actionInfo[nIndex].nActionId = jvalue_root["models"][nIndex]["actionId"].asInt();
//                            actionInfo[nIndex].nActionType = jvalue_root["models"][nIndex]["actionType"].asInt();
//                            nActionCount++;

//                        }
//                        bRslt =TRUE;
//                        break;
//                    }
//                }
//            }
//        } while (0);

//        SAFE_FREE(pData);
//        return bRslt;
//}


/**************************************************************************
* 函数名: LoginOut
* 功能: 退出登录
* 参数:
* 	@[in ] sLoginOutURL: 退出登录网络地址，如http://10.10.1.14:8080/ubx/user/loginOut
* 返回值: 退出登录成功返回TRUE，否则返回FALSE
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
bool loginLibMgr::LoginOut(const QString& sLoginOutURL)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    bool bRslt = false;
    do
    {
        char szParam[512] = {0};
        int nProd = -1;
        int nProdType  = -1;
        int appType = ALPHA_APP_TYPE;
        QString  serviceVersion = g_exefileinfo.sFileVer;
        char szTime[512] = {0};
        char szKey[33] = {0};
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            bRslt = false;
            break;
        }

        QByteArray TmpArray(szKey);
        //            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        sprintf(szParam, "{\"userId\":%d,\"token\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", g_nUserID, g_strToken.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);

        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sLoginOutURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            bRslt = false;
            break;
        }

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {

                    bRslt =true;
                    g_strToken = "";
                    g_nUserID = 0;
                    m_ZoneCode = "";
                    break;
                }
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return bRslt;
}

/**************************************************************************
* 函数名: FindPassWord
* 功能: 找回密码
* 参数:
* 	@[in ] sFindPassWordURL: 找回密码地址
* 参数:
* 	@[in ] sUserName: 用户名
* * 参数:
* 	@[in ] nType: 密码
* 返回值: 找回成功返回true，否则返回false
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
bool loginLibMgr::FindPassWord(const QString& sFindPassWordURL, const QString& sUserName , int nType)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    bool bRslt = false;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        int appType = ALPHA_APP_TYPE;
        QString serviceVersion = g_exefileinfo.sFileVer;
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            bRslt = false;
            break;
        }

        QByteArray TmpArray(szKey);

        char* pUserName = NULL;
        UnicodeToUtf8(sUserName, pUserName);
        QString sLanguage = QLocale::system().name();

        QString sLanguageCode = sLanguage.mid(sLanguage.lastIndexOf('_') + 1);

        switch(nType)
        {
        case EMAILNAME:
            sprintf(szParam, "{\"userEmail\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\", \"systemLanguage\":\"%s\"}",
                    sUserName.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime, sLanguageCode.toUtf8().data());
            break;
        case PHONENAME:
            sprintf(szParam, "{\"userPhone\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\", \"systemLanguage\":\"%s\"}",
                    sUserName.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime, sLanguageCode.toUtf8().data());
            break;
        default:
            sprintf(szParam, "{\"userEmail\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\", \"systemLanguage\":\"%s\"}",
                    sUserName.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime, sLanguageCode.toUtf8().data());
            break;
        }
        qDebug()<<"URL:"<<sFindPassWordURL<<"Param:"<<szParam<<endl;
        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sFindPassWordURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            bRslt = false;
            break;
        }

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {
                    bRslt = true;
                    break;
                }
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return bRslt;
}

/**************************************************************************
* 函数名: SetPassWord
* 功能: 设置密码
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
//{"userEmail":"lovycici@163.com","userPassword":"1FWEFWFF","token":"FWEFWFWFEWFEF"}
bool loginLibMgr::SetPassWord(const QString& sSetPassWordyURL,const QString&sUserName, const QString&sPassWord , int nType, const QString&sVerifyCode)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    bool bRslt = false;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        int appType = ALPHA_APP_TYPE;
        QString  serviceVersion = g_exefileinfo.sFileVer;
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            bRslt = false;
            break;
        }
        QByteArray TmpArray(szKey);
            switch(nType)
            {
            case EMAILNAME:
                sprintf(szParam, "{\"userEmail\":\"%s\",\"type\":%d,\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", sUserName.toUtf8().data(), nType, sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);
                break;
            case PHONENAME:
                sprintf(szParam, "{\"userPhone\":\"%s\",\"type\":%d,\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\",\"code\":\"%s\"}", sUserName.toUtf8().data(), nType, sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime, sVerifyCode.toUtf8().data());
                break;
            default:
                break;
            }
        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sSetPassWordyURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            bRslt = false;
            break;
        }

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {
                    bRslt = true;
                    break;
                }
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return bRslt;
}

/**************************************************************************
* 函数名: GetSmsCode
* 功能: 获取短信验证码
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
bool loginLibMgr::GetSmsCode(const QString& sSetSmsCodeURL,const QString&sMobilePhone, const QString & strLanguageKey)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    bool bRslt = false;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        int appType = ALPHA_APP_TYPE;
        QString  serviceVersion = g_exefileinfo.sFileVer;
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            bRslt = false;
            break;
        }
        QByteArray TmpArray(szKey);
        //            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        sprintf(szParam, "{\"code\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\",\"systemLanguage\":\"%s\"}",
                sMobilePhone.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime, strLanguageKey.toUtf8().data());


        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sSetSmsCodeURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
             bRslt = false;
            break;
        }

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {

                    bRslt = true;
                    break;
                }
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return bRslt;
    return true;
}

/**************************************************************************
* 函数名: VerifySmsCode
* 功能: 校验短信验证码
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
bool loginLibMgr::VerifySmsCode(const QString& sSetSmsCodeURL,const QString&sMobilePhone,const QString &scode)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    bool bRslt = false;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        int appType = ALPHA_APP_TYPE;
        QString  serviceVersion = g_exefileinfo.sFileVer;
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            bRslt = false;
            break;
        }
        QByteArray TmpArray(szKey);
        //            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        sprintf(szParam, "{\"phoneNumber\":\"%s\",\"code\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",
                sMobilePhone.toUtf8().data(), scode.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);


        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sSetSmsCodeURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
             bRslt = false;
            break;
        }

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(jvalue_root["status"].asBool())
                {
                    bRslt = true;
                    break;
                }
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return bRslt;
}

/**************************************************************************
* 函数名: QueryUser
* 功能: 查询用户
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
int loginLibMgr::QueryUser(const QString& sQueryUserURL,const QString&sName, int nType)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    int appType = ALPHA_APP_TYPE;
    QString  serviceVersion = g_exefileinfo.sFileVer;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }
        QByteArray TmpArray(szKey);
        //            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        switch(nType)
        {
        case USERNAME:
            sprintf(szParam, "{\"userName\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", sName.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);
            break;
        case EMAILNAME:
            sprintf(szParam, "{\"userEmail\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", sName.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);
            break;
        case PHONENAME:
            sprintf(szParam, "{\"userPhone\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", sName.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);
        }

        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sQueryUserURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                char sInfo[512] = {0};
                strcpy(sInfo, jvalue_root["info"].asCString());
                nRet = atoi(sInfo);
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return nRet;
}

/**************************************************************************
* 函数名: GetPersonalInfomation
* 功能: 获取个人信息
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
bool loginLibMgr::GetPersonalInfomation(const QString& sQueryUserURL, QString& userName, QString& phoneNum, QString& userMail, int& nSex)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    int appType = ALPHA_APP_TYPE;
    QString  serviceVersion = g_exefileinfo.sFileVer;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }
        QByteArray TmpArray(szKey);
        QString strUserId;
        strUserId.sprintf("%d", g_nUserID);
        //            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        // 		switch(nType)
        // 		{
        // 		case USERNAME:
        sprintf(szParam, "{\"userId\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", strUserId.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);
        // 			break;
        // 		case EMAILNAME:
        // 			sprintf(szParam, "{\"userEmail\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", sName.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);
        // 			break;
        // 		case PHONENAME:
        // 			sprintf(szParam, "{\"userPhone\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}", sName.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime);
        // 		}

        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sQueryUserURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                char sInfo[512] = {0};
                strcpy(sInfo, jvalue_root["info"].asCString());
                nRet = atoi(sInfo);

                int nIndex = 0;
                char tempName[512] = {0};
                char tempPhone[512] = {0};
                char tempMail[512] = {0};
                char tempGenter[512] = {0};
                QString userGenter;
                strcpy(tempName, jvalue_root["models"][nIndex]["userName"].asCString());
                userName = Utf8ToUnicode(tempName);

                strcpy(tempPhone, jvalue_root["models"][nIndex]["userPhone"].asCString());
                phoneNum = Utf8ToUnicode(tempPhone);

                strcpy(tempMail, jvalue_root["models"][nIndex]["userEmail"].asCString());
                userMail = Utf8ToUnicode(tempMail);

                // 				strcpy(tempGenter, jvalue_root["models"][nIndex]["userGender"].asCString());
                // 				userGenter = Utf8ToUnicode(tempGenter);
                // 				nSex = _wtoi(userGenter);
                nSex = jvalue_root["models"][nIndex]["userGender"].asInt();
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return nRet;
}

/**************************************************************************
* 函数名: EditPersonalInfo
* 功能: 编辑个人信息
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
bool loginLibMgr::EditPersonalInfo(const QString& sQueryUserURL,const QString&sName, const QString&sEmail, const QString&sPhone, const QString&sSex/*,const QString&sUserID*/)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    int appType = ALPHA_APP_TYPE;
    QString  serviceVersion = g_exefileinfo.sFileVer;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }
        QByteArray TmpArray(szKey);

        QString strUserId;
        strUserId.sprintf("%d", g_nUserID);

        //            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        sprintf(szParam, "{\"userId\":\"%s\",\"userName\":\"%s\",\"userGender\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\",\"token\":\"%s\"}",
                strUserId.toUtf8().data(), sName.toUtf8().data(), sSex.toUtf8().data(),appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime, g_strToken.toUtf8().data());

        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sQueryUserURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                char sInfo[512] = {0};
                strcpy(sInfo, jvalue_root["info"].asCString());
                nRet = atoi(sInfo);
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return nRet;
}

/**************************************************************************
* 函数名: QueryMyCollect
* 功能: 查询我的收藏
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
bool loginLibMgr::QueryMyCollect(const QString& sQueryUserURL)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    int appType = ALPHA_APP_TYPE;
    QString  serviceVersion = g_exefileinfo.sFileVer;
    do
    {
        char szParam[512] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }
        QByteArray TmpArray(szKey);

        QString strUserId;
        strUserId.sprintf("%d", g_nUserID);
        //            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        sprintf(szParam, "{\"collectRelationId\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\",\"token\":\"%s\"}",
                strUserId.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime, g_strToken.toUtf8().data());

        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sQueryUserURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                char sInfo[512] = {0};
                strcpy(sInfo, jvalue_root["info"].asCString());
                nRet = atoi(sInfo);
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return nRet;
}

/**************************************************************************
* 函数名: EditPersonalImage
* 功能: 编辑个人个头像
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
bool loginLibMgr::EditPersonalImage(const QString& sQueryUserURL, const QString&sHeadImage)
{
    byte* pData = NULL;
    qint64 nLen = 0;
    char sErr[255] = {0};
    int nRet = REQUEST_CONNECT_FAIL;
    int appType = ALPHA_APP_TYPE;
    QString  serviceVersion = g_exefileinfo.sFileVer;
    do
    {
        char szParam[51200] = {0};
        char szTime[512] = {0};
        char szKey[33] = {0};
        if (!GetRequestKey(szTime,sizeof(szTime), szKey, sizeof(szKey)))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }
        QByteArray TmpArray(szKey);
        QString strUserId;
        strUserId.sprintf("%d", g_nUserID);
        //            sprintf(szParam, "{\"countryCode\":\"%s\",\"userName\":\"%s\",\"userPassword\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\"}",  countryCode.toUtf8().data(), sUserName.toUtf8().data(), sPassWord.toUtf8().data(), appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(),szTime);
        sprintf(szParam, "{\"userId\":\"%s\",\"userImage\":\"%s\",\"appType\":%d,\"serviceVersion\":\"%s\",\"requestKey\":\"%s\",\"requestTime\":\"%s\",\"token\":\"%s\"}",
                strUserId.toUtf8().data(), sHeadImage.toUtf8().data(),appType, serviceVersion.toUtf8().data(), TmpArray.toUpper().data(), szTime, g_strToken.toUtf8().data());

        if(!m_pHttpOper->ExecuteRequest(eRequestPost, sQueryUserURL, szParam,NULL,NULL, pData, nLen,sErr))
        {
            nRet = REQUEST_NO_RESPONSES;
            break;
        }

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse((char*)pData, jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                char sInfo[512] = {0};
                strcpy(sInfo, jvalue_root["info"].asCString());
                nRet = atoi(sInfo);
            }
        }
    } while (0);

    SAFE_FREE(pData);
    return nRet;
}

/**************************************************************************
* 函数名: onRequestResult
* 功能: 请求结果响应槽
* 时间: 2015/09/22 9:32
* 作者: 周志平
*/
void loginLibMgr::onRequestResult(bool bRet, char *pData, int nDataLen)
{
    switch (m_RequestTask)
    {
    case PHONENAME:

        break;

    case EMAILNAME:
        break;
    default:
        break;
    }
}

