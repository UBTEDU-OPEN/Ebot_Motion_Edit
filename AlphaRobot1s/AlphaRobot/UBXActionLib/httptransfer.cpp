/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：httptransfer.cpp
* 创建时间：2016/03/24
* 文件标识：
* 文件摘要：http传输类，完成http指令请求和数据的获取
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/24
* 版本摘要：
*/

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QApplication>
#include <QDir>
#include <QUuid>

#include "httptransfer.h"
#include "ubxactionlibconstants.h"
#include "actupdatemangr.h"
#include "alpharobotlog.h"
#include "../zxLib/configs.h"
#include "../common/filedirhandle.h"
#include "Util.h"
#include "ubtlib.h"

HttpTransfer::HttpTransfer()
{
    connect(this, &HttpTransfer::SigPostQueryActList, this, &HttpTransfer::OnPostQueryActList);
    connect(this, &HttpTransfer::sigPostQueryActionComment, this, &HttpTransfer::slotOnPostQueryActionComment);
    connect(this, &HttpTransfer::sigPostAddActionComment, this, &HttpTransfer::slotOnPostAddActionComment);
    connect(this, &HttpTransfer::sigPostDownloadImage, this, &HttpTransfer::slotOnPostDownloadImage);
    connect(this, &HttpTransfer::sigOnDownloadReplierIcon, this, &HttpTransfer::slotOnDownloadReplierIcon);
    connect(this, &HttpTransfer::SigPostQueryRecommendList, this, &HttpTransfer::onPostQueryRecommendList);
    connect(this, &HttpTransfer::sigPostQueryBusinessPromoteData, this, &HttpTransfer::slotOnPostQueryBusinessPromoteData);
    connect(this, &HttpTransfer::sigPostQueryRankActionData, this, &HttpTransfer::slotOnPostQueryRankActionData);
    connect(this, &HttpTransfer::SigPostQueryFavorites, this, &HttpTransfer::OnPostQueryFavorites);
    connect(this, &HttpTransfer::sigOnPraiseAction, this, &HttpTransfer::slotOnPraiseAction);
    connect(this, &HttpTransfer::SigPostCollectAction, this, &HttpTransfer::OnPostCollectAction);
    connect(this, &HttpTransfer::SigPostCancelCollect, this, &HttpTransfer::OnPostCancelCollect);
    connect(this, &HttpTransfer::SigPostQueryIsCollected, this, &HttpTransfer::OnPostQueryIsCollected);
    connect(this, &HttpTransfer::SigPostAddDownLoadCount, this, &HttpTransfer::OnPostAddDownloadCount);
    connect(this, &HttpTransfer::SigInit, this, &HttpTransfer::OnInit);
    //connect(this, SIGNAL(SigPostUploadFile(QString,int,QString,QString,QByteArray,qint64,int)), this, SLOT(OnPostUploadFile(QString,int,QString,QString,QByteArray,qint64,int)));
    connect(this, &HttpTransfer::SigPostUploadData, this, &HttpTransfer::OnPostUploadData);
    connect(this, &HttpTransfer::SigPostQueryCreateActions, this, &HttpTransfer::OnPostQueryCreateActions);
    connect(this, &HttpTransfer::SigPostDeleteCreateActions, this, &HttpTransfer::OnPostDeleteCreateActions);

    m_pNetManager = NULL;
    //connect(m_pNetManager, &QNetworkAccessManager::finished, this, &HttpTransfer::OnFinished);
    moveToThread(&m_oThread);
    m_nRequestID = -1;
    m_oThread.start();
    emit SigInit();
}

HttpTransfer::~HttpTransfer()
{
    if(m_pNetManager)
    {
        m_pNetManager->disconnect();
        SAFE_DELETE(m_pNetManager); // 出现释放打印异常
    }
}

/************************************
* 名称: stopThread
* 功能: 退出线程
* 参数:
* 返回:
* 时间:   2017/04/06
* 作者:   hwx
************************************/
void HttpTransfer::stopThread()
{
    m_oThread.quit();
    m_oThread.wait();
}

bool HttpTransfer::getActItemData(const QJsonObject &objIn, ActItemData &actItem)
{
    QJsonObject obj(objIn);

    if(obj.contains(NET_ACTION_TYPE))
    {
        actItem.m_nActType = obj.take(NET_ACTION_TYPE).toInt();
    }

    // 动作大类
    if (obj.contains(NET_ACTION_RESOURCE))
    {
        actItem.m_nActResource = obj.take(NET_ACTION_RESOURCE).toString().toInt();
    }

//    //服务器上竟然有actiontype为-1的类型，先注释掉，后面确认没问题再开启
//    if(nActionType < 0 || nActionType > eActTypeOther)
//    {
//        return false;
//    }

    //动作ID
    if(obj.contains(NET_ACTION_ID))
    {
        actItem.m_nItemID = obj.take(NET_ACTION_ID).toInt();
    }

    // 活动id
    if (obj.contains(NET_SCHEME_ID))
    {
        actItem.m_nSchemId = obj.take(NET_SCHEME_ID).toInt();
    }

    // 活动名称
    if (obj.contains(NET_SCHEME_NAME))
    {
        actItem.m_strSchemeName = obj.take(NET_SCHEME_NAME).toString();
    }

    //动作名称
    if(obj.contains(NET_ACTION_NAME))
    {
        actItem.m_sActName = obj.take(NET_ACTION_NAME).toString();
    }

    //动作描述
    if(obj.contains(NET_ACTION_DESCIBER))
    {
        actItem.m_sActDesc = obj.take(NET_ACTION_DESCIBER).toString();
    }

    //预览图地址
    if(obj.contains(NET_ACTION_HEADURL))
    {
        actItem.m_sImageHeadUrl = obj.take(NET_ACTION_HEADURL).toString();
    }

    //预览图地址
    if(obj.contains(NET_ACTION_IMAGE_PATH))
    {
        actItem.m_sImageURL = obj.take(NET_ACTION_IMAGE_PATH).toString();
    }

    //视频地址
    if(obj.contains(NET_ACTION_VIDEO_PATH))
    {
        actItem.m_sVideoURL = obj.take(NET_ACTION_VIDEO_PATH).toString();
    }

    //动作地址
    if(obj.contains(NET_ACTION_PATH))
    {
        actItem.m_sActDownloadURL = obj.take(NET_ACTION_PATH).toString();
    }

    //动作时间
    if(obj.contains(NET_ACTION_TIME))
    {
        double dActionTime = obj.take(NET_ACTION_TIME).toDouble();
        if (dActionTime <= 0)
        {
            dActionTime = 0;
        }
        actItem.m_dwActTimeLen = dActionTime;
    }

    //动作发布日期
    if(obj.contains(NET_ACTION_PUBLIC_DATE))
    {
        double dTime =  obj.take(NET_ACTION_PUBLIC_DATE).toDouble();
        QDateTime time;
        time.setTime_t(dTime / 1000);
        actItem.m_sPublishTime = time.toString(NET_ACTION_DATE_FORMAT);
    }

    //动作文件大小
    if(obj.contains(NET_ACTION_SIZE))
    {
        //strSize = GetFormatSizeString(obj.take(NET_ACTION_SIZE).toDouble());

//        actItem.m_nActSize = obj.take(NET_ACTION_SIZE).toString()  toDouble();

        const QString actionSize = obj.take(NET_ACTION_SIZE).toString();
        actItem.m_nActSize = actionSize.toDouble();
    }

    //动作下载总量
    if(obj.contains(NET_ACTION_DOWNLOAD_TIME))
    {
        // 如果返回值小于0，直接赋值给quint64会出现很大的数值
        double dDownloadTime = obj.take(NET_ACTION_DOWNLOAD_TIME).toDouble();
        if (dDownloadTime <= 0)
        {
            dDownloadTime = 0;
        }
        actItem.m_nActionDownloads = dDownloadTime;
    }

    //动作评论总量
    if(obj.contains(NET_ACTION_COMMENT_TIME))
    {
        // 如果返回值小于0，直接赋值给quint64会出现很大的数值
        double dCommentTime = obj.take(NET_ACTION_COMMENT_TIME).toDouble();
        if (dCommentTime <= 0)
        {
            dCommentTime = 0;
        }
        actItem.m_nActionComments = dCommentTime;
    }

    //动作点赞总量
    if(obj.contains(NET_ACTION_PRAISE_TIME))
    {
        // 如果返回值小于0，直接赋值给quint64会出现很大的数值
        double dPraiseTime = obj.take(NET_ACTION_PRAISE_TIME).toDouble();
        if (dPraiseTime <= 0)
        {
            dPraiseTime = 0;
        }
        actItem.m_nActionLikes = dPraiseTime;
    }

    // 浏览总量
    if (obj.contains(NET_ACTION_BROWN_TIME))
    {
        // 如果返回值小于0，直接赋值给quint64会出现很大的数值
        double dBrownTime = obj.take(NET_ACTION_BROWN_TIME).toDouble();
        if (dBrownTime <= 0)
        {
            dBrownTime = 0;
        }
        actItem.m_nActionBrownTime = dBrownTime;
    }

    // 动作热度值
    if (obj.contains(NET_ACTION_HOT_VALUE))
    {
        // 如果返回值小于0，直接赋值给quint64会出现很大的数值
        double dHotValue = obj.take(NET_ACTION_HOT_VALUE).toDouble();
        if (dHotValue <= 0)
        {
            dHotValue = 0;
        }
        actItem.m_nActionHotValue = dHotValue;
    }

    //发布者
    if(obj.contains(NET_ACTION_USER))
    {
        actItem.m_sPublisher = obj.take(NET_ACTION_USER).toString();
    }

    //动作标题
    if(obj.contains(NET_ACTION_TITLE))
    {
        actItem.m_sActionTitle = obj.take(NET_ACTION_TITLE).toString();
    }

    //收藏总量
    if(obj.contains(NET_ACTION_COLLECT_TIME))
    {
        double dCollectTime = obj.take(NET_ACTION_COLLECT_TIME).toDouble();
        if (dCollectTime <= 0)
        {
            dCollectTime = 0;
        }
        actItem.m_nActionCollectTime = dCollectTime;
    }

    //
    if(obj.contains(NET_ACTION_STATUS))
    {
        actItem.m_nActionStatus = obj.take(NET_ACTION_STATUS).toInt();
    }

    // userId
    if (obj.contains(NET_USER_ID))
    {
        actItem.m_nUserId = obj.value(NET_USER_ID).toInt();
    }

    // userName
    if (obj.contains(NET_USER_NAME))
    {
        actItem.m_sUserName = obj.value(NET_USER_NAME).toString();
    }

    // userImage
    if (obj.contains(NET_USER_IMAGE))
    {
        actItem.m_sUserImageUrl = obj.value(NET_USER_IMAGE).toString();
    }

    return true;
}

/************************************
* 名称: postQueryActList
* 功能: post http请求，内部封装了多线程处理
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 返回:   quint32 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/03/24
* 作者:   hels
************************************/
int HttpTransfer::postQueryActList(const QString &serverURL, const QString &data, int nOldRequestID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestID))
    {
        m_mapReplyActLst[nOldRequestID]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit SigPostQueryActList(nRequestID, serverURL, data);
    return nRequestID;
}

/************************************
* 名称: postQueryRecomendActList
* 功能: post http请求，内部封装了多线程处理
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/06/03
* 作者:   hwx
************************************/
int HttpTransfer::postQueryRecommendList(const QString &serverURL, const QString &data, int nOldRequestID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestID))
    {
        m_mapReplyActLst[nOldRequestID]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit SigPostQueryRecommendList(nRequestID, serverURL, data);
    return nRequestID;
}

/************************************
* 名称: postQueryBusinessPromoteData
* 功能: post http请求运营活动数据，内部封装了多线程处理
* 参数: [in]serverURL 请求资源地，
* 参数: [in]sInParam 请求参数数据
* 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/08/30
* 作者:   hwx
************************************/
int HttpTransfer::postQueryBusinessPromoteData(const QString &sServerURl, const QString &sInParam, int nOldRequestID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestID))
    {
        m_mapReplyActLst[nOldRequestID]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit sigPostQueryBusinessPromoteData(nRequestID, sServerURl, sInParam);
    return nRequestID;
}

/************************************
* 名称: postQueryRankActionData
* 功能: post http请求运营活动数据，内部封装了多线程处理
* 参数: [in]serverURL 请求资源地，
* 参数: [in]sInParam 请求参数数据
* 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 参数: [in]nSchemeID 活动ID
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/08/30
* 作者:   hwx
************************************/
int HttpTransfer::postQueryRankActionData(const QString &sServerURl, const QString &sInParam, int nOldRequestID, int nSchemeID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestID))
    {
        m_mapReplyActLst[nOldRequestID]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit sigPostQueryRankActionData(nRequestID, nSchemeID, sServerURl, sInParam);
    return nRequestID;
}

/************************************
* 名称: postQueryActionComment
* 功能: post http请求，内部封装了多线程处理
* 参数: [in]strServerUrl 请求资源地，
* 参数: [in]strInParam 请求参数数据
* 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/09/19
* 作者:   hwx
************************************/
int HttpTransfer::postQueryActionComment(const QString &strServerUrl, const QString &strInParam, int nOldRequestID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestID))
    {
        m_mapReplyActLst[nOldRequestID]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit sigPostQueryActionComment(nRequestID, strServerUrl, strInParam);
    return nRequestID;
}

