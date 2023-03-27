/* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：ActUpdateMangr.cpp
* 创建时间：2015/09/19 16:27
* 文件标识：
* 文件摘要：动作表更新管理器
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/09/19 16:27
* 版本摘要：
*/
// ActUpdateMangr.cpp : 实现文件
//

#include "stdafx.h"
#include "ActUpdateMangr.h"
#include "Json/json.h"
#include <QFileInfo>
#include <QDateTime>

#include "Markup.h"
#include "httptransfer.h"
#include "httpdownload.h"
#include "ubxactionlibconstants.h"
#include "alpharobotlog.h"
#include "filedirhandle.h"
#include "../zxLib/configs.h"
#include "../zxLib/inotify.h"

#include "cqiniuupoadhandle.h"
#include "ubtlib.h"


CActUpdateMangr    *g_actupdatemgr = NULL;   //更新动作表管理器

const UINT NOTIFY_MSG_ACTLIB_QUERYLIST_SEND =    QEvent::registerEventType();        //发送查询动作列表数据消息
const UINT NOTIFY_MSG_ACTLIB_QUERYLIST_RECV =    QEvent::registerEventType();        //查询动作列表数据消息
const UINT NOTIFY_MSG_ACTLIB_DOWNLOAD_PROGRESS = QEvent::registerEventType();     //动作表下载进度消息
const UINT NOTIFY_MSG_ACTLIB_DOWNLOAD_RESULT =   QEvent::registerEventType();      //动作表下载结果消息
const UINT NOTIFY_MSG_ACTLIB_DOWNLOAD_READY =    QEvent::registerEventType();        //准备下载动作表消息
const UINT NOTIFY_MSG_ACTLIB_SYNCACTION_PROCESS =    QEvent::registerEventType();        //同步动作进度通知消息



const QString ACT_INFO_ELEMENT_ACTLIST = "ACTLIST";
const QString ACT_INFO_ELEMENT_ACTINFO = "ACTINFO";
const QString ACT_INFO_ELEMENT_GUID = "GUID";
const QString ACT_INFO_ELEMENT_ITEMID = "ITEMID";
const QString ACT_INFO_ELEMENT_IMAGEHEADURL = "IMAGEHEADURL";
const QString ACT_INFO_ELEMENT_IMAGEURL = "IMAGEURL";
const QString ACT_INFO_ELEMENT_VIDEOURL = "VIDEOURL";
const QString ACT_INFO_ELEMENT_ACTNAME = "ACTNAME";
const QString ACT_INFO_ELEMENT_ACTDESC = "ACTDESC";
const QString ACT_INFO_ELEMENT_PUBLISHTIME = "PUBLISHTIME";
const QString ACT_INFO_ELEMENT_ACTSIZE = "ACTSIZE";
const QString ACT_INFO_ELEMENT_DOWNLOADURL = "DOWNLOADURL";
const QString ACT_INFO_ELEMENT_ACTTYPE = "ACTTYPE";
const QString ACT_INFO_ELEMENT_ACTTIMELEN = "ACTTIMELEN";
const QString ACT_INFO_ELEMENT_DOWNLOADS = "DOWNLOADS";
const QString ACT_INFO_ELEMENT_COMMENTS = "COMMENTS";
const QString ACT_INFO_ELEMENT_LIKES = "LIKES";
const QString ACT_INFO_ELEMENT_COLLECTTIME = "COLLECTTIME";
const QString ACT_INFO_ELEMENT_PUBLISHER = "PUBLISHER";
const QString ACT_INFO_ELEMENT_TITLE = "TITLE";
const QString ACT_INFO_ELEMENT_LOCALPATH = "LOCALPATH";
const QString ACT_INFO_ELEMENT_HASDOWNLOADED = "HASDOWNLOADED";


//动作表更新任务类型
enum enumActUpdateTaskType
{
    eActUpdateTypeQueryList, //查询动作表列表
    eActUpdateTypeDownload,  //下载动作表
};

//动作表线程任务
class CActThreadTask : public IThreadTask
{
public:
    /**************************************************************************
        * 函数名: CActThreadTask
        * 功能: 构造函数
        * 参数:
        *    @[in ] pUser: 调用者
        * 时间: 2015/09/22 17:56
        * 作者: ZDJ
        */
    CActThreadTask(CActUpdateMangr* pUser)
    {
        m_pUser = pUser;
    }

    CActUpdateMangr* m_pUser;
};

//动作表查询任务
class CActQueryTask : public CActThreadTask
{
public:
    /**************************************************************************
        * 函数名: CActQueryTask
        * 功能: 构造函数
        * 参数:
        *    @[in ] pUser: 调用者
        *    @[in ] nGroup: 动作分组
        *    @[in ] nActType: 动作类型
        *    @[in ] nPageNum: 页号
        *    @[in ] nPageSize: 页大小
        * 时间: 2015/09/22 17:56
        * 作者: ZDJ
        */
    CActQueryTask(CActUpdateMangr* pUser, int nGroup, int nActType, int nPageNum, int nPageSize, int nCountryCode)
        : CActThreadTask(pUser)
    {
        m_nGroupType = nGroup;
        m_nActType = nActType;
        m_nPageNum = nPageNum;
        m_nPageSize = nPageSize;
        m_nCountryCode = nCountryCode;
    }

    /**************************************************************************
        * 函数名: DoTask
        * 功能: 执行任务
        * 时间: 2015/09/22 17:50
        * 作者: ZDJ
        */
    virtual void DoTask()
    {
        vector<ActItemData> pActItemList;
        int nTotalPage = 0;

        SQUERYACTLISTPARA inParas;
        inParas.nGroup = m_nGroupType;
        inParas.nActType = m_nActType;
        inParas.nPageNum = m_nPageNum;
        inParas.nPageSize = m_nPageSize;
        inParas.nOldRequestID = -1;
        inParas.nCountryCode = m_nCountryCode;
//        m_pUser->QueryActListProc(m_nGroupType, m_nActType, m_nPageNum, m_nPageSize, -1, m_nCountryCode, pActItemList, nTotalPage);
        m_pUser->QueryActListProc(inParas);

        m_pUser->NotifyObservers(NOTIFY_MSG_ACTLIB_QUERYLIST_RECV, NULL, nTotalPage, (LPARAM)&pActItemList);
    }

private:
    int m_nGroupType;//动作分组
    int m_nActType;  //动作表类型
    int m_nPageNum;  //页号
    int m_nPageSize; //页大小
    int m_nCountryCode; //国家码
};

//动作表下载任务
class CActDownloadTask : public CActThreadTask
{
public:
    /**************************************************************************
        * 函数名: CActDownloadTask
        * 功能: 构造函数
        * 参数:
        *    @[in ] pUser: 使用的列表控件
        *    @[in ] nItem: 选择项索引
        * 时间: 2015/09/22 17:56
        * 作者: ZDJ
        */
    CActDownloadTask(CActUpdateMangr* pUser, const ActItemData& actdata)
        : CActThreadTask(pUser)
    {
        m_actdata = actdata;
    }

    /**************************************************************************
        * 函数名: DoTask
        * 功能: 执行任务
        * 时间: 2015/09/22 17:50
        * 作者: ZDJ
        */
    virtual void DoTask()
    {
        m_pUser->DownloadProc(this);
    }

public:	
    ActItemData  m_actdata;   //下载动作数据
};


// CActUpdateMangr 对话框

CActUpdateMangr::CActUpdateMangr()
    : m_bLoginFlag(FALSE)
{
    m_pHttpOper = new QtHttpOperSync();

    LoadConfig();

    removeRedundantActionFiles();//must have to before than removeRedundantActionInfos
    removeRedundantActionInfos();//must have to after to removeRedundantActionFiles

    //注册元对象类型，以便信号和槽能响应
    qRegisterMetaType<ActItemData>("ActItemData");
    qRegisterMetaType<MapStringDataType>("MapStringDataType");

    m_pHttpTrans = new HttpTransfer();
    connect(m_pHttpTrans, &HttpTransfer::SigItemDate, this, &CActUpdateMangr::SigItemDate);
    connect(m_pHttpTrans, &HttpTransfer::SigItemUpdate, this, &CActUpdateMangr::SigItemUpdate);
    connect(m_pHttpTrans, &HttpTransfer::SigTotalPage, this, &CActUpdateMangr::SigTotalPage);
    connect(m_pHttpTrans, &HttpTransfer::sigRecordCount, this, &CActUpdateMangr::sigRecordCount);
    connect(m_pHttpTrans, &HttpTransfer::SigTotalFavorites, this, &CActUpdateMangr::SigTotalFavorites);
    connect(m_pHttpTrans, &HttpTransfer::SigTotalMyUploads, this, &CActUpdateMangr::SigTotalMyUploads);
    connect(m_pHttpTrans, &HttpTransfer::SigIsCollectResult, this, &CActUpdateMangr::SigIsCollectResult);
    connect(m_pHttpTrans, &HttpTransfer::SigCancelMyUpload, this, &CActUpdateMangr::SigCancelMyUpload);
    connect(m_pHttpTrans, &HttpTransfer::SigIsAddDownLoadResult, this, &CActUpdateMangr::SigIsAddDownLoadResult);
    connect(m_pHttpTrans, &HttpTransfer::SigUploadProgress, this, &CActUpdateMangr::SigUploadProgress);
    connect(m_pHttpTrans, &HttpTransfer::sigAddRecommendAction, this, &CActUpdateMangr::sigAddRecommendAction);
    connect(m_pHttpTrans, &HttpTransfer::sigOnAddPromoteItem, this, &CActUpdateMangr::sigOnAddPromoteItem);
    connect(m_pHttpTrans, &HttpTransfer::sigOnUpdatePromoteData, this, &CActUpdateMangr::sigOnUpdatePromoteData);
    connect(m_pHttpTrans, &HttpTransfer::sigUploadActionResponse, this, &CActUpdateMangr::sigUploadActionResponse);
    connect(m_pHttpTrans, &HttpTransfer::sigOnHttpRequestFinished, this, &CActUpdateMangr::sigOnHttpRequestFinished);
    connect(m_pHttpTrans, &HttpTransfer::sigOnReceivedActionComment, this, &CActUpdateMangr::sigOnReceivedActionComment);
    connect(m_pHttpTrans, &HttpTransfer::sigOndownLoadImageDone, this, &CActUpdateMangr::sigOndownLoadImageDone);
    connect(m_pHttpTrans, &HttpTransfer::sigOnDownloadCommentReplierIconDone, this, &CActUpdateMangr::sigOnDownloadCommentReplierIconDone);

    m_nUserID = -1;
    m_pHttpDownload = new HttpDownload(g_sActPath);
    connect(m_pHttpDownload, &HttpDownload::SigDownloadProgress, this, &CActUpdateMangr::SigDownloadProgress);
    connect(m_pHttpDownload, &HttpDownload::SigStartDownload, this, &CActUpdateMangr::SigStartDownload);
    connect(m_pHttpDownload, &HttpDownload::SigHasPausedDownload, this, &CActUpdateMangr::SigHasPausedDownload);

    connect(m_pHttpDownload, &HttpDownload::SigFinishedDownload, this, &CActUpdateMangr::OnFinishedDownload);

    //    g_threadpool.Run();

}

CActUpdateMangr::~CActUpdateMangr()
{
    if(m_pHttpTrans)
    {
        m_pHttpTrans->stopThread();
        m_pHttpTrans->deleteLater();
    }
    if(m_pHttpDownload)
    {
        m_pHttpDownload->stopThread();
        m_pHttpDownload->deleteLater();
    }
    //    g_threadpool.Stop();
}

