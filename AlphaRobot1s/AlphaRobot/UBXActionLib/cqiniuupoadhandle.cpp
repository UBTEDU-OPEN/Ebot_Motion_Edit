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

#include "cqiniuupoadhandle.h"


#include "../zxLib/inotify.h"
#include "UBXPublic.h"
#include "filedirhandle.h"
#include "alpharobotlog.h"


int nBlkcountForQiniuUploadFile = 0; //
const int QINIU_MAX_UPLOADING_TASK_NUM  = 2;



const int QINIU_ERR_UPLOAD_OK = 200;
const int QINIU_ERR_DELETE_OK = 200;


CUploadWork::CUploadWork(const QString &strFilePath, int nUserID, int nRequestID, INotify *pNotifyRecvr)
{
    m_strFilePath = strFilePath;
    m_nUserID = nUserID;
    m_nRequestID = nRequestID;
    m_pNotifyRecvr = pNotifyRecvr;
}

CUploadWork::~CUploadWork()
{

}

void CUploadWork::onDowork()
{
    bool bOk = false;
    QString sBucketName = CQiniuUpoadHandle::getBucketName(m_strFilePath, m_nUserID);

    Qiniu_Client qiniu_client;
    Qiniu_Mac qiniu_mac = CQiniuUpoadHandle::getQiniuMac();

    // 初始化
    Qiniu_Global_Init(-1);
    Qiniu_Client_InitMacAuth(&qiniu_client, 1024, &qiniu_mac);

    do
    {
        /*上传一个本地文件到服务器*/
        if(CQiniuUpoadHandle::uploadQiniuFile(&qiniu_client, sBucketName, NULL, m_strFilePath, m_pNotifyRecvr))
        {
            bOk = true;
        }

    }while(0);

    // 反初始化
    Qiniu_Client_Cleanup(&qiniu_client);
    Qiniu_Global_Cleanup();

    emit sigUploadFinish(m_nRequestID, bOk);
    AlphaRobotLog::getInstance()->printDebug(QString("CUploadWork::onDowork() %1").arg(QString::number((int)bOk)));

//    return bOk;
}


static int QiniuNotify(void* recvr, int blkIdx, int blkSize, Qiniu_Rio_BlkputRet* ret)
{
    qDebug("nodify: %d, size: %d, off: %d", blkIdx, blkSize, ret->offset);

    if(recvr)
    {
        INotify *pNotify = (INotify*)(recvr);
        if(pNotify)
        {
            pNotify->onQiniuNotify(pNotify, nBlkcountForQiniuUploadFile, blkIdx, blkSize, ret->offset);
        }
    }

    return QINIU_RIO_NOTIFY_OK;
}

static int notifyErr(void* recvr, int blkIdx, int blkSize, Qiniu_Error err)
{
    qDebug("nodify: %d, size: %d, err: %d errmsg: %s", blkIdx, blkSize, err.code, err.message);

    QString error = QString("qiniu notifyErr:");
    error.append(QString("err id: %1\n").arg(QString::number(err.code)));
    error.append(QString("message:") + QString(err.message));
    AlphaRobotLog::getInstance()->printDebug(error);

    return QINIU_RIO_NOTIFY_OK;
}

int CQiniuUpoadHandle::m_nTaskNumUploading = 0;
QString CQiniuUpoadHandle::m_strBucketName = "";
CQiniuUpoadHandle::CQiniuUpoadHandle()
{
    m_nRequestID = -1;
    connect(this, &CQiniuUpoadHandle::sigUploadFile, this, &CQiniuUpoadHandle::onUploadFile);
}

CQiniuUpoadHandle::~CQiniuUpoadHandle()
{

}

// whether enable upload more file, you have the ask when want to upload file
bool CQiniuUpoadHandle::isEnableUpload()
{
    return (m_nTaskNumUploading < QINIU_MAX_UPLOADING_TASK_NUM)? true: false;
}

//deleteTask
void CQiniuUpoadHandle::deleteTask(int nRequestID)
{
   QThread *pThread = m_mapThread.value(nRequestID);
   if(pThread)
   {
       m_mapThread.remove(nRequestID);
       pThread->quit();
       pThread->wait();
       pThread->deleteLater();
       pThread = NULL;
   }
}

