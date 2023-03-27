/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：httptransfer.h
* 创建时间：2016/03/24
* 文件标识：
* 文件摘要：http传输类，完成http指令请求和数据的获取
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/24
* 版本摘要：
*/
#ifndef HTTPTRANSFER_H
#define HTTPTRANSFER_H
#include "UBXPublic.h"
#include "ActUpdateMangr.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMutexLocker>
#include <QMutex>
#include <QThread>
#include <QMap>
#include <QVector>

#include "datamodel/businesspromotedata.h"
#include "ubxactionlibconstants.h"
#include "datamodel/usercommentitem.h"

#define UPLOADDATA_KEYNAME_FILE   "file"   //上传文件数据的键名

//上传文件数据结构
typedef struct UPLOADFILEDATA
{
    int         nFileNameLen;   //文件名长度
    QString     sFileName;      //文件名
    qint64      nFileDataLen;   //文件数据长度
    QByteArray  bytesFileData;  //文件数据
}UPLOADFILEDATA;

class HttpTransfer : public QObject
{
    Q_OBJECT
public:
    HttpTransfer();
    ~HttpTransfer();

public:
    /************************************
    * 名称: stopThread
    * 功能: 退出线程
    * 参数:
    * 返回:
    * 时间:   2017/04/06
    * 作者:   hwx
    ************************************/
    void stopThread();

public:
    /************************************
    * 名称: getActItemData
    * 功能: 分析ItemData
    * 参数: [in]obj，
    * 参数: [out]actItem
    * 返回:   bool  true is ok
    * 时间:   2016/07/13
    * 作者:   asu
    ************************************/
    static bool getActItemData(const QJsonObject& obj, ActItemData& actItem);


    /************************************
    * 名称: postQueryActList
    * 功能: post http请求，内部封装了多线程处理
    * 参数: [in]serverURL 请求资源地，
    * 参数: [in]data 请求参数数据
    * 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
    * 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间:   2016/03/24
    * 作者:   hels
    ************************************/
    int postQueryActList(const QString& serverURL, const QString& data, int nOldRequestID);

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
    int postQueryRecommendList(const QString &serverURL, const QString &data, int nOldRequestID);

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
    int postQueryBusinessPromoteData(const QString &sServerURl, const QString &sInParam, int nOldRequestID);

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
    int postQueryRankActionData(const QString &sServerURl, const QString &sInParam, int nOldRequestID, int nSchemeID);

    /************************************
    * 名称: postQueryActionComment
    * 功能: post http请求动作评论数据，内部封装了多线程处理
    * 参数: [in]strServerUrl 请求资源地，
    * 参数: [in]strInParam 请求参数数据
    * 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
    * 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间:   2016/09/19
    * 作者:   hwx
    ************************************/
    int postQueryActionComment(const QString &strServerUrl, const QString &strInParam, int nOldRequestID);
    
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
    int postAddActionComment(const QString &strServerUrl, const QString &strInParam, int nOldRequestId);
    
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
    int postDownloadImage(const QString &strImageUrl, const QString &strDownloadPath, int nOldRequestId);

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
    int postQueryFavorites(const QString& serverURL, const QString& data, int nOldRequestID);

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
    int postCollectAction(const QString& serverURL, const QString& data, int nActionID);

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
    int postCancelCollect(const QString& serverURL, const QString& data, int nActionID);

    /************************************
    * 名称: postPraiseAction
    * 功能: 对动作点赞
    * 参数: [in]serverURL 请求资源地，
    * 参数: [in]data 请求参数数据
    * 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间:   2016/10/22
    * 作者:   hwx
    ************************************/
    int postPraiseAction(const QString& serverURL, const QString& data);

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
    int postQueryIsCollected(const QString& serverURL, const QString& data, int nActionID);

