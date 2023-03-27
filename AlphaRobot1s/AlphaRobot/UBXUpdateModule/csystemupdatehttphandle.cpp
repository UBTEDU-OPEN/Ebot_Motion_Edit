#include "csystemupdatehttphandle.h"

#include "cstringhandle.h"
#include "../Common/regexp.h"
#include "../Common/Util.h"


CSystemUpdateHttpHandle::CSystemUpdateHttpHandle()
{

}

CSystemUpdateHttpHandle::~CSystemUpdateHttpHandle()
{

}

/**************************************************************************
* 函数名: CompareVersion
* 功能: 比较版本
* 参数:
*    @[in ] sLocalVer: 本地版本，版本号如1.0.0.1
*    @[in ] sUpdateVer: 更新版本，版本号如1.0.0.1
*    @[in ] chktype: 更新类型
* 返回值: 更新版本较新返回1，更新版本较旧返回-1，相等返回0
* 时间: 2017/07/18 9:49
* 作者: asu
*/
int CSystemUpdateHttpHandle::compareVersion(const QString &sLocalVer, const QString &sUpdateVer)
{
    QString strLocalVersion = CSystemUpdateHttpHandle::getOnlyNumVersion(sLocalVer);
    QString strUpdateVersion = CSystemUpdateHttpHandle::getOnlyNumVersion(sUpdateVer);

    vector<QString> vecLocalVerNumList = CUtil::StrSplit(strLocalVersion, '.');
    vector<QString> vecUpgradeVerNumList = CUtil::StrSplit(strUpdateVersion, '.');
    const int nvecLocalVerNumListSize = vecLocalVerNumList.size();
    const int nvecUpgradeVerNumListSize = vecUpgradeVerNumList.size();

    for(int i = 0; (i < nvecLocalVerNumListSize) && (i < nvecUpgradeVerNumListSize); i++)
    {
        int nLocalVerNum = vecLocalVerNumList[i].toInt();
        int nUpgradeVerNum = vecUpgradeVerNumList[i].toInt();
        if(nUpgradeVerNum > nLocalVerNum)
            return 1;
        else if(nUpgradeVerNum < nLocalVerNum)
        {
            return -1;
        }
    }

    return 0;
}

/**************************************************************************
* 函数名: getOnlyNumVersion
* 功能:   alphaV1.0.0.1 => 1.0.0.1
* 参数:
*    @[in ] sLocalVer: 本地版本，版本号如1.0.0.1
* 返回值:
* 时间: 2017/07/18 9:49
* 作者: asu
*/
QString CSystemUpdateHttpHandle::getOnlyNumVersion(const QString &sLocalVer)
{
    QRegExp rx = CRegExp::getRegExpVersionNumDot();
    QRegExpValidator v(rx, 0);

    const int nStrSize = sLocalVer.size();
    for(int i = 0; i < nStrSize; i++)
    {
        int nPos = 0;
        QString strTemp = sLocalVer.right(nStrSize - i);
        QValidator::State  stateTemp = v.validate(strTemp, nPos);
        if(stateTemp == QValidator::Acceptable)
        {
            return strTemp;
        }
    }
    return "";
}

QString CSystemUpdateHttpHandle::getUrlRobotUpdateLoad()
{
    QString strUrlUpdateLoad;
    CIniConfig::getInstance().getValueServerRobotUpate(strUrlUpdateLoad);
    strUrlUpdateLoad = strUrlUpdateLoad + "/file/download";
    return strUrlUpdateLoad;
}

QString CSystemUpdateHttpHandle::getUrlRobotUpdateToken()
{
    QString strUrlToken;
    CIniConfig::getInstance().getValueServerRobotUpate(strUrlToken);
    strUrlToken = strUrlToken + "/oauth/token";
    return strUrlToken;
}