//get request id
int CQiniuUpoadHandle::generateRequestID()
{
    ++m_nRequestID;

    return m_nRequestID;
}

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
bool CQiniuUpoadHandle::batchDelete(const QStringList &listFilePaths, int nUserID)
{
    if(listFilePaths.isEmpty())
    {
        return false;
    }

    Qiniu_Mac qiniu_mac = CQiniuUpoadHandle::getQiniuMac();

    Qiniu_Client qiniu_client;
    // 初始化
    Qiniu_Global_Init(-1);
    Qiniu_Client_InitMacAuth(&qiniu_client, 1024, &qiniu_mac);

    const int nEntryCount = listFilePaths.count();
    Qiniu_RS_EntryPath *pEntries = (Qiniu_RS_EntryPath *)calloc(nEntryCount, sizeof(Qiniu_RS_EntryPath));
    for(int index = 0; index < nEntryCount; ++index)
    {
        const QString strFilePath = listFilePaths.at(index);
        QString strBaseBucketName = QString(QINIU_BUCKET_UBT);
        QString strBucketName = CQiniuUpoadHandle::getBucketName(strFilePath, nUserID);
        QString strFileName = CQiniuUpoadHandle::getFileName(strBucketName);

        char* pszBucketName = NULL;
        UnicodeToUtf8(strBaseBucketName, pszBucketName);
        pEntries[index].bucket = pszBucketName;

        char* pszFileName = NULL;
        UnicodeToUtf8(strFileName, pszFileName);
        pEntries[index].key = pszFileName;
    }

    bool bOk = batchDelete(&qiniu_client, pEntries, nEntryCount);
    free(pEntries);

    return bOk;
}

//batchDelete
bool CQiniuUpoadHandle::batchDelete(Qiniu_Client *pClient, Qiniu_RS_EntryPath *pEntries, Qiniu_ItemCount nEntryCount)
{
    if(!pClient || !pEntries || (nEntryCount <= 0))
    {
        return false;
    }

    Qiniu_RS_BatchItemRet* rets = (Qiniu_RS_BatchItemRet*)calloc(nEntryCount, sizeof(Qiniu_RS_BatchItemRet));
    Qiniu_Error err = Qiniu_RS_BatchDelete(pClient, rets, pEntries, nEntryCount);

    int curr = 0;
    while (curr < nEntryCount)
    {
        if (rets[curr].code != QINIU_ERR_DELETE_OK)
        {
            printf("error: %s\n", rets[curr].error);

        }

        curr++;
    }

    free(rets);

    bool bOk = false;
    if (err.code == QINIU_ERR_DELETE_OK)
    {
        bOk = true;
    }

    return bOk;
}


/************************************
* 名称: UploadActionDescVideo
* 功能: 上传动作描述视频
* 参数:
*    @[in ] notifyRecvr: notify Recvr
*    @[in ] sFilePath: 文件路径
*    @[in ] nActionID: 动作ID
*    @[in ] nUserID: 用户ID
*    @[in ] nOldRequestID: 旧请求ID
* 返回值: 成功返回正整数，出错返回其他值
* 时间: 2016/06/16
* 作者: ZDJ
*/
int CQiniuUpoadHandle::uploadFile(const QString &strFilePath, int nUserID, int nOldRequestID, INotify *pNotifyRecvr)
{
    if(m_nTaskNumUploading < 0)
    {
        m_nTaskNumUploading = 0;
    }
    m_nTaskNumUploading++;

    int nNewRequestID = generateRequestID();
    sigUploadFile(strFilePath, nUserID, nNewRequestID, pNotifyRecvr);

    return nNewRequestID;
}

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
void CQiniuUpoadHandle::onUploadFile(const QString &strFilePath, int nUserID, int nRequestID, INotify *pNotifyRecvr)
{
    AlphaRobotLog::getInstance()->printDebug(QString("CQiniuUpoadHandle::onUploadFile: %1").arg(strFilePath));

    QThread *pThreadUpload = new QThread;
    CUploadWork *pUploadWork = new CUploadWork(strFilePath, nUserID, nRequestID, pNotifyRecvr);
    pUploadWork->moveToThread(pThreadUpload);

    deleteTask(nRequestID);
    m_mapThread.insert(nRequestID, pThreadUpload);

    connect(this, &CQiniuUpoadHandle::sigDowork, pUploadWork, &CUploadWork::onDowork);
    connect(pUploadWork, &CUploadWork::sigUploadFinish, this, &CQiniuUpoadHandle::sigUploadFinish);
    connect(pUploadWork, &CUploadWork::sigUploadFinish, this, &CQiniuUpoadHandle::onUploadFinish);

    pThreadUpload->start();
    emit sigDowork();
}