/************************************
* 名称: postAddActionComment
* 功能: post http请求，内部封装了多线程处理
* 参数: [in]strServerUrl 请求资源地，
* 参数: [in]strInParam 请求参数数据
* 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/09/19
* 作者:   hwx
************************************/
int HttpTransfer::postAddActionComment(const QString &strServerUrl, const QString &strInParam, int nOldRequestId)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestId))
    {
        m_mapReplyActLst[nOldRequestId]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit sigPostAddActionComment(nRequestID, strServerUrl, strInParam);
    return nRequestID;
}

/************************************
* 名称: postDownloadImage
* 功能: post http请求，内部封装了多线程处理，下载图片
* 参数: [in]strImageUrl 请求资源地，
* 参数: [in]strDownloadPath 下载保存的路径
* 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/11/18
* 作者:   hwx
************************************/
int HttpTransfer::postDownloadImage(const QString &strImageUrl, const QString &strDownloadPath, int nOldRequestId)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestId))
    {
        m_mapReplyActLst[nOldRequestId]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit sigPostDownloadImage(nRequestID, strImageUrl, strDownloadPath);
    return nRequestID;
}

/************************************
* 名称: postQueryFavorites
* 功能: post 我的收藏请求，内部封装了多线程处理
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/04/14
* 作者:   hels
************************************/
int HttpTransfer::postQueryFavorites(const QString &serverURL, const QString &data, int nOldRequestID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestID))
    {
        m_mapReplyActLst[nOldRequestID]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit SigPostQueryFavorites(nRequestID, serverURL, data);
    return nRequestID;
}

/************************************
* 名称: postCollectAction
* 功能: 请求收藏动作
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nActionID 要收藏的Action ID
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/04/14
* 作者:   hels
************************************/
int HttpTransfer::postCollectAction(const QString &serverURL, const QString &data, int nActionID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    int nRequestID = createUnusedRequestID();
    emit SigPostCollectAction(nRequestID, serverURL, data, nActionID);
    return nRequestID;
}

/************************************
* 名称: postCancelCollect
* 功能: 取消收藏动作
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nActionID 要收藏的Action ID
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/04/14
* 作者:   hels
************************************/
int HttpTransfer::postCancelCollect(const QString &serverURL, const QString &data, int nActionID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    int nRequestID = createUnusedRequestID();
    emit SigPostCancelCollect(nRequestID, serverURL, data, nActionID);
    return nRequestID;
}

/************************************
* 名称: postPraiseAction
* 功能: 对动作点赞
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/10/22
* 作者:   hwx
************************************/
int HttpTransfer::postPraiseAction(const QString &serverURL, const QString &data)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    int nRequestID = createUnusedRequestID();
    emit sigOnPraiseAction(nRequestID, serverURL, data);
    return nRequestID;
}

/************************************
* 名称: postQueryIsCollected
* 功能: 查询指定Action是否已经收藏
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nActionID 要查询的Action ID
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/04/14
* 作者:   hels
************************************/
int HttpTransfer::postQueryIsCollected(const QString &serverURL, const QString &data, int nActionID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    int nRequestID = createUnusedRequestID();
    emit SigPostQueryIsCollected(nRequestID, serverURL, data, nActionID);
    return nRequestID;
}

/************************************
* 名称: postAddDownLoadCount
* 功能: 增加下载数量
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nActionID 要查询的Action ID
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/04/14
* 作者:   hels
************************************/
int HttpTransfer::postAddDownLoadCount(const QString &serverURL, const QString &data, int nActionID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    int nRequestID = createUnusedRequestID();
    emit SigPostAddDownLoadCount(nRequestID, serverURL, data, nActionID);
    return nRequestID;
}

#if 0
/************************************
* 名称: postUploadFile
* 功能: 请求上传文件
* 参数:
*    @[in ] sRequestType: 请求类型字符串
*    @[in ] sServerURL:  请求资源地址
*    @[in ] sFileName:  文件名称
*    @[in ] bytesFileData:  文件数据
*    @[in ] nActionID:  动作ID
*    @[in ] nUserID:  用户ID
*    @[in ] nOldRequestID: 上次请求的ID，若还没完成，可以结束上次请求
* 返回: 请求ID
* 时间: 2016/05/27 19:27
* 作者: ZDJ
*/
int HttpTransfer::postUploadFile(const QString& sRequestType, const QString& sServerURL, const QString& sFileName, const QByteArray& bytesFileData, qint64 nActionID, int nUserID, int nOldRequestID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestID))
    {
        m_mapReplyActLst[nOldRequestID]->abort();
    }

    ++m_nRequestID;
    emit SigPostUploadFile(sRequestType, m_nRequestID, sServerURL, sFileName, bytesFileData, nActionID, nUserID);
    return m_nRequestID;
}
#endif

/************************************
* 名称: postUploadData
* 功能: 请求上传数据
* 参数:
*    @[in ] sRequestType: 请求类型字符串
*    @[in ] sServerURL:  请求资源地址
*    @[in ] mapData: 上传数据
*    @[in ] nActionOriginalId:  动作nActionOriginalId
*    @[in ] nUserID:  用户ID
*    @[in ] nOldRequestID: 上次请求的ID，若还没完成，可以结束上次请求
* 返回: 请求ID
* 时间: 2016/05/27 19:27
* 作者: ZDJ
*/
int HttpTransfer::postUploadData(const QString& sRequestType,
                                 const QString& sServerURL,
                                 const MapStringDataType& mapData,
                                 qint64 nActionOriginalId,
                                 int nUserID,
                                 int nOldRequestID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestID))
    {
        m_mapReplyActLst[nOldRequestID]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit SigPostUploadData(sRequestType, nRequestID, sServerURL, mapData, nActionOriginalId, nUserID);

    ///////////////////////////Log信息/////////////////////////
    QString strLog = "";
    QString strParam = "";
    MapStringDataType::const_iterator it;
    for (it = mapData.begin(); it != mapData.end(); it++)
    {
        strParam += QString("[%1->%2]").arg(it.key()).arg(QString(it.value()));
    }

    strLog += QString("[uploadData] requestType: %1, serverUrl: %2, actionOriginalId: %3, userId: %4\n").arg(sRequestType).arg(sServerURL).arg(nActionOriginalId).arg(nUserID);
    strLog += QString("param: %1").arg(strParam);
    AlphaRobotLog::getInstance()->printDebug(strLog);
    ///////////////////////////Log信息/////////////////////////

    return nRequestID;
}

/************************************
* 名称: postQueryCreateActions
* 功能: post 我的新建动作请求，内部封装了多线程处理
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/06/08
* 作者:   Zhouzp
************************************/
int HttpTransfer::postQueryCreateActions(const QString &serverURL, const QString &data, int nOldRequestID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyActLst.contains(nOldRequestID))
    {
        m_mapReplyActLst[nOldRequestID]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit SigPostQueryCreateActions(nRequestID, serverURL, data);
    return nRequestID;
}

/************************************
* 名称: postDeleteCreateActions
* 功能: 删除新建动作
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nActionID 要收藏的Action ID
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/06/08
* 作者:   Zhouzp
************************************/
int HttpTransfer::postDeleteCreateActions(const QString &serverURL, const QString &data, int nActionID)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    int nRequestID = createUnusedRequestID();
    emit SigPostDeleteCreateActions(nRequestID, serverURL, data, nActionID);
    return nRequestID;
}

/************************************************************************************
* 函数名: downloadReplierIcon
* 功能: 下载评论者头像
* 参数:
*    @[in ] strUrl: 评论者头像URL
* 返回值:
* 时间: 2016/09/20 9:48
* 作者: hwx
*/
void HttpTransfer::downloadReplierIcon(const QString &strUrl)
{
    if(!m_oThread.isRunning())
        m_oThread.start();

    emit sigOnDownloadReplierIcon(strUrl);
}

/************************************
* 名称: HandleActionListData
* 功能: 处理ActionListData
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/03/25
* 作者:   hels
************************************/
void HttpTransfer::HandleActionListData(int requestID)
{
    if(!m_mapRecivedData.contains(requestID))
    {
        return;
    }

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[requestID];

    QString strDebug = QString("HandleActionListData: %1").arg(QString(data));
    AlphaRobotLog::getInstance()->printDebug(strDebug);

    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    if(QJsonParseError::NoError == jsonError.error)
    {
        int nTotalPage = 0;
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            //获取总页数
            if(obj.contains(NET_ACTION_TOTALPAGE))
            {
                nTotalPage = obj.take(NET_ACTION_TOTALPAGE).toInt();
            }
            m_mapTotalPages.insert(requestID, nTotalPage);
            emit SigTotalPage(requestID, nTotalPage);

            if(obj.contains(NET_TOTAL_RECORDS_COUNT))
            {
                const int nTotalRecordCount = obj.take(NET_TOTAL_RECORDS_COUNT).toInt();
                emit sigRecordCount(requestID, nTotalRecordCount);
            }

            //item 数组
            if(obj.contains(NET_MODELS))
            {
                QJsonValue valueModels = obj.take(NET_MODELS);
                if(valueModels.isArray())
                {
                    QJsonArray array = valueModels.toArray();
                    int nSize = array.size();
                   // int nQueryTime = 1;
                    //qDebug()<<"ActionCount:"<<nSize<<endl;
                    for(int i = 0; i < nSize; i++)
                    {
                        QJsonValue value = array.at(i);
                        if(value.isObject())
                        {
                            QJsonObject obj = value.toObject();

                            ActItemData newActItem;
                            getActItemData(obj, newActItem);

                            m_mapActItem.insert(newActItem.m_nItemID, newActItem);

                            QFileInfo fileInfo(newActItem.m_sImageHeadUrl);
                            QString strFielName = CConfigs::getLocalPreviewImgPath(fileInfo.fileName());
                            m_mapActItem[newActItem.m_nItemID].m_sImageHeadUrl = strFielName;

                            //完成ActitemData的获取后，需要单独下载预览图
                            //不存在则下载
                            downLoadPreviewImage(requestID, newActItem);

                            // 下载动作详情页动作描述图片
                            postDownloadImage(newActItem.m_sImageURL, CConfigs::getLocalDetailedImageDir(), Constants::HTTP_DEFAULT_REQUEST_ID);

                            emit SigItemDate(requestID, m_mapActItem[newActItem.m_nItemID], m_mapTotalPages[requestID]);//请求下载预览图后，发送Signal设置ItemData

                        }
                    }
                }
            }
        }
    }

    m_mapRecivedData.remove(requestID);
}

/************************************
* 名称: handleRecommendActionData
* 功能: 出来推荐的动作数据
* 参数: [in]nRequestID 请求的ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/06/04
* 作者:   hwx
************************************/
void HttpTransfer::handleRecommendListData(int nRequestID)
{
    if(!m_mapRecivedData.contains(nRequestID))
        return;

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[nRequestID];

    QString strDebugInfo = QString("Receive http recommend list data: %1").arg(QString(data));
    AlphaRobotLog::getInstance()->printDebug(strDebugInfo);

    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();

            // 从Json string从恢复基本动作数据
            recoverRecommendActionDataFromJsonString(obj, nRequestID);
        }
    }

    m_mapRecivedData.remove(nRequestID);
}

