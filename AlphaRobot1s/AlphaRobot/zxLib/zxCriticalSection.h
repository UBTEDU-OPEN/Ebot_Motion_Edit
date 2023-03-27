/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxCriticalSection.h
* 创建时间：2015/08/26 15:52
* 文件标识：
* 文件摘要：临界区
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/08/26 15:52
* 版本摘要：
*/
#ifndef ZXCRITICALSECTION_H
#define ZXCRITICALSECTION_H

#include <QMutex>

namespace zxLib
{
//#pragma region 临界区声明

	class ZXLIB_DLL zxCriticalSection
	{
	public:
		zxCriticalSection();
		virtual ~zxCriticalSection();

		/**************************************************************************
		* 函数名: Lock
		* 功能: 锁定临界区
		* 时间: 2015/08/26 15:51
		* 作者: ZDJ
		*/
		void Lock();

		/**************************************************************************
		* 函数名: Unlock
		* 功能: 解锁临界区
		* 时间: 2015/08/26 15:51
		* 作者: ZDJ
		*/
		void Unlock();

		/**************************************************************************
		* 函数名: TryLock
		* 功能: 尝试锁定
		* 返回值: 锁定成功返回TRUE，失败返回FALSE
		* 时间: 2015/08/27 10:06
		* 作者: ZDJ
		*/
		BOOL TryLock();

	private:
        //CRITICAL_SECTION m_cri_sec;//所有的派生类共享这一临界区
        QMutex   m_cri_sec;
	};

//#pragma endregion
}

#endif