/**************************************************************************
* 函数名: QueryActList
* 功能: 查询动作列表
* 参数: 
*    @[in ] nGroup: 动作分组
*    @[in ] nActType: 动作类型
*    @[in ] nPageNo: 页号
*    @[in ] nPageSize: 页大小
*    @[in ] nOldRequestID: 请求ID
*    @[in ] nCountryCode: 国家码
* 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间: 2015/10/16 20:26
* 作者: ZDJ
*/
int CActUpdateMangr::QueryActList(int nGroup, int nActType, int nPageNo, int nPageSize, int nOldRequestID, int nCountryCode)
{
    //	IThreadTask* pNewTask = new CActQueryTask(this, nGroup, nActType, nPageNo, nPageSize);
    //    g_threadpool.AddTask(pNewTask);

//    vector<ActItemData> pActItemList;
//    int nTotalPage = 0;

    SQUERYACTLISTPARA inParas;
    inParas.nGroup = nGroup;
    inParas.nActType = nActType;
    inParas.nPageNum = nPageNo;
    inParas.nPageSize = nPageSize;
    inParas.nOldRequestID = nOldRequestID;
    inParas.nCountryCode = nCountryCode;
//    return QueryActListProc(nGroup, nActType, nPageNo, nPageSize, nOldRequestID, nCountryCode, pActItemList, nTotalPage);
    return QueryActListProc(inParas/*, pActItemList, nTotalPage*/);
}

/**************************************************************************
* 函数名: QueryRecommandAct
* 功能: 查询动作列表
* 参数:
*    @[in ] nRecommendType: 请求类型
*    @[in ] nOldRequestID: 请求ID
*    @[in ] nCountryCode: 国家码
* 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间: 2015/10/16 20:26
* 作者: ZDJ
*/
int CActUpdateMangr::QueryRecommendList(int nRecommendType, int nCountryCode, int nOldRequestID, const QString &strSysLanguage)
{
    QString sRequestURL;
    QString sAppPath = QCoreApplication::applicationFilePath();

    FILEVERINFO fileinfo;
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号

    QString sParam, sParamActParam, sParamCountryCode;
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    sParam += ",";
    sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/recommend/find";

    sParamActParam.sprintf("\"recommendType\":\"%d\"", nRecommendType);
    sParam += sParamActParam + ",";

    // 添加系统语言
    sParam += QString("\"systemLanguage\":\"%1\",").arg(strSysLanguage);

    if(nCountryCode > 0)
        sParamCountryCode.sprintf("\"countryCode\":\"%d\"", nCountryCode);
    else
        sParamCountryCode.sprintf("\"countryCode\":\"\"");  //没有国家码
    sParam += sParamCountryCode;

    sParam += "}";

    QString strDebugInfo = QString("request recommend data: [url->%1] [para->%2]").arg(sRequestURL).arg(sParam);
    AlphaRobotLog::getInstance()->printDebug(strDebugInfo);

    return m_pHttpTrans->postQueryRecommendList(sRequestURL, sParam, nOldRequestID);
}

/**************************************************************************
* 函数名: queryBusinessPromoteData
* 功能: 请求运营活动推广数据
* 参数:
*    @[in ] nOldRequestID: 请求ID
* 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间: 2016/08/30 20:26
* 作者: hwx
*/
int CActUpdateMangr::queryBusinessPromoteData(int nOldRequestID)
{
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/getScheme";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());;
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString sParam;
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    sParam += "}";

    QString strDebugInfo = QString("request business promote data: [url->%1] [para->%2]").arg(sRequestURL).arg(sParam);
    AlphaRobotLog::getInstance()->printDebug(strDebugInfo);

    return m_pHttpTrans->postQueryBusinessPromoteData(sRequestURL, sParam, nOldRequestID);
}

int CActUpdateMangr::queryRankActionData(int nOldRequestID, int nSchemeID)
{
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/host";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString sParam;
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    sParam += "}";

    QString strDebugInfo = QString("request rank action data: [url->%1] [para->%2]").arg(sRequestURL).arg(sParam);
    AlphaRobotLog::getInstance()->printDebug(strDebugInfo);

    return m_pHttpTrans->postQueryRankActionData(sRequestURL, sParam, nOldRequestID, nSchemeID);
}

/**************************************************************************
* 函数名: queryActionComment
* 功能: 查询动作评论
* 参数:
*    @[in ] nActionId: 动作id
*    @[in ] nPageId: page id
*    @[in ] nOldRequestID: 请求ID
* 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间: 2016/09/19 20:26
* 作者: hwx
*/
int CActUpdateMangr::queryActionComment(int nActionId, int nPageId, int nOldRequestId)
{
    QString strRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/comment/findNew";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString strParam;
    strParam = "{";
    strParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    strParam += ",";

    strParam += QString("\"actionId\":%1,").arg(nActionId);
    strParam += QString("\"page\":%1").arg(nPageId);

    strParam += "}";

    ///////////////////////////Log///////////////////////
    QString strDebugInfo = QString("request action comment: [url->%1] [para->%2]").arg(strRequestURL).arg(strParam);
    AlphaRobotLog::getInstance()->printDebug(strDebugInfo);
    //////////////////////////Log///////////////////////

    return m_pHttpTrans->postQueryActionComment(strRequestURL, strParam, nOldRequestId);
}

/**************************************************************************
* 函数名: addActionComment
* 功能: 增加动作评论
* 参数:
*    @[in ] nActionId: 动作ID
*    @[in] nUserId: 评论用户id
*    @[in] nRepliedCommentId: 被回复的评论id
*    @[in] strComment: 评论内容
*    @[in] eType: 评论类型
*    @[in ] nOldRequestID: 请求ID
* 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间: 2016/09/20 9:48
* 作者: hwx
*/
int CActUpdateMangr::addActionComment(int nActionId, int nUserId, int nRepliedCommentId, const QString &strComment, Constants::emCommentType eType, int nOldRequestId)
{
    QString strRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/comment/addNew";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString strParam;
    strParam = "{";
    strParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    strParam += ",";

    strParam += QString("\"commentType\":%1,").arg((int)eType);
    strParam += QString("\"actionId\":%1,").arg(nActionId);
    strParam += QString("\"commentUserId\":%1,").arg(nUserId);
    strParam += QString("\"commentContext\":\"%1\",").arg(strComment);
    strParam += QString("\"replyCommentId\":%1").arg(nRepliedCommentId);
    strParam += "}";

    ////////////////////////////////////Log///////////////////////////
    QString strDebugInfo = QString("request add action comment: [url->%1] [para->%2]").arg(strRequestURL).arg(strParam);
    AlphaRobotLog::getInstance()->printDebug(strDebugInfo);
    ////////////////////////////////////Log///////////////////////////

    return m_pHttpTrans->postAddActionComment(strRequestURL, strParam, nOldRequestId);
}

/************************************
* 名称: QueryFavorites
* 功能: 查询我的收藏
* 参数: [in]nOldRequestID 旧的请求ID
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
int CActUpdateMangr::QueryFavorites(int nOldRequestID)
{
    QString sRequestURL;
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString sParam, sParamActParam;
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    sParam += ",";

    sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/collect/find";
    sParamActParam.sprintf("\"collectUserId\":\"%d\"", m_nUserID);
    sParam += sParamActParam;
    sParam += "}";

    AlphaRobotLog::getInstance()->printDebug(QString("requeset for favourite actions: %1").arg(sRequestURL + sParam));
    return m_pHttpTrans->postQueryFavorites(sRequestURL, sParam, nOldRequestID);
}

/**************************************************************************
* 函数名: ActUserLogin
* 功能: 动作表用户登录验证
* 参数: 
*    @[in ] sUserName: 用户名
*    @[in ] sPwd: 密码
*    @[out] nUserId: 用户id
* 返回值: 登录成功返回TRUE，失败返回FALSE
* 时间: 2015/10/08 10:15
* 作者: ZDJ
*/
BOOL CActUpdateMangr::ActUserLogin(const QString& sUserName, const QString& sPwd, int& nUserId)
{
    byte* pOutData = NULL;
    qint64 nOutLen = 0;
    char sErr[255] = {0};
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/login";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    BOOL bRet = FALSE;
    do
    {
        QString sParam = "{";
        sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
        QString sTemp;
        sTemp.sprintf(",\"userName\":\"%s\",\"userPassword\":\"%s\"", sUserName.toStdString().c_str(), sPwd.toStdString().c_str());

        sParam += sTemp;
        sParam += "}";

        if(!m_pHttpOper->ExecuteRequest(
                    eRequestPost, sRequestURL, sParam.toStdString().c_str(), NULL, NULL, pOutData, nOutLen, sErr))
        {
            break;
        }


        string sData = Utf8ToAnsiEx(pOutData, nOutLen);

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse(sData.c_str(), jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(!jvalue_root["status"].isNull() && jvalue_root["status"].asBool())
                {
                    int i = 0;
                    if(!jvalue_root["models"][i]["userId"].isNull())
                        nUserId = jvalue_root["models"][i]["userId"].asInt();
                    bRet = TRUE;
                }
            }
        }

    } while (0);

    SAFE_FREE(pOutData);
    return bRet;
}

/**************************************************************************
* 函数名: ActUserReg
* 功能: 动作表用户注册
* 参数: 
*    @[in ] sUserName: 用户名
*    @[in ] sPwd: 密码
*    @[in ] nType: 帐号类型，1为邮箱，2为手机号
* 返回值: 注册成功返回TRUE，失败返回FALSE
* 时间: 2015/10/08 10:15
* 作者: ZDJ
*/
BOOL CActUpdateMangr::ActUserReg(const QString& sUserName, const QString& sPwd, int nType)
{
    byte* pOutData = NULL;
    qint64 nOutLen = 0;
    char sErr[255] = {0};
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/register";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    BOOL bRet = FALSE;
    do
    {
        QString sParam = "{";
        sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
        QString sTemp;
        sTemp.sprintf(",\"userName\":\"%s\",\"userPassword\":\"%s\",\"type\":\"%d\"", sUserName.toStdString().c_str(), sPwd.toStdString().c_str(), nType);

        sParam += sTemp;
        sParam += "}";

        if(!m_pHttpOper->ExecuteRequest(
                    eRequestPost, sRequestURL, sParam.toStdString().c_str(), NULL, NULL, pOutData, nOutLen, sErr))
        {
            break;
        }

        string sData = Utf8ToAnsiEx(pOutData, nOutLen);

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse(sData.c_str(), jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(!jvalue_root["status"].isNull() && jvalue_root["status"].asBool())
                    bRet = TRUE;
            }
        }


    } while (0);

    SAFE_FREE(pOutData);
    return bRet;
}

