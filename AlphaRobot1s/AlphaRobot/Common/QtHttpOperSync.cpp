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
#include "QtHttpOperSync.h"

QtHttpOperSync::QtHttpOperSync(QWidget *parent) : CBaseHttpOperSync(parent)
{
    m_pNetMgr = NULL;
    m_pNetReply = NULL;
    m_pRequest = NULL;
    m_pEventLoop = NULL;
    m_bCancel = false;
    m_bErr = false;

    m_nTotalSize = 0;
}


QtHttpOperSync::~QtHttpOperSync(void)
{

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
bool QtHttpOperSync::ExecuteRequest(emRequestType eReqType,
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
//        int nRecvLen = 0; //接收的字节数
//        int nReadLen = 0; //读取的字节数
//        int nDataCursor = 0; //数据游标

        QByteArray byteArrOut;
        byteArrOut.clear();

        //设置地址
        m_pRequest->setUrl(QUrl(sURL));

        if(eReqType == eRequestGet)
        {
            //GET方式
            m_pNetReply = m_pNetMgr->get(*m_pRequest); //Get请求
            connect(m_pNetReply, &QNetworkReply::downloadProgress,
                    this, &QtHttpOperSync::downloadProgress);
        }
        else
        {
            //设置内容类型标头
            m_pRequest->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            m_pRequest->setHeader(QNetworkRequest::ContentLengthHeader, (int)strlen(pszParam));
            //设置连接状态为保持连接
            m_pRequest->setRawHeader(QByteArray("Connection"),QByteArray("Keep-Alive"));

            m_pNetReply = m_pNetMgr->post(*m_pRequest, pszParam); //Post请求

            connect(m_pNetReply, &QNetworkReply::uploadProgress,
                    this, &QtHttpOperSync::uploadProgress);
        }

        connect(m_pNetReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                this, &QtHttpOperSync::OnError);

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

//            char* pszBuf = bytearr.data();
//            nReadLen = bytearr.length();

//            nRecvLen += nReadLen;
//            if(!pOutData)
//            {
//                pOutData = (byte*)malloc(nRecvLen);
//            }
//            else
//            {
//                pOutData = (byte*)realloc(pOutData, nRecvLen);
//            }

//            if(!pOutData)
//            {
//                m_bErr = true;
//                if(pszErr)
//                {
//                    strcpy(pszErr, "memory alloc failed");
//                }

//                break;
//            }

//            memcpy(pOutData + nDataCursor, pszBuf, nReadLen);

//            nDataCursor += nReadLen;

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
