/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：HttpOper.h
* 创建时间：2015/05/18 8:58
* 文件标识：
* 文件摘要：http操作类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/05/18 8:58
* 版本摘要：
*/
#pragma once

/**************************************************************************
* 函数类型: FILEDOWNPROCESSNOTIFY
* 功能: 文件下载进度通知
* 参数: 
*    @[in ] hWnd: 目标窗口句柄
*    @[in ] data1: 文件名
*    @[in ] data2: 文件长度
*    @[in ] data3: 当前已下载长度
* 返回值: void
* 时间: 2015/06/12 11:17
* 作者: ZDJ
*/
typedef void (CALLBACK *FILEDOWNPROCESSNOTIFY)(HWND hWnd, LPCTSTR data1, int data2, int data3);

/**************************************************************************
* 函数类型: FILEDOWNPROCESSTHREADNOTIFYEX
* 功能: 文件下载进度线程通知
* 参数: 
*    @[in ] pUser: 通知目标
*    @[in ] data1: 文件名
*    @[in ] data2: 文件长度
*    @[in ] data3: 当前已下载长度
* 返回值: void
* 时间: 2015/06/12 11:17
* 作者: ZDJ
*/
typedef void (CALLBACK *FILEDOWNPROCESSNOTIFYEX)(LPVOID pUser, LPCTSTR data1, int data2, int data3);

class CHttpOper
{
public:
	CHttpOper(void);
	virtual ~CHttpOper(void);

	/**************************************************************************
	* 函数名: DownloadFile
	* 功能: 下载远程文件
	* 参数: 
	*    @[in ] lpURL: 网络地址
	*    @[out] pFileData: 返回的文件数据，返回成功时需要在外调用free释放内存
	*    @[out] nFileLen: 返回的文件数据长度
	*    @[out] lpStrErr: 失败时返回的错误内容
	*    @[in ] processnotify: 下载进度通知回调
	*	 @[in ] hUser: 通知目标
	* 返回值: 成功返回TRUE，失败返回FALSE
	* 时间: 2015/05/14 10:20
	* 作者: ZDJ
	*/
	BOOL DownloadFile(
		LPCTSTR lpURL, 
		byte*& pFileData, 
		long& nFileLen, 
		LPCTSTR lpStrErr = NULL, 
		FILEDOWNPROCESSNOTIFY processnotify = NULL, 
		HWND hUser = NULL);

	/**************************************************************************
	* 函数名: ExecuteRequest
	* 功能: 发起请求
	* 参数: 
	*    @[in ] nMethod: 请求方法，如GET或POST
	*    @[in ] lpURL: 网络地址
	*    @[in ] pParam: 请求参数
	*    @[out] pOutData: 返回的数据，返回成功时需要在外调用free释放内存
	*    @[out] nOutDataLen: 返回的数据长度
	*    @[out] lpStrErr: 失败时返回的错误内容
	*    @[in ] processnotify: 下载进度通知回调
	*	 @[in ] hUser: 通知目标
	* 返回值: 成功返回TRUE，失败返回FALSE
	* 时间: 2015/05/14 10:20
	* 作者: ZDJ
	*/
	BOOL ExecuteRequest(
		int nMethod,
		LPCTSTR lpURL, 
		char* pParam,
		byte*& pOutData, 
		long& nOutDataLen, 
		LPCTSTR lpStrErr = NULL, 
		FILEDOWNPROCESSNOTIFY processnotify = NULL, 
		HWND hUser = NULL);
	
	/**************************************************************************
	* 函数名: ExecuteRequestEx
	* 功能: 发起请求
	* 参数: 
	*    @[in ] nMethod: 请求方法，如GET或POST
	*    @[in ] lpURL: 网络地址
	*    @[in ] pParam: 请求参数
	*    @[out] pOutData: 返回的数据，返回成功时需要在外调用free释放内存
	*    @[out] nOutDataLen: 返回的数据长度
	*    @[out] lpStrErr: 失败时返回的错误内容
	*    @[in ] processnotify: 下载进度通知回调
	*	 @[in ] pUser: 通知目标
	* 返回值: 成功返回TRUE，失败返回FALSE
	* 时间: 2015/05/14 10:20
	* 作者: ZDJ
	*/
	BOOL ExecuteRequestEx(
		int nMethod,
		LPCTSTR lpURL, 
		char* pParam,
		byte*& pOutData, 
		long& nOutDataLen, 
		LPCTSTR lpStrErr = NULL, 
		FILEDOWNPROCESSNOTIFYEX processnotify = NULL, 
		LPVOID pUser = NULL);

	/**************************************************************************
	* 函数名: Cancel
	* 功能: 取消请求
	* 时间: 2015/06/12 10:22
	* 作者: ZDJ
	*/
	void Cancel();

private:
	BOOL m_bCancel;//取消下载标识
};