/************************************
* 名称: handleBusinessPromoteData
* 功能: 处理运营活动数据
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/08/30
* 作者:   hwx
************************************/
void HttpTransfer::handleBusinessPromoteData(int nRequestID)
{
    if (!m_mapRecivedData.contains(nRequestID))
    {
        return;
    }

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[nRequestID];

    /////////////////////////////Log/////////////////////////
    QString strDebugInfo = QString("Receive business promote data: %1").arg(QString(data));
    AlphaRobotLog::getInstance()->printDebug(strDebugInfo);
    //////////////////////////////Log////////////////////////

    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject objRoot = parseDocument.object();

            // 获取models字段
            if (!objRoot.contains(NET_MODELS))
            {
                AlphaRobotLog::getInstance()->printError(QString("handleBusinessPromoteData: Not contains [%1]").arg(NET_MODELS));
                return;
            }

            QJsonValue valueModels = objRoot.value(NET_MODELS);
            if(!valueModels.isObject())
            {
                AlphaRobotLog::getInstance()->printError(QString("handleBusinessPromoteData: [%1] is not object").arg(NET_MODELS));
                return;
            }

            QJsonObject objModels = valueModels.toObject();

            //////////////////////////////////////////////////////////////////////////////
            // schemeInfo
            ////////////////////////////////////////////////////////////////////////////////
            int nId = -1;
            int nType = -1;
            QString sName;
            QString sDateTime;
            QString sCoverImageUrl;
            QString strDetailedImageUrl;
            QString sDescriptionMsg;
            if (!objModels.contains(NET_SCHEME_INFO))
            {
                AlphaRobotLog::getInstance()->printError(QString("handleBusinessPromoteData: Not contains [%1]").arg(NET_SCHEME_INFO));
                return;
            }

            QJsonValue valueSchemeInfo = objModels.value(NET_SCHEME_INFO);
            if (!valueSchemeInfo.isArray())
            {
                AlphaRobotLog::getInstance()->printError(QString("handleBusinessPromoteData: [%1] is not array").arg(NET_SCHEME_INFO));
                return;
            }

            QJsonArray arraySchemeInfo = valueSchemeInfo.toArray();
            for (int i = 0; i < arraySchemeInfo.size(); i++)
            {
                QJsonValue valueItem = arraySchemeInfo.at(i);
                if (!valueItem.isObject())
                {
                    AlphaRobotLog::getInstance()->printError(QString("handleBusinessPromoteData:array item is not object"));
                    return;
                }

                QJsonObject objItem = valueItem.toObject();

                // schemeId
                if (objItem.contains(NET_SCHEME_ID))
                {
                    nId = objItem.value(NET_SCHEME_ID).toInt();
                }

                // schemeType
                if (objItem.contains(NET_SCHEME_TYPE))
                {
                    nType = objItem.value(NET_SCHEME_TYPE).toInt();
                }

                // schemeName
                if (objItem.contains(NET_SCHEME_NAME))
                {
                    sName = objItem.value(NET_SCHEME_NAME).toString();
                }

                // schemeIcon
                if (objItem.contains(NET_SCHEME_COVER_IMAGE))
                {
                    sCoverImageUrl = objItem.value(NET_SCHEME_COVER_IMAGE).toString();
                }

                // schemeImage
                if (objItem.contains(NET_SCHEME_DETAILED_IMAGE))
                {
                    strDetailedImageUrl = objItem.value(NET_SCHEME_DETAILED_IMAGE).toString();
                }

                // schemeDate
                if (objItem.contains(NET_SCHEME_DATE))
                {
                    double dTime = objItem.value(NET_SCHEME_DATE).toDouble();
                    QDateTime time;
                    time.setTime_t(dTime / 1000);
                    sDateTime = time.toString(NET_ACTION_DATE_FORMAT);
                }

                // schemeContent
                if (objItem.contains(NET_SCHEME_CONTENT))
                {
                    sDescriptionMsg = objItem.value(NET_SCHEME_CONTENT).toString();
                }
            }   // for arraySchemeInfo

            // 推广活动数据
            BusinessPromoteData *pBusinessPromoteItem = new BusinessPromoteData();
            pBusinessPromoteItem->setId(nId);
            pBusinessPromoteItem->setType(nType);
            pBusinessPromoteItem->setName(sName);
            pBusinessPromoteItem->setDateTime(sDateTime);
            pBusinessPromoteItem->setCoverImageUrl(sCoverImageUrl);
            pBusinessPromoteItem->setDetailedImageUrl(strDetailedImageUrl);
            pBusinessPromoteItem->setDescriptionMsg(sDescriptionMsg);
            if (!m_mapBusinessPromoteItemList.contains(nId))
            {
                m_mapBusinessPromoteItemList.insert(nId, pBusinessPromoteItem);
            }

            // 下载活动封面图片
            downloadBusinessPromoteImage(nId, sCoverImageUrl, Constants::eTypeCoverImage);

            // 下载活动详情图片
            downloadBusinessPromoteImage(nId, strDetailedImageUrl, Constants::eTypeDetailedImage);

//            ///////////////////////////////////////////////////////////////////////////
//            // schemeAction
//            /////////////////////////////////////////////////////////////////////////
//            if (!objModels.contains(NET_SCHEME_ACTION))
//            {
//                AlphaRobotLog::getInstance()->printError(QString("handleBusinessPromoteData: Not contains [%1]").arg(NET_SCHEME_ACTION));
//                return;
//            }

//            QJsonValue valueActionList = objModels.value(NET_SCHEME_ACTION);
//            if (!valueActionList.isArray())
//            {
//                AlphaRobotLog::getInstance()->printError(QString("handleBusinessPromoteData: [%1] is not array").arg(NET_SCHEME_ACTION));
//                return;
//            }

//            QJsonArray arrayAction = valueActionList.toArray();
//            for (int i = 0; i < arrayAction.size(); i++)
//            {
//                QJsonValue valueIAction = arrayAction.at(i);
//                if (!valueIAction.isObject())
//                {
//                    AlphaRobotLog::getInstance()->printError(QString("handleBusinessPromoteData:action item is not object"));
//                    continue;
//                }

//                UserRankItem userRankItem;
//                ActItemData &actionItem = userRankItem.getActionItemData();
//                QJsonObject objRankItem = valueIAction.toObject();

//                // 动作数据
//               getActItemData(objRankItem, actionItem);

//               m_mapActItem.insert(actionItem.m_nItemID, actionItem);

//               // 下载动作预览图片
//               m_nRequestID++;
//               downLoadPreviewImage(m_nRequestID, actionItem);

//               // 下载用户头像
//               downloadUserImage(pBusinessPromoteItem->getId(), actionItem);

//               pBusinessPromoteItem->addUserRankItem(userRankItem);
//            }   // for arrayAction
        } // if(QJsonParseError::NoError == jsonError.error)
    }

    m_mapRecivedData.remove(nRequestID);
}

/************************************
* 名称: handleRankActionData
* 功能: 处理排行榜动作数据
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 参数: [in]nSchemeID 活动ID
* 返回:   void
* 时间:   2016/10/30
* 作者:   hwx
************************************/
void HttpTransfer::handleRankActionData(int nRequestID, int nSchemeID)
{
    if (!m_mapRecivedData.contains(nRequestID))
    {
        return;
    }

    if (!m_mapBusinessPromoteItemList.contains(nSchemeID))
    {
        return;
    }

    BusinessPromoteData *pPromoteData = m_mapBusinessPromoteItemList.value(nSchemeID);
    if (!pPromoteData)
    {
        return;
    }

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[nRequestID];

    /////////////////////////////Log/////////////////////////
    QString strDebugInfo = QString("Receive rank action data: %1").arg(QString(data));
    AlphaRobotLog::getInstance()->printDebug(strDebugInfo);
    //////////////////////////////Log////////////////////////

    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject objRoot = parseDocument.object();

            // 返回状态
            if (objRoot.contains(NET_STATUS) && (objRoot.value(NET_STATUS).toBool() != true))
            {
                AlphaRobotLog::getInstance()->printError(QString("handleRankActionData: HTTP Error"));
                return;
            }

            // 获取models字段
            if (!objRoot.contains(NET_MODELS))
            {
                AlphaRobotLog::getInstance()->printError(QString("handleRankActionData: Not contains [%1]").arg(NET_MODELS));
                return;
            }

            QJsonValue valueActionList = objRoot.value(NET_MODELS);
            if(!valueActionList.isArray())
            {
                AlphaRobotLog::getInstance()->printError(QString("handleRankActionData: [%1] is not Array").arg(NET_MODELS));
                return;
            }

            QJsonArray arrayAction = valueActionList.toArray();
            for (int i = 0; i < arrayAction.size(); i++)
            {
                QJsonValue valueIAction = arrayAction.at(i);
                if (!valueIAction.isObject())
                {
                    AlphaRobotLog::getInstance()->printError(QString("handleRankActionData: action item is not object"));
                    continue;
                }

                UserRankItem userRankItem;
                ActItemData &actionItem = userRankItem.getActionItemData();
                QJsonObject objRankItem = valueIAction.toObject();

                // 动作数据
               getActItemData(objRankItem, actionItem);

               m_mapActItem.insert(actionItem.m_nItemID, actionItem);

               // 下载动作预览图片
                int nRequestID = createUnusedRequestID();
               downLoadPreviewImage(nRequestID, actionItem);

               // 下载动作详情页动作描述图片
               postDownloadImage(actionItem.m_sImageURL, CConfigs::getLocalDetailedImageDir(), Constants::HTTP_DEFAULT_REQUEST_ID);

               // 下载用户头像
               downloadUserImage(pPromoteData->getId(), actionItem);

               pPromoteData->addUserRankItem(userRankItem);
            }   // for arrayAction
            emit sigOnUpdatePromoteData(pPromoteData);
        } // if(parseDocument.isObject())
    } // if(QJsonParseError::NoError == jsonError.error)
}

/************************************
* 名称: handleActionCommentData
* 功能: 处理动作评论数据
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/09/19
* 作者:   hwx
************************************/
void HttpTransfer::handleActionCommentData(int nRequestID)
{
    if (!m_mapRecivedData.contains(nRequestID))
    {
        return;
    }

    QByteArray bytearrayData = m_mapRecivedData[nRequestID];

    /////////////////////////////Log/////////////////////////
    QString strDebugInfo = QString("Receive action comment data: %1").arg(QString(bytearrayData));
    AlphaRobotLog::getInstance()->printDebug(strDebugInfo);
    //////////////////////////////Log////////////////////////

    if (bytearrayData.size() <= 0)
    {
        return;
    }

    emit sigOnReceivedActionComment(bytearrayData);
    m_mapRecivedData.remove(nRequestID);
}

/************************************
* 名称: HandleFavoritesData
* 功能: 处理FavoritesData
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void HttpTransfer::HandleFavoritesData(int requestID)
{
    if(!m_mapRecivedData.contains(requestID))
    {
        return;
    }

    int nTotalPage = 0;
    if (m_mapTotalPages.contains(requestID))
    {
        nTotalPage = m_mapTotalPages[requestID];
    }

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[requestID];
//    qDebug()<<"QueryCollect"<<data<<endl;
    AlphaRobotLog::getInstance()->printDebug(QString("Get Collect Response: %1").arg(QString(data)));
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            //item 数组
            if(obj.contains(NET_MODELS))
            {
                QJsonValue valueModels = obj.take(NET_MODELS);
                if(valueModels.isArray())
                {
                    QJsonArray array = valueModels.toArray();
                    int nSize = array.size();
                    emit SigTotalFavorites(requestID, nSize);
                    for(int i = 0; i < nSize; i++)
                    {
                        int nActionID = -1;
                        QJsonValue value = array.at(i);
                        if(value.isObject())
                        {
                            QJsonObject obj = value.toObject();

                            // 新的接口中actionInfo字段返回了动作详细信息，
                            // 但是为了兼容老版本，解决不到该字段时，还是需要
                            // 走原来的流程
                            if (obj.contains(NET_ACTION_INFO))
                            {
                                QJsonValue valueActInfo = obj.take(NET_ACTION_INFO);
                                if(valueActInfo.isObject())
                                {
                                    QJsonObject objActionInfo = valueActInfo.toObject();
                                   // int nQueryTime = 1;
                                    //qDebug()<<"ActionCount:"<<nSize<<endl;
                                    ActItemData newActItem;
                                    getActItemData(objActionInfo, newActItem);

                                    m_mapActItem.insert(newActItem.m_nItemID, newActItem);

                                    QFileInfo fileInfo(newActItem.m_sImageHeadUrl);
                                    QString strFielName = CConfigs::getLocalPreviewImgPath(fileInfo.fileName());
                                    m_mapActItem[newActItem.m_nItemID].m_sImageHeadUrl = strFielName;

                                    //完成ActitemData的获取后，需要单独下载预览图
                                    //不存在则下载
                                    downLoadPreviewImage(requestID, newActItem);

                                    // 下载动作详情页动作描述图片
                                    postDownloadImage(newActItem.m_sImageURL, CConfigs::getLocalDetailedImageDir(), Constants::HTTP_DEFAULT_REQUEST_ID);

                                    emit SigItemDate(requestID, m_mapActItem[newActItem.m_nItemID], nTotalPage);//请求下载预览图后，发送Signal设置ItemData
                                }
                                continue;
                            }


                            //动作类型
                            if(obj.contains(NET_COLLECT_RELATION_ID))
                            {
                                nActionID = obj.take(NET_COLLECT_RELATION_ID).toInt();
                            }

                            QString sRequestURL = g_sServer + NET_PATH_DETAIL;
                            FILEVERINFO fileinfo;
                            QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
                            CUtil::GetFileVersion(sAppPath, fileinfo);

                            int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
                            QString sServiceVersion = fileinfo.sFileVer;  //版本号
                            QString sParam, sParamActParam;
                            sParam = "{";
                            sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
                            sParam += ",";

                            if(nActionID != -1)
                            {
                                sParamActParam.sprintf("\"actionId\":\"%d\"", nActionID);
                                sParam += sParamActParam;
                            }

                            sParam += "}";

                            AlphaRobotLog::getInstance()->printDebug(QString("[Request Action Detailed] Request URL: %1, Request Param: %2").arg(sRequestURL).arg(sParam));

                            //单独查询单个动作详情
                            QNetworkRequest requestDetail;
                            QUrl url(sRequestURL);
                            requestDetail.setUrl(url);
                            requestDetail.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                            requestDetail.setHeader(QNetworkRequest::ContentLengthHeader, sParam.toUtf8().size());

                            requestDetail.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)requestID));//设置ID
                            requestDetail.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID),qVariantFromValue((int)nActionID));//设置ID
                            requestDetail.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_ACTION_DETAIL);//设置类型
//                            qDebug()<<"sRequestFavouriteURL"<<sRequestURL<<"sParam"<<sParam<<endl;
                            QNetworkReply* reply = m_pNetManager->post(requestDetail, sParam.toUtf8());
                            connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
                            connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                                    this, &HttpTransfer::OnError);
                            connect(reply, &QNetworkReply::downloadProgress,
                                    this, &HttpTransfer::OnDownloadProgress);
                        }
                    }
                }
            }
        }
    }
    m_mapRecivedData.remove(requestID);
}


/************************************
* 名称: HandleMyUploadsData
* 功能: 处理ActionListData
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 返回:   void
* 时间:   20160608
* 作者:   asu
************************************/
void HttpTransfer::HandleMyUploadsData(int requestID)
{
    if(!m_mapRecivedData.contains(requestID))
    {
        return;
    }

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[requestID];
//    qDebug()<<"QueryCollect"<<data<<endl;
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            //item 数组
            if(obj.contains("models"))
            {
                QJsonValue valueModels = obj.take("models");
                if(valueModels.isArray())
                {
                    QJsonArray array = valueModels.toArray();
                    int nSize = array.size();
                    emit SigTotalMyUploads(requestID, nSize);
                    for(int i = 0; i < nSize; i++)
                    {
                        int nActionID = -1;
                        QJsonValue value = array.at(i);
                        if(value.isObject())
                        {
                            //动作类型
                            QJsonObject obj = value.toObject();
                            if(obj.contains("actionId"))
                            {
//                                QString strActionID = obj.take("actionId").toString();
                                nActionID = obj.take("actionId").toInt();
                            }

                            FILEVERINFO fileinfo;
                            QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
                            CUtil::GetFileVersion(sAppPath, fileinfo);

                            int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
                            QString sServiceVersion = fileinfo.sFileVer;  //版本号
                            QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/detail";

                            QString sParam, sParamActParam;
                            sParam = "{";
                            sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
                            sParam += ",";

                            if(nActionID != -1)
                            {
                                sParamActParam.sprintf("\"actionId\":\"%d\"", nActionID);
//                                sParamActParam.sprintf("\"actionId\":\"%lld\"", nActionID);
                                sParam += sParamActParam;
                            }

                            sParam += "}";


                            //单独查询单个动作详情
                            QNetworkRequest requestDetail;
                            QUrl url(sRequestURL);
                            requestDetail.setUrl(url);
                            requestDetail.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                            requestDetail.setHeader(QNetworkRequest::ContentLengthHeader, sParam.toUtf8().size());

                            requestDetail.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)requestID));//设置ID
                            requestDetail.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID),qVariantFromValue((int)nActionID));//设置ID
                            requestDetail.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_ACTION_DETAIL);//设置类型
