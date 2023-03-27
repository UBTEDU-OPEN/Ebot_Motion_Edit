/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：MyLog.h
* 创建时间：2015/07/21 13:46
* 文件标识：
* 文件摘要：日志记录
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/07/21 13:46
* 版本摘要：
*/
#ifndef MYLOG_H
#define MYLOG_H

class CMyLog
{
public:
	CMyLog(void);
	~CMyLog(void);

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
    void AddLog(const QString& sLog, BOOL bAddTime = TRUE);
};

extern CMyLog  g_log;

#endif