/**************************************************************************
* 函数名: DownloadProc
* 功能: 下载处理函数
* 参数: 
*    @[in ] lpParam: 参数
* 时间: 2015/09/23 20:15
* 作者: ZDJ
*/
void CActUpdateMangr::DownloadProc(LPVOID lpParam)
{
#if 1
    CActDownloadTask* pTask = (CActDownloadTask*)lpParam;

    g_actupdatemgr->AddDownloadingActInfo(pTask->m_actdata);//添加正在下载的动作配置信息
    NotifyObservers(NOTIFY_MSG_ACTLIB_DOWNLOAD_READY, NULL, TRUE, (LPARAM)&pTask->m_actdata);

    QString sRequestURL = pTask->m_actdata.m_sActDownloadURL;
    QString sActPath = g_sActPath;

    if(!PathFileExists(sActPath))
    {
        QDir dir;
        if(!dir.mkpath(sActPath))
        {
            //CMessageBox(_T("create act dir failed"));
            return;
        }
    }

    QString sFileName = sRequestURL.mid(sRequestURL.lastIndexOf(QDir::separator()) + 1);
    QString sShortName = sFileName;
    int nTempIndex = sShortName.lastIndexOf('.');
    if(nTempIndex != -1)
        sShortName = sShortName.left(nTempIndex);

    //CString sPrefix = g_sActTypePrefix[pTask->m_actdata.m_nActType];//文件名前缀
    //if(sFileName.Find(sPrefix) != 0)
    //	sActPath += sShortName + _T("\\") + sPrefix + sFileName;
    //else
    sActPath += sShortName + "\\" + sFileName;

    //sActPath += sPrefix + sTempFileName;

    BOOL bResult = DownloadFile(sRequestURL, sActPath, OnActDownloadNotify, pTask);
    if(bResult)
    {
        g_actupdatemgr->RemoveDownloadingActInfo(pTask->m_actdata.m_sGuid);
        g_actupdatemgr->AddDownloadedActInfo(pTask->m_actdata);
    }
    NotifyObservers(NOTIFY_MSG_ACTLIB_DOWNLOAD_RESULT, NULL, bResult, (LPARAM)&pTask->m_actdata);

#endif
}

/**************************************************************************
* 函数名: QueryActListProc
* 功能: 查询动作列表
* 参数: 
*    @[in ] nGroup: 分组类型
*    @[in ] nActType: 动作类型
*    @[in ] nPageNum: 页号
*    @[in ] nPageSize: 页大小
*    @[in ] nOldRequestID: 请求ID
*    @[in ] nCountryCode: 国家码
*    @[out] actitemlist: 动作列表
*    @[out] nTotalPage: 总页数
* 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间: 2015/09/23 9:48
* 作者: ZDJ
*/
//int CActUpdateMangr::QueryActListProc(int nGroup, int nActType, int nPageNum, int nPageSize, int nOldRequestID, int nCountryCode, vector<ActItemData>& /*actitemlist*/, int& /*nTotalPage*/)
int CActUpdateMangr::QueryActListProc(const SQUERYACTLISTPARA &inParas)
{
    //    byte* pOutData = NULL;
    //    qint64 nOutLen = 0;
    //    TCHAR sErr[255] = {0};y

    //url
    QString sRequestURL;
    sRequestURL.clear();

    switch(inParas.nGroup)
    {
    case eActGroupAllLastUpdate:
    case eActGroupOfficalLastUpdate:
    {
        // Modify by huangwuxian 2017.03.03 参见活动的动作在动作列表里边没有获取到
        sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/getListByPageForPc";
        //sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/getListByPage";
        break;
    }

    case eActGroupAllHotest:
    case eActGroupOfficalHotest:
    {
        sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/getListByPageHost";
        break;
    }

    case eActGroupAllMostComment:
    case eActGroupOfficalMostComment:
    {
        sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/getListByPageComment";
        break;
    }

    default:
    {
        sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/getListByPage";
        break;
    }
    }

    switch(inParas.eRequestType)
    {
    case eReqTypeMyuploadActionsDetail:
    {
        sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/getListByPageForPc";
        break;
    }

    default:
    {
        break;
        //nothing
    }
    }

    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    ////////////
    QString sParam;
    sParam.clear();
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    const QString strComma = ",";

    //actionResource全部不传这个字段,  用户上传传1, 官方传0, 这个是string类型
    QString strActionResource;
    strActionResource.clear();

    switch(inParas.nGroup)
    {
    case eActGroupOfficalLastUpdate:
    case eActGroupOfficalHotest:
    case eActGroupOfficalMostComment:
    {
        strActionResource = QString("0");//官方传0
        break;
    }

    case eActGroupAllLastUpdate:
    case eActGroupAllHotest:
    case eActGroupAllMostComment:
    {
        strActionResource = QString("1");//用户上传传1
        break;

    }
    default:
    {
        strActionResource.clear();//全部不传这个字段
        break;
    }
    }

    if(!strActionResource.isEmpty())
    {
        QString actionResource = QString("\"actionResource\":\"%1\"").arg(strActionResource);

        sParam += strComma;
        sParam += actionResource;
    }

    //
    if(inParas.nActType != eActTypeAll)
    {
        QString sParamActParam;
        sParamActParam.sprintf("\"actionSonType\":\"%d\"", inParas.nActType);

        sParam += strComma;
        sParam += sParamActParam;
    }

    QString sParamCountryCode;
    if(inParas.nCountryCode > 0)
    {
        sParamCountryCode.sprintf("\"countryCode\":\"%d\"", inParas.nCountryCode);
    }
    else
    {
        sParamCountryCode.sprintf("\"countryCode\":\"\"");  //没有国家码
    }
    sParam += strComma;
    sParam += sParamCountryCode;

    QString sParamPageNum;
    sParamPageNum.sprintf("\"page\":\"%d\"", inParas.nPageNum);
    sParam += strComma;
    sParam += sParamPageNum;

    if(inParas.nPageSize != -1)
    {
        QString sParamPageSize;
        sParamPageSize.sprintf("\"pageSize\":\"%d\"", inParas.nPageSize);

        sParam += strComma;
        sParam += sParamPageSize;
    }

    if(inParas.nUserID > 0)
    {
        QString userID;
        userID.sprintf("\"actionUser\":\"%d\"", inParas.nUserID);

        sParam += strComma;
        sParam += userID;
    }

    sParam += "}";

    QString strDebug = QString("QueryActListProc Url: %1\n parameter: %2").arg(sRequestURL).arg(sParam);
    AlphaRobotLog::getInstance()->printDebug(strDebug);

    //qDebug()<<"postQueryActList::"<<"sRequestURL:"<<sRequestURL<<"sparam"<<sParam<<endl;
    return m_pHttpTrans->postQueryActList(sRequestURL, sParam, inParas.nOldRequestID);

    //    BOOL bRet = FALSE;
    //	do
    //    {
    //        const char* pszParam = sParam.toStdString().c_str();
    //        if(!m_pHttpOper->ExecuteRequest(
    //            eRequestPost, sRequestURL, pszParam, NULL, NULL, pOutData, nOutLen, sErr))
    //		{
    //			break;
    //		}

    //        string sData = Utf8ToAnsiEx(pOutData, nOutLen);

    //		Json::Reader jsonreader;
    //		Json::Value  jvalue_root;
    //		if(jsonreader.parse(sData.c_str(), jvalue_root))
    //		{
    //			if(jvalue_root.size() > 0)
    //			{
    //				if(!jvalue_root["totalPage"].isNull())
    //				{
    //					nTotalPage = jvalue_root["totalPage"].asInt();
    //				}
    //#if 1
    //				for(int i = 0; i < jvalue_root["models"].size(); i++)
    //				{
    //					int nActType = -1;
    //					if(!jvalue_root["models"][i]["actionType"].isNull())
    //						nActType = jvalue_root["models"][i]["actionType"].asInt();

    //					if(nActType == -1)
    //						continue;

    //					int nActId = 0;
    //					if(!jvalue_root["models"][i]["actionId"].isNull())
    //						nActId = jvalue_root["models"][i]["actionId"].asInt();

    //					char* pszName = NULL;
    //					if(!jvalue_root["models"][i]["actionName"].isNull())
    //						pszName = (char*)jvalue_root["models"][i]["actionName"].asCString();

    //					char* pszDesc = NULL;
    //					if(!jvalue_root["models"][i]["actionDesciber"].isNull())
    //						pszDesc = (char*)jvalue_root["models"][i]["actionDesciber"].asCString();

    //					char* pszImgPath = NULL;
    //					if(!jvalue_root["models"][i]["actionImagePath"].isNull())
    //						pszImgPath = (char*)jvalue_root["models"][i]["actionImagePath"].asCString();

    //					char* pszVideoPath = NULL;
    //					if(!jvalue_root["models"][i]["actionVideoPath"].isNull())
    //						pszVideoPath = (char*)jvalue_root["models"][i]["actionVideoPath"].asCString();

    //					char* pszDownloadPath = NULL;
    //					if(!jvalue_root["models"][i]["actionPath"].isNull())
    //						pszDownloadPath = (char*)jvalue_root["models"][i]["actionPath"].asCString();

    //					DWORD dwActTimeLen = 0;
    //					if(!jvalue_root["models"][i]["actionTime"].isNull())
    //						dwActTimeLen = jvalue_root["models"][i]["actionTime"].asDouble();

    //					ULONGLONG lTime = 0;
    //					if(!jvalue_root["models"][i]["actionDate"].isNull())
    //						lTime = jvalue_root["models"][i]["actionDate"].asDouble();

    //                    QDateTime timeobj;
    //                    timeobj.setTime_t(lTime/1000);
    //                    QString sTime = timeobj.toString("yyyy-MM-dd hh:mm:ss");

    //					ULONGLONG lSize = 0;
    //					if(!jvalue_root["models"][i]["actionSize"].isNull())
    //						lSize = jvalue_root["models"][i]["actionSize"].asDouble();

    //                    QString sSize = GetFormatSizeString(lSize);

    //					//DWORD dwActParise = jvalue_root["models"][i]["actionPraiseTime"].asDouble();


    //					ActItemData newitem;
    //					newitem.m_nItemID = nActId;
    //					if(pszImgPath && strlen(pszImgPath) > 0)
    //					{
    //                        newitem.m_sImageURL = pszImgPath;
    //					}
    //					else if(pszVideoPath && strlen(pszVideoPath) > 0)
    //					{
    //                        newitem.m_sVideoURL = pszVideoPath;
    //					}

    //					//test
    //					//newitem.m_nPreviewType = ePreviewImage;
    //					//newitem.m_sURL = _T("http://e.hiphotos.baidu.com/image/pic/item/1e30e924b899a90188e8393f1f950a7b0308f5de.jpg");
    //					//newitem.m_sImageURL = _T("Q:\\截图\\123.jpg");
    //					//newitem.m_sVideoURL = _T("D:\\test\\action.mp4");
    //					////////////
    //					if(pszDesc)
    //                        newitem.m_sActDesc = pszDesc;
    //					if(pszName)
    //                        newitem.m_sActName = pszName;
    //					newitem.m_sPublishTime = sTime;
    //					if(pszDownloadPath)
    //                        newitem.m_sActDownloadURL = pszDownloadPath;
    //					newitem.m_nActType = nActType;
    //					newitem.m_dwActTimeLen = dwActTimeLen;
    //					//newitem.m_dwParise = dwActParise;


    //					actitemlist.push_back(newitem);
    //#endif

    //				}
    //			}
    //		}

    //		bRet = TRUE;
    //	} while (0);

    //	SAFE_FREE(pOutData);

    //    return bRet;
}

/**************************************************************************
* 函数名: QueryActParise
* 功能: 查询动作表点赞数量
* 参数: 
*    @[in ] nActId: 动作ID
*    @[out] actParise: 点赞数量
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/09/23 9:48
* 作者: ZDJ
*/
BOOL CActUpdateMangr::QueryActParise(int nActId, int& actParise)
{
    byte* pOutData = NULL;
    qint64 nOutLen = 0;
    char sErr[255] = {0};
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/detail";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString sParam, sParamActParam, sParamPageNum, sParamPageSize;
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    sParam += ",";

    if(nActId != -1)
    {
        sParamActParam.sprintf("\"actionId\":\"%d\"", nActId);
        sParam += sParamActParam;
    }

    sParam += "}";

    BOOL bRet = FALSE;
    do
    {
        const char* pszParam = sParam.toStdString().c_str();
        if(!m_pHttpOper->ExecuteRequest(
                    eRequestPost, sRequestURL, pszParam, NULL, NULL, pOutData, nOutLen, sErr))
        {
            break;
        }

        string sData = Utf8ToAnsiEx(pOutData, nOutLen);

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse(sData.c_str(), jvalue_root))
        {
            if(jvalue_root.size() > 0)
            {
                if(!jvalue_root["models"]["actionPraiseTime"].isNull())
                    actParise = jvalue_root["models"]["actionPraiseTime"].asDouble();

                bRet = TRUE;
            }
        }

    } while (0);

    SAFE_FREE(pOutData);

    return bRet;
}