    /************************************
    * 名称: postAddDownLoadCount
    * 功能: 查询指定Action是否已经收藏
    * 参数: [in]serverURL 请求资源地，
    * 参数: [in]data 请求参数数据
    * 参数: [in]nActionID 要查询的Action ID
    * 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    int postAddDownLoadCount(const QString& serverURL, const QString& data, int nActionID);

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
    int postUploadFile(const QString& sRequestType, const QString& sServerURL, const QString& sFileName, const QByteArray &bytesFileData, qint64 nActionID, int nUserID, int nOldRequestID);
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
    int postUploadData(const QString& sRequestType,
                       const QString& sServerURL,
                       const MapStringDataType& mapData,
                       qint64 nActionOriginalId,
                       int nUserID,
                       int nOldRequestID);

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
    int postQueryCreateActions(const QString &serverURL, const QString &data, int nOldRequestID);

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
    int postDeleteCreateActions(const QString &serverURL, const QString &data, int nActionID);

    /************************************************************************************
   * 函数名: downloadReplierIcon
   * 功能: 下载评论者头像
   * 参数:
   *    @[in ] strUrl: 评论者头像URL
   * 返回值:
   * 时间: 2016/09/20 9:48
   * 作者: hwx
   */
    void downloadReplierIcon(const QString &strUrl);

signals:
    /************************************
    * 名称: SigItemDate
    * 功能: 从服务器获取到列表项信息后发送
    * 参数: [in]requestID 请求ID，用于区分是否是自己关注的请求(由postQueryActList返回)
    * 参数: [in]itemData 列表项数据
    * 参数: [in]nTotalPage 一共有多少页
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void SigItemDate(int requestID, ActItemData itemData, int nTotalPage);

    void SigItemUpdate();

    /************************************
    * 名称: sigAddRecommendAction
    * 功能: 通知主界面增加推荐动作
    * 参数: [out]actionItem 动作数据
    * 返回:   void
    * 时间:   2016/06/04
    * 作者:   hwx
    ************************************/
    void sigAddRecommendAction(RecommendActItemData *pRecommendItem);

    /************************************
    * 名称: sigOnAddPromoteItem
    * 功能: 添加推广活动数据
    * 参数: [out]pPromoteItem 推广活动数据
    * 返回:   void
    * 时间:   2016/08/31
    * 作者:   hwx
    ************************************/
    void sigOnAddPromoteItem(BusinessPromoteData *pPromoteItem);

    /************************************
    * 名称: sigOnUpdatePromoteData
    * 功能: 更新推广活动数据，更新排行榜
    * 参数: [out]pPromoteItem 推广活动数据
    * 返回:   void
    * 时间:   2016/08/31
    * 作者:   hwx
    ************************************/
    void sigOnUpdatePromoteData(BusinessPromoteData *pPromoteItem);

    /************************************
    * 名称: sigOnDownloadCommentReplierIconDone
    * 功能: 下载评论者图标完成
    * 返回:   void
    * 时间:   2016/09/20
    * 作者:   hwx
    ************************************/
    void sigOnDownloadCommentReplierIconDone();

    /************************************
    * 名称: sigOnDownloadReplierIcon
    * 功能: 下载评论者头像
    * 参数: [in]strUrl 评论者头像URL
    * 返回:   void
    * 时间:   2016/11/29
    * 作者:   hwx
    ************************************/
    void sigOnDownloadReplierIcon(const QString &strUrl);

    /************************************
    * 名称: sigOndownLoadImageDone
    * 功能: 下载图片完成
    * 返回:   void
    * 时间:   2016/11/18
    * 作者:   hwx
    ************************************/
    void sigOndownLoadImageDone(int nRequestId);

    /************************************
    * 名称: sigOnReceivedActionComment
    * 功能: 通知其他控件获取到动作评论数据
    * 参数: [out]data 动作评论数据
    * 返回:   void
    * 时间:   2016/09/19
    * 作者:   hwx
    ************************************/
    void sigOnReceivedActionComment(const QByteArray &data);

    /************************************
    * 名称: SigTotalPage
    * 功能: 从服务器获取页码总数后发送信号
    * 参数: [in]requestID 请求ID
    * 参数: [in]nTotalPage 页码总数
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void SigTotalPage(int requestID, int nTotalPage);

    /************************************
    * 名称: sigRecordCount
    * 功能: 从服务器获取记录总数后发送信号
    * 参数: [in]requestID 请求ID
    * 参数: [in]nTotalRecordCount   records总数
    * 返回:   void
    * 时间:   2016/06/15
    * 作者:   asu
    ************************************/
    void sigRecordCount(int requestID, int nTotalRecordCount);

