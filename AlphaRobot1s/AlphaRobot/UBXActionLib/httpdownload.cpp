/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：httpdownload.cpp
* 创建时间：2016/03/31
* 文件标识：
* 文件摘要：动作文件下载类
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/31
* 版本摘要：
*/
#include "httpdownload.h"
#include "ubxactionlibconstants.h"
#include "UBXPublic.h"

HttpDownload::HttpDownload(const QString &downloadDir)
{
    m_strDownloadDir = downloadDir;
    m_pNetManager = NULL;
    moveToThread(&m_oThread);
    m_nRequestID = -1;
    connect(this, &HttpDownload::SigToPauseDownload, this, &HttpDownload::OnPauseDownload);
    connect(this, &HttpDownload::SigToResumeDownload, this, &HttpDownload::OnResumeDownload);
    connect(this, &HttpDownload::SigInit, this, &HttpDownload::OnInit);
    connect(this, &HttpDownload::SigDownloadAction, this, &HttpDownload::OnDownloadAction);
    m_oThread.start();
    emit SigInit();
}

HttpDownload::~HttpDownload()
{
    if(m_pNetManager)
    {
        m_pNetManager->disconnect();
        delete m_pNetManager;  // 出现释放打印异常
    }
}

/************************************
* 名称: stopThread
* 功能: 停止线程
* 参数:
* 返回:
* 时间:   2017/04/06
* 作者:   hwx
************************************/
void HttpDownload::stopThread()
{
    m_oThread.quit();
    m_oThread.wait();
}

/************************************
* 名称: DownloadAction
* 功能: 发送下载动作请求
* 参数: [in]itemData 下载动作数据信息
* 返回:   int 返回请求ID，外部监听者可以据此判断是否是自己的请求
* 时间:   2016/03/31
* 作者:   hels
************************************/
int HttpDownload::DownloadAction(ActItemData itemData)
{
    //不重复进行下载
    if(m_mapReply.contains(itemData.m_nItemID))
    {
        return REQUSET_ID_DEFAULT_INVALID;
    }

    ++m_nRequestID;
    QFileInfo fileInfo(itemData.m_sActDownloadURL);

    //在下载目录下先以文件名（不含后缀）创建一个文件夹
    QDir fileDir;
    fileDir.setPath(QDir::toNativeSeparators(m_strDownloadDir + "/" + fileInfo.baseName()));
    if(!fileDir.exists())
    {
        fileDir.mkpath(fileDir.absolutePath());
    }

    if(!m_oThread.isRunning())
    {
        m_oThread.start();
    }

    emit SigDownloadAction(m_nRequestID, itemData);
    return m_nRequestID;
}

/************************************
* 名称: ResumeDownload
* 功能: 重新下载
* 参数: [in]itemData 下载项
* 返回:   int 返回请求ID
* 时间:   2016/04/06
* 作者:   hels
************************************/
int HttpDownload::ResumeDownload(ActItemData itemData)
{
    ++m_nRequestID;
    QFileInfo fileInfo(itemData.m_sActDownloadURL);

    //在下载目录下先以文件名（不含后缀）创建一个文件夹
    QDir fileDir;
    fileDir.setPath(QDir::toNativeSeparators(m_strDownloadDir + "/" + fileInfo.baseName()));
    if(!fileDir.exists())
    {
        fileDir.mkpath(fileDir.absolutePath());
    }

    if(!m_oThread.isRunning())
    {
        m_oThread.start();
    }

    emit SigToResumeDownload(m_nRequestID, itemData);
    return m_nRequestID;
}

/************************************
* 名称: OnError
* 功能: 网络错误槽函数
* 参数: [in]code 错误码
* 返回:   void
* 时间:   2016/03/31
* 作者:   hels
************************************/
void HttpDownload::OnError(QNetworkReply::NetworkError code)
{
    if(code == QNetworkReply::ConnectionRefusedError ||
       code == QNetworkReply::HostNotFoundError ||
       code == QNetworkReply::TimeoutError ||
       code == QNetworkReply::NetworkSessionFailedError)
    {
        //特殊错误处理，暂时没有需求
    }

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
        //移除存储的本地路径
        m_mapIteDada.remove(nActionID);
        m_mapReply.remove(nActionID);
        reply->deleteLater();
    }
}