/**************************************************************************
* 函数类型: OnActDownloadNotify
* 功能: 文件下载进度线程通知
* 参数:
*    @[in ] pUser: 通知目标
*    @[in ] sFileName: 文件名
*    @[in ] nFileTotalSize: 文件长度
*    @[in ] nFileCurSize: 当前已下载长度
* 返回值: void
* 时间: 2015/06/12 11:17
* 作者: ZDJ
*/
void CALLBACK CActUpdateMangr::OnActDownloadNotify(LPVOID pUser, const QString& sFileName, qint64 nFileTotalSize, qint64 nFileCurSize)
{
    CActDownloadTask* pTask = (CActDownloadTask*)(pUser);
    if(pTask)
    {
        //CString sName = data1;
        //sName = sName.Mid(sName.ReverseFind('/') + 1);

        //CString sTemp;
        //sTemp.Format(_T("正在下载:%s 文件大小:%d 当前已下载:%d"), sName, data2, data3);
        //sTemp.Format(_T("文件大小:%d 当前已下载:%d"), data2, data3);
        //sTemp.Format(_T("%s %s   %s:%s, %s:%s"), language_loadstring(AFX_IDS_APP_TITLE), language_loadstring(IDS_SURE_UPDATEPROGRESS), language_loadstring(IDS_SURE_FILESIZE), GetFileFormatSize(data2), language_loadstring(IDS_SURE_CURDOWNLOAD), GetFileFormatSize(data3));
        //CAutolockRes res;
        //sTemp.Format(_T("%s %s"), language_loadstring(AFX_IDS_APP_TITLE), language_loadstring(IDS_SURE_UPDATEPROGRESS));
        //sTemp.Format(_T("AlphaRobot1s %s"), language_loadstring(IDS_SURE_UPDATEPROGRESS));
        //pDlg->SetWindowText(sTemp);
#if 1
        //CMyListCtrl* pListCtrl = pTask->m_pUser->GetListCtrl();
        //ASSERT(pListCtrl != NULL);

        //pListCtrl->SetProgress(pTask->m_nItem, g_nProgressCol);
        //pListCtrl->SetItemText(pTask->m_nItem, eColActStatus, LANGUAGE_LOAD(ID_UPDATEACT_ACTLIST_DOWNLOADING_TEXT));

        float fprocess = nFileCurSize*1.0/nFileTotalSize * 100;
        static int nCurCtrlPos =  0;//pDlg->m_progressctrl.GetPos();
        if((int)fprocess <= 0)
            nCurCtrlPos = 0;

        if(fprocess - nCurCtrlPos >= 1)
        {
            nCurCtrlPos = fprocess;

            //pListCtrl->SetProgress(pTask->m_nItem, fprocess);

            //sTemp.Format(_T("%d%%"), (int)fprocess);
            //pDlg->m_labelProgress.SetWindowText(sTemp);
            //::SendMessage(pTask->m_pUser->GetActLibPanel()->GetSafeHwnd(), NOTIFY_MSG_ACTLIB_DOWNLOAD_PROGRESS, pTask->m_nItemID, fprocess);
            g_actupdatemgr->NotifyObservers(NOTIFY_MSG_ACTLIB_DOWNLOAD_PROGRESS, NULL, pTask->m_actdata.m_nItemID, fprocess);

        }
#endif
    }
}

/**************************************************************************
* 函数名: DownloadFile
* 功能: 下载文件
* 参数: 
* 	@[in ] sFileURL: 文件的网络地址，地址如http://www.test.com/test.exe
* 	@[in ] sSavePath: 文件的保存路径
* 	@[in ] procnotify: 通知回调
* 	@[in ] hUser: 通知目标
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/05/15 9:32
* 作者: ZDJ
*/
BOOL CActUpdateMangr::DownloadFile(
        const QString& sFileURL,
        const QString& sSavePath,
        FILEDOWNPROCESSNOTIFY procnotify/* = NULL*/,
        LPVOID pUser /*= NULL*/)
{
    char sErr[255] = {0};
    BOOL bRet = FALSE;

    do
    {
        if(!m_pHttpOper->DownloadFile(sFileURL, sSavePath, procnotify, pUser, sErr))
            break;

        QString sFileName = sSavePath.mid(sSavePath.lastIndexOf(QDir::separator()) + 1);
        int nIndexTemp = sFileName.lastIndexOf('.');
        QString sShortFileName = sFileName.left(nIndexTemp);
        QString sFileExt = sFileName.mid(nIndexTemp);

        if(sFileExt.compare(".zip", Qt::CaseInsensitive) == 0)
        {
            //#pragma region 解压zip
            UnZipFile(sSavePath);

            bRet = TRUE;
            //#pragma endregion
        }
    } while (0);

    return bRet;
}

/**************************************************************************
* 函数名: DownloadAct
* 功能: 下载动作表
* 参数: 
*    @[in ] actdata: 动作表
* 返回值: int 请求的ID号
* 时间: 2015/10/08 16:24
* 作者: ZDJ
*/
int CActUpdateMangr::DownloadAct(const ActItemData& actdata)
{
    //先使用独立线程完成下载，以后等线程池完善了再修改回线程池的方式
    //	IThreadTask* pNewTask = new CActDownloadTask(this, actdata);
    //    g_threadpool.AddTask(pNewTask);

     QFileInfo fileInfo(actdata.m_sActDownloadURL);
     QString strFileName = QDir::toNativeSeparators(g_sActPath + "/" + fileInfo.baseName() + "/" + fileInfo.fileName());
     QFile localFile(strFileName);

     //不存在则为刚开始下载
     const bool bExistLocalFile = localFile.exists();
     if(!bExistLocalFile)
     {
         g_actupdatemgr->RemoveDownloadingActInfo(actdata.m_sGuid);
         g_actupdatemgr->RemoveDownloadedActInfo(actdata.m_sGuid);
     }

     bool bExistAtDownLoading = false;
     bool bExistAtDownLoaded = false;

     vector<ActItemData> vecActListDownloading;
     g_actupdatemgr->GetDownloadingActList(vecActListDownloading);
     vector<ActItemData>::iterator iter = vecActListDownloading.begin();
     for(; iter != vecActListDownloading.end(); iter++)
     {
         if(iter->m_nItemID == actdata.m_nItemID)
         {
             bExistAtDownLoading = true;

             break;
         }
     }


     QMap<int, ActItemData> mapActListLoaded;
     g_actupdatemgr->GetDownloadedActList(mapActListLoaded);
     bExistAtDownLoaded = mapActListLoaded.contains(actdata.m_nItemID);


     if(!bExistLocalFile && !bExistAtDownLoaded && !bExistAtDownLoading)
     {
         //
         g_actupdatemgr->AddDownloadingActInfo(actdata);//添加正在下载的动作配置信息
         emit SigStartDownload(-1, actdata, 0, -1);

         return m_pHttpDownload->DownloadAction(actdata);
     }

     return REQUSET_ID_DEFAULT_INVALID;
}

/************************************
* 名称: PauseDownload
* 功能: 暂停下载
* 参数: [in]nActionID 要暂停下载的Action ID
* 返回:   void
* 时间:   2016/04/06
* 作者:   hels
************************************/
void CActUpdateMangr::PauseDownload(int nActionID)
{
    emit m_pHttpDownload->SigToPauseDownload(nActionID);
}

/************************************
* 名称: ResumeDownload
* 功能: 重新下载
* 参数: [in]itemData 下载项
* 返回:   int 返回请求ID
* 时间:   2016/04/06
* 作者:   hels
************************************/
int CActUpdateMangr::ResumeDownload(const ActItemData &itemData)
{
    return m_pHttpDownload->ResumeDownload(itemData);
}

/**************************************************************************
* 函数名: GetDownloadedActList
* 功能: 获取已下载的动作
* 参数: 
*    @[out] mapActList: 动作列表
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/10 16:24
* 作者: ZDJ
*/
BOOL CActUpdateMangr::GetDownloadedActList(QMap<int, ActItemData>& mapActList)
{
    const QString strDownloadedActFilePath = g_sDownloadedActInfoXMLPath;
    mapActList.clear();

    return getActList(strDownloadedActFilePath, mapActList);
}

/**************************************************************************
* 函数名: AddDownloadedActInfo
* 功能: 添加已下载的动作配置信息
* 参数: 
*    @[in ] actdata: 动作配置信息
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/10 16:24
* 作者: ZDJ
*/
BOOL CActUpdateMangr::AddDownloadedActInfo(const ActItemData& actdata)
{
//    BOOL bExistFile = PathFileExists(g_sDownloadedActInfoXMLPath);

//    CMarkup xmlloader;

//    if(bExistFile)
//    {
//        if(!xmlloader.Load(g_sDownloadedActInfoXMLPath.toStdString().c_str()))
//            return FALSE;

//        if(!xmlloader.FindElem(ACT_INFO_ELEMENT_ACTLIST.toStdString()))
//            return FALSE;

//        if(!xmlloader.IntoElem())
//            return FALSE;

//        while(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTINFO.toStdString()))
//        {
//        }
//    }
//    else
//    {
//        xmlloader.SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
//        xmlloader.AddElem(ACT_INFO_ELEMENT_ACTLIST.toStdString());
//        xmlloader.IntoElem();
//    }

//    xmlloader.AddElem(ACT_INFO_ELEMENT_ACTINFO.toStdString());
//    xmlloader.SetAttrib(ACT_INFO_ELEMENT_GUID.toStdString(), actdata.m_sGuid.toStdString().c_str());

//    setXMLData(xmlloader, actdata);

////    xmlloader.SetAttrib("ITEMID", actdata.m_nItemID);
////    xmlloader.SetAttrib("IMAGEURL", actdata.m_sImageURL.toStdString().c_str());
////    xmlloader.SetAttrib("VIDEOURL", actdata.m_sVideoURL.toStdString().c_str());
////    xmlloader.SetAttrib("ACTNAME", actdata.m_sActName.toStdString().c_str());
////    xmlloader.SetAttrib("ACTDESC", actdata.m_sActDesc.toStdString().c_str());
////    xmlloader.SetAttrib("PUBLISHTIME", actdata.m_sPublishTime.toStdString().c_str());
////    xmlloader.SetAttrib("ACTSIZE", actdata.m_nActSize);
////    xmlloader.SetAttrib("DOWNLOADURL", actdata.m_sActDownloadURL.toStdString().c_str());
////    xmlloader.SetAttrib("ACTTYPE", actdata.m_nActType);
////    xmlloader.SetAttrib("ACTTIMELEN", actdata.m_dwActTimeLen);
////    xmlloader.SetAttrib("DOWNLOADS", actdata.m_nActionDownloads);    //下载总量
////    xmlloader.SetAttrib("COMMENTS", actdata.m_nActionComments);  //评论总量
////    xmlloader.SetAttrib("LIKES", actdata.m_nActionLikes);    //点赞总量
////    xmlloader.SetAttrib("COLLECTTIME", actdata.m_nActionCollectTime); //收藏总量
////    xmlloader.SetAttrib("PUBLISHER", actdata.m_sPublisher.toStdString().c_str());    //发布者
////    xmlloader.SetAttrib("TITLE", actdata.m_sActionTitle.toStdString().c_str());      //动作标题
////    xmlloader.SetAttrib("LOCALPATH", actdata.m_sLocalPath.toStdString().c_str());    //本地路径
////    xmlloader.SetAttrib("HASDOWNLOADED", actdata.m_nHasDownloadedBytes); //已经下载字节数

//    xmlloader.OutOfElem();


//    if(!xmlloader.Save(g_sDownloadedActInfoXMLPath.toStdString().c_str()))
//        return FALSE;

//    return TRUE;

    return AddActInfo(g_sDownloadedActInfoXMLPath, actdata);
}

