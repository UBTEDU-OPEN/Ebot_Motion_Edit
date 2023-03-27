/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：QtHttpOperSync.h
* 创建时间：2015/05/18 8:58
* 文件标识：
* 文件摘要：http同步操作类
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/05/18 8:58
* 版本摘要：
*/
#ifndef CBASEHTTPOPERSYNC_H
#define CBASEHTTPOPERSYNC_H

#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QWidget>

#include "UBXPublic.h"


/**************************************************************************
* 函数类型: FILEDOWNPROCESSNOTIFY
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
typedef void (CALLBACK *FILEDOWNPROCESSNOTIFY)(LPVOID pUser, const QString& sFileName, qint64 nFileTotalSize, qint64 nFileCurSize);


class CBaseHttpOperSync : public QObject
{
    Q_OBJECT

public:
    CBaseHttpOperSync(QWidget *parent = NULL);
    virtual ~CBaseHttpOperSync(void);

        /**************************************************************************
        * 函数名: DownloadFile
        * 功能: 下载远程文件
        * 参数:
    *    @[in ] sURL: 文件的网络下载地址
    *    @[in ] sSavePath: 下载的文件本地保存路径
    *    @[in ] pNotify: 通知回调地址
    *    @[in ] pUser: 通知回调目标
    *    @[out] pszErr: 失败时返回的错误内容
    * 返回值: 成功返回true，失败返回false
        * 时间: 2015/05/14 10:20
        * 作者: ZDJ
        */
    bool DownloadFile(const QString& sURL,
        const QString& sSavePath,
        FILEDOWNPROCESSNOTIFY pNotify = NULL,
        LPVOID pUser = NULL,
        char* pszErr = NULL);

        /**************************************************************************
        * 函数名: ExecuteRequest
        * 功能: 发起请求
        * 参数:
    *    @[in ] eReqType: 请求方法，如GET或POST
    *    @[in ] sURL: 网络地址
    *    @[in ] sParam: 请求参数
    *    @[in ] pNotify: 通知回调地址
    *    @[in ] pUser: 通知回调目标
    *    @[out] pOutData: 返回的数据，返回成功时需要在外调用free释放内存
    *    @[out] nOutDataLen: 返回的数据长度
    *    @[out] pszErr: 失败时返回的错误内容
    * 返回值: 成功返回true，失败返回false
        * 时间: 2015/05/14 10:20
        * 作者: ZDJ
        */
    virtual bool ExecuteRequest(emRequestType eReqType,
        const QString& sURL,
        const char* pszParam,
        FILEDOWNPROCESSNOTIFY pNotify,
        LPVOID pUser,
        byte*& pOutData,
        qint64& nOutDataLen,
        char* pszErr);

        /**************************************************************************
        * 函数名: Cancel
        * 功能: 取消请求
        * 时间: 2015/06/12 10:22
        * 作者: ZDJ
        */
    void Cancel();

protected slots:
    void OnError(QNetworkReply::NetworkError code);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

protected:
    bool                   m_bCancel;     //取消下载标识
    bool                   m_bErr;
    QNetworkAccessManager* m_pNetMgr;   //下载管理器
    QNetworkReply*        m_pNetReply;   //下载应答
    QNetworkRequest*      m_pRequest;    //请求对象
    QEventLoop*           m_pEventLoop; //事件循环
    qint64                 m_nTotalSize;   //数据总长度

};

#endif // CBASEHTTPOPERSYNC_H