//                            qDebug()<<"sRequestFavouriteURL"<<sRequestURL<<"sParam"<<sParam<<endl;
                            QNetworkReply* reply = m_pNetManager->post(requestDetail, sParam.toUtf8());
                            connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
                            connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                                    this, &HttpTransfer::OnError);
                            connect(reply, &QNetworkReply::downloadProgress,
                                    this, &HttpTransfer::OnDownloadProgress);
                        }
                    }
                }
            }
        }
    }

    m_mapRecivedData.remove(requestID);
}

/************************************
* 名称: HandleCollectActionData
* 功能: 处理收藏数据
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 参数: [in]nActionID Action ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void HttpTransfer::HandleCollectActionData(int requestID, int nActionID)
{
    if(!m_mapRecivedData.contains(requestID))
        return;

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[requestID];
    AlphaRobotLog::getInstance()->printDebug(QString("get action collection result: %1").arg(QString(data)));
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    int nRet = -1;
//    qDebug()<<data;
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(obj.contains("info"))
            {
                QString strRet = obj.take("info").toString();
                if(0 == strRet.compare("0000"))
                {
                    nRet = 1;  //收藏成功
                }
                else if(0 == strRet.compare("4200"))
                {
                    nRet = 0;//已经收藏过
                }
            }
        }
    }
    emit SigIsCollectResult(requestID, nActionID, nRet);
    m_mapRecivedData.remove(requestID);
}

/************************************
* 名称: HandleCancelCollectData
* 功能: 处理取消收藏数据
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 参数: [in]nActionID Action ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void HttpTransfer::HandleCancelCollectData(int requestID, int nActionID)
{
    if(!m_mapRecivedData.contains(requestID))
        return;

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[requestID];
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    int nRet = -1;
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(obj.contains("info"))
            {
                QString strRet = obj.take("info").toString();
                if(0 == strRet.compare("0000"))
                {
                    nRet = -2;  //取消收藏成功
                }
            }
        }
    }
    emit SigIsCollectResult(requestID, nActionID, nRet);
    m_mapRecivedData.remove(requestID);
}

void HttpTransfer::HandleCancelMyUploadData(int requestID, int nActionID)
{
    if(!m_mapRecivedData.contains(requestID))
    {
        return;
    }

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[requestID];
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    int nRet = Constants::HTTP_RESULT_FAIL;
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(obj.contains("info"))
            {
                QString strRet = obj.take("info").toString();
                if(0 == strRet.compare("0000"))
                {
                    nRet = Constants::HTTP_RESULT_SUCESS;  //取消收藏成功
                }
            }
        }
    }

    emit SigCancelMyUpload(requestID, nActionID, nRet);
    m_mapRecivedData.remove(requestID);
}

/************************************
* 名称: HandleIsCollectData
* 功能: 处理是否收藏查询数据
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 参数: [in]nActionID Action ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void HttpTransfer::HandleIsCollectData(int requestID, int nActionID)
{
    if(!m_mapRecivedData.contains(requestID))
        return;

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[requestID];
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    int nRet = -1;

    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(obj.contains("info"))
            {
                QString strRet = obj.take("info").toString();
                if(0 == strRet.compare("0000"))
                {
                    if(obj.contains("models"))
                    {
                        QJsonArray arr = obj.take("models").toArray();
                        if(arr.size() > 0)
                        {
                            nRet = 1;//收藏成功
                        }
                    }
                }
                else if(0 == strRet.compare("4200"))
                {
                    nRet = 0;//已经收藏过
                }
            }
        }
    }
    emit SigIsCollectResult(requestID, nActionID, nRet);
    m_mapRecivedData.remove(requestID);
}

/************************************
* 名称: handlePraiseActionResult
* 功能: 处理点赞动作返回结果
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/10/22
* 作者:   hwx
************************************/
void HttpTransfer::handlePraiseActionResult(int nRequestId)
{
    if(!m_mapRecivedData.contains(nRequestId))
    {
        return;
    }

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[nRequestId];

    //////////////////////////////////////Log////////////////////////////
    QString strLog = QString("Receive action praise result: %1").arg(QString(data));
    AlphaRobotLog::getInstance()->printDebug(strLog);
    /////////////////////////////////////Log////////////////////////////

    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    int nRet = -1;

    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(obj.contains("info"))
            {
                QString strRet = obj.take("info").toString();
                if(0 == strRet.compare("0000"))
                {
                    nRet = 1;//点赞成功
                }
            }
        }
    }
    emit sigPraiseActionResult(nRequestId, nRet);
    m_mapRecivedData.remove(nRequestId);
}

/************************************
* 名称: HandleActionDetail
* 功能: 处理动作详情数据
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 参数: [in]nActionID Action ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void HttpTransfer::HandleActionDetail(int requestID, int nActionID)
{
    if(!m_mapActionDetailData.contains(nActionID))
        return;

    int nTotalPage = 0;
    if (m_mapTotalPages.contains(requestID))
    {
        nTotalPage = m_mapTotalPages[requestID];
    }

    //解析动作列表json格式数据
    QByteArray &data = m_mapActionDetailData[nActionID];
//    qDebug()<<"HandleActionDetail"<<data<<endl;
    AlphaRobotLog::getInstance()->printDebug(QString("HandleActionDetail: %1").arg(QString(data)));
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(obj.contains("models"))
            {
                QJsonValue valueModels = obj.take("models");
                if(valueModels.isObject())
                {
                    obj = valueModels.toObject();

                    ActItemData newActItem;
                    getActItemData(obj, newActItem);

                    if(m_mapActItem.contains(newActItem.m_nItemID))
                    {
                        emit SigItemDate(requestID, m_mapActItem[nActionID], nTotalPage);
                    }
                    else
                    {
                        m_mapActItem.insert(newActItem.m_nItemID, newActItem);

                        //完成ActitemData的获取后，需要单独下载预览图
                        //不存在则下载
                        QFileInfo fileInfo(newActItem.m_sImageHeadUrl);                        
                        QString strFielName = CConfigs::getLocalPreviewImgPath(fileInfo.fileName());
                        m_mapActItem[newActItem.m_nItemID].m_sImageHeadUrl = strFielName;                        

                        downLoadPreviewImage(requestID, newActItem);

                        // 下载动作详情页动作描述图片
                        postDownloadImage(newActItem.m_sImageURL, CConfigs::getLocalDetailedImageDir(), Constants::HTTP_DEFAULT_REQUEST_ID);

                        emit SigItemDate(requestID, m_mapActItem[nActionID], nTotalPage);
                    }
                }
            }
        }
    }

    m_mapActionDetailData.remove(nActionID);
}

/************************************
* 名称: HandlePrevewImageData
* 功能: 处理Preview Image Data
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 参数: [in]nActionID Action ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/03/25
* 作者:   hels
************************************/
void HttpTransfer::HandlePrevewImageData(int /*requestID*/, int nActionID)
{
    if(!m_mapPreviewImageData.contains(nActionID))
    {
        return;
    }

    if (!m_mapActItem.contains(nActionID))
    {
        return;
    }

    ActItemData actItem = m_mapActItem[nActionID];

    QFileInfo fileInfo(actItem.m_sImageHeadUrl);
    QString strFielName = CConfigs::getLocalPreviewImgPath(fileInfo.fileName());
    QByteArray &data = m_mapPreviewImageData[nActionID];

    bool bOk = CFileDirHandle::saveFile(strFielName, data);
    if(bOk)
    {
        emit SigItemUpdate();//通知动作列表进行刷新
    }

    //    emit SigItemDate(requestID, m_mapActItem[nActionID], m_mapTotalPages[requestID]);//这里不再发送信号，改为在获取动作列表时发送，以免顺序发生错乱

    m_mapPreviewImageData.remove(nActionID);
    m_mapActItem.remove(nActionID);
}

/************************************
* 名称: saveLocalRecommendImage
* 功能: 保存推荐动作的图片
* 参数：nRecommendID,请求的类型
* 返回:   void
* 时间:   2016/03/25
* 作者:   hwx
************************************/
void HttpTransfer::saveLocalRecommendImage(int nRecommendID, QByteArray data)
{
    if(!m_mapRecommendItemList.contains(nRecommendID))
    {
        return;
    }

    RecommendActItemData *pRecommendItem = m_mapRecommendItemList[nRecommendID];
    QFileInfo fileInfo(pRecommendItem->m_strRecommendImage);    

    QString strFielName = CConfigs::getLocalPreviewImgPath(fileInfo.fileName());
    pRecommendItem->m_strRecommendImage = strFielName;
        qDebug() << "download recommend image finished" << endl;
    bool bOk = CFileDirHandle::saveFile(strFielName, data);
    if(bOk)
    {
        emit sigAddRecommendAction(m_mapRecommendItemList[nRecommendID]);
    }
}

/************************************
* 名称: saveLocalRecommendImage
* 功能: 保存用户头像图片
* 参数：nItemId, 运营活动id
* 参数：strUrl, 用户图片url
* 参数：data，图片数据
* 返回:   void
* 时间:   2016/08/31
* 作者:   hwx
************************************/
void HttpTransfer::saveLocalUserImage(int nItemId, const QString &strUrl, const QByteArray &data)
{
    if(!m_mapBusinessPromoteItemList.contains(nItemId) || strUrl.isEmpty())
    {
        return;
    }

    BusinessPromoteData *pPromoteItem = m_mapBusinessPromoteItemList[nItemId];
    if (!pPromoteItem)
    {
        return;
    }

    QFileInfo fileInfo(strUrl);
    QString strFielName = CConfigs::getLocalPreviewImgPath(fileInfo.fileName());
    bool bOk = CFileDirHandle::saveFile(strFielName, data);
//    if(bOk)
//    {
//        emit sigOnUpdatePromoteItem(pPromoteItem);
//     }
}

/************************************
* 名称: saveLocalPromoteImage
* 功能: 保存推广活动图片
* 参数：[in] nItemId: 运营活动id
*           [in] data: 图片数据
*           [in] eType: 图片类型
* 返回:   void
* 时间:   2016/08/31
* 作者:   hwx
************************************/
void HttpTransfer::saveLocalPromoteImage(int nItemId, const QByteArray &data, Constants::emActivityImageType eType)
{
    if(!m_mapBusinessPromoteItemList.contains(nItemId))
    {
        return;
    }

    BusinessPromoteData *pPromoteItem = m_mapBusinessPromoteItemList[nItemId];
    if (!pPromoteItem)
    {
        return;
    }

    QString strUrl;

    switch (eType) {
        case Constants::eTypeCoverImage:
        {
        qDebug() << "download cover finished" << endl;
            strUrl =  pPromoteItem->getCoverImageUrl();
            break;
        }
        case Constants::eTypeDetailedImage:
        {
            strUrl = pPromoteItem->getDetailedImageUrl();
        }
        default:
        {
            break;
        }
    }

    if (strUrl.isEmpty())
    {
        return;
    }

    QFileInfo fileInfo(strUrl);
    QString strFielName = CConfigs::getLocalPreviewImgPath(fileInfo.fileName());
    bool bOk = CFileDirHandle::saveFile(strFielName, data);
    if(bOk && (eType == Constants::eTypeCoverImage))
    {
        emit sigOnAddPromoteItem(pPromoteItem);
    }
}

