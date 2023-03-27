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
#ifndef QTHTTPOPERSYNC_H
#define QTHTTPOPERSYNC_H

#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QWidget>

#include "UBXPublic.h"
#include "cbasehttpopersync.h"


class QtHttpOperSync : public CBaseHttpOperSync
{
    Q_OBJECT
public:
    QtHttpOperSync(QWidget *parent = NULL);
    virtual ~QtHttpOperSync(void);

public:
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
};

#endif
