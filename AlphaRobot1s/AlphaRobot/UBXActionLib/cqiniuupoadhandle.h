/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CQiniuUpoadHandle.h
* 创建时间：20160912
* 文件标识：
* 文件摘要：qiniu upload handle
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：
* 版本摘要：
*/


#ifndef CQINIUUPOADHANDLE_H
#define CQINIUUPOADHANDLE_H


#include <QThread>
#include <QObject>
#include <QMap>

#ifdef USEQINIU
extern "C"
{
#include "io.h"
#include "resumable_io.h"
#include "rs.h"
}
#endif

#include "../common/singleton.h"




const QString QINIU_BUCKET_UBT = "ubtech";




class INotify;


class CUploadWork : public QObject
{
    Q_OBJECT

public:
    explicit CUploadWork(const QString &strFilePath, int nUserID, int nRequestID, INotify *pNotifyRecvr);
    ~CUploadWork();

public:

signals:
    void sigUploadFinish(const int nRequestID, bool bOk);

public slots:
    void onDowork();

private:
    QString m_strFilePath;
    int m_nUserID;
    int m_nRequestID;
    INotify *m_pNotifyRecvr;
};



class CQiniuUpoadHandle : public QObject
{    
    Q_OBJECT

public:
    SINGLETON(CQiniuUpoadHandle)

//public:
//    explicit CQiniuUpoadHandle();
//    ~CQiniuUpoadHandle();


public:
    // whether enable upload more file, you have the ask when want to upload file
    bool isEnableUpload();

    //delete task
    void deleteTask(int nRequestID);

    //get request id
    int generateRequestID();

    /************************************
    * 名称: batchDelete
    * 功能: batch delete files
    * 参数:
    *    @[in ] listFilePaths: file paths
    *    @[in ] nUserID:
    * 返回值:
    * 时间: 20160914
    * 作者: asu
    */
    bool batchDelete(const QStringList& listFilePaths, int nUserID);

    /************************************
    * 名称: UploadActionDescVideo
    * 功能: 上传动作描述视频
    * 参数:
    *    @[in ] notifyRecvr: notify Recvr
    *    @[in ] sFilePath: 文件路径
    *    @[in ] nUserID: 用户ID
    *    @[in ] nOldRequestID: 旧请求ID
    * 返回值: 成功返回正整数，出错返回其他值
    * 时间: 2016/06/16
    * 作者: ZDJ
    */
    int uploadFile(const QString& strFilePath, int nUserID, int nOldRequestID, INotify* pNotifyRecvr = NULL);

    /************************************
    * 名称: uploadQiniuFile
    * 功能: 上传文件到七牛服务器
    * 参数:
    *    @[in ] pClient: 七牛上传客户端
    *    @[in ] sBucketName: 会话标识
    *    @[in ] mac: 授权信息
    *    @[in ] sFilePath: 上传文件的本地路径
    * 返回值: 成功返回true，失败返回false
    * 时间: 2016/06/16
    * 作者: ZDJ
    */
    static bool uploadQiniuFile(Qiniu_Client* pClient, const QString &strBucketName, Qiniu_Mac* pMac,
                         const QString &strFilePath, INotify* pNotifyRecvr);


    //get qiniu mac
    static Qiniu_Mac getQiniuMac();

    /************************************
    * 名称: getQiniuDownloadURL
    * 功能: 获取七牛服务器上文件的下载地址
    * 参数:
    *    @[in ] sBucketName: 会话标识
    *    @[in ] mac: 授权信息
    *    @[in ] sFileName: 文件名称，必须为UTF8编码
    *    @[out] sFileUrl: 文件url
    * 返回值: 成功返回true，失败返回false
    * 时间: 2016/06/16
    * 作者: ZDJ
    */
    static bool getQiniuDownloadURL(const QString& sBucketName, Qiniu_Mac* pMac,
                                    const QString& strFileName, QString& strFileUrl);

    /************************************
    * 名称: getBucketName
    * 功能: 获取七牛BucketName
    * 参数:
    *    @[in ] strFilePath: file path
    *    @[in ] nUserID:
    * 返回值:
    * 时间: 20160914
    * 作者: asu
    */
    static QString getBucketName(const QString& strFilePath, int nUserID);

    /************************************
    * 名称: getFileName
    * 功能: 获取filename from BucketName
    * 参数:
    *    @[in ] strBucketName: BucketName
    * 返回值:
    * 时间: 20160914
    * 作者: asu
    */
    static QString getFileName(const QString& strBucketName);

    // 生成上传视频的bucket，与上传动作id（而不是视频文件名称）绑定
    static void createBucketName(quint64 nActionId,  int nUserID, const QString& strFilePath);

signals:    
    void sigUploadFile(const QString& strFilePath, int nUserID, int nOldRequestID, INotify* pNotifyRecvr = NULL);
    void sigUploadFinish(const int nRequestID, bool bOk);
    void sigDowork();

protected slots:

    /************************************
    * 名称: onUploadFile
    * 功能: 上传视频
    * 参数:
    *    @[in ] strFilePath:
    *    @[in ] nUserID: 用户ID
    *    @[in ] nOldRequestID: 旧请求ID
    *    @[in ] pNotifyRecvr:
    * 返回值:
    * 时间: 20160912
    * 作者: asu
    */
    void onUploadFile(const QString& strFilePath, int nUserID, int nRequestID, INotify* pNotifyRecvr = NULL);

    //on upload finish
    void onUploadFinish(const int nRequestID, bool bOk);

private:
    //batchDelete
    static bool batchDelete(Qiniu_Client* client, Qiniu_RS_EntryPath* entries, Qiniu_ItemCount entryCount);

private:
    QMap<int , QThread*> m_mapThread;
    QAtomicInt  m_nRequestID;//生成请求ID

    static int m_nTaskNumUploading;
    static QString m_strBucketName;
};

#endif // CQINIUUPOADHANDLE_H