/**************************************************************************
* 函数名: GetDownloadingActList
* 功能: 获取正在下载的动作
* 参数: 
*    @[out] vecActList: 动作列表
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/10 16:24
* 作者: ZDJ
*/
BOOL CActUpdateMangr::GetDownloadingActList(vector<ActItemData>& vecActList)
{
    vecActList.clear();

    const QString strDownloadingActListFilePath = g_sDownloadingActInfoXMLPath;
    QMap<int, ActItemData> mapActList;
    bool bGet = getActList(strDownloadingActListFilePath, mapActList);
    if(true == bGet)
    {
        QMap<int, ActItemData>::const_iterator iMap = mapActList.constBegin();
        while (iMap != mapActList.constEnd())
        {
            vecActList.push_back(iMap.value());

            ++iMap;
        }
    }

    return bGet;
}


/**************************************************************************
* 函数名: getActList
* 功能: 获取动作info
* 参数:
*    @[in ] strFilePath: 动作配置path
*    @[out] mapActList: 动作列表
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2016/05/30
* 作者: suhaiwu
*/
BOOL CActUpdateMangr::getActList(const QString &strFilePath, QMap<int, ActItemData> &mapActList)
{
    if(strFilePath.isEmpty())
    {
        return FALSE;
    }

    CMarkup xmlloader;
    if(!xmlloader.Load(strFilePath.toStdString().c_str()))
    {
        return FALSE;
    }

    mapActList.clear();

    if(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTLIST.toStdString()))
    {
        xmlloader.IntoElem();

        while(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTINFO.toStdString()))
        {
            ActItemData actdata;

            getXMLData(xmlloader, actdata);

//            actdata.m_sGuid = QString::fromStdString(xmlloader.GetAttrib("GUID"));                    //GUID
//            actdata.m_nItemID = QString::fromStdString(xmlloader.GetAttrib("ITEMID")).toInt();        //项ID
//            actdata.m_sImageURL = QString::fromStdString(xmlloader.GetAttrib("IMAGEURL"));            //预览图片地址
//            actdata.m_sVideoURL = QString::fromStdString(xmlloader.GetAttrib("VIDEOURL"));            //预览视频地址
//            actdata.m_sActName = QString::fromStdString(xmlloader.GetAttrib("ACTNAME"));              //动作名称
//            actdata.m_sActDesc = QString::fromStdString(xmlloader.GetAttrib("ACTDESC"));              //动作描述
//            actdata.m_sPublishTime = QString::fromStdString(xmlloader.GetAttrib("PUBLISHTIME"));      //动作发布时间
//            actdata.m_nActSize = QString::fromStdString(xmlloader.GetAttrib("ACTSIZE")).toDouble();    //动作文件大小
//            actdata.m_sActDownloadURL = QString::fromStdString(xmlloader.GetAttrib("DOWNLOADURL"));   //动作下载地址
//            actdata.m_nActType = QString::fromStdString(xmlloader.GetAttrib("ACTTYPE")).toInt();              //动作类型
//            actdata.m_dwActTimeLen = QString::fromStdString(xmlloader.GetAttrib("ACTTIMELEN")).toInt();       //动作时长
//            actdata.m_nActionDownloads = QString::fromStdString(xmlloader.GetAttrib("DOWNLOADS")).toDouble();    //下载总量
//            actdata.m_nActionComments = QString::fromStdString(xmlloader.GetAttrib("COMMENTS")).toDouble();  //评论总量
//            actdata.m_nActionLikes = QString::fromStdString(xmlloader.GetAttrib("LIKES")).toDouble();    //点赞总量
//            actdata.m_nActionCollectTime = QString::fromStdString(xmlloader.GetAttrib("COLLECTTIME")).toDouble();
//            actdata.m_sPublisher = QString::fromStdString(xmlloader.GetAttrib("PUBLISHER"));    //发布者
//            actdata.m_sActionTitle = QString::fromStdString(xmlloader.GetAttrib("TITLE"));      //动作标题
//            actdata.m_sLocalPath = QString::fromStdString(xmlloader.GetAttrib("LOCALPATH"));    //本地路径
//            actdata.m_nHasDownloadedBytes = QString::fromStdString(xmlloader.GetAttrib("HASDOWNLOADED")).toDouble(); //已经下载字节数


            if(actdata.m_sGuid.isEmpty() ||
                    (0 >= actdata.m_nItemID) )
            {
                //do nothing, bad data which is not to added to info list
            }
            else
            {
                mapActList.insert(actdata.m_nItemID, actdata);
            }
        }

        xmlloader.OutOfElem();
    }

    return TRUE;
}

/**************************************************************************
* 函数名: AddDownloadingActInfo
* 功能: 添加正在下载的动作配置信息
* 参数: 
*    @[in ] actdata: 动作配置信息
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/10 16:24
* 作者: ZDJ
*/
BOOL CActUpdateMangr::AddDownloadingActInfo(const ActItemData& actdata)
{
//    BOOL bExistFile = PathFileExists(g_sDownloadingActInfoXMLPath);

//    CMarkup xmlloader;

//    if(bExistFile)
//    {
//        if(!xmlloader.Load(g_sDownloadingActInfoXMLPath.toStdString().c_str()))
//            return FALSE;

//        if(!xmlloader.FindElem(ACT_INFO_ELEMENT_ACTLIST.toStdString()))
//            return FALSE;

//        if(!xmlloader.IntoElem())
//            return FALSE;

//        while(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTINFO.toStdString()))
//        {
//        }
//    }
//    else
//    {
//        xmlloader.SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
//        xmlloader.AddElem(ACT_INFO_ELEMENT_ACTLIST.toStdString());
//        xmlloader.IntoElem();
//    }

//    xmlloader.AddElem(ACT_INFO_ELEMENT_ACTINFO.toStdString());
//    xmlloader.SetAttrib(ACT_INFO_ELEMENT_GUID.toStdString(), actdata.m_sGuid.toStdString().c_str());

//    setXMLData(xmlloader, actdata);

//    xmlloader.OutOfElem();


//    if(!xmlloader.Save(g_sDownloadingActInfoXMLPath.toStdString().c_str()))
//        return FALSE;

//    return TRUE;

    return AddActInfo(g_sDownloadingActInfoXMLPath, actdata);
}

/**************************************************************************
* 函数名: AddActInfo
* 功能: 添加动作配置信息
* 参数:
*    @[in ] actdata: 动作配置信息
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2016/06/21
* 作者: asu
*/
BOOL CActUpdateMangr::AddActInfo(const QString &strFilePath, const ActItemData &actdata) const
{
    BOOL bExistFile = PathFileExists(strFilePath);

    CMarkup xmlloader;

    if(bExistFile)
    {
        if(!xmlloader.Load(strFilePath.toStdString().c_str()))
            return FALSE;

        if(!xmlloader.FindElem(ACT_INFO_ELEMENT_ACTLIST.toStdString()))
            return FALSE;

        if(!xmlloader.IntoElem())
            return FALSE;

        while(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTINFO.toStdString()))
        {
        }
    }
    else
    {
        xmlloader.SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
        xmlloader.AddElem(ACT_INFO_ELEMENT_ACTLIST.toStdString());
        xmlloader.IntoElem();
    }

    xmlloader.AddElem(ACT_INFO_ELEMENT_ACTINFO.toStdString());
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_GUID.toStdString(), actdata.m_sGuid.toStdString().c_str());

    setXMLData(xmlloader, actdata);

    xmlloader.OutOfElem();

    if(!xmlloader.Save(strFilePath.toStdString().c_str()))
        return FALSE;

    return TRUE;
}

BOOL CActUpdateMangr::UpdateDownloadingActInfo(const ActItemData &actdata)
{
    CMarkup xmlloader;
    if(!xmlloader.Load(g_sDownloadingActInfoXMLPath.toStdString().c_str()))
        return FALSE;

    BOOL bFind = FALSE;
    if(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTLIST.toStdString()))
    {
        xmlloader.IntoElem();

        while(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTINFO.toStdString()))
        {
            QString sTempGuid = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_GUID.toStdString()));            //GUID
            if(sTempGuid.compare(actdata.m_sGuid) == 0)
            {
                bFind = TRUE;
                //                xmlloader.SetAttrib("GUID", actdata.m_sGuid.toStdString().c_str());

                setXMLData(xmlloader, actdata);

//                xmlloader.SetAttrib("ITEMID", actdata.m_nItemID);
//                xmlloader.SetAttrib("IMAGEURL", actdata.m_sImageURL.toStdString().c_str());
//                xmlloader.SetAttrib("VIDEOURL", actdata.m_sVideoURL.toStdString().c_str());
//                xmlloader.SetAttrib("ACTNAME", actdata.m_sActName.toStdString().c_str());
//                xmlloader.SetAttrib("ACTDESC", actdata.m_sActDesc.toStdString().c_str());
//                xmlloader.SetAttrib("PUBLISHTIME", actdata.m_sPublishTime.toStdString().c_str());
//                xmlloader.SetAttrib("ACTSIZE", actdata.m_nActSize);
//                xmlloader.SetAttrib("DOWNLOADURL", actdata.m_sActDownloadURL.toStdString().c_str());
//                xmlloader.SetAttrib("ACTTYPE", actdata.m_nActType);
//                xmlloader.SetAttrib("ACTTIMELEN", actdata.m_dwActTimeLen);
//                xmlloader.SetAttrib("DOWNLOADS", actdata.m_nActionDownloads);    //下载总量
//                xmlloader.SetAttrib("COMMENTS", actdata.m_nActionComments);  //评论总量
//                xmlloader.SetAttrib("LIKES", actdata.m_nActionLikes);    //点赞总量
//                xmlloader.SetAttrib("COLLECTTIME",actdata.m_nActionCollectTime);//收藏总数
//                xmlloader.SetAttrib("PUBLISHER", actdata.m_sPublisher.toStdString().c_str());    //发布者
//                xmlloader.SetAttrib("TITLE", actdata.m_sActionTitle.toStdString().c_str());      //动作标题
//                xmlloader.SetAttrib("LOCALPATH", actdata.m_sLocalPath.toStdString().c_str());    //本地路径
//                xmlloader.SetAttrib("HASDOWNLOADED", actdata.m_nHasDownloadedBytes); //已经下载字节数

                break;
            }
        }

        xmlloader.OutOfElem();
    }

    if(!bFind)
        return FALSE;

    if(!xmlloader.Save(g_sDownloadingActInfoXMLPath.toStdString().c_str()))
        return FALSE;

    return TRUE;
}