    /************************************
    * 名称: SigTotalFavorites
    * 功能: 通知我的收藏总数
    * 参数: [in]requestID 请求ID
    * 参数: [in]nTotals 收藏总数
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void SigTotalFavorites(int requestID, int nTotals);

    /************************************
    * 名称: SigTotalMyUploads
    * 功能: 通知我的收藏总数
    * 参数: [in]requestID 请求ID
    * 参数: [in]nTotals 收藏总数
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void SigTotalMyUploads(int requestID, int nTotals);

    /************************************
    * 名称: SigIsCollectResult
    * 功能: 通知查询是否收藏结果
    * 参数: [in]requestID 请求ID
    * 参数: [in]nActionID 收藏总数
    * 参数: [in]nRet 请求结果
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void SigIsCollectResult(int requestID, int nActionID, int nRet);

    /************************************
    * 名称: sigPraiseActionResult
    * 功能: 返回点赞动作结果
    * 参数: [in]requestID 请求ID
    * 参数: [in]nRet 请求结果
    * 返回:   void
    * 时间:   2016/10/22
    * 作者:   hwx
    ************************************/
    void sigPraiseActionResult(int nRequestId, int nRet);

    /************************************
    * 名称: SigCancelMyUpload
    * 功能: 通知结果
    * 参数: [in]requestID 请求ID
    * 参数: [in]nActionID 收藏总数
    * 参数: [in]nRet 请求结果
    * 返回:   void
    * 作者:   asu
    ************************************/
    void SigCancelMyUpload(int requestID, int nActionID, int nRet);

    void SigIsAddDownLoadResult(int requestID, int nActionID, int nRet);

    /************************************
    * 名称: SigPostQueryActList
    * 功能: 以信号的方式通知http线程进行处理
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地
    * 参数: [in]data 请求参数数据
    * 返回:   void
    * 时间:   2016/03/24
    * 作者:   hels
    ************************************/
    void SigPostQueryActList(int nNewRequestID, const QString& serverURL, const QString& data);

    /************************************
    * 名称: SigPostQueryActList
    * 功能: 以信号的方式通知http线程进行处理
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地
    * 参数: [in]data 请求参数数据
    * 返回:   void
    * 时间:   2016/03/24
    * 作者:   hels
    ************************************/
    void SigPostQueryRecommendList(int nNewRequestID, const QString &strServerURL, const QString &data);

    /************************************
    * 名称: sigPostQueryBusinessPromoteData
    * 功能: 以信号的方式通知http线程进行处理运营活动请求
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地
    * 参数: [in]data 请求参数数据
    * 返回:   void
    * 时间:   2016/08/30
    * 作者:   hwx
    ************************************/
    void sigPostQueryBusinessPromoteData(int nNewRequestID, const QString &strServerURL, const QString &data);

    /************************************
    * 名称: sigPostQueryRankActionData
    * 功能: 以信号的方式通知http线程进行处理排行榜动作数据请求
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]nSchemeID 活动ID
    * 参数: [in]strServerURL 请求资源地
    * 参数: [in]strInParam 请求参数数据
    * 返回:   void
    * 时间:   2016/10/30
    * 作者:   hwx
    ************************************/
    void sigPostQueryRankActionData(int nNewRequestID, int nSchemeID, const QString &strServerURL, const QString &strInParam);

    /************************************
    * 名称: sigPostQueryActionComment
    * 功能: 以信号的方式通知http线程进行动作评论请求
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]strServerURL 请求资源地
    * 参数: [in]strParam 请求参数数据
    * 返回:   void
    * 时间:   2016/09/19
    * 作者:   hwx
    ************************************/
    void sigPostQueryActionComment(int nNewRequestID, const QString &strServerURL, const QString &strParam);

