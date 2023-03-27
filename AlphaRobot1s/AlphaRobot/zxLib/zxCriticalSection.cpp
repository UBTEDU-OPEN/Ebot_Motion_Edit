/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxCriticalSection.cpp
* 创建时间：2015/08/26 15:53
* 文件标识：
* 文件摘要：临界区
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/08/26 15:53
* 版本摘要：
*/
#include "stdafx.h"
#include "zxCriticalSection.h"

namespace zxLib
{

//#pragma region 临界区实现

	zxCriticalSection::zxCriticalSection()
	{
        //InitializeCriticalSection(&m_cri_sec);//只初始化一次
	}

	zxCriticalSection::~zxCriticalSection()
	{
        //DeleteCriticalSection(&m_cri_sec);
	}

	/**************************************************************************
	* 函数名: Lock
	* 功能: 锁定临界区
	* 时间: 2015/08/26 15:51
	* 作者: ZDJ
	*/
	void zxCriticalSection::Lock()
	{
        //EnterCriticalSection(&m_cri_sec);
        m_cri_sec.lock();
	}

	/**************************************************************************
	* 函数名: Unlock
	* 功能: 解锁临界区
	* 时间: 2015/08/26 15:51
	* 作者: ZDJ
	*/
	void zxCriticalSection::Unlock()
	{
        //LeaveCriticalSection(&m_cri_sec);
        m_cri_sec.unlock();
	}

	/**************************************************************************
	* 函数名: TryLock
	* 功能: 尝试锁定
	* 返回值: 锁定成功返回TRUE，失败返回FALSE
	* 时间: 2015/08/27 10:06
	* 作者: ZDJ
	*/
	BOOL zxCriticalSection::TryLock()
	{
//		DWORD dwMilSeconds = 100; //总尝试时间
//		int nTryCount = 10; //尝试次数
//		DWORD dwSleep = dwMilSeconds / nTryCount; //每次尝试后sleep的时间

//		int nSum = 0;
//		while(nSum < nTryCount)
//		{
//			if(TryEnterCriticalSection(&m_cri_sec))
//				return TRUE;

//			Sleep(dwSleep);
//			nSum++;
//		}
//		return FALSE;
        return m_cri_sec.tryLock();
	}

//#pragma endregion
}