/**************************************************************************
* 函数名: setXMLData
* 功能: set data to xml
* 参数:
*    @[int]
* 返回值:void
* 时间: 2016/06/21
* 作者: suhaiwu
*/
void CActUpdateMangr::setXMLData(CMarkup &xmlloader, const ActItemData &actdata)
{
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_ITEMID.toStdString(), actdata.m_nItemID);
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_IMAGEHEADURL.toStdString(), actdata.m_sImageHeadUrl.toStdString().c_str());
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_IMAGEURL.toStdString(), actdata.m_sImageURL.toStdString().c_str());
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_VIDEOURL.toStdString(), actdata.m_sVideoURL.toStdString().c_str());
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_ACTNAME.toStdString(), actdata.m_sActName.toStdString().c_str());
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_ACTDESC.toStdString(), actdata.m_sActDesc.toStdString().c_str());
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_PUBLISHTIME.toStdString(), actdata.m_sPublishTime.toStdString().c_str());
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_ACTSIZE.toStdString(), actdata.m_nActSize);
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_DOWNLOADURL.toStdString(), actdata.m_sActDownloadURL.toStdString().c_str());
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_ACTTYPE.toStdString(), actdata.m_nActType);
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_ACTTIMELEN.toStdString(), actdata.m_dwActTimeLen);
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_DOWNLOADS.toStdString(), actdata.m_nActionDownloads);    //下载总量
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_COMMENTS.toStdString(), actdata.m_nActionComments);  //评论总量
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_LIKES.toStdString(), actdata.m_nActionLikes);    //点赞总量
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_COLLECTTIME.toStdString(),actdata.m_nActionCollectTime);//收藏总数
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_PUBLISHER.toStdString(), actdata.m_sPublisher.toStdString().c_str());    //发布者
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_TITLE.toStdString(), actdata.m_sActionTitle.toStdString().c_str());      //动作标题
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_LOCALPATH.toStdString(), actdata.m_sLocalPath.toStdString().c_str());    //本地路径
    xmlloader.SetAttrib(ACT_INFO_ELEMENT_HASDOWNLOADED.toStdString(), actdata.m_nHasDownloadedBytes); //已经下载字节数
}

/**************************************************************************
* 函数名: getXMLData
* 功能: get xml datal
* 参数:
*    @[int]
* 返回值:void
* 时间: 2016/06/21
* 作者: suhaiwu
*/
void CActUpdateMangr::getXMLData(const CMarkup &xmlloader, ActItemData &actdata)
{
    actdata.m_sGuid = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_GUID.toStdString()));                    //GUID
    actdata.m_nItemID = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_ITEMID.toStdString())).toInt();        //项ID
    actdata.m_sImageHeadUrl = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_IMAGEHEADURL.toStdString()));            //预览图片地址
    actdata.m_sImageURL = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_IMAGEURL.toStdString()));            //预览图片地址
    actdata.m_sVideoURL = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_VIDEOURL.toStdString()));            //预览视频地址
    actdata.m_sActName = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_ACTNAME.toStdString()));              //动作名称
    actdata.m_sActDesc = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_ACTDESC.toStdString()));              //动作描述
    actdata.m_sPublishTime = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_PUBLISHTIME.toStdString()));      //动作发布时间
    actdata.m_nActSize = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_ACTSIZE.toStdString())).toDouble();    //动作文件大小
    actdata.m_sActDownloadURL = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_DOWNLOADURL.toStdString()));   //动作下载地址
    actdata.m_nActType = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_ACTTYPE.toStdString())).toInt();              //动作类型
    actdata.m_dwActTimeLen = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_ACTTIMELEN.toStdString())).toInt();       //动作时长
    actdata.m_nActionDownloads = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_DOWNLOADS.toStdString())).toDouble();    //下载总量
    actdata.m_nActionComments = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_COMMENTS.toStdString())).toDouble();  //评论总量
    actdata.m_nActionLikes = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_LIKES.toStdString())).toDouble();    //点赞总量
    actdata.m_nActionCollectTime = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_COLLECTTIME.toStdString())).toDouble();
    actdata.m_sPublisher = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_PUBLISHER.toStdString()));    //发布者
    actdata.m_sActionTitle = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_TITLE.toStdString()));      //动作标题
    actdata.m_sLocalPath = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_LOCALPATH.toStdString()));    //本地路径
    actdata.m_nHasDownloadedBytes = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_HASDOWNLOADED.toStdString())).toDouble(); //已经下载字节数
}

/**************************************************************************
* 函数名: RemoveDownloadedActInfo
* 功能: 移除已下载动作配置信息
* 参数: 
*    @[in ] sActGuid: 动作GUID
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/16 19:10
* 作者: ZDJ
*/
BOOL CActUpdateMangr::RemoveDownloadedActInfo(const QString& sActGuid)
{
//    CMarkup xmlloader;
//    if(!xmlloader.Load(g_sDownloadedActInfoXMLPath.toStdString().c_str()))
//        return FALSE;

//    BOOL bFind = FALSE;
//    if(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTLIST.toStdString()))
//    {
//        xmlloader.IntoElem();

//        while(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTINFO.toStdString()))
//        {
//            QString sTempGuid = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_GUID.toStdString()));            //GUID
//            if(sTempGuid.compare(sActGuid) == 0)
//            {
//                bFind = TRUE;
//                xmlloader.RemoveElem();
//                break;
//            }
//        }

//        xmlloader.OutOfElem();
//    }

//    if(!bFind)
//        return FALSE;

//    if(!xmlloader.Save(g_sDownloadedActInfoXMLPath.toStdString().c_str()))
//        return FALSE;

//    return TRUE;

    return RemoveActInfo(g_sDownloadedActInfoXMLPath, sActGuid);
}

/**************************************************************************
* 函数名: RemoveDownloadingActInfo
* 功能: 移除正在下载动作配置信息
* 参数: 
*    @[in ] sActGuid: 动作GUID
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/16 19:10
* 作者: ZDJ
*/
BOOL CActUpdateMangr::RemoveDownloadingActInfo(const QString& sActGuid)
{
//    CMarkup xmlloader;
//    if(!xmlloader.Load(g_sDownloadingActInfoXMLPath.toStdString().c_str()))
//        return FALSE;

//    BOOL bFind = FALSE;
//    if(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTLIST.toStdString()))
//    {
//        xmlloader.IntoElem();

//        while(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTINFO.toStdString()))
//        {
//            QString sTempGuid = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_GUID.toStdString()));            //GUID
//            if(sTempGuid.compare(sActGuid) == 0)
//            {
//                bFind = TRUE;
//                xmlloader.RemoveElem();
//                break;
//            }
//        }

//        xmlloader.OutOfElem();
//    }

//    if(!bFind)
//        return FALSE;

//    if(!xmlloader.Save(g_sDownloadingActInfoXMLPath.toStdString().c_str()))
//        return FALSE;

//    return TRUE;

    return RemoveActInfo(g_sDownloadingActInfoXMLPath, sActGuid);
}

/**************************************************************************
* 函数名: RemoveActInfo
* 功能: 移除已下载动作配置信息
* 参数:
*    @[in ] sActGuid: 动作GUID
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2016/06/21
* 作者: asu
*/
BOOL CActUpdateMangr::RemoveActInfo(const QString& strFilePath, const QString& sActGuid)
{
    CMarkup xmlloader;
    if(!xmlloader.Load(strFilePath.toStdString()))
    {
        return FALSE;
    }

    BOOL bFind = FALSE;
    if(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTLIST.toStdString()))
    {
        xmlloader.IntoElem();

        while(xmlloader.FindElem(ACT_INFO_ELEMENT_ACTINFO.toStdString()))
        {
            QString sTempGuid = QString::fromStdString(xmlloader.GetAttrib(ACT_INFO_ELEMENT_GUID.toStdString()));            //GUID
            if(sTempGuid.compare(sActGuid) == 0)
            {
                bFind = TRUE;
                xmlloader.RemoveElem();
                break;
            }
        }

        xmlloader.OutOfElem();
    }

    if(!bFind)
    {
        return FALSE;
    }

    if(!xmlloader.Save(strFilePath.toStdString().c_str()))
    {
        return FALSE;
    }

    return TRUE;
}

/**************************************************************************
* 函数名: pariseAction
* 功能: 动作表点赞
* 参数:
*    @[in ] nUserId: 用户ID
*    @[in ] nActID: 动作表ID
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/17 17:16
* 作者: ZDJ
*/
BOOL CActUpdateMangr::pariseAction(int nUserId, int nActId)
{
    QString strRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/praise";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString strParam;
    strParam = "{";
    strParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    strParam += QString(",\"praiseObjectId\":%1").arg(nActId);
    strParam += QString(",\"praiseType\":1");
    strParam += QString(",\"praiseUserId\":%1").arg(nUserId);
    strParam += "}";

    ////////////////////////////Log////////////////////////////////
    QString strLog = QString("http request Url: %1\n param: %2").arg(strRequestURL).arg(strParam);
    AlphaRobotLog::getInstance()->printDebug(strLog);
    ////////////////////////////Log////////////////////////////////
    return m_pHttpTrans->postPraiseAction(strRequestURL, strParam);
}

/**************************************************************************
* 函数名: FavourAction
* 功能: 收藏动作表
* 参数: 
*    @[in ] nUserId: 用户ID
*    @[in ] nActId: 动作ID
* 返回值: 成功返回1，失败返回-1，已收藏返回0
* 时间: 2015/09/23 9:48
* 作者: ZDJ
*/
int CActUpdateMangr::FavourAction(int nUserId, int nActId)
{
    byte* pOutData = NULL;
    qint64 nOutLen = 0;
    char sErr[255] = {0};
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/collect/add";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString sParam, sParamActParam, sParamPageNum, sParamPageSize;
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    sParam += ",";

    if(nActId != -1)
    {
        sParamActParam.sprintf("\"collectRelationId\":\"%d\"", nActId);
        sParam += sParamActParam;
        sParam += ",";
    }

    int nCollectType = 1; //收藏类型，1动作，2应用
    sParamActParam.sprintf("\"collectUserId\":\"%d\",\"collectType\":\"%d\"", nUserId, nCollectType);
    sParam += sParamActParam;

    sParam += "}";

    AlphaRobotLog::getInstance()->printDebug(QString("@request favour action@url: %1, parameter: %2").arg(sRequestURL).arg(sParamActParam));

    return m_pHttpTrans->postCollectAction(sRequestURL, sParam, nActId);
    //	int nRet = -1;
    //	do
    //    {
    //        const char* pszParam = sParam.toStdString().c_str();
    //        if(!m_pHttpOper->ExecuteRequest(
    //            eRequestPost, sRequestURL, pszParam, NULL, NULL, pOutData, nOutLen, sErr))
    //		{
    //			break;
    //		}

    //        string sData = Utf8ToAnsiEx(pOutData, nOutLen);

    //		Json::Reader jsonreader;
    //		Json::Value  jvalue_root;
    //		if(jsonreader.parse(sData.c_str(), jvalue_root))
    //		{
    //			if(jvalue_root.size() > 0)
    //			{
    //				if(!jvalue_root["info"].isNull())
    //				{
    //                    QString sRet = jvalue_root["info"].asCString();
    //                    if(sRet.compare("0000") == 0)
    //						nRet = 1;  //收藏成功
    //                    else if(sRet.compare("4200") == 0)
    //						nRet = 0;  //已收藏过
    //				}
    //			}
    //		}

    //	} while (0);

    //	SAFE_FREE(pOutData);

    //	return nRet;
}