void CQiniuUpoadHandle::onUploadFinish(const int nRequestID, bool /*bOk*/)
{
    if(m_nTaskNumUploading > 0)
    {
        m_nTaskNumUploading--;
    }

    deleteTask(nRequestID);
}


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
bool CQiniuUpoadHandle::uploadQiniuFile(Qiniu_Client *pClient, const QString &strBucketName,
                                        Qiniu_Mac *pMac, const QString &strFilePath, INotify *pNotifyRecvr)
{
    if(!pClient)
    {
        return false;
    }

    bool bRet = false;

    char* pszUploadName = NULL;
    char* pszBucketName = NULL;
    char* pszLocalFilePath = NULL;

    do
    {
 //        QString sFileName = CFileDirHandle::getFileName(strFilePath);
 //        UnicodeToUtf8(sFileName, pszUploadName);
         QString sFileName = CQiniuUpoadHandle::getFileName(strBucketName);
        UnicodeToUtf8(sFileName, pszUploadName);

        if(!pszUploadName)
        {
            AlphaRobotLog::getInstance()->printError(QString("CQiniuUpoadHandle::uploadQiniuFile:pszUploadName is empty"));
            break;
        }

 //        UnicodeToAnsi(strBucketName, pszBucketName);
         UnicodeToUtf8(strBucketName, pszBucketName);
        if(!pszBucketName)
        {
            AlphaRobotLog::getInstance()->printError(QString("CQiniuUpoadHandle::uploadQiniuFile:pszBucketName is empty"));
            break;
        }

        QString sLocalFilePath = QDir::toNativeSeparators(strFilePath);
#ifdef Q_OS_MAC
        UnicodeToUtf8(sLocalFilePath, pszLocalFilePath);
#elif defined(Q_OS_WIN)
        UnicodeToAnsi(sLocalFilePath, pszLocalFilePath);
#endif

        if(!pszLocalFilePath)
        {
            AlphaRobotLog::getInstance()->printError(QString("CQiniuUpoadHandle::uploadQiniuFile:pszLocalFilePath is empty"));
            break;
        }

        Qiniu_RS_PutPolicy putPolicy;
        Qiniu_Zero(putPolicy);
        putPolicy.scope = pszBucketName;
        putPolicy.expires = 60*10;

        char* pszUploadtoken = Qiniu_RS_PutPolicy_Token(&putPolicy, pMac); /*得到uploadKey*/

        Qiniu_Rio_PutRet putRet;
        Qiniu_Zero(putRet);

        Qiniu_Rio_PutExtra putExtra;
        Qiniu_Zero(putExtra);
        putExtra.bucket = pszBucketName;
        putExtra.notifyRecvr = (void*)pNotifyRecvr;
        putExtra.notify = QiniuNotify;
        putExtra.notifyErr = notifyErr;

        qint64 nFileSize = CFileDirHandle::getFileSize(sLocalFilePath);
        qint64 nFileSizeForMB = CFileDirHandle::dBToMB(nFileSize);
        const int nMBForQiniuUploadPerBlock = 4;
        nBlkcountForQiniuUploadFile = nFileSizeForMB/nMBForQiniuUploadPerBlock + 1;

        Qiniu_Error error = Qiniu_Rio_PutFile(pClient, &putRet, pszUploadtoken, pszUploadName, pszLocalFilePath, &putExtra);
        Qiniu_Free((void*)pszUploadtoken);

        if (error.code != QINIU_ERR_UPLOAD_OK)
        {
            //qDebug("Upload File %s To %s:%s error.\n", pLocalFilePath, pszBucketName,  uploadName);
            //debuginfo(pClient, error);

            QString err = QString("CQiniuUpoadHandle::uploadQiniuFile:");
            err.append(QString("err id: %1\n").arg(QString::number(error.code)));
            err.append(QString("message:") + QString(error.message));
            AlphaRobotLog::getInstance()->printDebug(err);

            bRet = false;
        }
        else
        {
            //qDebug("Upload File %s To %s:%s success.\n", pLocalFilePath, pszBucketName,  uploadName);
            bRet = true;
        }

        AlphaRobotLog::getInstance()->printDebug(QString("CQiniuUpoadHandle::uploadQiniuFile %1").arg(QString::number((int)bRet)));

    }while(0);

    SAFE_DELETE_ARRAY(pszUploadName);
    SAFE_DELETE_ARRAY(pszBucketName);
    SAFE_DELETE_ARRAY(pszLocalFilePath);

    return bRet;
}