    /************************************
    * 名称: sigPostAddActionComment
    * 功能: 添加动作评论
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]strServerUrl 请求资源地
    * 参数: [in]strParam 请求参数数据
    * 返回:   void
    * 时间:   2016/09/19
    * 作者:   hwx
    ************************************/
    void sigPostAddActionComment(int nNewRequestId, const QString &strServerUrl, const QString &strParam);
    
    void sigPostDownloadImage(int nNewRequestId, const QString &strImageUrl, const QString &strDownloadPath);

    /************************************
    * 名称: SigPostQueryFavorites
    * 功能: 以信号的方式通知http线程进行处理
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地
    * 参数: [in]data 请求参数数据
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void SigPostQueryFavorites(int nNewRequestID, const QString& serverURL, const QString& data);

    /************************************
    * 名称: SigPostCollectAction
    * 功能: 请求收藏动作
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地，
    * 参数: [in]data 请求参数数据
    * 参数: [in]nActionID 要收藏的Action ID
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void SigPostCollectAction(int nNewRequestID,const QString& serverURL, const QString& data, int nActionID);

    /************************************
    * 名称: SigPostCancelCollect
    * 功能: 请求取消收藏
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地，
    * 参数: [in]data 请求参数数据
    * 参数: [in]nActionID 要收藏的Action ID
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void SigPostCancelCollect(int nNewRequestID,const QString& serverURL, const QString& data, int nActionID);

    /************************************
    * 名称: sigOnPraiseAction
    * 功能: 对动作点赞
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]strServerURL 请求资源地，
    * 参数: [in]strData 请求参数数据
    * 返回:   void
    * 时间:   2016/10/22
    * 作者:   hwx
    ************************************/
    void sigOnPraiseAction(int nNewRequestID,const QString& strServerURL, const QString& strData);

    /************************************
    * 名称: SigPostQueryIsCollected
    * 功能: 查询指定Action是否已经收藏
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地，
    * 参数: [in]data 请求参数数据
    * 参数: [in]nActionID 要查询的Action ID
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void SigPostQueryIsCollected(int nNewRequestID, const QString& serverURL, const QString& data, int nActionID);


    void SigPostAddDownLoadCount(int nNewRequestID, const QString& serverURL, const QString& data, int nActionID);

    /************************************
    * 名称: SigInit
    * 功能: 初始化,先运行线程，然后通过信号的方式通知线程初始化,确保网络对象属于线程
    * 返回:   void
    * 时间:   2016/04/07
    * 作者:   hels
    ************************************/
    void SigInit();

#if 0
    /************************************
    * 名称: SigPostUploadFile
    * 功能: 请求上传文件信号
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
    */
    void SigPostUploadFile(const QString& sRequestType, int nNewRequestID, const QString& sServerURL, const QString& sFileName, QByteArray bytesFileData, qint64 nActionID, int nUserID);
#endif
    /************************************
    * 名称: SigPostUploadData
    * 功能: 请求上传数据信号
    * 参数:
    *    @[in ] sRequestType: 请求类型字符串
    *    @[in ] nNewRequestID: 新建的请求ID
    *    @[in ] sServerURL:  请求资源地址
    *    @[in ] mapData:  上传数据
    *    @[in ] nActionID:  动作ID
    *    @[in ] nUserID:  用户ID
    * 返回: void
    * 时间: 2016/05/27 19:27
    * 作者: ZDJ
    */
    void SigPostUploadData(const QString& sRequestType, int nNewRequestID, const QString& sServerURL, MapStringDataType mapData, qint64 nActionID, int nUserID);

    /************************************
    * 名称: SigUploadProgress
    * 功能: 文件上传进度信号
    * 参数:
    *    @[in ] sRequestType: 请求类型字符串
    *    @[in ] nRequestID: 请求ID
    *    @[in ] nActionID: 动作ID
    *    @[in ] bytesUploaded: 已上传的数据长度
    *    @[in ] bytesTotal: 计划上传的数据总长度
    * 返回: void
    * 时间: 2016/05/27 19:27
    * 作者: ZDJ
    */
    void SigUploadProgress(const QString& sRequestType, int nRequestID, qint64 nActionID, qint64 bytesUploaded, qint64 bytesTotal);