/**************************************************************************
* 函数名: UnfavourAction
* 功能: 取消收藏动作表
* 参数: 
*    @[in ] nUserId: 用户ID
*    @[in ] nActId: 动作ID
* 返回值: 成功返回1，失败返回-1
* 时间: 2015/09/23 9:48
* 作者: ZDJ
*/
int CActUpdateMangr::UnfavourAction(int nUserId, int nActId)
{
    byte* pOutData = NULL;
    qint64 nOutLen = 0;
    char sErr[255] = {0};
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/collect/cancel";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString sParam, sParamActParam, sParamPageNum, sParamPageSize;
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    sParam += ",";

    if(nActId != -1)
    {
        sParamActParam.sprintf("\"collectRelationId\":\"%d\"", nActId);
        sParam += sParamActParam;
        sParam += ",";
    }

    int nCollectType = 1; //收藏类型，1动作，2应用
    sParamActParam.sprintf("\"collectUserId\":\"%d\",\"collectType\":\"%d\"", nUserId, nCollectType);
    sParam += sParamActParam;

    sParam += "}";

    AlphaRobotLog::getInstance()->printDebug(QString("@request unfavour action@url: %1, parameter: %2").arg(sRequestURL).arg(sParamActParam));

    return m_pHttpTrans->postCancelCollect(sRequestURL, sParam, nActId);
    //	int nRet = -1;
    //	do
    //    {
    //        const char* pszParam = sParam.toStdString().c_str();
    //        if(!m_pHttpOper->ExecuteRequest(
    //            eRequestPost, sRequestURL, pszParam, NULL, NULL, pOutData, nOutLen, sErr))
    //		{
    //			break;
    //		}

    //        string sData = Utf8ToAnsiEx(pOutData, nOutLen);

    //		Json::Reader jsonreader;
    //		Json::Value  jvalue_root;
    //		if(jsonreader.parse(sData.c_str(), jvalue_root))
    //		{
    //			if(jvalue_root.size() > 0)
    //			{
    //				if(!jvalue_root["info"].isNull())
    //				{
    //                    QString sRet = jvalue_root["info"].asCString();
    //                    if(sRet.compare("0000") == 0)
    //						nRet = 1;  //取消收藏成功
    //				}
    //			}
    //		}

    //	} while (0);

    //	SAFE_FREE(pOutData);

    //	return nRet;
}

/**************************************************************************
* 函数名: QueryActParise
* 功能: 查询动作收藏状态
* 参数: 
*    @[in ] nUserId: 用户ID
*    @[in ] nActId: 动作ID
* 返回值: 返回请求ID
* 时间: 2015/09/23 9:48
* 作者: ZDJ
*/
int CActUpdateMangr::QueryActFavour(int nUserId, int nActId)
{
    byte* pOutData = NULL;
    qint64 nOutLen = 0;
    char sErr[255] = {0};
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/collect/find";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString sParam, sParamActParam, sParamPageNum, sParamPageSize;
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    sParam += ",";

    if(nActId != -1)
    {
        sParamActParam.sprintf("\"collectRelationId\":\"%d\"", nActId);
        sParam += sParamActParam;
        sParam += ",";
    }

    sParamActParam.sprintf("\"collectUserId\":\"%d\"", nUserId);
    sParam += sParamActParam;

    sParam += "}";

    AlphaRobotLog::getInstance()->printDebug(QString("query favour action, url: %1, parameter: %2").arg(sRequestURL).arg(sParamActParam));

    return m_pHttpTrans->postQueryIsCollected(sRequestURL, sParam, nActId);
    //	int nRet = -1;
    //	do
    //    {
    //        const char* pszParam = sParam.toStdString().c_str();
    //        if(!m_pHttpOper->ExecuteRequest(
    //            eRequestPost, sRequestURL, pszParam, NULL, NULL, pOutData, nOutLen, sErr))
    //		{
    //			break;
    //		}

    //        string sData = Utf8ToAnsiEx(pOutData, nOutLen);

    //		Json::Reader jsonreader;
    //		Json::Value  jvalue_root;
    //		if(jsonreader.parse(sData.c_str(), jvalue_root))
    //		{
    //			if(jvalue_root.size() > 0)
    //			{
    //				if(!jvalue_root["info"].isNull())
    //				{
    //                    QString sRet = jvalue_root["info"].asCString();
    //                    if(sRet.compare("0000") == 0)
    //					{
    //						if(!jvalue_root["models"].isNull())
    //						{
    //							if(jvalue_root["models"].size() > 0)
    //								nRet = 1;  //收藏成功
    //						}
    //					}
    //                    else if(sRet.compare("4200") == 0)
    //						nRet = 0;  //已收藏过
    //				}
    //			}
    //		}

    //	} while (0);

    //	SAFE_FREE(pOutData);

    //    return nRet;
}

/************************************
* 名称: OninishedDownload
* 功能: 下载完成槽函数
* 参数: [in]requestID 请求的ID号
* 参数: [in]itemData 对应动作列表数据信息
* 返回:   void
* 时间:   2016/04/01
* 作者:   hels
************************************/
void CActUpdateMangr::OnFinishedDownload(int requestID, ActItemData itemData)
{
    g_actupdatemgr->RemoveDownloadingActInfo(itemData.m_sGuid);
    g_actupdatemgr->AddDownloadedActInfo(itemData);

    // 动作下载完成，通知界面更新状态
    emit sigOnDownloadFinish(itemData.m_nItemID);
}

/************************************
* 名称: SetUserID
* 功能: 设置登录用户ID
* 参数: [in]userID 用户ID
* 返回:   void
* 时间:   2016/04/13
* 作者:   hels
************************************/
void CActUpdateMangr::SetUserID(int userID)
{
    m_nUserID = userID;
}

/************************************
* 名称: GetUserID
* 功能: 获取已经登录用户ID
* 返回:   int 返回当前登录用户ID
* 时间:   2016/04/13
* 作者:   hels
************************************/
int CActUpdateMangr::GetUserID()
{
    return m_nUserID;
}

/************************************
* 名称: DownLoadMedia
* 功能: 下载动作视频文件
* 参数: [in]nActionID 动作ID
* 参数: [in]strUrl 动作路径
* 返回:   void
* 时间:   2016/04/16
* 作者:   hels
************************************/
void CActUpdateMangr::DownLoadMedia(int nActionID, const QString &strUrl)
{

}

int CActUpdateMangr::AddDownLoadCount(int nActionID)
{
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/addDownload";
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString sParam, sParamActParam, sParamPageNum, sParamPageSize;
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
    sParam += ",";

    if(nActionID != -1)
    {
        sParamActParam.sprintf("\"actionId\":\"%d\"", nActionID);
        sParam += sParamActParam;
    }
    sParam += "}";
    return m_pHttpTrans->postAddDownLoadCount(sRequestURL, sParam, nActionID);
}


int CActUpdateMangr::saveFileActInfoToServer(const QString &sFilePath,
                                             const ActItemData &actData,
                                             int nUserID,
                                             int nOldRequestID)
{
    //!!!the info have to add: actionOriginalId, actionUserId, actionName, actionDesciber, actionTime, actionType,

    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/createaction/upload";
    MapStringDataType  mapUploadParams;
    CActItemDataHandle::ActItemDataToMapStrType(actData, mapUploadParams);

    QByteArray byteArrfileData;
    CFileDirHandle::getFileByteArrayForUpload(sFilePath, byteArrfileData);
    mapUploadParams.insert(UPLOADDATA_KEYNAME_FILE, byteArrfileData);

    mapUploadParams.insert(NET_ACTION_USER_ID, QString::number(nUserID).toLatin1()); //动作关联的用户ID
//    mapUploadParams.insert(NET_ACTION_ORIGINAL_ID, QString::number(nActionID).toLatin1()); //动作ID

    return m_pHttpTrans->postUploadData(Constants::HTTP_REQUEST_UPLOADACTIONFILE,
                                                           sRequestURL,
                                                           mapUploadParams,
                                                           actData.m_nActionOriginalId,
                                                           nUserID,
                                                           nOldRequestID);
}

/**************************************************************************
* 函数名: UploadActionFile
* 功能: 上传动作文件
* 参数:
*    @[in ] sFilePath: 文件路径
*    @[in ] nActionOriginalId: 动作actionOriginalId
*    @[in ] nUserID: 用户ID
*    @[in ] nOldRequestID: 旧请求ID
* 返回值: 成功返回正整数，出错返回其他值
* 时间: 2015/09/23 9:48
* 作者: ZDJ
*/
int CActUpdateMangr::UploadActionFile(const QString& sFilePath,
                                      qint64 nActionOriginalId,
                                      int nUserID,
                                      int nOldRequestID)
{
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/createaction/upload";
    MapStringDataType  mapUploadParams;

    QByteArray byteArrfileData;
    CFileDirHandle::getFileByteArrayForUpload(sFilePath, byteArrfileData);
    mapUploadParams.insert(UPLOADDATA_KEYNAME_FILE, byteArrfileData);

    mapUploadParams.insert(NET_ACTION_USER_ID, QString::number(nUserID).toLatin1()); //动作关联的用户ID
    mapUploadParams.insert(NET_ACTION_ORIGINAL_ID, QString::number(nActionOriginalId).toLatin1()); //动作ID

    return m_pHttpTrans->postUploadData(Constants::HTTP_REQUEST_UPLOADACTIONFILE,
                                                           sRequestURL,
                                                           mapUploadParams,
                                                           nActionOriginalId,
                                                           nUserID,
                                                           nOldRequestID);
}

/************************************
* 名称: UploadActionInfo
* 功能: 上传动作信息
* 参数:
*    @[in ] actData: 动作数据
*    @[in ] sHeadImgUrl: 头像url
*    @[in ] sDescImgUrl: 描述图片url
*    @[in ] nActionID: 动作ID
*    @[in ] nUserID: 用户ID
*    @[in ] nOldRequestID: 旧请求ID
* 返回值: 成功返回正整数，出错返回其他值
* 时间: 2016/05/27 19:27
* 作者: ZDJ
*/
int CActUpdateMangr::UploadActionInfo(const ActItemData& actData,
                                      const QString& sHeadImgUrl,
                                      const QString& sDescImgUrl,
                                      qint64 nActionID,
                                      int nUserID,
                                      int nOldRequestID)
{
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/uploadActionFromPC";

    MapStringDataType  mapUploadParams;

    mapUploadParams.insert(NET_ACTION_USER_ID, QString::number(nUserID).toLatin1()); //动作关联的用户ID
    mapUploadParams.insert(NET_ACTION_ORIGINAL_ID, QString::number(nActionID).toLatin1()); //动作ID
    mapUploadParams.insert(NET_SCHEME_ID, QString::number(actData.m_nSchemId).toLatin1()); // 活动id
    mapUploadParams.insert(NET_SCHEME_NAME, actData.m_strSchemeName.toUtf8());
    mapUploadParams.insert(NET_ACTION_NAME, actData.m_sActName.toUtf8()); //动作名称
//    mapUploadParams.insert("actionType", QString::number(actData.m_nActType).toLatin1()); //动作分类
    mapUploadParams.insert(NET_ACTION_TYPE, QString::number(actData.m_nActType).toLatin1()); //动作分类

    mapUploadParams.insert(NET_ACTION_DESCIBER, actData.m_sActDesc.toUtf8()); //动作描述文本
    mapUploadParams.insert(NET_ACTION_HEADURL, sHeadImgUrl.toUtf8()); //动作头像url
    mapUploadParams.insert(NET_ACTION_IMAGE_PATH, sDescImgUrl.toUtf8()); //动作描述图片url
    mapUploadParams.insert(NET_ACTION_VIDEO_PATH, actData.m_sVideoURL.toUtf8()); // 视频文件URL
    mapUploadParams.insert(NET_ACTION_SIZE, QString::number(actData.m_nActSize).toLatin1()); //动作文件大小
    mapUploadParams.insert(NET_ACTION_TIME, QString::number(actData.m_dwActTimeLen).toLatin1()); //动作总时间

    return m_pHttpTrans->postUploadData(Constants::HTTP_REQUEST_UPLOADACTIONINFO, sRequestURL, mapUploadParams, nActionID, nUserID, nOldRequestID);

}