/************************************
* 名称: OnDownloadProgress
* 功能: 下载进度处理槽函数
* 参数: [in]bytesReceived 已经接收到的字节数
* 参数: [in]bytesTotal 总共的字节数
* 返回:   void
* 时间:   2016/03/31
* 作者:   hels
************************************/
void HttpDownload::OnDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply && bytesReceived != 0)
    {
        int nRequestID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestID)).toInt();
        int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();
        quint64 nbytesAvailable = reply->bytesAvailable();
        if(nbytesAvailable > 0)
        {
            QFileInfo fileInfo(m_mapIteDada[nActionID].m_sActDownloadURL);
            QString strFileName = QDir::toNativeSeparators(m_strDownloadDir + "/" + fileInfo.baseName() + "/" + fileInfo.fileName());
            QFile localFile(strFileName);

            //不存在则为刚开始下载
            if(!localFile.exists())
            {
                m_mapIteDada[nActionID].m_nActSize = bytesTotal;
                m_mapIteDada[nActionID].m_sLocalPath = strFileName;
                emit SigStartDownload(nRequestID, m_mapIteDada[nActionID], bytesReceived, bytesTotal);
            }

            localFile.open(QIODevice::WriteOnly | QIODevice::Unbuffered | QIODevice::Append);
            if (!localFile.isOpen())
            {
                //如果文件无法写入，直接结束本次下载
                reply->abort();
                return;
            }

            quint64 dataSize = nbytesAvailable;
            quint64 nReadedBytes = 0;
            do
            {
                QByteArray data = reply->read(nbytesAvailable);

                nReadedBytes = dataSize = data.size();
                char *writeBuff = data.data();
                while (dataSize > 0)
                {
                    quint64 writedSize = localFile.write(writeBuff, dataSize);
                    dataSize -= writedSize;
                    writeBuff += writedSize;
                }
                localFile.flush();
                localFile.close();
                m_mapIteDada[nActionID].m_nHasDownloadedBytes += nReadedBytes;

            }while((nbytesAvailable -= nReadedBytes) > 0);

            //已经存在则是断点下载，此时的received和total需要自己计算,因此最好不使用槽的参数，而使用自己记录过的值
            emit SigDownloadProgress(nRequestID, nActionID, m_mapIteDada[nActionID].m_nHasDownloadedBytes, m_mapIteDada[nActionID].m_nActSize);

            //写入本地文件
            if(bytesReceived == bytesTotal)
            {
                //如果是zip文件，则先解压
                QString strFileSuffix = strFileName.mid(strFileName.lastIndexOf("."));
                if(strFileSuffix.compare(".zip", Qt::CaseInsensitive) == 0)
                {
                    UnZipFile(strFileName);
                }
                g_actupdatemgr->AddDownLoadCount(nActionID);
            }
        }
    }
}

/************************************
* 名称: OnPauseDownload
* 功能: 响应暂停下载槽函数
* 参数: [in]nActionID 指定要暂停的action ID
* 返回:   void
* 时间:   2016/04/06
* 作者:   hels
************************************/
void HttpDownload::OnPauseDownload(int nActionID)
{
    if(m_mapReply.contains(nActionID))
    {
        m_mapReply[nActionID]->abort();
        m_mapReply.remove(nActionID);
        emit SigHasPausedDownload(nActionID);
    }
}

/************************************
* 名称: OnFinished
* 功能: 指令请求完成槽函数
* 返回:   void
* 时间:   2016/04/06
* 作者:   hels
************************************/
void HttpDownload::OnFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        int nRequestID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPRequestID)).toInt();
        int nActionID = reply->request().attribute(QNetworkRequest::Attribute(Constants::HTTPActionID)).toInt();

        m_mapReply.remove(nActionID);
        reply->deleteLater();
        if(m_mapIteDada.contains(nActionID) && !m_mapIteDada[nActionID].m_sLocalPath.isEmpty())
        {
            emit SigFinishedDownload(nRequestID, m_mapIteDada[nActionID]);
        }
        m_mapIteDada.remove(nActionID);
    }
}

/************************************
* 名称: OnResumeDownload
* 功能: 重新下载
* 参数: [in]nRequestID 请求ID
* 参数: [in]itemData 下载项
* 返回:   void
* 时间:   2016/04/06
* 作者:   hels
************************************/
void HttpDownload::OnResumeDownload(int nRequestID, ActItemData itemData)
{
    m_mapIteDada.insert(itemData.m_nItemID, itemData);
    QNetworkRequest request;
    request.setUrl(QUrl(itemData.m_sActDownloadURL));

    QString strRange = "bytes " + QString::number(itemData.m_nHasDownloadedBytes) + "-";
    request.setRawHeader("Range",strRange.toLatin1());

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID), (int)nRequestID);
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID), itemData.m_nItemID);
    QNetworkReply* reply = m_pNetManager->get(request);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpDownload::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpDownload::OnDownloadProgress);
    connect(reply, &QNetworkReply::finished, this, &HttpDownload::OnFinished);
    m_mapReply.insert(itemData.m_nItemID, reply);
}

/************************************
* 名称: OnDownloadAction
* 功能: 重新下载
* 参数: [in]nRequestID 请求ID
* 参数: [in]itemData 下载项
* 返回:   void
* 时间:   2016/04/06
* 作者:   hels
************************************/
void HttpDownload::OnDownloadAction(int nRequestID, ActItemData itemData)
{
    m_mapIteDada.insert(itemData.m_nItemID, itemData);
    QNetworkRequest request;
    request.setUrl(QUrl(itemData.m_sActDownloadURL));

    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPRequestID), (int)nRequestID);
    request.setAttribute(QNetworkRequest::Attribute(Constants::HTTPActionID), itemData.m_nItemID);
    QNetworkReply* reply = m_pNetManager->get(request);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpDownload::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpDownload::OnDownloadProgress);
    connect(reply, &QNetworkReply::finished, this, &HttpDownload::OnFinished);
    m_mapReply.insert(itemData.m_nItemID, reply);
}

/************************************
* 名称: OnInit
* 功能: 初始化槽函数
* 返回:   void
* 时间:   2016/04/07
* 作者:   hels
************************************/
void HttpDownload::OnInit()
{
    m_pNetManager = new QNetworkAccessManager();
}