/************************************
* 名称: saveLocalCommentReplierIcon
* 功能: 保存评论者图标
* 参数：[in] strUrl: 资源文件路径
*           [in] nRequestId: 请求id
* 返回:   void
* 时间:   2016/09/20
* 作者:   hwx
************************************/
void HttpTransfer::saveLocalCommentReplierIcon(int nRequestId, const QString &strUrl)
{
    if (strUrl.isEmpty() || !m_mapRecivedData.contains(nRequestId))
    {
        return;
    }

    QByteArray arrayData = m_mapRecivedData[nRequestId];

    QFileInfo fileInfo(strUrl);
    QString strFielName = CConfigs::getLocalPreviewImgPath(fileInfo.fileName());
    bool bOk = CFileDirHandle::saveFile(strFielName, arrayData);
    if(bOk)
    {
        emit sigOnDownloadCommentReplierIconDone();
    }
}

/************************************
* 名称: saveImageFile
* 功能: 保存网络图片
* 参数：[in] nRequestId: 请求ID
*           [in] strFilePath: 保存文件，含路径
* 返回:   void
* 时间:   2016/11/18
* 作者:   hwx
************************************/
void HttpTransfer::saveImageFile(int nRequestId, const QString &strFilePath)
{
    if (strFilePath.isEmpty() || !m_mapRecivedData.contains(nRequestId))
    {
        return;
    }

    QByteArray arrayData = m_mapRecivedData[nRequestId];
    bool bOk = CFileDirHandle::saveFile(strFilePath, arrayData);
    if(bOk)
    {
        emit sigOndownLoadImageDone(nRequestId);
    }
}

/************************************
* 名称: HandleAddDownLoad
* 功能: 处理增加下载数量
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/06/08
* 作者:   Zhouzp
************************************/
void HttpTransfer::HandleAddDownLoad(int requestID, int nActionID)
{
    if(!m_mapRecivedData.contains(requestID))
        return;

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[requestID];
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    int nRet = -1;

    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(obj.contains("info"))
            {
                QString strRet = obj.take("info").toString();
                if(0 == strRet.compare("0000"))
                {
                    if(obj.contains("models"))
                    {
                        QJsonArray arr = obj.take("models").toArray();
                        if(arr.size() > 0)
                        {
                            nRet = 1;//Add DownLoad Success!
                        }
                    }
                }
            }
        }
    }
    emit SigIsAddDownLoadResult(requestID, nActionID, nRet);
    m_mapRecivedData.remove(requestID);
}

/************************************
* 名称: HandleCreateActionsData
* 功能: 处理新建动作列表数据
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/06/08
* 作者:   Zhouzp
************************************/
void HttpTransfer::HandleCreateActionsData(int requestID)
{
    if(!m_mapRecivedData.contains(requestID))
        return;

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[requestID];
//    qDebug()<<"QueryCollect"<<data<<endl;
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            //item 数组
            if(obj.contains("models"))
            {
                QJsonValue valueModels = obj.take("models");
                if(valueModels.isArray())
                {
                    QJsonArray array = valueModels.toArray();
                    int nSize = array.size();
                    emit SigTotalFavorites(requestID, nSize);
                    for(int i = 0; i < nSize; i++)
                    {
                        int nActionID = -1;
                        QJsonValue value = array.at(i);
                        if(value.isObject())
                        {
                            //动作类型
                            QJsonObject obj = value.toObject();
                            if(obj.contains("collectRelationId"))
                            {
                                nActionID = obj.take("collectRelationId").toInt();
                            }

                            QString sRequestURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/action/detail";
                            FILEVERINFO fileinfo;
                            QString sAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
                            CUtil::GetFileVersion(sAppPath, fileinfo);

                            int nAppType = ALPHA_APP_TYPE;   //产品种类，1为1s，2为2代
                            QString sServiceVersion = fileinfo.sFileVer;  //版本号
                            QString sParam, sParamActParam;
                            sParam = "{";
                            sParam += QString::fromStdString(GetBasicParam(nAppType,sServiceVersion.toStdString()));
                            sParam += ",";

                            if(nActionID != -1)
                            {
                                sParamActParam.sprintf("\"actionId\":\"%d\"", nActionID);
                                sParam += sParamActParam;
                            }

                            sParam += "}";


                            //单独查询单个动作详情
                            QNetworkRequest requestDetail;
                            QUrl url(sRequestURL);
                            requestDetail.setUrl(url);
                            requestDetail.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                            requestDetail.setHeader(QNetworkRequest::ContentLengthHeader, sParam.toUtf8().size());

                            requestDetail.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)requestID));//设置ID
                            requestDetail.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID),qVariantFromValue((int)nActionID));//设置ID
                            requestDetail.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_ACTION_DETAIL);//设置类型
//                            qDebug()<<"sRequestFavouriteURL"<<sRequestURL<<"sParam"<<sParam<<endl;
                            QNetworkReply* reply = m_pNetManager->post(requestDetail, sParam.toUtf8());
                            connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
                            connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                                    this, &HttpTransfer::OnError);
                            connect(reply, &QNetworkReply::downloadProgress,
                                    this, &HttpTransfer::OnDownloadProgress);
                        }
                    }
                }
            }
        }
    }
    m_mapRecivedData.remove(requestID);
}

/************************************
* 名称: HandleDeleteCreateActionsData
* 功能: 处理删除新建动作列表数据
* 参数: [in]requestID 请求的ID，据此判断处理的数据
* 参数: [in]nActionID Action ID，据此判断处理的数据
* 返回:   void
* 时间:   2016/06/08
* 作者:   Zhouzp
************************************/
void HttpTransfer::HandleDeleteCreateActionsData(int requestID, int nActionID)
{
    if(!m_mapRecivedData.contains(requestID))
        return;

    //解析动作列表json格式数据
    QByteArray &data = m_mapRecivedData[requestID];
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);
    int nRet = -1;
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(obj.contains("info"))
            {
                QString strRet = obj.take("info").toString();
                if(0 == strRet.compare("0000"))
                {
                    nRet = -2;  //删除动作成功
                }
            }
        }
    }
    emit SigIsCollectResult(requestID, nActionID, nRet);
    m_mapRecivedData.remove(requestID);
}

/************************************
* 名称: OnPostQueryActList
* 功能: post http请求槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/03/24
* 作者:   hels
************************************/
void HttpTransfer::OnPostQueryActList(int nNewRequestID, const QString &serverURL, const QString &data)
{
    m_mapParamData.insert(nNewRequestID, data.toUtf8());
    QNetworkRequest request;
    QUrl url(serverURL);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_ACTION_LIST);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
    m_mapReplyActLst.insert(nNewRequestID, reply);
}

/************************************
* 名称: onPostQueryRecommendActList
* 功能: post http请求槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/06/03
* 作者:   hwx
************************************/
void HttpTransfer::onPostQueryRecommendList(int nNewRequestID, const QString &serverURL, const QString &data)
{
    m_mapParamData.insert(nNewRequestID, data.toUtf8());

    QNetworkRequest request;
    QUrl url(serverURL);

    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_RECOMMEND_LIST);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
    m_mapReplyActLst.insert(nNewRequestID, reply);
}

/************************************
* 名称: slotOnPostQueryBusinessPromoteData
* 功能: post http请求运营活动数据槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]serverURL 请求资源地址
* 参数: [in]data 请求参数数据
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/08/30
* 作者:   hwx
************************************/
void HttpTransfer::slotOnPostQueryBusinessPromoteData(int nNewRequestID, const QString &serverURL, const QString &data)
{
    m_mapParamData.insert(nNewRequestID, data.toUtf8());

    QNetworkRequest request;
    QUrl url(serverURL);

    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_BUSINESS_PROMOTE_DATA);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
    m_mapReplyActLst.insert(nNewRequestID, reply);
}

/************************************
* 名称: slotOnPostQueryRankActionData
* 功能: post http请求运营活动数据槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]nSchemeID 活动ID
* 参数: [in]strServerURL 请求资源地址
* 参数: [in]strInParame 请求参数数据
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/08/30
* 作者:   hwx
************************************/
void HttpTransfer::slotOnPostQueryRankActionData(int nNewRequestID, int nSchemeID, const QString &strServerURL, const QString &strInParame)
{
    m_mapParamData.insert(nNewRequestID, strInParame.toUtf8());

    QNetworkRequest request;
    QUrl url(strServerURL);

    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPSchemeID),qVariantFromValue((int)nSchemeID));//设置活动ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_RANK_ACTION_DATA);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
    m_mapReplyActLst.insert(nNewRequestID, reply);
}

/************************************
* 名称: slotOnPostQueryActionComment
* 功能: post http请求动作评论数据槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]strServerURL 请求资源地址
* 参数: [in]strParam 请求参数数据
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/09/19
* 作者:   hwx
************************************/
void HttpTransfer::slotOnPostQueryActionComment(int nNewRequestID, const QString &strServerURL, const QString &strParam)
{
    m_mapParamData.insert(nNewRequestID, strParam.toUtf8());

    QNetworkRequest request;
    QUrl url(strServerURL);

    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_ACTION_COMMENT);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
    m_mapReplyActLst.insert(nNewRequestID, reply);
}

/************************************
* 名称: slotOnPostAddActionComment
* 功能: post http请求添加动作评论
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]strServerUrl 请求资源地址
* 参数: [in]strParam 请求参数数据
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2016/09/20
* 作者:   hwx
************************************/
void HttpTransfer::slotOnPostAddActionComment(int nNewRequestId, const QString &strServerUrl, const QString &strParam)
{
    m_mapParamData.insert(nNewRequestId, strParam.toUtf8());

    QNetworkRequest request;
    QUrl url(strServerUrl);

    resetResp();

    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestId].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestId));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_ADD_ACTION_COMMENT);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestId]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::readyRead, this, &HttpTransfer::readyRead);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
    m_mapReplyActLst.insert(nNewRequestId, reply);
}

/************************************
* 名称: slotOnPostDownloadImage
* 功能: post http请求下载图片
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]strImageUrl 请求资源地址
* 参数: [in]strDownloadPath 图片保存路径
* 返回:
* 时间:   2016/11/17
* 作者:   hwx
************************************/
void HttpTransfer::slotOnPostDownloadImage(int nNewRequestId, const QString &strImageUrl, const QString &strDownloadPath)
{
    if (strImageUrl.isEmpty() || strDownloadPath.isEmpty())
    {
        return;
    }

    QString strDownloadFilePath = strDownloadPath;

    QMap<int, QVariant> mapAttribute;
    mapAttribute.insert(Constants::HTTPRequestID, qVariantFromValue((int)nNewRequestId));
    mapAttribute.insert(Constants::HTTPRequestType, Constants::HTTP_REQUEST_DOWNLOAD_IMAGE);
    mapAttribute.insert(Constants::HTTPDownloadFilePath, strDownloadFilePath);

    // 设置本地文件路径，覆盖原来url
    QFileInfo fileInfo(strImageUrl);
    QString strLocalImgFielName = CConfigs::instance()->getFilePath(strDownloadPath, fileInfo.fileName());
    if (QFile::exists(strLocalImgFielName))
    {
        return;
    }
    requestNetworkImage(strImageUrl, strDownloadPath, mapAttribute);
}

/************************************
* 名称: slotOnDownloadReplierIcon
* 功能: 下载评论者头像的槽函数
    * 参数: strReplierIconUrl 评论者头像URL
* 返回:
* 时间:   2016/11/29
* 作者:   hwx
************************************/
void HttpTransfer::slotOnDownloadReplierIcon(const QString &strReplierIconUrl)
{
    QString strUrl = strReplierIconUrl;
    if (strUrl.isEmpty())
    {
        return;
    }

    // 设置本地文件路径，覆盖原来url
    QFileInfo fileInfo(strUrl);
    QString previewImageDir = CConfigs::instance()->getLocalPreviewImgDir();
    QString strLocalImgFielName = CConfigs::instance()->getFilePath(previewImageDir, fileInfo.fileName());

    if (!QFile::exists(strLocalImgFielName))
    {
        QMap<int, QVariant> mapAttribute;
        int nRequestID = createUnusedRequestID();
        mapAttribute.insert(Constants::HTTPRequestID, qVariantFromValue((int)nRequestID));
        mapAttribute.insert(Constants::HTTPRequestType, Constants::HTTP_REQUEST_COMMENT_REPLIER_ICON);
         requestNetworkImage(strUrl, previewImageDir, mapAttribute);
    }
}

/************************************
* 名称: OnPostQueryFavorites
* 功能: 我的收藏请求槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]serverURL 请求资源地
* 参数: [in]data 请求参数数据
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void HttpTransfer::OnPostQueryFavorites(int nNewRequestID, const QString &serverURL, const QString &data)
{
    m_mapParamData.insert(nNewRequestID, data.toUtf8());
    QNetworkRequest request;
    QUrl url(serverURL);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_FAVORITES);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
    m_mapReplyActLst.insert(nNewRequestID, reply);
}

/************************************
* 名称: OnPostCollectAction
* 功能: 收藏动作槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nActionID 要收藏的Action ID
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void HttpTransfer::OnPostCollectAction(int nNewRequestID, const QString &serverURL, const QString &data, int nActionID)
{
    m_mapParamData.insert(nNewRequestID, data.toUtf8());
    QNetworkRequest request;
    QUrl url(serverURL);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID),qVariantFromValue((int)nActionID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_COLLECT_ACTION);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
}

/************************************
* 名称: slotOnPraiseAction
* 功能: 对动作点赞
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]strServerURL 请求资源地，
* 参数: [in]strData 请求参数数据
* 返回:   void
* 时间:   2016/10/20
* 作者:   hwx
************************************/
void HttpTransfer::slotOnPraiseAction(int nNewRequestID,const QString& strServerURL, const QString& strData)
{
    m_mapParamData.insert(nNewRequestID, strData.toUtf8());
    QNetworkRequest request;
    QUrl url(strServerURL);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_PRAISE_ACTION);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);

    m_mapReplyActLst.insert(nNewRequestID, reply);
}