//get qiniu mac
Qiniu_Mac CQiniuUpoadHandle::getQiniuMac()
{
    Qiniu_Mac qiniu_mac;

    //    qiniu_mac.accessKey ="og9UTrj8I83ndelDQrzlpjXS8HwtiQVMV2S_v7D1";
    //    qiniu_mac.secretKey = "oGUsG0nvsCcltrlkci08qY48DaM-uC7MQjsWRPe0";

    qiniu_mac.accessKey ="OJb5DHhgOxDo42se8R2JvwLyaykWLUBYowqMA3Nu";
    qiniu_mac.secretKey = "tRfcP40zLvGUwfVANQEClOnyn2ATb2spLki9K7cH";

    QINIU_ACCESS_KEY = qiniu_mac.accessKey;
    QINIU_SECRET_KEY = qiniu_mac.secretKey;

    return qiniu_mac;
}



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
bool CQiniuUpoadHandle::getQiniuDownloadURL(const QString &strBucketName, Qiniu_Mac* pMac,
                                            const QString& /*strFileName*/, QString& strFileUrl)
{
    bool bRet = false;    
    strFileUrl.clear();

    char* pszDomain = NULL;
    char* pszFileName = NULL;

    do
    {
//        QString sDomain = strBucketName + ".u.qiniudn.com";
        //http://7xl1fl.com5.z0.glb.clouddn.com/chon1.mp4 == http://video.ubtrobot.com
        QString sDomain = "video.ubtrobot.com";

        Qiniu_RS_GetPolicy getPolicy;
        Qiniu_Zero(getPolicy);

        UnicodeToAnsi(sDomain, pszDomain);
        if(!pszDomain)
            break;

        QString strFileKey = CQiniuUpoadHandle::getFileName(strBucketName);
//        UnicodeToUtf8(strFileName, pszFileName);
        UnicodeToUtf8(strFileKey, pszFileName);

        if(!pszFileName)
            break;

        char* baseUrl = Qiniu_RS_MakeBaseUrl(pszDomain, pszFileName);
        char* pUrl = Qiniu_RS_GetPolicy_MakeRequest(&getPolicy, baseUrl, pMac);

        if (0 == baseUrl)
        {
            //qDebug("get URL %s:%s error.\n", pszBucketName, downloadName);
            bRet = false;
        }
        else
        {
//            strFileUrl = QString::fromLatin1(pUrl);
            strFileUrl = QString::fromLatin1(baseUrl);

            bRet = true;
        }

        Qiniu_Free(baseUrl);
        Qiniu_Free(pUrl);

    }while(0);

    SAFE_DELETE_ARRAY(pszDomain);
    SAFE_DELETE_ARRAY(pszFileName);

    return bRet;
}

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
QString CQiniuUpoadHandle::getBucketName(const QString &strFilePath, int nUserID)
{
//    QString strFileName = CFileDirHandle::getFileName(strFilePath);
//    strFileName.prepend(QString::number(nUserID) + "/");

//    //    QString sBucketName = QString("qiniu-action-upload-video:%1").arg(sFileName);
//    QString strBucketName = QString(QString(QINIU_BUCKET_UBT).append(":%1")).arg(strFileName);

//    return strBucketName;
    return m_strBucketName;
}

/************************************
* 名称: getFileName
* 功能: 获取filename from BucketName
* 参数:
*    @[in ] strBucketName: BucketName
* 返回值:
* 时间: 20160914
* 作者: asu
*/
QString CQiniuUpoadHandle::getFileName(const QString &strBucketName)
{
    QString strFileName = strBucketName;
    strFileName = strFileName.remove(QString(QINIU_BUCKET_UBT).append(":"));
    return strFileName;
}

void CQiniuUpoadHandle::createBucketName(quint64 nActionId,  int nUserID, const QString& strFilePath)
{
    QString strFileSuffix = CFileDirHandle::getFileSuffix(strFilePath);
    QString strFileNameOnQiniu = QString("%1.%2").arg(nActionId).arg(strFileSuffix);
    strFileNameOnQiniu.prepend(QString::number(nUserID) + "/");

    //    QString sBucketName = QString("qiniu-action-upload-video:%1").arg(sFileName);
    m_strBucketName = QString(QString(QINIU_BUCKET_UBT).append(":%1")).arg(strFileNameOnQiniu);
}