QString CSystemUpdateHttpHandle::getUrlParamsRobotUpdateLoad(const QString& strUpdateToken, const QString& strCurrentVersion,
                                                             const QString& strRobotID, const QString& strRobotType)
{
    //http://10.10.20.30:8020/file/download?access_token=94d67d4f94c24616b704ffbdadb32930&model=1-v1.0&robot_id=NBA19123ECC7A26A341B2E51ACC08AB2&robot_type=zqd
    // 下载升级文件接口示例：
    // http://120.25.57.42:8084/upgrade/file/download?access_token=94d67d4f94c24616b704ffbdadb32930&model=1-v1.0&robot_id=NBA19123ECC7A26A341B2E51ACC08AB2&robot_type=zqd

    QString strTempVersion = strCurrentVersion.trimmed();
    strTempVersion = strTempVersion.toUpper();
    QString strVersion = "alpha1e_V";
    strVersion = strVersion.toUpper();
    strTempVersion = strTempVersion.replace(strVersion, "");

    QString strRobotUpdateLoad = "access_token=%1&model=1-v%2&robot_id=%3&robot_type=%4";
    strRobotUpdateLoad = strRobotUpdateLoad.arg(strUpdateToken)
            .arg(strTempVersion)
            .arg(strRobotID)
            .arg(strRobotType);

    return strRobotUpdateLoad;
}

QString CSystemUpdateHttpHandle::getUrlParamsRobotUpdateToken()
{
    // http://10.10.20.30:8020/oauth/token?client_id=mobile_1&client_secret=secret_1&grant_type=password&username=aa&password=aa
    // 正式环境获取token接口：
    // http://120.25.57.42:8084/upgrade/oauth/token?client_id=mobile_1&client_secret=secret_1&grant_type=password&username=aa&password=aa
     QString strRobotUpdateToken = "client_id=mobile_1&client_secret=secret_1&grant_type=password&username=aa&password=aa";
     return strRobotUpdateToken;
}

QString CSystemUpdateHttpHandle::getUpdateTokenFromRes(const QString &strResJsonToken)
{
    QString strToken;
    QByteArray data = strResJsonToken.toStdString().c_str();
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);

    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(!obj.empty())
            {
                strToken = obj.take("access_token").toString();
            }
        }
    }
    return strToken;
}

QString CSystemUpdateHttpHandle::getVersionFromRes(const QString &strResJsonToken)
{
   // {"data":{"times":[{"time":"3600000","module_id":"1"}],"version":[{"upgradeType":"0","module_id":"1","remark":"差分文件","fromVersion":"v0.1.5",
    //"md5":"0FC2D2955E9DCFD8BB2D205E0CD52E20","MD5Main":"677AC4A623CE2A5D8080BE046D90E9C9","updateType":"0","toVersion":"v0.1.6",
    //"urlMain":"http://10.10.20.30:8010/server_30/main/92/1/1499160818710/alpha1e_v0.1.6.swu.tar.gz",
    //"url":"http://10.10.20.30:8010/server_30/diff/92/1/1499160818905/alpha1e_v0.1.5_v0.1.6.patch.tar.gz"}]},"code":"0","msg":"success"}

    QString strTempVersion;
    QByteArray data = strResJsonToken.toStdString().c_str();
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);

    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(!obj.empty())
            {
                QString strMsg = obj.take("msg").toString();
                if(strMsg.compare("success", Qt::CaseInsensitive) == 0)
                {
                    QJsonObject dataObject = obj.take("data").toObject();
                    QJsonValue versionValue = dataObject.take("version");
                    if(versionValue.isArray())
                    {
                        QJsonValue ver = versionValue.toArray().at(0);
                        QJsonObject data1 = ver.toObject();
                        strTempVersion = data1.take("toVersion").toString();
                    }
                }
            }
        }
    }

    return strTempVersion;
}