/************************************
* 名称: OnPostCancelCollect
* 功能: 取消收藏动作槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nActionID 要收藏的Action ID
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void HttpTransfer::OnPostCancelCollect(int nNewRequestID, const QString &serverURL, const QString &data, int nActionID)
{
    m_mapParamData.insert(nNewRequestID, data.toUtf8());
    QNetworkRequest request;
    QUrl url(serverURL);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID),qVariantFromValue((int)nActionID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_CANCEL_COLLECT);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
}

/************************************
* 名称: OnPostQueryIsCollected
* 功能: 查询指定Action是否已经收藏槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nActionID 要查询的Action ID
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void HttpTransfer::OnPostQueryIsCollected(int nNewRequestID, const QString &serverURL, const QString &data, int nActionID)
{
//    qDebug()<<serverURL<<":"<<data;
    m_mapParamData.insert(nNewRequestID, data.toUtf8());
    QNetworkRequest request;
    QUrl url(serverURL);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID),qVariantFromValue((int)nActionID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_IS_COLLECTED);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
}

void HttpTransfer::OnPostAddDownloadCount(int nNewRequestID, const QString &serverURL, const QString &data, int nActionID)
{
//    qDebug()<<serverURL<<":"<<data;
    m_mapParamData.insert(nNewRequestID, data.toUtf8());
    QNetworkRequest request;
    QUrl url(serverURL);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID),qVariantFromValue((int)nActionID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_ADD_DOWDLOAD_COUNT);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
}

/************************************
* 名称: OnFinished
* 功能: 指令请求完成槽函数
* 参数: [in]QNetworkReply* reply请求对象指针
* 返回:   void
* 时间:   2016/03/24
* 作者:   hels
************************************/
void HttpTransfer::OnFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        int nRequestID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestID)).toInt();
        qint64 nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toLongLong();
        const QString sRequestType = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestType)).toByteArray();

        //上传动作的应答
        emit sigUploadActionResponse(sRequestType, nRequestID, nActionID, m_bytesResponse);

        if (sRequestType == Constants::HTTP_ADD_ACTION_COMMENT)
        {
            AlphaRobotLog::getInstance()->printDebug(QString("[Httptransfer OnFinished] RequestType: %1, Http response: %2").arg(sRequestType).arg(QString(m_bytesResponse)));
            emit sigOnHttpRequestFinished(nRequestID, m_bytesResponse);
        }

        if (m_mapReplyActLst.contains(nRequestID))
        {
             m_mapReplyActLst.remove(nRequestID);
        }

        reply->deleteLater();
    }
}

/************************************
* 名称: OnError
* 功能: 错误处理槽函数
* 参数: [in]QNetworkReply::NetworkError code请求错误代码
* 返回:   void
* 时间:   2016/03/24
* 作者:   hels
************************************/
void HttpTransfer::OnError(QNetworkReply::NetworkError code)
{
    if(code == QNetworkReply::ConnectionRefusedError ||
            code == QNetworkReply::HostNotFoundError ||
            code == QNetworkReply::TimeoutError ||
            code == QNetworkReply::NetworkSessionFailedError)
    {
        //emit serverError();错误处理
    }
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        QString requestType = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestType)).toString();
        AlphaRobotLog::getInstance()->printDebug(QString("Http Error: %1").arg(reply->errorString()));
        int nRequestID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestID)).toInt();
        if(requestType == Constants::HTTP_REQUEST_ACTION_LIST ||
                requestType == Constants::HTTP_REQUEST_FAVORITES ||
                requestType == Constants::HTTP_REQUEST_COLLECT_ACTION ||
                requestType == Constants::HTTP_REQUEST_CANCEL_COLLECT ||
                requestType == Constants::HTTP_REQUEST_IS_COLLECTED||
                requestType == Constants::HTTP_REQUEST_QUERY_CREATEACTIONS ||
                requestType == Constants::HTTP_REQUEST_DELETE_CREATEACTIONS||
                requestType == Constants::HTTP_REQUEST_ADD_DOWDLOAD_COUNT ||
                requestType == Constants::HTTP_REQUEST_ACTION_COMMENT ||
                requestType == Constants::HTTP_ADD_ACTION_COMMENT ||
                requestType == Constants::HTTP_PRAISE_ACTION)
        {
            //移除请求参数数据
            if (m_mapParamData.contains(nRequestID))
            {
                 m_mapParamData.remove(nRequestID);
            }

            //移除已经读取的数据
            if (m_mapRecivedData.contains(nRequestID))
            {
                   m_mapRecivedData.remove(nRequestID);
            }
        }
        else if(requestType == Constants::HTTP_REQUEST_ACTION_DETAIL)
        {
            int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
            if (m_mapActionDetailData.contains(nActionID))
            {
                m_mapActionDetailData.remove(nActionID);
            }
        }
        //Action preview image 请求
        else if(requestType == Constants::HTTP_REQUEST_PREVIEW_IMAGE)
        {
            //有可能preview Image path非法，或者服务器失败，但除了预览图外的信息还是要发出去
            int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
            int nTotalPage = 0;
            if (m_mapTotalPages.contains(nRequestID))
            {
                nTotalPage = m_mapTotalPages[nRequestID];
            }

            if (m_mapActItem.contains(nActionID))
            {
                emit SigItemDate(nRequestID, m_mapActItem[nActionID], nTotalPage);
                m_mapActItem.remove(nActionID);
            }

            if (m_mapPreviewImageData.contains(nActionID))
            {
                m_mapPreviewImageData.remove(nActionID);
            }
        }
        else if (requestType == Constants::HTTP_REQUEST_RECOMMEND_LIST) // 请求推荐动作的图片
        {
            //有可能preview Image path非法，或者服务器失败，但除了预览图外的信息还是要发出去
            int nRecommendID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRecommendID)).toInt();
            if (m_mapRecommendItemList.contains(nRecommendID))
            {
                emit sigAddRecommendAction(m_mapRecommendItemList[nRecommendID]);
            }
        }
        else if (requestType == Constants::HTTP_REQUEST_BUSINESS_PROMOTE_DATA) // 请求运营活动数据
        {
            //有可能图片路径信息等非法，或者服务器失败，但除了部分预览图片无法显示，还是要发送出去
            int nSchemeID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPSchemeID)).toInt();
            if (m_mapBusinessPromoteItemList.contains(nSchemeID))
            {
                emit sigOnAddPromoteItem(m_mapBusinessPromoteItemList[nSchemeID]);
            }
        }

        if (m_mapReplyActLst.contains(nRequestID))
        {
              m_mapReplyActLst.remove(nRequestID);
        }

        reply->deleteLater();
    }
}

/************************************
* 名称: OnDownloadProgress
* 功能: 下载进度处理槽函数
* 参数: [in]bytesReceived 已经接收到的字节数
* 参数: [in]bytesTotal 总共字节数
* 返回:   void
* 时间:   2016/03/24
* 作者:   hels
************************************/
void HttpTransfer::OnDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply && bytesReceived != 0)
    {
        // 注意：这里会触发两次信号，
        // 第一次：bytesReceived和bytesTotal不相等
        // 第二次：bytesReceived和bytesTotal相等
        // 必须要等待接收数据和总数据相等的时候再读取数据
        // 否则第二次读取到的数据是0
        if (bytesReceived != bytesTotal)
        {
            return;
        }
        QByteArray data = reply->readAll();
        if (data.size() <= 0)
        {
            return;
        }

        QString requestType = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestType)).toString();
        int nHTTPRequestID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestID)).toInt();

        //ActionList 请求
        if(requestType == Constants::HTTP_REQUEST_ACTION_LIST)
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            HandleActionListData(nHTTPRequestID);
        }
        else if(requestType == Constants::HTTP_REQUEST_FAVORITES)//查询收藏列表
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            HandleFavoritesData(nHTTPRequestID);
        }
        else if(requestType == Constants::HTTP_REQUEST_COLLECT_ACTION)//收藏动作
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
            HandleCollectActionData(nHTTPRequestID, nActionID);
        }
        else if(requestType == Constants::HTTP_REQUEST_CANCEL_COLLECT)//取消收藏
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
            HandleCancelCollectData(nHTTPRequestID, nActionID);
        }
        else if (requestType == Constants::HTTP_PRAISE_ACTION) // 点赞动作
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            handlePraiseActionResult(nHTTPRequestID);
        }
        else if(requestType == Constants::HTTP_REQUEST_IS_COLLECTED)//查询时候已收藏
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
            HandleIsCollectData(nHTTPRequestID, nActionID);
        }
        else if(requestType == Constants::HTTP_REQUEST_ACTION_DETAIL)
        {
            int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
            if(m_mapActionDetailData.contains(nActionID))
            {
                m_mapActionDetailData[nActionID].append(data);
            }
            else
            {
                m_mapActionDetailData.insert(nActionID, data);
            }

            HandleActionDetail(nHTTPRequestID, nActionID);
        }
        else if(requestType == Constants::HTTP_REQUEST_QUERY_CREATEACTIONS)//查询新建动作列表
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            HandleMyUploadsData(nHTTPRequestID);

        }
        else if(requestType == Constants::HTTP_REQUEST_DELETE_CREATEACTIONS)//删除新建动作列表
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
//            HandleCancelCollectData(nHTTPRequestID, listActionID);
            HandleCancelMyUploadData(nHTTPRequestID, nActionID);
        }
        //Action preview image 请求
        else if(requestType == Constants::HTTP_REQUEST_PREVIEW_IMAGE)
        {
            int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
            if(m_mapPreviewImageData.contains(nActionID))
            {
                m_mapPreviewImageData[nActionID].append(data);
            }
            else
            {
                m_mapPreviewImageData.insert(nActionID, data);
            }

            HandlePrevewImageData(nHTTPRequestID, nActionID);
        }
        else if(requestType == Constants::HTTP_REQUEST_ADD_DOWDLOAD_COUNT)
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
            HandleAddDownLoad(nHTTPRequestID, nActionID);
        }
        else if (requestType == Constants::HTTP_REQUEST_RECOMMEND_LIST)  // 查询显示在主界面flow中推荐的动作
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            handleRecommendListData(nHTTPRequestID);
        }
        else if (requestType == Constants::HTTP_REQUEST_RECOMMEND_IMAGE) // 推荐主题图片
        {
            if (bytesReceived == bytesTotal)
            {
                int nRecommendID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRecommendID)).toInt();
                saveLocalRecommendImage(nRecommendID, data);
            }
        }
        else if (requestType == Constants::HTTP_REQUEST_BUSINESS_PROMOTE_DATA)  // 活动推广数据
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            handleBusinessPromoteData(nHTTPRequestID);
        }
        else if (requestType == Constants::HTTP_REQUEST_RANK_ACTION_DATA) // 排行榜动作数据
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            int nSchemeID =  reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPSchemeID)).toInt();
            handleRankActionData(nHTTPRequestID, nSchemeID);
        }
        else if (requestType == Constants::HTTP_REQUEST_USER_IMAGE) // 用户头像
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            int nPromoteDataId =  reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPSchemeID)).toInt();
            QUrl urlImage = reply->request().url();
            saveLocalUserImage(nPromoteDataId, urlImage.toString(), data);
        }
        else if (requestType == Constants::HTTP_REQUEST_PROMOTE_IMAGE) // 推广图片
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            int nPromoteDataId =  reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPSchemeID)).toInt();
            int nImageTypeId =  reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPTypeID)).toInt();
            saveLocalPromoteImage(nPromoteDataId, data, (Constants::emActivityImageType) nImageTypeId);
        }
        else if (requestType == Constants::HTTP_REQUEST_ACTION_COMMENT) // 动作评论数据
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            handleActionCommentData(nHTTPRequestID);
        }
        else if (requestType == Constants::HTTP_REQUEST_COMMENT_REPLIER_ICON) // 评论者图标
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

           QUrl urlImage = reply->request().url();
            saveLocalCommentReplierIcon(nHTTPRequestID, urlImage.toString());
        }
        else if (requestType == Constants::HTTP_REQUEST_DOWNLOAD_IMAGE)  // 保存下载图片
        {
            if(m_mapRecivedData.contains(nHTTPRequestID))
            {
                m_mapRecivedData[nHTTPRequestID].append(data);
            }
            else
            {
                m_mapRecivedData.insert(nHTTPRequestID, data);
            }

            QFileInfo fileInfo(reply->request().url().toString());
            QString strDownloadPath = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPDownloadFilePath)).toString();
            QString strFilePath = CConfigs::getFilePath(strDownloadPath, fileInfo.fileName());
            saveImageFile(nHTTPRequestID, strFilePath);
        }
    }
}

/************************************
* 名称: OnInit
* 功能: 初始化槽函数
* 返回:   void
* 时间:   2016/04/07
* 作者:   hels
************************************/
void HttpTransfer::OnInit()
{
    m_pNetManager = new QNetworkAccessManager();
}