    /************************************
    * 名称: SigPostQueryCreateActions
    * 功能: 查询我的新建动作信号
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地
    * 参数: [in]data 请求参数数据
    * 返回:   void
    * 时间:   2016/06/08
    * 作者:   zhouzp
    ************************************/
    void SigPostQueryCreateActions(int nNewRequestID, const QString& serverURL, const QString& data);

    /************************************
    * 名称: SigPostDeleteCreateActions
    * 功能: 删除我的新建动作信号
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地，
    * 参数: [in]data 请求参数数据
    * 参数: [in]nActionID 要收藏的Action ID
    * 返回:   void
    * 时间:   2016/06/08
    * 作者:   zhouzp
    ************************************/
    void SigPostDeleteCreateActions(int nNewRequestID,const QString& serverURL, const QString& data, int nActionID);

    /************************************
    * 名称: sigUploadActionResponse
    * 功能: 上传动作应答信号
    * 参数:
    *    @[in ] sRequestType: 请求类型字符串
    *    @[in ] nRequestID: 请求ID
    *    @[in ] nActionID: 动作ID
    *    @[in ] bytesData: 应答数据
    * 返回: void
    * 时间: 2016/06/15
    * 作者: ZDJ
    */
    void sigUploadActionResponse(const QString& sRequestType, int nRequestID, qint64 nActionID, const QByteArray& bytesData);

    /************************************
    * 名称: sigOnHttpRequestFinished
    * 功能: Http请求结束
    * 参数: [out]requestID 请求的ID，据此判断处理的数据
    *            [out]baResponse 应答数据
    * 返回: void
    * 时间: 2016/09/26
    * 作者: hwx
    */
    void sigOnHttpRequestFinished(int nRequestId, const QByteArray baResponse);

protected:
    /************************************
    * 名称: HandleActionListData
    * 功能: 处理ActionListData
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void HandleActionListData(int requestID);

    /************************************
    * 名称: handleRecommendActionData
    * 功能: 处理推荐动作数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/06/03
    * 作者:   hels
    ************************************/
    void handleRecommendListData(int nRequestID);

    /************************************
    * 名称: handleBusinessPromoteData
    * 功能: 处理运营活动数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/08/30
    * 作者:   hwx
    ************************************/
    void handleBusinessPromoteData(int nRequestID);

    /************************************
    * 名称: handleRankActionData
    * 功能: 处理排行榜动作数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 参数: [in]nSchemeID 活动ID
    * 返回:   void
    * 时间:   2016/10/30
    * 作者:   hwx
    ************************************/
    void handleRankActionData(int nRequestID, int nSchemeID);

    /************************************
    * 名称: handleActionCommentData
    * 功能: 处理动作评论数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/09/19
    * 作者:   hwx
    ************************************/
    void handleActionCommentData(int nRequestID);

    /************************************
    * 名称: HandleFavoritesData
    * 功能: 处理ActionListData
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void HandleFavoritesData(int requestID);

    /************************************
    * 名称: HandleMyUploadsData
    * 功能: 处理ActionListData
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 返回:   void
    * 时间:   20160608
    * 作者:   asu
    ************************************/
    void HandleMyUploadsData(int requestID);

    /************************************
    * 名称: HandleCollectActionData
    * 功能: 处理收藏数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 参数: [in]nActionID Action ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void HandleCollectActionData(int requestID, int nActionID);

    /************************************
    * 名称: HandleCancelCollectData
    * 功能: 处理取消收藏数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 参数: [in]nActionID Action ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void HandleCancelCollectData(int requestID, int nActionID);

    /************************************
    * 名称: HandleCancelMyUploadData
    * 功能: 处理取消收藏数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 参数: [in]nActionID Action ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void HandleCancelMyUploadData(int requestID, int nActionID);

    /************************************
    * 名称: HandleIsCollectData
    * 功能: 处理是否收藏查询数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 参数: [in]nActionID Action ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void HandleIsCollectData(int requestID, int nActionID);

    /************************************
    * 名称: handlePraiseActionResult
    * 功能: 处理点赞动作返回结果
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/10/22
    * 作者:   hwx
    ************************************/
    void handlePraiseActionResult(int nRequestId);

