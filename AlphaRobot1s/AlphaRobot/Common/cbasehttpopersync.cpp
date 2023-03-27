/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：QtHttpOperSync.cpp
* 创建时间：2015/05/18 8:59
* 文件标识：
* 文件摘要：http同步操作类
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/05/18 8:59
* 版本摘要：
*/
#include "StdAfx.h"

#include "cbasehttpopersync.h"



CBaseHttpOperSync::CBaseHttpOperSync(QWidget *parent)
{
    m_pNetMgr = NULL;
    m_pNetReply = NULL;
    m_pRequest = NULL;
    m_pEventLoop = NULL;
    m_bCancel = false;
    m_bErr = false;

    m_nTotalSize = 0;
}


CBaseHttpOperSync::~CBaseHttpOperSync(void)
{
    if(m_pNetReply)
    {
        m_pNetReply->deleteLater();
        m_pNetReply = NULL;
    }
}

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
bool CBaseHttpOperSync::DownloadFile(const QString& sURL,
    const QString& sSavePath,
    FILEDOWNPROCESSNOTIFY pNotify,
    LPVOID pUser,
    char* pszErr)
{
    byte* pData = NULL;
    qint64 nDataLen = 0;
    bool bRet = ExecuteRequest(eRequestGet, sURL, NULL, pNotify, pUser, pData, nDataLen, pszErr);
    if(!bRet)
    {
        AlphaRobotLog::getInstance()->printError("Http request error: " + QString(pszErr));
        return false;
    }

    QFile file(sSavePath);
    if(file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        file.write((char*)pData, nDataLen);
        file.close();
        return true;
    }
    else
    {
        AlphaRobotLog::getInstance()->printError(QString("Failed to open file %1 to write").arg(sSavePath));
    }
//    if (!CFileDirHandle::saveFile(sSavePath, QByteArray((char *)pData)))
//    {
//         AlphaRobotLog::getInstance()->printError(QString("Failed to open file %1 to write").arg(sSavePath));
//         return false;
//    }

    return true;
}

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
bool CBaseHttpOperSync::ExecuteRequest(emRequestType eReqType,
    const QString& sURL,
    const char* pszParam,
    FILEDOWNPROCESSNOTIFY pNotify,
    LPVOID pUser,
    byte*& pOutData,
    qint64& nOutDataLen,
    char* pszErr)
{
    m_bCancel = false;
    bool bRet = false;

    m_pNetMgr = new QNetworkAccessManager();
    m_pRequest = new QNetworkRequest();
    m_pEventLoop = new QEventLoop();

    do
    {
        QByteArray byteArrOut;
        byteArrOut.clear();

        //设置地址
        m_pRequest->setUrl(QUrl(sURL));

        if(eReqType == eRequestGet)
        {
            //GET方式
            m_pNetReply = m_pNetMgr->get(*m_pRequest); //Get请求
            connect(m_pNetReply, &QNetworkReply::downloadProgress,
                    this, &CBaseHttpOperSync::downloadProgress);
        }
        else
        {
            m_pNetReply = m_pNetMgr->post(*m_pRequest, pszParam); //Post请求

            connect(m_pNetReply, &QNetworkReply::uploadProgress,
                    this, &CBaseHttpOperSync::uploadProgress);
        }

        connect(m_pNetReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                this, &CBaseHttpOperSync::OnError);

        m_bErr = false;

        while(m_pNetReply &&
              !m_bCancel &&
              !m_bErr &&
              !m_pNetReply->isFinished() &&
              m_pNetReply->isReadable())
        {
            m_pEventLoop->exec();

            if (m_pNetReply)
            {
                QByteArray bytearr = m_pNetReply->readAll();
                byteArrOut.append(bytearr);
            }

            //读取中再通知
            if(!m_bErr && (eRequestGet == eReqType) && pNotify)
            {
                const int nRecvLen = byteArrOut.length();

                pNotify(pUser, sURL, m_nTotalSize, nRecvLen);
            }
        }


        pOutData = (byte*)malloc(byteArrOut.length());
        if(!pOutData)
        {
            m_bErr = true;
            if(pszErr)
            {
                strcpy(pszErr, "memory alloc failed");
            }
        }

        if(pOutData)
        {
            char* pszBuf = byteArrOut.data();
            const int nReadLen = byteArrOut.length();
            memcpy(pOutData, pszBuf, nReadLen);
        }

        if(m_bErr)
        {
            break;
        }

        nOutDataLen = byteArrOut.length();
        bRet = true;

    }while(0);

//    if(m_pNetReply)
//    {
//        m_pNetReply->deleteLater();
//        m_pNetReply = NULL;
//    }

    SAFE_DELETE(m_pNetReply);
    SAFE_DELETE(m_pNetMgr);
    SAFE_DELETE(m_pRequest);
    SAFE_DELETE(m_pEventLoop);

    if(m_bCancel)
    {
        bRet = false;
    }

    return bRet;
}

/**************************************************************************
* 函数名: Cancel
* 功能: 取消请求
* 时间: 2015/06/12 10:22
* 作者: ZDJ
*/
void CBaseHttpOperSync::Cancel()
{
    m_bCancel = true;

//    if(m_pNetReply)
//    {
//        m_pNetReply->abort();
//        m_pNetReply->deleteLater();
//        m_pNetReply = NULL;
//    }

//    SAFE_DELETE(m_pNetMgr);
//    SAFE_DELETE(m_pRequest);
//    SAFE_DELETE(m_pEventLoop);

    //emit requestResultSignal(false, NULL, 0);
}

void CBaseHttpOperSync::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_nTotalSize = bytesTotal;

    if(m_pEventLoop)
        m_pEventLoop->quit();
}

/************************************
* 名称: OnError
* 功能: 网络错误槽函数
* 参数: [in]code 错误码
* 返回:   void
* 时间:   2016/03/31
* 作者:   hels
************************************/
void CBaseHttpOperSync::OnError(QNetworkReply::NetworkError code)
{
    m_bErr = true;

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        reply->abort();
    }
}

void CBaseHttpOperSync::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    m_nTotalSize = bytesTotal;

    if(m_pEventLoop)
        m_pEventLoop->quit();
}