/**************************************************************************
* 函数名: OnUploadProgress
* 功能: 上传进度处理槽函数
* 参数:
*    @[in ] bytesReceived: 已经发送的字节数
*    @[in ] bytesTotal: 总共字节数
* 返回值: void
* 时间: 2016/05/27 13:20
* 作者: ZDJ
*/
void HttpTransfer::OnUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply && bytesSent != 0)
    {
        int nRequestID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestID)).toInt();
        qint64 nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toLongLong();
        const QString sRequestType = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestType)).toByteArray();
//        if(bytesSent >= bytesTotal)
//        {
//            //发送完成
//            //HandleUploadFile(nRequestID, nActionID);
//        }
//        else
//        {
            //正在发送
            emit SigUploadProgress(sRequestType, nRequestID, nActionID, bytesSent, bytesTotal);
        //}
    }
}

#if 0
/************************************
* 名称: OnPostUploadFile
* 功能: 请求上传文件槽
* 参数:
*    @[in ] sRequestType: 请求类型字符串
*    @[in ] nNewRequestID: 新建的请求ID
*    @[in ] sServerURL:  请求资源地址
*    @[in ] sFileName:  文件名称
*    @[in ] bytesFileData:  文件数据
*    @[in ] nActionID:  动作ID
*    @[in ] nUserID:  用户ID
* 返回: void
* 时间: 2016/05/27 19:27
* 作者: ZDJ
************************************/
void HttpTransfer::OnPostUploadFile(const QString& sRequestType, int nNewRequestID, const QString& sServerURL, const QString& sFileName, QByteArray bytesFileData, qint64 nActionID, int nUserID)
{
    QString sActIdParam = QString::number(nActionID); //动作ID参数
    QString sUserIdParam = QString::number(nUserID);  //用户ID参数

    QNetworkRequest request;
    request.setUrl(QUrl(sServerURL));

    //创建boundary
    QString crlf = "\r\n";
    qsrand(QDateTime::currentDateTime().toTime_t());
    QString boundary = "---------------------------" + QUuid::createUuid().toString();
    QString contentType = "multipart/form-data;boundary=" + boundary;

    QString beginBoundary = "--" + boundary + crlf;
    QString tempBoundary = crlf + beginBoundary;
    QString endBoundary = crlf + "--" + boundary + "--" + crlf; //数据结束分隔字符串
    /**********************************/

    QByteArray bytesTemp;
    bytesTemp.append(beginBoundary.toLatin1());

    bytesTemp.append(QString("Content-Disposition:form-data;name=\"actionOriginalId\"" + crlf).toLatin1());
    bytesTemp.append(QString("Content-Transfer-Encoding: 8bit" + crlf).toLatin1());
    bytesTemp.append(crlf.toLatin1());
    bytesTemp.append(sActIdParam.toUtf8());

    bytesTemp.append(tempBoundary.toLatin1());

    bytesTemp.append(QString("Content-Disposition:form-data;name=\"actionUserId\"" + crlf).toLatin1());
    bytesTemp.append(QString("Content-Transfer-Encoding: 8bit" + crlf).toLatin1());
    bytesTemp.append(crlf.toLatin1());
    bytesTemp.append(sUserIdParam.toUtf8());

    bytesTemp.append(tempBoundary.toLatin1());

    bytesTemp.append(QString("Content-Disposition:form-data;name=\"file\";filename=\""+ sFileName + "\"" + crlf).toLatin1());
    bytesTemp.append(QString("Content-Transfer-Encoding: 8bit" + crlf).toLatin1());
    bytesTemp.append(crlf.toLatin1());
    bytesTemp.append(bytesFileData);

    bytesTemp.append(endBoundary.toLatin1());

    m_mapParamData.insert(nNewRequestID, bytesTemp);

    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType.toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(m_mapParamData[nNewRequestID].length()).toString());
    request.setRawHeader(QByteArray("Connection"),QByteArray("Keep-Alive"));

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), sRequestType);//设置类型
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID), nActionID);//设置类型

    m_bytesResponse.clear();

    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::uploadProgress,
            this, &HttpTransfer::OnUploadProgress);
    connect(reply, &QNetworkReply::readyRead,
            this, &HttpTransfer::readyRead);

    m_mapReplyActLst.insert(nNewRequestID, reply);
}
#endif

/************************************
* 名称: recoverRecommendActionDataFromJsonString
* 功能: 从jason字符串中恢复基本动作数据
* 参数:
*    @[in ] arrayData: 新建的请求ID
*    @[out ] actItem:  基本动作数据
* 返回: void
* 时间: 2016/06/04 11:16
* 作者: hwx
*/
void HttpTransfer::recoverRecommendActionDataFromJsonString(const QJsonObject &JSonobj, int nRequestID)
{
        // 获取models字段
        if (!JSonobj.contains(NET_MODELS))
        {
            return;
        }

        // models下是数组格式
        QJsonValue valueModels = JSonobj.value(NET_MODELS);
        if(!valueModels.isArray())
        {
            return;
        }

        QJsonArray arrayRecomendImages = valueModels.toArray();
        for (int iRecommendImageID = 0; iRecommendImageID < arrayRecomendImages.size(); iRecommendImageID++)
        {
            QJsonValue valueRecommend = arrayRecomendImages.at(iRecommendImageID);

            // 格式是Object
            if (!valueRecommend.isObject())
            {
                continue;
            }

            QJsonObject objRecommend = valueRecommend.toObject();
            if (!objRecommend.contains(NET_SUBJECT_RECOMMEND_IMAGE))
            {
                continue;
            }
            
            // 获取recommendID
            int nRecomendID = -1;
            if (objRecommend.contains(NET_SUBJECT_RECOMMEND_ID))
            {
                nRecomendID = objRecommend.value(NET_SUBJECT_RECOMMEND_ID).toInt();
            }

            // recommendOrder
            int nRecommendOrder = -1;
            if (objRecommend.contains(NET_SUBJECT_RECOMMEND_ORDER))
            {
                nRecommendOrder = objRecommend.value(NET_SUBJECT_RECOMMEND_ORDER).toInt();
            }

            // 获取recommendType
            int nRecommendType = -1;
            if (objRecommend.contains(NET_SUBJECT_RECOMMEND_TYPE))
            {
                nRecommendType = objRecommend.value(NET_SUBJECT_RECOMMEND_TYPE).toInt();
            }

            // recommendForwardType
            int nRecommendForwardType = -1;
            if (objRecommend.contains(NET_SUBJECT_RECOMMEND_FORWARD_TYPE))
            {
                nRecommendForwardType = objRecommend.value(NET_SUBJECT_RECOMMEND_FORWARD_TYPE).toInt();
            }

            // 获取推荐动作图片
            QString strRecommendImage;
            if (objRecommend.contains(NET_SUBJECT_RECOMMEND_IMAGE))
            {
                strRecommendImage = objRecommend.value(NET_SUBJECT_RECOMMEND_IMAGE).toString();
            }
            
            RecommendActItemData *pRecommendItem = new RecommendActItemData;
            pRecommendItem->m_nRecommendID = nRecomendID;
            pRecommendItem->m_nRecommendType = nRecommendType;
            pRecommendItem->m_nRecomendOrder = nRecommendOrder;
            pRecommendItem->m_nRecommendForwardType = nRecommendForwardType;
            pRecommendItem->m_strRecommendImage = strRecommendImage;

            m_mapRecommendItemList.insert(nRecomendID, pRecommendItem);

            // 下载推荐主题图片
            downloadRecommendSubjectImage(nRecomendID);
            
            // 解析动作数据
            if (!objRecommend.contains(NET_ACTION_CLICK_FORWARD))
            {
                continue;
            }

            QJsonValue valueActions = objRecommend.value(NET_ACTION_CLICK_FORWARD);
            if (!valueActions.isArray())
            {
                continue;
            }

            QJsonArray arrayActions = valueActions.toArray();

            // 获取跳转动作信息
            for(int iRecommendActionID = 0; iRecommendActionID < arrayActions.size(); iRecommendActionID++)
            {
                QJsonValue valueAction = arrayActions.at(iRecommendActionID);
                if(!valueAction.isObject())
                {
                    continue;
                }                

                QJsonObject objAction = valueAction.toObject();
                ActItemData *pActItem = new ActItemData();
                getActItemData(objAction, *pActItem);

                pRecommendItem->addItem(pActItem);

                // 保存到缓存（会调用拷贝构造函数进行深拷贝)
                m_mapActItem.insert(pActItem->m_nItemID, *pActItem);

                // 处理动作预览图片，如果本地不存在，则下载到本地
                int nRequestID = createUnusedRequestID();
                downLoadPreviewImage(nRequestID, *pActItem);

                // 下载动作详情页动作描述图片
                postDownloadImage(pActItem->m_sImageURL, CConfigs::getLocalDetailedImageDir(), Constants::HTTP_DEFAULT_REQUEST_ID);

            } // end iRecommendActionID

        }  // end for iRecommendImageID
}

/************************************
* 名称: downloadRecommendSubjectImage
* 功能: 处理推荐主题图片，如果本地不存在，则下载到本地
* 参数:
*    @[in] nRecommendID: 推荐主题图片标识
* 返回: void
* 时间: 2016/06/07
* 作者: hwx
*/
void HttpTransfer::downloadRecommendSubjectImage(int nRecommendID)
{
    if (!m_mapRecommendItemList.contains(nRecommendID))
    {
        return;
    }

    RecommendActItemData *pRecommendItem = m_mapRecommendItemList[nRecommendID];
    if (!pRecommendItem)
    {
        return;
    }

    QString strRecommendImagePath = pRecommendItem->m_strRecommendImage;
    if (strRecommendImagePath.isEmpty())
    {
        return;
    }

    QFileInfo fileInfo(strRecommendImagePath);
    QString strFileName =  CConfigs::getLocalPreviewImgPath(fileInfo.fileName());

    // 设置下载到本地的推荐动作的图片路径
    // 不能在下载完成后后设置，因为图片可能已经 存在，不会下载
    pRecommendItem->m_strRecommendImage = strFileName;

    // 如果图片不存在则下载图片
    if (!QFile::exists(strFileName))
    {
         QString previewImageDir = CConfigs::instance()->getLocalPreviewImgDir();
        QMap<int, QVariant> mapAttribute;
        mapAttribute.insert(Constants::HTTPRecommendID, qVariantFromValue((int)pRecommendItem->m_nRecommendID));
        mapAttribute.insert(Constants::HTTPRequestType, Constants::HTTP_REQUEST_RECOMMEND_IMAGE);
        requestNetworkImage(strRecommendImagePath, previewImageDir, mapAttribute);
    }
//    else
//    {
//        // 本地图片如果已经存在，此时可以发送信号显示推荐的图片
        emit sigAddRecommendAction(pRecommendItem);
//    }

//        QNetworkRequest requestPreImg;
//        QUrl url(strRecommendImagePath);
//        requestPreImg.setUrl(url);
//        requestPreImg.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRecommendID), qVariantFromValue((int)pRecommendItem->m_nRecommendID)); // 设置推荐动作的ID
//        requestPreImg.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_RECOMMEND_IMAGE);//设置类型
//        QNetworkReply* reply = m_pNetManager->get(requestPreImg);
//        connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
//        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
//                this, &HttpTransfer::OnError);
//        connect(reply, &QNetworkReply::downloadProgress,
//                this, &HttpTransfer::OnDownloadProgress);
}

/************************************
* 名称: downLoadPreviewImage
* 功能: 处理推荐动作图片，如果本地不存在，则下载到本地
* 参数:[in] requestID 请求id
* 参数:[in] actionItem: 动作信息
* 返回: void
* 时间: 2016/07/19
* 作者: asu
*/
void HttpTransfer::downLoadPreviewImage(int requestID, ActItemData &actionItem)
{
    int nActID = actionItem.m_nItemID;
    QString strUrl = actionItem.m_sImageHeadUrl;

    QFileInfo fileInfo(strUrl);
    QString previewImageDir = CConfigs::instance()->getLocalPreviewImgDir();
    QString strLocalImgFielName = CConfigs::instance()->getFilePath(previewImageDir, fileInfo.fileName());
    QFile localFile(strLocalImgFielName);
    actionItem.m_sImageHeadUrl = strLocalImgFielName;
    if (localFile.exists())
    {
        return;
    }

    QMap<int, QVariant> mapAttribute;
    mapAttribute.insert(Constants::HTTPRequestID, qVariantFromValue((int)requestID));
     mapAttribute.insert(Constants::HTTPActionID, qVariantFromValue((int)nActID));
    mapAttribute.insert(Constants::HTTPRequestType, Constants::HTTP_REQUEST_PREVIEW_IMAGE);
    requestNetworkImage(strUrl, previewImageDir, mapAttribute);

//    //完成ActitemData的获取后，需要单独下载预览图
//    //不存在则下载
//    QFileInfo fileInfo(imageHeadUrlNet);
//    QString previewImageDir = CConfigs::instance()->getLocalPreviewImgDir();
//    QString strLocalPreviewImgFielName = CConfigs::instance()->getFilePath(previewImageDir, fileInfo.fileName());

//    QFile localFile(strLocalPreviewImgFielName);
//    if (!localFile.exists())
//    {
//        QMap<int, QVariant> mapAttribute;
//        mapAttribute.insert(Constants::HTTPRequestID, qVariantFromValue((int)requestID));
//         mapAttribute.insert(Constants::HTTPActionID, qVariantFromValue((int)nActID));
//        mapAttribute.insert(Constants::HTTPRequestType, Constants::HTTP_REQUEST_PREVIEW_IMAGE);
//        downloadImage(imageHeadUrlNet, mapAttribute);

//        QNetworkRequest requestPreImg;
//        QUrl url(imageHeadUrlNet);
//        requestPreImg.setUrl(url);

//        requestPreImg.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)requestID));//设置ID
//        requestPreImg.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID),qVariantFromValue((int)nActID));//设置ID
//        requestPreImg.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_PREVIEW_IMAGE);//设置类型

//        QNetworkReply* reply = m_pNetManager->get(requestPreImg);
//        connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
//        connect(reply,
//                static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
//                this,
//                &HttpTransfer::OnError);
//        connect(reply,
//                &QNetworkReply::downloadProgress,
//                this,
//                &HttpTransfer::OnDownloadProgress);
    //    }
}