/************************************
* 名称: UploadActionImage
* 功能: 上传动作图片，头像或描述图片
* 参数:
*    @[in ] eImgType: 图片类型
*    @[in ] sFilePath: 文件路径
*    @[in ] nActionID: 动作ID
*    @[in ] nUserID: 用户ID
*    @[in ] nOldRequestID: 旧请求ID
* 返回值: 成功返回正整数，出错返回其他值
* 时间: 2016/05/27 19:27
* 作者: ZDJ
*/
int CActUpdateMangr::UploadActionImage(emUploadFileType eImgType, const QString& sFilePath, qint64 nActionID, int nUserID, int nOldRequestID)
{
    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/uploadImage";

    MapStringDataType  mapUploadParams;

    QFile fileobj(sFilePath);
    if(fileobj.open(QIODevice::ReadOnly))
    {
        QByteArray bytesFileData = fileobj.readAll();
        fileobj.close();

        QByteArray bytesArrTemp;

        QString sFileName = CFileDirHandle::getFileName(sFilePath);
        const int nFileNameLenTypeLen = sizeof(int); //文件名长度的数据类型长度
        char szFileNameLen[nFileNameLenTypeLen] = {0};//保存文件名长度
        int nFileNameLen = sFileName.toUtf8().length();
        memcpy(szFileNameLen, &nFileNameLen, sizeof(szFileNameLen));

        bytesArrTemp.append(szFileNameLen, sizeof(szFileNameLen)); //文件名长度
        bytesArrTemp.append(sFileName.toUtf8()); //文件名

        const int nFileDataLenTypeLen = sizeof(int); //文件数据长度的数据类型长度
        char szFileDataLen[nFileDataLenTypeLen] = {0};//保存文件数据长度
        int nFileDataLen = bytesFileData.length();
        memcpy(szFileDataLen, &nFileDataLen, sizeof(szFileDataLen));

        bytesArrTemp.append(szFileDataLen, sizeof(szFileDataLen)); //文件数据长度
        bytesArrTemp.append(bytesFileData);//文件数据

        mapUploadParams.insert(UPLOADDATA_KEYNAME_FILE, bytesArrTemp);
    }

    mapUploadParams.insert(NET_ACTION_USER_ID, QString::number(nUserID).toLatin1()); //动作关联的用户ID
    mapUploadParams.insert(NET_ACTION_ORIGINAL_ID, QString::number(nActionID).toLatin1()); //动作ID
    mapUploadParams.insert("imageType", QString::number(eImgType).toLatin1()); //动作分类

    char* pszReqType = NULL;
    if(eImgType == eUploadFileHeadImg)
        pszReqType = (char*)Constants::HTTP_REQUEST_UPLOADACTIONHEADIMG;
    else if(eImgType == eUploadFileDescImg)
        pszReqType = (char*)Constants::HTTP_REQUEST_UPLOADACTIONDESCIMG;

    return m_pHttpTrans->postUploadData(pszReqType, sRequestURL, mapUploadParams, nActionID, nUserID, nOldRequestID);

}

#ifdef USEQINIU

/************************************
* 名称: UploadActionDescVideo
* 功能: 上传动作描述视频
* 参数:
*    @[in ] sFilePath: 文件路径
*    @[in ] nActionID: 动作ID
*    @[in ] nUserID: 用户ID
*    @[in ] nOldRequestID: 旧请求ID
*    @[out] strVideoUrl: 视频文件URL
* 返回值:
* 时间: 2016/05/27 19:27
* 作者: ZDJ
*/
int CActUpdateMangr::UploadActionDescVideo(const QString& strFilePath, qint64 nActionID, int nUserID,
                                           int nOldRequestID, QString &strVideoUrl, INotify* pNotifyRecvr)
{
    AlphaRobotLog::getInstance()->printDebug(QString("CActUpdateMangr::UploadActionDescVideo"));

    int nRet = -1;
    strVideoUrl.clear();

    // 生成bucketName
    CQiniuUpoadHandle::createBucketName(nActionID, nUserID, strFilePath);

    QString sBucketName = CQiniuUpoadHandle::getBucketName(strFilePath, nUserID);
    QString sVideoFileUrl;  //视频文件的下载路径

    /*得到一个服务器上的文件的下载url*/
    CQiniuUpoadHandle::getQiniuDownloadURL(sBucketName, NULL, "", sVideoFileUrl);

    if(sVideoFileUrl.isEmpty())
    {
        AlphaRobotLog::getInstance()->printError(QString("UploadActionDescVideo: get Null video Url from qiniu"));
        return nRet;
    }

    //==================================
    // huangwuxian 2016.09.22 修改开始
    // 视频URL上传方式调整，由原来的独立上传URL
    // 修改为将视频URL和其他数据一起上传
    //==================================
//    //上传视频信息到后台
//    QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/uploadActionFromPC";

//    MapStringDataType  mapUploadParams;
//    mapUploadParams.insert(NET_ACTION_USER_ID, QString::number(nUserID).toLatin1()); //动作关联的用户ID
//    mapUploadParams.insert(NET_ACTION_ORIGINAL_ID, QString::number(nActionID).toLatin1()); //动作ID

//    mapUploadParams.insert(NET_ACTION_VIDEO_PATH, sVideoFileUrl.toUtf8()); //动作描述视频url

//    nRet = m_pHttpTrans->postUploadData(Constants::HTTP_REQUEST_UPLOADACTIONDESCVIDEO,
//                                        sRequestURL, mapUploadParams, nActionID, nUserID, nOldRequestID);
    strVideoUrl = sVideoFileUrl;
    // huangwuxian 2016.09.22 修改结束

    m_nOldRequestIDOfQiniuUpload = CQiniuUpoadHandle::getInstance().uploadFile(strFilePath,
                                                                               nUserID,
                                                                               m_nOldRequestIDOfQiniuUpload,
                                                                               pNotifyRecvr);    
    nRet = m_nOldRequestIDOfQiniuUpload;
    return nRet;
}
#endif

/************************************
* 名称: QueryCreateActions
* 功能: 查询我的新建动作
* 参数: [in]nOldRequestID 旧的请求ID
* 返回:   void
* 时间:   2016/06/08
* 作者:   Zhouzp
************************************/
int CActUpdateMangr::QueryCreateActions(int nOldRequestID, int nActType, int nCurrPageNum, int nPageSize)
{
    SQUERYACTLISTPARA inParas;
    inParas.eRequestType = eReqTypeMyuploadActionsDetail;
    inParas.nUserID = m_nUserID;
    inParas.nGroup = eActGroupUserLastUpdate;//eActGroupDefaultAct
    inParas.nActType = nActType;//eActTypeAll
    inParas.nPageNum = nCurrPageNum;
    inParas.nPageSize = nPageSize;//2147483647;

    inParas.nOldRequestID = nOldRequestID;

    return QueryActListProc(inParas);
}

/***********************************
* 函数名: DeleteCreateActions
* 功能: 删除我的新建动作
* 参数:
*    @[in ] nUserId: 用户ID
*    @[in ] nActId: 动作ID
* 返回值: 成功返回1，失败返回-1
* 时间:   2016/06/08
* 作者:   Zhouzp
* 备注: http://10.10.1.14:8080/ubx/createaction/delete,参数 actionIds 也可以多个action一起，用,隔开
************************************/
int CActUpdateMangr::DeleteCreateActions(/*int nUserId, */int nActId)
{
    QString sRequestURL = g_sServer + NET_PATH_ACTION_DELETE;
    FILEVERINFO fileinfo;
    QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    CUtil::GetFileVersion(sAppPath, fileinfo);

    int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
    QString sServiceVersion = fileinfo.sFileVer;  //版本号
    QString sParam, sParamActParam;
    sParam = "{";
    sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));

    if (nActId != -1)
    {

        sParamActParam.sprintf("\"actionIds\":\"%d\"", nActId);
        sParam += ",";
        sParam += sParamActParam;
    }

    sParam += "}";
    qDebug()<<"DeleteCreateActions:"<<sParam;
    return m_pHttpTrans->postDeleteCreateActions(sRequestURL, sParam, nActId);

}

/************************************
* 名称: removeRedundantActionFiles
* 功能: for remove the redundant action files which is not consistent to download xml
* 返回:   void
* 时间:   2016/06/01
* 作者:   suhaiwu
************************************/
void CActUpdateMangr::removeRedundantActionFiles()
{
//must have to before than removeRedundantActionInfos

    QStringList listActionRootDirs = CFileDirHandle::getRootDirPaths(g_sActPath);
    //        const QString strActionDirPath = QDir::toNativeSeparators(QString(g_sActPath + "/"));
    if(!listActionRootDirs.isEmpty())
    {
        QStringList filterList;
        filterList.clear();

        QMap<int, ActItemData> mapItem;
        CActUpdateMangr::GetDownloadedActList(mapItem);
        foreach (ActItemData item, mapItem)
        {
            //                filterList.append(strActionDirPath + item.m_sActName);
            QFileInfo info(item.m_sLocalPath);
            filterList.append(info.absolutePath());
        }

        vector<ActItemData> vecActList;
        CActUpdateMangr::GetDownloadingActList(vecActList);
        vector<ActItemData>::iterator iter = vecActList.begin();
        for(; iter != vecActList.end(); iter++)
        {
            QFileInfo info(iter->m_sLocalPath);
            filterList.append(info.absolutePath());
        }

        QStringList removeDirs = CUtil::listFilter(listActionRootDirs, filterList);
        for(int index = 0; index < removeDirs.count(); ++index)
        {
            CFileDirHandle::clearDir(removeDirs.at(index), true);
        }
    }
}

/************************************
* 名称: removeRedundantActionInfos
* 功能: for remove the redundant action infos on downloaded xml which is not consistent to dir files
* 返回:   void
* 时间:   2016/06/01
* 作者:   suhaiwu
************************************/
void CActUpdateMangr::removeRedundantActionInfos()
{
//must have to after to removeRedundantActionFiles
    QMap<int, ActItemData> mapItem;
    CActUpdateMangr::GetDownloadedActList(mapItem);
    foreach (ActItemData item, mapItem)
    {
        if(!QFile::exists(item.m_sLocalPath))
        {
            CActUpdateMangr::RemoveDownloadedActInfo(item.m_sGuid);
        }
    }
}


bool CActUpdateMangr::zipFiles(const vector<QString> &vecFilePathList, const QString &sZipPath)
{
    return ZipFile(vecFilePathList, sZipPath);
}

/************************************************************************************
* 函数名: downloadReplierIcon
* 功能: 下载评论者头像
* 参数:
*    @[in ] strUrl: 评论者头像Url
* 返回值:
* 时间: 2016/09/20 9:48
* 作者: hwx
*/
void CActUpdateMangr::downloadReplierIcon(const QString &strUrl)
{
    if (m_pHttpTrans)
    {
        m_pHttpTrans->downloadReplierIcon(strUrl);
    }
}

/**************************************************************************
* 函数名: queryDownloadImage
* 功能: 请求下载图片到指定目录
* 参数:
*    @[in ] strImgeUrl: 图片Url
*    @[in] strDownloadPath: 下载保存的路径
*    @[in ] nOldRequestID: 旧的请求ID
* 返回值: int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间: 2016/11/18
* 作者: hwx
*/
int CActUpdateMangr::queryDownloadImage(const QString &strImgeUrl, const QString strDownloadPath, int nOldRequestID)
{
    if (m_pHttpTrans)
    {
        return m_pHttpTrans->postDownloadImage(strImgeUrl, strDownloadPath, nOldRequestID);
    }

    return Constants::HTTP_DEFAULT_REQUEST_ID;
}