    /************************************
    * 名称: HandleActionDetail
    * 功能: 处理动作详情数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 参数: [in]nActionID Action ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void HandleActionDetail(int requestID, int nActionID);

    /************************************
    * 名称: HandlePrevewImageData
    * 功能: 处理Preview Image Data
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 参数: [in]nActionID Action ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void HandlePrevewImageData(int requestID, int nActionID);

    /************************************
    * 名称: saveLocalRecommendImage
    * 功能: 保存推荐动作的图片
    * 参数：nRecommendID, 推荐ID
    * 参数：data，图片数据
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hwx
    ************************************/
    void saveLocalRecommendImage(int nRecommendID, QByteArray data);

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
    void saveLocalUserImage(int nItemId, const QString &strUrl, const QByteArray &data);

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
    void saveLocalPromoteImage(int nItemId, const QByteArray &data, Constants::emActivityImageType eType);

    /************************************
    * 名称: saveLocalCommentReplierIcon
    * 功能: 保存评论者图标
    * 参数：[in] strUrl: 资源文件路径
    *           [in] nRequestId: 请求id
    * 返回:   void
    * 时间:   2016/09/20
    * 作者:   hwx
    ************************************/
    void saveLocalCommentReplierIcon(int nRequestId, const QString &strUrl);

    /************************************
    * 名称: saveImageFile
    * 功能: 保存网络图片
    * 参数：[in] nRequestId: 请求ID
    *           [in] strFilePath: 保存文件，含路径
    * 返回:   void
    * 时间:   2016/11/18
    * 作者:   hwx
    ************************************/
    void saveImageFile(int nRequestId, const QString &strFilePath);

    /************************************
    * 名称: HandleAddDownLoad
    * 功能: 处理增加下载数量
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/06/08
    * 作者:   Zhouzp
    ************************************/
    void HandleAddDownLoad(int requestID, int nActionID);

    /************************************
    * 名称: HandleCreateActionsData
    * 功能: 处理新建动作列表数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/06/08
    * 作者:   Zhouzp
    ************************************/
    void HandleCreateActionsData(int requestID);

    /************************************
    * 名称: HandleDeleteCreateActionsData
    * 功能: 处理删除新建动作列表数据
    * 参数: [in]requestID 请求的ID，据此判断处理的数据
    * 参数: [in]nActionID Action ID，据此判断处理的数据
    * 返回:   void
    * 时间:   2016/06/08
    * 作者:   Zhouzp
    ************************************/
    void HandleDeleteCreateActionsData(int requestID, int nActionID);

protected slots:
    /************************************
    * 名称: OnPostQueryActList
    * 功能: post http请求槽函数
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地，
    * 参数: [in]data 请求参数数据
    * 返回:
    * 时间:   2016/03/24
    * 作者:   hels
    ************************************/
    void OnPostQueryActList(int nNewRequestID, const QString& serverURL, const QString& data);

    /************************************
    * 名称: onPostQueryRecommendActList
    * 功能: post http请求槽函数
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地，
    * 参数: [in]data 请求参数数据
    * 返回:
    * 时间:   2016/06/03
    * 作者:   hwx
    ************************************/
    void onPostQueryRecommendList(int nNewRequestID, const QString& serverURL, const QString& data);

    /************************************
    * 名称: slotOnPostQueryBusinessPromoteData
    * 功能: post http请求运营活动数据槽函数
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]serverURL 请求资源地址
    * 参数: [in]data 请求参数数据
    * 返回:
    * 时间:   2016/08/30
    * 作者:   hwx
    ************************************/
    void slotOnPostQueryBusinessPromoteData(int nNewRequestID, const QString& serverURL, const QString& data);

    /************************************
    * 名称: slotOnPostQueryRankActionData
    * 功能: post http请求运营活动数据槽函数
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]nSchemeID 活动ID
    * 参数: [in]strServerURL 请求资源地址
    * 参数: [in]strInParame 请求参数数据
    * 返回:
    * 时间:   2016/08/30
    * 作者:   hwx
    ************************************/
    void slotOnPostQueryRankActionData(int nNewRequestID, int nSchemeID, const QString& strServerURL, const QString& strInParame);

