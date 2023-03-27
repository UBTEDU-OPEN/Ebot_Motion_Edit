/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：QtHttpOper.h
* 创建时间：2015/05/18 8:58
* 文件标识：
* 文件摘要：http操作类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/05/18 8:58
* 版本摘要：
*/
#ifndef QTHTTPOPER_H
#define QTHTTPOPER_H

#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QWidget>

#include "UBXPublic.h"

class QtHttpOper : public QWidget
{
    Q_OBJECT
public:
    QtHttpOper(QWidget *parent = NULL);
    virtual ~QtHttpOper(void);

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
    bool DownloadFile(const QString& sURL,
        const QString& sSavePath,
        char* pszErr = NULL);

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
    bool ExecuteRequest(
        emRequestType eReqType,
        const QString& sURL,
        const char* pszParam = NULL,
        char* pszErr = NULL);

	/**************************************************************************
	* 函数名: Cancel
	* 功能: 取消请求
	* 时间: 2015/06/12 10:22
	* 作者: ZDJ
	*/
	void Cancel();

signals:
    void downloadProgressSignal(const QString& sFilePath, qint64 bytesReceived, qint64 bytesTotal);
    void requestResultSignal(bool bRet, char* pData, int nDataLen);

private slots:
    void readyRead();
    void httpFinish();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    bool                   m_bCancel;     //取消下载标识
    QNetworkAccessManager m_objNetMgr;   //下载管理器
    QNetworkReply*        m_pNetReply;   //下载应答
    QNetworkRequest       m_objRequest;   //请求对象
    QFile*                 m_pFile;      //下载文件
};

#endif