int CSystemUpdateHttpHandle::checkUpdate(const QString &sLocalVer, FILEINFO& fileinfo)
{
    CBaseHttpOperSync httpoper;  //http文件操作对象
    byte* pOutData = NULL;
    qint64 nOutLen = 0;
    char sErr[255] = {0};

    int nRet = CHECKRESULT_NOUPDATE;
    do
    {
        QString strlToken = CSystemUpdateHttpHandle::getUpdateToken();
        QString strUrlUpdateLoad = CSystemUpdateHttpHandle::getUrlRobotUpdateLoad();
        QString strUpdateLoadParam = CSystemUpdateHttpHandle::getUrlParamsRobotUpdateLoad(strlToken, sLocalVer,
                                                                                          STR_SYS_UPDATE_ROBOT_ROBOTID,
                                                                                          STR_SYS_UPDATE_ROBOT_ROBOTTYPE);

        strUrlUpdateLoad += "?" + strUpdateLoadParam;
        ///////////////////////////Log////////////////////////////////////////////////////////////
        QString strLog = QString("[UpdateLoad] URL->%1").arg(strUrlUpdateLoad);
        AlphaRobotLog::getInstance()->printDebug(strLog);
        //////////////////////////////////////////////////////////////////////////////////////////////


        bool bOk = httpoper.ExecuteRequest(eRequestGet, strUrlUpdateLoad, NULL,
                                           NULL, NULL, pOutData, nOutLen, sErr);
        if(!bOk)
        {
            nRet = CHECKRESULT_ERROR;
            AlphaRobotLog::getInstance()->printError(QString(sErr));
            break;
        }

        QString strRes = CStringHandle::getQString((char*)pOutData, nOutLen);
        const QString strVersionFromServer = CSystemUpdateHttpHandle::getVersionFromRes(strRes);

        ///////////////////////////Log////////////////////////////////////////////////////////////
        strLog = QString("UpdateLoad check answer: %1").arg(QString(strRes));
        AlphaRobotLog::getInstance()->printDebug(strLog);
        strLog = QString("last version: %1").arg(QString(strVersionFromServer));
        AlphaRobotLog::getInstance()->printDebug(strLog);
        //////////////////////////////////////////////////////////////////////////////////////////////

        fileinfo.m_sVersion = strVersionFromServer;

        if(strVersionFromServer.isEmpty())
        {
            nRet = CHECKRESULT_ERROR;
        }
        else
        {


            int nCompareVersion = CSystemUpdateHttpHandle::compareVersion(sLocalVer, strVersionFromServer);
            if(nCompareVersion > 0)
            {
                nRet = CHECKRESULT_NEEDUPDATE;
            }
            else
            {
                nRet = CHECKRESULT_NOUPDATE;
            }
        }

    } while (0);

    SAFE_FREE(pOutData);
    return nRet;
}

QString CSystemUpdateHttpHandle::getUpdateToken()
{
    CBaseHttpOperSync httpoper;  //http文件操作对象
    QString strTokenGet;

    byte* pOutData = NULL;
    qint64 nOutLen = 0;
    char sErr[255] = {0};

    QString strUrlToken = CSystemUpdateHttpHandle::getUrlRobotUpdateToken();
    QString strParams = CSystemUpdateHttpHandle::getUrlParamsRobotUpdateToken();

    strUrlToken += "?" + strParams;
    ///////////////////////////Log////////////////////////////////////////////////////////////
    QString strLog = QString("[token] URL->%1").arg(strUrlToken);
    AlphaRobotLog::getInstance()->printDebug(strLog);
    //////////////////////////////////////////////////////////////////////////////////////////////

    bool bOk = httpoper.ExecuteRequest(eRequestGet, strUrlToken, NULL, NULL, NULL, pOutData, nOutLen, sErr);
    if(!bOk)
    {
        AlphaRobotLog::getInstance()->printError(QString(sErr));
    }
    else
    {
        QString strTokenRes = CStringHandle::getQString((char*)pOutData, nOutLen);
        strTokenGet = CSystemUpdateHttpHandle::getUpdateTokenFromRes(strTokenRes);
        AlphaRobotLog::getInstance()->printDebug(strTokenRes);
    }

    SAFE_FREE(pOutData);

    return strTokenGet;
}


