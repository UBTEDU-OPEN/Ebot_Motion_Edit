/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：httpdownload.h
* 创建时间：2016/03/31
* 文件标识：
* 文件摘要：动作文件下载类
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/31
* 版本摘要：
*/

#ifndef HTTPDOWNLOAD_H
#define HTTPDOWNLOAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QThread>
#include "ActUpdateMangr.h"

struct ActItemData;

class HttpDownload : public QObject
{
    Q_OBJECT
public:
    //downloadDir动作库本地保存目录
    HttpDownload(const QString& downloadDir);
    ~HttpDownload();

    /************************************
    * 名称: stopThread
    * 功能: 停止线程
    * 参数:
    * 返回:
    * 时间:   2017/04/06
    * 作者:   hwx
    ************************************/
    void stopThread();

    /************************************
    * 名称: DownloadAction
    * 功能: 发送下载动作请求
    * 参数: [in]itemData 下载动作数据信息
    * 返回:   int 返回请求ID，外部监听者可以据此判断是否是自己的请求
    * 时间:   2016/03/31
    * 作者:   hels
    ************************************/
    int DownloadAction(ActItemData itemData);

    /************************************
    * 名称: ResumeDownload
    * 功能: 重新下载
    * 参数: [in]itemData 下载项
    * 返回:   int 返回请求ID
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    int ResumeDownload(ActItemData itemData);

signals:
    /************************************
    * 名称: SigStartDownload
    * 功能: 通知外部开始下载
    * 参数: [in]requestID 对应的请求ID
    * 参数: [in]itemData 下载项
    * 参数: [in]bytesReceived 已经接收到的字节数
    * 参数: [in]bytesTotal 总共的字节数
    * 返回:   void
    * 时间:   2016/04/05
    * 作者:   hels
    ************************************/
    void SigStartDownload(int requestID, ActItemData itemData, qint64 bytesReceived, qint64 bytesTotal);

    /************************************
    * 名称: SigDownloadProgress
    * 功能: 通知外部下载进度
    * 参数: [in]requestID 对应的请求ID
    * 参数: [in]nActionID 下载项的ID
    * 参数: [in]bytesReceived 已经接收到的字节数
    * 参数: [in]bytesTotal 总共的字节数
    * 返回:   void
    * 时间:   2016/04/01
    * 作者:   hels
    ************************************/
    void SigDownloadProgress(int requestID, int nActionID, qint64 bytesReceived, qint64 bytesTotal);

    /************************************
    * 名称: SigFinishedDownload
    * 功能: 下载完成后发送此信号
    * 参数: [in]requestID 请求的ID号
    * 参数: [in]itemData 对应动作列表数据信息
    * 返回:   void
    * 时间:   2016/04/01
    * 作者:   hels
    ************************************/
    void SigFinishedDownload(int requestID, ActItemData itemData);

    /************************************
    * 名称: SigHasPausedDownload
    * 功能: 通知外部已经完成暂停下载
    * 参数: [in]nActionID 暂停的action ID
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void SigHasPausedDownload(int nActionID);

    //以下为本类响应信号，即外部通知此类对象的信号
signals:
    /************************************
    * 名称: SigToPauseDownload
    * 功能: 通知本类要暂停下载
    * 参数: [in]nActionID 指定要暂停的action ID
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void SigToPauseDownload(int nActionID);

    /************************************
    * 名称: SigToResumeDownload
    * 功能: 重新下载
    * 参数: [in]nRequestID 请求ID
    * 参数: [in]itemData 下载项
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void SigToResumeDownload(int nRequestID, ActItemData itemData);

    /************************************
    * 名称: SigDownloadAction
    * 功能: 重新下载
    * 参数: [in]nRequestID 请求ID
    * 参数: [in]itemData 下载项
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void SigDownloadAction(int nRequestID, ActItemData itemData);

    /************************************
    * 名称: SigInit
    * 功能: 初始化,先运行线程，然后通过信号的方式通知线程初始化,确保网络对象属于线程
    * 返回:   void
    * 时间:   2016/04/07
    * 作者:   hels
    ************************************/
    void SigInit();

protected slots:
    /************************************
    * 名称: OnError
    * 功能: 网络错误槽函数
    * 参数: [in]code 错误码
    * 返回:   void
    * 时间:   2016/03/31
    * 作者:   hels
    ************************************/
    void OnError(QNetworkReply::NetworkError code);

    /************************************
    * 名称: OnDownloadProgress
    * 功能: 下载进度处理槽函数
    * 参数: [in]bytesReceived 已经接收到的字节数
    * 参数: [in]bytesTotal 总共的字节数
    * 返回:   void
    * 时间:   2016/03/31
    * 作者:   hels
    ************************************/
    void OnDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    /************************************
    * 名称: OnPauseDownload
    * 功能: 响应暂停下载槽函数
    * 参数: [in]nActionID 指定要暂停的action ID
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void OnPauseDownload(int nActionID);

    /************************************
    * 名称: OnFinished
    * 功能: 指令请求完成槽函数
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void OnFinished();

    /************************************
    * 名称: OnResumeDownload
    * 功能: 重新下载
    * 参数: [in]nRequestID 请求ID
    * 参数: [in]itemData 下载项
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void OnResumeDownload(int nRequestID, ActItemData itemData);

    /************************************
    * 名称: OnDownloadAction
    * 功能: 重新下载
    * 参数: [in]nRequestID 请求ID
    * 参数: [in]itemData 下载项
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void OnDownloadAction(int nRequestID, ActItemData itemData);

    /************************************
    * 名称: OnInit
    * 功能: 初始化槽函数
    * 返回:   void
    * 时间:   2016/04/07
    * 作者:   hels
    ************************************/
    void OnInit();

private:
    QNetworkAccessManager *m_pNetManager; //必须在线程中new，否则get回来的reply对象会变成在主线程中，跨线程调用会崩溃
                                          //方式是，先运行线程，然后通过信号的方式通知线程初始化
    QThread m_oThread;//线程对象，封装在类中方便管理
    QAtomicInt m_nRequestID;//请求ID
    QString m_strDownloadDir;//下载主目录

    QMap<int, ActItemData> m_mapIteDada;//记录ID对应的Item项，方便查找
    QMap<int, QNetworkReply*> m_mapReply;//Action Download请求,方便暂停重启
};

#endif // HTTPDOWNLOAD_H
