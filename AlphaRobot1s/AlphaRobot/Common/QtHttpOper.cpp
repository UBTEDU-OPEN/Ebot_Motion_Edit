/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：QtHttpOper.cpp
* 创建时间：2015/05/18 8:59
* 文件标识：
* 文件摘要：http操作类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/05/18 8:59
* 版本摘要：
*/
#include "StdAfx.h"
#include "QtHttpOper.h"

QtHttpOper::QtHttpOper(QWidget *parent)
    : QWidget(parent)
{
    m_pNetReply = NULL;
    m_bCancel = false;
    m_pFile = NULL;
}


QtHttpOper::~QtHttpOper(void)
{    
    if(m_pNetReply)
    {
        m_pNetReply->deleteLater();
        m_pNetReply = NULL;
    }

    if(m_pFile)
    {
        delete m_pFile;
        m_pFile = NULL;
    }
}

/**************************************************************************
* 函数名: DownloadFile
* 功能: 下载远程文件
* 参数:
*    @[in ] sURL: 文件的网络下载地址
*    @[in ] sSavePath: 下载的文件本地保存路径
*    @[out] pszErr: 失败时返回的错误内容
* 返回值: 成功返回true，失败返回false
* 时间: 2015/05/14 10:20
* 作者: ZDJ
*/
bool QtHttpOper::DownloadFile(
    const QString& sURL,
    const QString& sSavePath,
    char* pszErr)
{    
    return ExecuteRequest(eRequestGet, sURL, sSavePath.toStdString().c_str(), pszErr);
}

/**************************************************************************
* 函数名: ExecuteRequest
* 功能: 发起请求
* 参数:
*    @[in ] eReqType: 请求方法，如GET或POST
*    @[in ] sURL: 网络地址
*    @[in ] sParam: 请求参数
*    @[out] pszErr: 失败时返回的错误内容
* 返回值: 成功返回true，失败返回false
* 时间: 2015/05/14 10:20
* 作者: ZDJ
*/
bool QtHttpOper::ExecuteRequest(
    emRequestType eReqType,
    const QString& sURL,
    const char* pszParam,
    char* pszErr)
{
    m_bCancel = false;

    if(eReqType == eRequestGet)
    {
//        QString sFileName = sURL.mid(sURL.lastIndexOf('/') + 1);
//        if(sFileName.isEmpty())
//        {
//            if(pszErr)
//                strcpy(pszErr, "file url error");
//            return false;
//        }
        QString sFileName = QString::fromStdString(pszParam);
        //m_objFile.setFileName(sFileName);
        if(m_pFile)
        {
            delete m_pFile;
            m_pFile = NULL;
        }
        m_pFile = new QFile(sFileName);
        if(m_pFile && m_pFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            m_objRequest.setUrl(QUrl(sURL));

            m_pNetReply = m_objNetMgr.get(m_objRequest);
            connect(m_pNetReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
            connect(m_pNetReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
            connect(m_pNetReply, SIGNAL(finished()), this, SLOT(httpFinish()));
        }
        else
        {
            if(pszErr)
                strcpy(pszErr, "file create failed");
            return false;
        }
    }
    else if(eReqType == eRequestPost)
    {

    }

    return true;
}

/**************************************************************************
* 函数名: Cancel
* 功能: 取消请求
* 时间: 2015/06/12 10:22
* 作者: ZDJ
*/
void QtHttpOper::Cancel()
{ 
    m_bCancel = true;
    m_pNetReply->abort();
    emit requestResultSignal(false, NULL, 0);
}

void QtHttpOper::readyRead()
{
    if(m_pFile && m_pFile->isOpen())
        m_pFile->write(m_pNetReply->readAll());
}

void QtHttpOper::httpFinish()
{
    if(m_pFile && m_pFile->isOpen())
    {
        m_pFile->flush();
        m_pFile->close();

        if(m_bCancel)
            m_pFile->remove();
        if(m_pFile)
        {
            delete m_pFile;
            m_pFile = NULL;
        }
    }

    if(m_pNetReply)
    {
        m_pNetReply->deleteLater();
        m_pNetReply = NULL;
    }

    emit requestResultSignal(!m_bCancel, NULL, 0);
}

void QtHttpOper::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(m_pFile)
        emit downloadProgressSignal(m_pFile->fileName(), bytesReceived, bytesTotal);
}