/************************************
* 名称: downloadUserImage
* 功能: 下载用户头像
* 参数: @[in] actionItem: 动作数据
* 参数: @[in] nDataId: 推广活动数据id
* 返回: void
* 时间: 2016/08/30
* 作者: hwx
*/
void HttpTransfer::downloadUserImage(int nDataId, ActItemData &actionItem)
{
    if (!m_mapBusinessPromoteItemList.contains(nDataId))
    {
        return;
    }

    QString strUrl = actionItem.m_sUserImageUrl;
    if (strUrl.isEmpty())
    {
        return;
    }

    BusinessPromoteData *pPromoteItem = m_mapBusinessPromoteItemList[nDataId];
    if (!pPromoteItem)
    {
        return;
    }

    QString previewImageDir = CConfigs::instance()->getLocalPreviewImgDir();
    QMap<int, QVariant> mapAttribute;
    int nRequestID = createUnusedRequestID();
    mapAttribute.insert(Constants::HTTPSchemeID, qVariantFromValue((int)nDataId));
    mapAttribute.insert(Constants::HTTPRequestID, qVariantFromValue((int)nRequestID));
    mapAttribute.insert(Constants::HTTPRequestType, Constants::HTTP_REQUEST_USER_IMAGE);

    // 设置本地文件路径，覆盖原来url
    QFileInfo fileInfo(strUrl);
    QString strLocalImgFielName = CConfigs::instance()->getFilePath(previewImageDir, fileInfo.fileName());
    actionItem.m_sUserImageUrl = strLocalImgFielName;

    requestNetworkImage(strUrl, previewImageDir, mapAttribute);

//    // 如果本地图片已经存在，需要更新界面
//    emit sigOnUpdatePromoteItem(pPromoteItem);
}

/************************************
* 名称: downloadBusinessPromoteImage
* 功能: 下载运营活动图片
* 参数: @[in] nItemId: 运营活动id
* 参数: @[in] strUrl: 图片url
* 参数: @[in] eType: 图片类型
* 返回: void
* 时间: 2016/08/30
* 作者: hwx
*/
void HttpTransfer::downloadBusinessPromoteImage(int nItemId, const QString &strUrl, Constants::emActivityImageType eType)
{
    if (!m_mapBusinessPromoteItemList.contains(nItemId) || strUrl.isEmpty())
    {
        return;
    }

    BusinessPromoteData *pPromoteItem = m_mapBusinessPromoteItemList[nItemId];
    if (!pPromoteItem)
    {
        return;
    }

    QString previewImageDir = CConfigs::instance()->getLocalPreviewImgDir();
    QMap<int, QVariant> mapAttribute;
    int nRequestID = createUnusedRequestID();
    mapAttribute.insert(Constants::HTTPSchemeID, qVariantFromValue((int)nItemId));
    mapAttribute.insert(Constants::HTTPRequestID, qVariantFromValue((int)nRequestID));
    mapAttribute.insert(Constants::HTTPTypeID, qVariantFromValue((int)eType));
    mapAttribute.insert(Constants::HTTPRequestType, Constants::HTTP_REQUEST_PROMOTE_IMAGE);

    // 设置本地文件路径，覆盖原来url
    QFileInfo fileInfo(strUrl);
    QString strLocalImgFielName = CConfigs::instance()->getFilePath(previewImageDir, fileInfo.fileName());

    if (eType == Constants::eTypeCoverImage)
    {
        pPromoteItem->setCoverImageUrl(strLocalImgFielName);
    }
    else if (eType == Constants::eTypeDetailedImage)
    {
        pPromoteItem->setDetailedImageUrl(strLocalImgFielName);
    }

    requestNetworkImage(strUrl, previewImageDir, mapAttribute);

    // 本地封面图片如果已经存在，需要更新
    if (QFile::exists(strLocalImgFielName) && (eType == Constants::eTypeCoverImage))
    {
        emit sigOnAddPromoteItem(pPromoteItem);
    }
}

/************************************
* 名称: requestNetworkImage
* 功能: 通用下载图片函数，本地路径为previewImage文件夹
* 参数:
*    @[in] sUrl: 网络图片Url
*    @[in] strDownloadPath: 下载图片保存的路径
*    @[in] mapAttributes: 请求属性
* 返回: void
* 时间: 2016/08/30
* 作者: hwx
*/
void HttpTransfer::requestNetworkImage(const QString &sUrl, const QString &strDownloadPath, QMap<int, QVariant> &mapAttributes)
{
    if (sUrl.isEmpty())
    {
        return;
    }

    QFileInfo fileInfo(sUrl);
    QString strLocalImgFielName = CConfigs::instance()->getFilePath(strDownloadPath, fileInfo.fileName());
    QFile localFile(strLocalImgFielName);
    if (localFile.exists())
    {
        return;
    }

    QNetworkRequest request;
    QUrl url(sUrl);
    request.setUrl(url);

    QMap<int, QVariant>::iterator it;
    for (it = mapAttributes.begin(); it != mapAttributes.end(); it++)
    {
        request.setAttribute(QNetworkRequest::Attribute(it.key()), it.value());// 设置属性值
    }

    QNetworkReply* reply = m_pNetManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress, this, &HttpTransfer::OnDownloadProgress);
}

/************************************
* 名称: getActionItem
* 功能: 根据nRecommendID获取指定nActionID的动作
* 参数:
*    @[in] nRecommendID: 推荐动作ID
*    @[in] nRecommendID: 动作ID
* 返回: void
* 时间: 2016/06/07
* 作者: hwx
*/
ActItemData *HttpTransfer::findActionItem(int nRecommendID, int nActionID)
{
    if (!m_mapRecommendItemList.contains(nRecommendID))
    {
        return NULL;
    }

    QList<ActItemData *> lstActItems = m_mapRecommendItemList[nRecommendID]->m_lstActionItems;
    for (int i = 0; i < lstActItems.size(); i++)
    {
        if (lstActItems[i]->m_nItemID == nActionID)
        {
            return lstActItems[i];
        }
    }

    return NULL;

}


/************************************
* 名称: OnPostUploadData
* 功能: 请求上传数据槽
* 参数:
*    @[in ] sRequestType: 请求类型字符串
*    @[in ] nNewRequestID: 新建的请求ID
*    @[in ] sServerURL:  请求资源地址
*    @[in ] mapData: 上传数据
*    @[in ] nActionID:  动作ID
*    @[in ] nUserID:  用户ID
* 返回: void
* 时间: 2016/05/27 19:27
* 作者: ZDJ
*/
void HttpTransfer::OnPostUploadData(const QString& sRequestType, int nNewRequestID, const QString& sServerURL, MapStringDataType mapData, qint64 nActionID, int nUserID)
{
    QNetworkRequest request;
    request.setUrl(QUrl(sServerURL));

    //创建boundary
    QString crlf = "\r\n";
    qsrand(QDateTime::currentDateTime().toTime_t());
    QString boundary = "---------------------------" + QUuid::createUuid().toString();
    QString contentType = "multipart/form-data;boundary=" + boundary;

    QString beginBoundary = "--" + boundary + crlf;
    QString tempBoundary = crlf + beginBoundary;
    QString endBoundary = crlf + "--" + boundary + "--" + crlf; //数据结束分隔字符串
    /**********************************/

    QByteArray bytesTemp;
    bytesTemp.append(beginBoundary.toLatin1());

    MapStringDataType::iterator itor = mapData.begin();
    for(int i = 0; itor != mapData.end(); itor++, i++)
    {
        QString sKey = itor.key();
        QByteArray bytesData = itor.value();

        if(sKey != UPLOADDATA_KEYNAME_FILE)
        {
            bytesTemp.append(QString("Content-Disposition:form-data;name=\"%1\"" + crlf).arg(sKey).toLatin1());
            bytesTemp.append(QString("Content-Transfer-Encoding: 8bit" + crlf).toLatin1());
            bytesTemp.append(crlf.toLatin1());
            bytesTemp.append(bytesData);
        }
        else
        {
            char* pBuf = bytesData.data();

            int nFileNameLen = 0;
            memcpy(&nFileNameLen, pBuf, sizeof(int));
            pBuf += sizeof(int);

            QString sFileName = QString::fromUtf8(pBuf, nFileNameLen); //文件名
            pBuf += nFileNameLen;

            int nFileDataLen = 0;
            memcpy(&nFileDataLen, pBuf, sizeof(int));
            pBuf += sizeof(int);

            char* pFileData = pBuf; //文件数据
            QByteArray bytesFileData = QByteArray::fromRawData(pFileData, nFileDataLen);

            bytesTemp.append(QString("Content-Disposition:form-data;name=\"file\";filename=\""+ sFileName + "\"" + crlf).toLatin1());
            bytesTemp.append(QString("Content-Transfer-Encoding: 8bit" + crlf).toLatin1());
            bytesTemp.append(crlf.toLatin1());
            bytesTemp.append(bytesFileData);
        }

        if(i < mapData.size() - 1)
            bytesTemp.append(tempBoundary.toLatin1());
    }

    bytesTemp.append(endBoundary.toLatin1());

    m_mapParamData.insert(nNewRequestID, bytesTemp);

    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType.toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(m_mapParamData[nNewRequestID].length()).toString());
    request.setRawHeader(QByteArray("Connection"),QByteArray("Keep-Alive"));

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), sRequestType);//设置类型
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID), nActionID);//设置类型

    resetResp();

    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::uploadProgress,
            this, &HttpTransfer::OnUploadProgress);
    connect(reply, &QNetworkReply::readyRead,
            this, &HttpTransfer::readyRead);

    m_mapReplyActLst.insert(nNewRequestID, reply);
}


void HttpTransfer::connectDownloadReply(QNetworkReply *reply)
{
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
}

void HttpTransfer::resetResp()
{
    m_bytesResponse.clear();
}

int HttpTransfer::createUnusedRequestID()
{
    QMutexLocker lock(&m_mutex);
    ++m_nRequestID;
    return m_nRequestID;
}

/************************************
* 名称: connectDoloadReply
* 功能: connect reply single to httptransfer
* 参数:
*    @[in] reply: reply need to connect
* 返回: void
* 时间: 2016/06/08
* 作者: asu
*/
void HttpTransfer::connectUploadReply(QNetworkReply *reply)
{
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::uploadProgress,
            this, &HttpTransfer::OnUploadProgress);
}

/************************************
* 名称: OnPostQueryCreateActions
* 功能: 查询我的新建动作槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]serverURL 请求资源地
* 参数: [in]data 请求参数数据
* 返回:   void
* 时间:   2016/06/08
* 作者:   Zhouzp
************************************/
void HttpTransfer::OnPostQueryCreateActions(int nNewRequestID, const QString &serverURL, const QString &data)
{
    m_mapParamData.insert(nNewRequestID, data.toUtf8());
    QNetworkRequest request;
    QUrl url(serverURL);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_QUERY_CREATEACTIONS);//设置类型

    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
    m_mapReplyActLst.insert(nNewRequestID, reply);
}

/************************************
* 名称: OnPostDeleteCreateActions
* 功能: 删除我的新建动作槽函数
* 参数: [in]nNewRequestID 新建的请求ID
* 参数: [in]serverURL 请求资源地，
* 参数: [in]data 请求参数数据
* 参数: [in]nActionID 要收藏的Action ID列表
* 返回:   void
* 时间:   2016/06/08
* 作者:   Zhouzp
************************************/
void HttpTransfer::OnPostDeleteCreateActions(int nNewRequestID, const QString &serverURL, const QString &data, int nActionID)
{
    m_mapParamData.insert(nNewRequestID, data.toUtf8());
    QNetworkRequest request;
    QUrl url(serverURL);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置请求ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID),qVariantFromValue((int)nActionID));//设置ID
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestType), Constants::HTTP_REQUEST_DELETE_CREATEACTIONS);//设置类型
    QNetworkReply* reply = m_pNetManager->post(request, m_mapParamData[nNewRequestID]);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
}

//数据应答槽
void HttpTransfer::readyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
//        int nRequestID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestID)).toInt();
//        qint64 nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toLongLong();
//        const QString sRequestType = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestType)).toByteArray();

//        //上传动作的应答
//        emit sigUploadActionResponse(sRequestType, nRequestID, nActionID, reply->readAll());
        m_bytesResponse.append(reply->readAll());
    }
}