    /************************************
    * 名称: slotOnPostQueryActionComment
    * 功能: post http请求动作评论数据槽函数
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]strServerURL 请求资源地址
    * 参数: [in]strParam 请求参数数据
    * 返回:
    * 时间:   2016/09/19
    * 作者:   hwx
    ************************************/
    void slotOnPostQueryActionComment(int nNewRequestID, const QString &strServerURL, const QString &strParam);

    /************************************
    * 名称: slotOnPostAddActionComment
    * 功能: post http请求添加动作评论
    * 参数: [in]nNewRequestID 新建的请求ID
    * 参数: [in]strServerUrl 请求资源地址
    * 参数: [in]strParam 请求参数数据
    * 返回:
    * 时间:   2016/09/20
    * 作者:   hwx
    ************************************/
    void slotOnPostAddActionComment(int nNewRequestId, const QString &strServerUrl, const QString &strParam);
    
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
    void slotOnPostDownloadImage(int nNewRequestId, const QString &strImageUrl, const QString &strDownloadPath);

    /************************************
    * 名称: slotOnDownloadReplierIcon
    * 功能: 下载评论者头像的槽函数
    * 参数: strReplierIconUrl 评论者头像URL
    * 返回:
    * 时间:   2016/11/29
    * 作者:   hwx
    ************************************/
    void slotOnDownloadReplierIcon(const QString &strReplierIconUrl);

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
    void OnPostQueryFavorites(int nNewRequestID, const QString& serverURL, const QString& data);

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
    void OnPostCollectAction(int nNewRequestID,const QString& serverURL, const QString& data, int nActionID);

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
    void slotOnPraiseAction(int nNewRequestID,const QString& strServerURL, const QString& strData);

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
    void OnPostCancelCollect(int nNewRequestID,const QString& serverURL, const QString& data, int nActionID);

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
    void OnPostQueryIsCollected(int nNewRequestID,const QString& serverURL, const QString& data, int nActionID);

    void OnPostAddDownloadCount(int nNewRequestID,const QString& serverURL, const QString& data, int nActionID);

    /************************************
    * 名称: OnFinished
    * 功能: 指令请求完成槽函数
    * 返回:   void
    * 时间:   2016/03/24
    * 作者:   hels
    ************************************/
    void OnFinished();

    /************************************
    * 名称: OnError
    * 功能: 错误处理槽函数
    * 参数: [in]QNetworkReply::NetworkError code请求错误代码
    * 返回:   void
    * 时间:   2016/03/24
    * 作者:   hels
    ************************************/
    void OnError(QNetworkReply::NetworkError code);

    /************************************
    * 名称: OnDownloadProgress
    * 功能: 下载进度处理槽函数
    * 参数: [in]bytesReceived 已经接收到的字节数
    * 参数: [in]bytesTotal 总共字节数
    * 返回:   void
    * 时间:   2016/03/24
    * 作者:   hels
    ************************************/
    void OnDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    /************************************
    * 名称: OnInit
    * 功能: 初始化槽函数
    * 返回:   void
    * 时间:   2016/04/07
    * 作者:   hels
    ************************************/
    void OnInit();

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
    void OnUploadProgress(qint64 bytesSent, qint64 bytesTotal);

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
    */
    void OnPostUploadFile(const QString& sRequestType, int nNewRequestID, const QString& sServerURL, const QString& sFileName, QByteArray bytesFileData, qint64 nActionID, int nUserID);
#endif

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
    void OnPostUploadData(const QString& sRequestType, int nNewRequestID, const QString& sServerURL, MapStringDataType mapData, qint64 nActionID, int nUserID);

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
    void OnPostQueryCreateActions(int nNewRequestID, const QString& serverURL, const QString& data);

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
    void OnPostDeleteCreateActions(int nNewRequestID,const QString& serverURL, const QString& data, int nActionID);

