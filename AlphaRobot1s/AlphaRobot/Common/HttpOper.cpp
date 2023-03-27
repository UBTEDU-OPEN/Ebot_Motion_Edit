/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：HttpOper.cpp
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
#include "HttpOper.h"

CHttpOper::CHttpOper(void)
	: m_bCancel(FALSE)
{
}


CHttpOper::~CHttpOper(void)
{
}

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
BOOL CHttpOper::DownloadFile(
	LPCTSTR lpURL, 
	byte*& pFileData, 
	long& nFileLen, 
	LPCTSTR lpStrErr,
	FILEDOWNPROCESSNOTIFY processnotify /*= NULL*/, 
	HWND hUser/* = NULL*/)
{	
	return ExecuteRequest(CHttpConnection::HTTP_VERB_GET, 
		lpURL, NULL, pFileData, nFileLen, 
		lpStrErr, processnotify, hUser);
}

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
BOOL CHttpOper::ExecuteRequest(
	int nMethod,
	LPCTSTR lpURL,
	char* pParam,
	byte*& pOutData, 
	long& nOutDataLen, 
	LPCTSTR lpStrErr,
	FILEDOWNPROCESSNOTIFY processnotify /*= NULL*/, 
	HWND hUser/* = NULL*/)
{
	CString sStrErr;

	CInternetSession session; //会话期对象
	CHttpConnection* pServer = NULL; // 指向服务器地址(URL)
	CHttpFile * pHttpFile = NULL;//HTTP文件指针

	CString strServerName; //服务器名
	CString strObject; //查询对象名(http文件)
	INTERNET_PORT nPort = 0; //端口
	DWORD dwServiceType = 0; //服务类型
	DWORD dwHttpRequestFlags = INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_RELOAD;//请求标志

	CString sHeaders;
	if(nMethod == CHttpConnection::HTTP_VERB_GET)
		sHeaders = _T("Accept: */*\r\nUser-Agent:HttpClient\r\n");
	else if(nMethod == CHttpConnection::HTTP_VERB_POST)
		sHeaders = _T("Accept:*/*\r\nUser-Agent:Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1;SV1)\r\nContent-Type:application/json;charset=UTF-8\r\nConnection:Keep-Alive\r\n");
	else
	{
		sStrErr = _T("method not support");
		return FALSE;
	}	
	
	m_bCancel = FALSE;
	BOOL bReturn = FALSE;
	do 
	{
		if(!session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10000)  
			|| !session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 10000)  
			|| !session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 20000))  
		{
			sStrErr = _T("SetOption出错"); 			
			break;
		}

		BOOL bRet = AfxParseURL( //词法分析
			lpURL, //被分析URL串
			dwServiceType, //服务类型，ftp，http等
			strServerName, //服务器名
			strObject, //URL中被查询对象
			nPort); //URL指定的端口，可能为空	

		if (!bRet || dwServiceType != INTERNET_SERVICE_HTTP)
		{
			sStrErr = _T("URL出错"); 			
			break;
		}

		pServer = session.GetHttpConnection(strServerName, nPort); //获得服务器名
		if(!pServer)
		{
			sStrErr = _T("GetHttpConnection出错"); 
			break;
		}

		pHttpFile = pServer->OpenRequest(nMethod, strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
		if(!pHttpFile)
		{
			sStrErr = _T("OpenRequest出错"); 
			break;
		}		

		////向服务器发送请求，建立http连接，
		////建立本机上的http文件指针
		//if(!pHttpFile->AddRequestHeaders(szHeaders))
		//{
		//	sStrErr = _T("AddRequestHeaders出错"); 
		//	break;
		//}

		try
		{
			//string szParam = pParam;
// 			if(nMethod == CHttpConnection::HTTP_VERB_POST)
// 			{
// 				szParam = "{\"versionType\":\"3\",\"versionName\":\"V0.0.0.1\"}";
// 			}
			int nParamLen = pParam ? strlen(pParam):0;
			if(!pHttpFile->SendRequest(sHeaders, sHeaders.GetLength(), pParam, nParamLen)) //发送请求
			{
				sStrErr = _T("SendRequest出错"); 
				break;
			}
		}
		catch (...)
		{
			sStrErr = _T("SendRequest出错"); 
			break;
		}	

		DWORD dwStateCode = 0;
		if(!pHttpFile->QueryInfoStatusCode(dwStateCode))
		{
			sStrErr = _T("QueryInfoStatusCode出错"); 
			break;
		}

		if(dwStateCode != HTTP_STATUS_OK)
		{
			sStrErr = _T("访问出错"); 
			break;
		}

		const int nReadBufLen = 1024;		
		char szBuf[nReadBufLen] = {0}; //缓存		
		
		DWORD dwFileSize = 0;

		if(nMethod == CHttpConnection::HTTP_VERB_GET)
		{
			if (!pHttpFile->QueryInfo(
				HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, dwFileSize))
			{ 			
				sStrErr = _T("QueryInfo出错"); 
				break;
			}

			if(dwFileSize <= 0)
			{			
				sStrErr = _T("文件长度错误"); 
				break;
			}
		}

		int nRecvLen = 0;
		int nReadLen = 0;
		int nDataCursor = 0;
		BOOL bSucc = FALSE;

		//读取之前通知一次
		if(nMethod == CHttpConnection::HTTP_VERB_GET)
		{
			if(processnotify)
				processnotify(hUser, lpURL, dwFileSize, nRecvLen);
		}

		try
		{	
			while((nReadLen = pHttpFile->Read(szBuf, nReadBufLen)) > 0)
			{
				if(m_bCancel)
					break;

				bSucc = FALSE;
				nRecvLen += nReadLen;
				if(!pOutData)			
					pOutData = (byte*)malloc(nRecvLen);
				else
					pOutData = (byte*)realloc(pOutData, nRecvLen);
				if(!pOutData)
				{
					sStrErr = _T("内存分配失败");				
					break;
				}

				memcpy(pOutData + nDataCursor, szBuf, nReadLen);				

				nDataCursor += nReadLen;
				bSucc = TRUE;		

				//读取中再通知
				if(nMethod == CHttpConnection::HTTP_VERB_GET)
				{
					if(processnotify)
						processnotify(hUser, lpURL, dwFileSize, nRecvLen);
				}
			}
		}
		catch (...)
		{
			break;
		}

		if(!bSucc)
			break;

		if(nMethod == CHttpConnection::HTTP_VERB_GET)
		{
			if(nRecvLen != dwFileSize)
			{
				sStrErr = _T("接收文件长度错误");				
				break;
			}
		}

		nOutDataLen = nRecvLen;
		bReturn = TRUE;
	} while (0);


	if(pHttpFile) 
	{
		pHttpFile ->Close();
		delete pHttpFile;
		pHttpFile = NULL;
	}

	if(pServer) 
	{
		pServer ->Close();
		delete pServer;
		pServer = NULL;
	}

	session.Close();

	if(!bReturn)
	{
		SAFE_FREE(pOutData);
		if(lpStrErr)
		{
			lstrcpy((LPWSTR)lpStrErr, sStrErr.GetBuffer());
			sStrErr.ReleaseBuffer();
		}
	}
	return bReturn;
}

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
BOOL CHttpOper::ExecuteRequestEx(
	int nMethod,
	LPCTSTR lpURL,
	char* pParam,
	byte*& pOutData, 
	long& nOutDataLen, 
	LPCTSTR lpStrErr,
	FILEDOWNPROCESSNOTIFYEX processnotify/* = NULL*/, 
	LPVOID pUser/* = NULL*/)
{
	CString sStrErr;

	CInternetSession session; //会话期对象
	CHttpConnection* pServer = NULL; // 指向服务器地址(URL)
	CHttpFile * pHttpFile = NULL;//HTTP文件指针

	CString strServerName; //服务器名
	CString strObject; //查询对象名(http文件)
	INTERNET_PORT nPort = 0; //端口
	DWORD dwServiceType = 0; //服务类型
	DWORD dwHttpRequestFlags = INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_RELOAD;//请求标志

	CString sHeaders;
	if(nMethod == CHttpConnection::HTTP_VERB_GET)
		sHeaders = _T("Accept: */*\r\nUser-Agent:HttpClient\r\n");
	else if(nMethod == CHttpConnection::HTTP_VERB_POST)
		sHeaders = _T("Accept:*/*\r\nUser-Agent:Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1;SV1)\r\nContent-Type:application/json;charset=UTF-8\r\nConnection:Keep-Alive\r\n");
	else
	{
		sStrErr = _T("method not support");
		return FALSE;
	}	
	
	m_bCancel = FALSE;
	BOOL bReturn = FALSE;
	do 
	{
		if(!session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10000)  
			|| !session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 10000)  
			|| !session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 20000))  
		{
			sStrErr = _T("SetOption出错"); 			
			break;
		}

		BOOL bRet = AfxParseURL( //词法分析
			lpURL, //被分析URL串
			dwServiceType, //服务类型，ftp，http等
			strServerName, //服务器名
			strObject, //URL中被查询对象
			nPort); //URL指定的端口，可能为空	

		if (!bRet || dwServiceType != INTERNET_SERVICE_HTTP)
		{
			sStrErr = _T("URL出错"); 			
			break;
		}

		pServer = session.GetHttpConnection(strServerName, nPort); //获得服务器名
		if(!pServer)
		{
			sStrErr = _T("GetHttpConnection出错"); 
			break;
		}

		pHttpFile = pServer->OpenRequest(nMethod, strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
		if(!pHttpFile)
		{
			sStrErr = _T("OpenRequest出错"); 
			break;
		}		

		////向服务器发送请求，建立http连接，
		////建立本机上的http文件指针
		//if(!pHttpFile->AddRequestHeaders(szHeaders))
		//{
		//	sStrErr = _T("AddRequestHeaders出错"); 
		//	break;
		//}

		try
		{
			//string szParam = pParam;
// 			if(nMethod == CHttpConnection::HTTP_VERB_POST)
// 			{
// 				szParam = "{\"versionType\":\"3\",\"versionName\":\"V0.0.0.1\"}";
// 			}
			int nParamLen = pParam ? strlen(pParam):0;
			if(!pHttpFile->SendRequest(sHeaders, sHeaders.GetLength(), pParam, nParamLen)) //发送请求
			{
				sStrErr = _T("SendRequest出错"); 
				break;
			}
		}
		catch (...)
		{
			sStrErr = _T("SendRequest出错"); 
			break;
		}	

		DWORD dwStateCode = 0;
		if(!pHttpFile->QueryInfoStatusCode(dwStateCode))
		{
			sStrErr = _T("QueryInfoStatusCode出错"); 
			break;
		}

		if(dwStateCode != HTTP_STATUS_OK)
		{
			sStrErr = _T("访问出错"); 
			break;
		}

		const int nReadBufLen = 1024;		
		char szBuf[nReadBufLen] = {0}; //缓存		
		
		DWORD dwFileSize = 0;

		if(nMethod == CHttpConnection::HTTP_VERB_GET)
		{
			if (!pHttpFile->QueryInfo(
				HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, dwFileSize))
			{ 			
				sStrErr = _T("QueryInfo出错"); 
				break;
			}

			if(dwFileSize <= 0)
			{			
				sStrErr = _T("文件长度错误"); 
				break;
			}
		}

		int nRecvLen = 0;
		int nReadLen = 0;
		int nDataCursor = 0;
		BOOL bSucc = FALSE;

		//读取之前通知一次
		if(nMethod == CHttpConnection::HTTP_VERB_GET)
		{
			if(processnotify)
				processnotify(pUser, lpURL, dwFileSize, nRecvLen);
		}

		try
		{	
			while((nReadLen = pHttpFile->Read(szBuf, nReadBufLen)) > 0)
			{
				if(m_bCancel)
					break;

				bSucc = FALSE;
				nRecvLen += nReadLen;
				if(!pOutData)			
					pOutData = (byte*)malloc(nRecvLen);
				else
					pOutData = (byte*)realloc(pOutData, nRecvLen);
				if(!pOutData)
				{
					sStrErr = _T("内存分配失败");				
					break;
				}			

				memcpy(pOutData + nDataCursor, szBuf, nReadLen);				

				nDataCursor += nReadLen;
				bSucc = TRUE;		

				//读取中再通知
				if(nMethod == CHttpConnection::HTTP_VERB_GET)
				{
					if(processnotify)
						processnotify(pUser, lpURL, dwFileSize, nRecvLen);
				}
			}
		}
		catch (...)
		{
			break;
		}

		if(!bSucc)
			break;

		if(nMethod == CHttpConnection::HTTP_VERB_GET)
		{
			if(nRecvLen != dwFileSize)
			{
				sStrErr = _T("接收文件长度错误");				
				break;
			}
		}

		nOutDataLen = nRecvLen;
		bReturn = TRUE;
	} while (0);


	if(pHttpFile) 
	{
		pHttpFile ->Close();
		delete pHttpFile;
		pHttpFile = NULL;
	}

	if(pServer) 
	{
		pServer ->Close();
		delete pServer;
		pServer = NULL;
	}

	session.Close();

	if(!bReturn)
	{
		SAFE_FREE(pOutData);
		if(lpStrErr)
		{
			lstrcpy((LPWSTR)lpStrErr, sStrErr.GetBuffer());
			sStrErr.ReleaseBuffer();
		}
	}
	return bReturn;
}

/**************************************************************************
* 函数名: Cancel
* 功能: 取消请求
* 时间: 2015/06/12 10:22
* 作者: ZDJ
*/
void CHttpOper::Cancel() 
{ 
	m_bCancel = TRUE;
}
