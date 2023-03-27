/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：MyLog.cpp
* 创建时间：2015/07/21 13:46
* 文件标识：
* 文件摘要：日志记录
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/07/21 13:46
* 版本摘要：
*/
#include "StdAfx.h"
#include "MyLog.h"
#include <QDir>
#include <QDateTime>

CMyLog  g_log;

CMyLog::CMyLog(void)
{
}


CMyLog::~CMyLog(void)
{
}

/**************************************************************************
* 函数名: AddLog
* 功能: 添加日志
* 参数: 
*    @[in ] sLog: 日志内容
*    @[in ] bAddTime: 是否添加时间标签
* 返回值: void
* 时间: 2015/07/21 14:02
* 作者: ZDJ
*/
void CMyLog::AddLog(const QString &sLog, BOOL bAddTime)
{
    const QString sLogPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/log.txt"); //日志文件路径
    QFile fileobj(sLogPath);
    if(fileobj.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
	{
        //fileobj.seek(fileobj.size());

        QString sInfo;
		if(bAddTime)
		{
            QDateTime curtime = QDateTime::currentDateTime();
            QString sTime = curtime.toString("yyyy-MM-dd hh:mm:ss");

            sInfo = sTime + " ";
		}

        sInfo += sLog;
        fileobj.write(sInfo.toLatin1());
        fileobj.close();
	}
}