    //数据应答槽
    void readyRead();

private:
    /************************************
    * 名称: recoverRecommendActionDataFromJsonString
    * 功能: 从jason字符串中恢复基本动作数据
    * 参数:
    *    @[in ] arrayData: 新建的请求ID
    *    @[out ] nRequestID:  请求ID
    * 返回: void
    * 时间: 2016/06/04
    * 作者: hwx
    */
    void recoverRecommendActionDataFromJsonString(const QJsonObject &JSonobj, int nRequestID);

    /************************************
    * 名称: downloadRecommendSubjectImage
    * 功能: 处理推荐主题图片，如果本地不存在，则下载到本地
    * 参数:
    *    @[in] nRecommendID: 推荐主题图片标识
    * 返回: void
    * 时间: 2016/06/07
    * 作者: hwx
    */
    void downloadRecommendSubjectImage(int nRecommendID);

    /************************************
    * 名称: downLoadPreviewImage
    * 功能: 处理推荐动作图片，如果本地不存在，则下载到本地
    * 参数:[in] requestID 请求id
    * 参数:[in] actionItem: 动作信息
    * 返回: void
    * 时间: 2016/07/19
    * 作者: asu
    */
    void downLoadPreviewImage(int requestID, ActItemData &actionItem);

    /************************************
    * 名称: downLoadPreviewImage
    * 功能: 处理推荐动作图片，如果本地不存在，则下载到本地
    * 参数:
    *    @[in] imageHeadUrlNet: 动作图片标识
    * 返回: void
    * 时间: 2016/07/19
    * 作者: asu
    */
    void downLoadPreviewImage(int requestID, int nActID, const QString &imageHeadUrlNet);

    /************************************
    * 名称: downloadUserImage
    * 功能: 下载用户头像
    * 参数: @[in] actionItem: 动作数据
    * 参数: @[in] nDataId: 推广活动数据id
    * 返回: void
    * 时间: 2016/08/30
    * 作者: hwx
    */
    void downloadUserImage(int nDataId,  ActItemData &actionItem);

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
    void downloadBusinessPromoteImage(int nItemId, const QString &strUrl, Constants::emActivityImageType eType);

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
    void requestNetworkImage(const QString &sUrl, const QString &strDownloadPath, QMap<int, QVariant> &mapAttributes);

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
    ActItemData *findActionItem(int nRecommendID, int nActionID);

    /************************************
    * 名称: connectUploadReply
    * 功能: connect reply single to httptransfer
    * 参数:
    *    @[in] reply: reply need to connect
    * 返回: void
    * 时间: 2016/06/08
    * 作者: asu
    */
    void connectUploadReply(QNetworkReply *reply);
    void connectDownloadReply(QNetworkReply *reply);

    // 清除应答数据
    void resetResp();

    // 更新、获取请求ID
    int createUnusedRequestID();

private:
    QNetworkAccessManager *m_pNetManager;//必须在线程中new，否则get回来的reply对象会变成在主线程中，跨线程调用会崩溃
    //方式是，先运行线程，然后通过信号的方式通知线程初始化
    QThread m_oThread;//线程对象

    QMutex m_mutex; // 互斥锁
    QAtomicInt  m_nRequestID;//生成请求ID

    //Qt帮助文档对post请求的说明，因此需要保存data数据，以防发生意外，在完成后清空保存的数据
    //data must be open for reading and must remain valid until the finished() signal is emitted for this reply.
    QMap<int, QByteArray> m_mapParamData;//保存请求数据

    QMap<int, QByteArray> m_mapRecivedData;//根据请求ID保存读取到的数据
    QMap<int, ActItemData> m_mapActItem;//根据Action ID保存解析的Action Item
    QMap<int, RecommendActItemData *> m_mapRecommendItemList; // 推荐动作数据
    QMap<int, BusinessPromoteData *> m_mapBusinessPromoteItemList; // 运营活动数据
    QMap<int, QByteArray> m_mapPreviewImageData;//根据Action ID保存预览图数据
    QMap<int, QByteArray> m_mapActionDetailData;//动作详情数据
    QMap<int, int> m_mapTotalPages;//记录对应请求的totalPage数
    QMap<int, QNetworkReply*> m_mapReplyActLst;//Action List请求

    QByteArray   m_bytesResponse;  //应答数据
};

#endif // HTTPTRANSFER_H
