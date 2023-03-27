/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxEventNotify.cpp
* 创建时间：2015/08/27 9:02
* 文件标识：
* 文件摘要：事件通知类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/08/27 9:02
* 版本摘要：
*/
#include "StdAfx.h"
#include "zxEventNotify.h"

namespace zxLib
{
//#pragma region 事件通知类实现
    zxCriticalSection       m_criSec;            //同步队列对象

	zxEventNotify::zxEventNotify(void)
	{
#ifdef Q_OS_WIN
		m_hThreadPostHandle = NULL;
#else
        m_hThreadPostHandle = 0;        
        m_bQuit = FALSE;
#endif
	}


	zxEventNotify::~zxEventNotify(void)
	{
        if(m_hThreadPostHandle)
            Stop();
	}
#ifdef Q_OS_WIN
	/**************************************************************************
	* 函数名: Alloc
	* 功能: 创建新事件
	* 参数: 
	*    @[in ] dwThreadId: 目标线程id
	*    @[in ] dwMsgId: 消息id
	*    @[in ] wParam: 参数1
	*    @[in ] lParam: 参数2
	* 返回值: 事件地址
	* 时间: 2015/08/27 9:15
	* 作者: ZDJ
	*/
	zxEvent* zxEventNotify::Alloc(DWORD dwThreadId, DWORD dwMsgId, WPARAM wParam, LPARAM lParam)
	{
        zxEvent* pNewEvent = new zxEvent(dwMsgId);
		pNewEvent->eEventType = zxEventTypeThread;
		pNewEvent->lReceiver = dwThreadId;
		pNewEvent->dwMsgId = dwMsgId;
		pNewEvent->wParam = wParam;
		pNewEvent->lParam = lParam;

		return pNewEvent;
	}

	/**************************************************************************
	* 函数名: Alloc
	* 功能: 创建新事件
	* 参数: 
	*    @[in ] hDestWnd: 目标窗口句柄
	*    @[in ] dwMsgId: 消息id
	*    @[in ] wParam: 参数1
	*    @[in ] lParam: 参数2
	* 返回值: 事件地址
	* 时间: 2015/08/27 9:15
	* 作者: ZDJ
	*/
	zxEvent* zxEventNotify::Alloc(HWND hDestWnd, DWORD dwMsgId, WPARAM wParam, LPARAM lParam)
	{
        zxEvent* pNewEvent = new zxEvent(dwMsgId);
		pNewEvent->eEventType = zxEventTypeWindow;
		pNewEvent->lReceiver = (long)hDestWnd;
		pNewEvent->dwMsgId = dwMsgId;
		pNewEvent->wParam = wParam;
		pNewEvent->lParam = lParam;

		return pNewEvent;
	}
#else
    /**************************************************************************
    * 函数名: Alloc
    * 功能: 创建新事件
    * 参数:
    *    @[in ] pDestObj: 投递目标
    *    @[in ] dwMsgId: 消息id
    *    @[in ] wParam: 参数1
    *    @[in ] lParam: 参数2
    * 返回值: 事件地址
    * 时间: 2015/08/27 9:15
    * 作者: ZDJ
    */
    zxEvent* zxEventNotify::Alloc(QObject* pDestObj, DWORD dwMsgId, WPARAM wParam, LPARAM lParam)
    {
        zxEvent* pNewEvent = new zxEvent(dwMsgId);
        pNewEvent->eEventType = zxEventTypeThread;
        pNewEvent->lReceiver = (long)pDestObj;
        pNewEvent->dwMsgId = dwMsgId;
        pNewEvent->wParam = wParam;
        pNewEvent->lParam = lParam;

        print_msg("alloc new event %08X\n", pNewEvent);
        return pNewEvent;
    }
#endif

	/**************************************************************************
	* 函数名: Recycle
	* 功能: 回收事件
	* 参数: 
	*    @[in ] pEvent: 事件地址
	* 返回值: void
	* 时间: 2015/08/27 9:16
	* 作者: ZDJ
	*/
	void zxEventNotify::Recycle(zxEvent* pEvent)
	{
		SAFE_DELETE(pEvent);
	}

	/**************************************************************************
	* 函数名: PostEvent
	* 功能: 投递事件
	* 参数: 
	*    @[in ] pEvent: 事件地址
	* 返回值: 成功返回TRUE，失败返回FALSE
	* 时间: 2015/08/27 9:18
	* 作者: ZDJ
	*/
	BOOL zxEventNotify::PostEvent(zxEvent* pEvent)
	{
//        if(m_criSec.TryLock())
//        {
        m_criSec.Lock();
			m_queEvent.push(pEvent);
            m_criSec.Unlock();

			return TRUE;
//        }
//        else
//        {
//            Recycle(pEvent);//入队失败则回收
//            return FALSE;
//        }

	}
#ifdef Q_OS_WIN
	/**************************************************************************
	* 函数名: ThreadProcPost
	* 功能: 投递执行线程
	* 参数: 
	*    @[in ] lpParam: 参数
	* 返回值: 执行完成返回0
	* 时间: 2015/08/27 11:12
	* 作者: ZDJ
	*/
	UINT STDCALL zxEventNotify::ThreadProcPost(LPVOID lpParam)	
#else   
    LPVOID zxEventNotify::ThreadProcPost(LPVOID lpParam)
#endif
    {
        zxEventNotify* pNotify = (zxEventNotify*)lpParam;
        while(pNotify)
        {
            if(pNotify->m_bQuit)
                break;

            m_criSec.Lock();

            //run
            if(pNotify->m_queEvent.size() > 0)
            {
                zxEvent* pEvent = pNotify->m_queEvent.front();
                QCoreApplication::postEvent((QObject*)pEvent->lReceiver, pEvent);

                pNotify->m_queEvent.pop();
                //pNotify->Recycle(pEvent);//投递完成则回收事件
            }

            m_criSec.Unlock();

            Sleep(50);
        }

        return 0;
    }

	/**************************************************************************
	* 函数名: Start
	* 功能: 启动事件通知
	* 时间: 2015/08/27 9:30
	* 作者: ZDJ
	*/
	void zxEventNotify::Start()
	{
		if(!m_hThreadPostHandle)
		{
#ifdef Q_OS_WIN
            //m_hRunProcEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
            //m_hQuitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			m_hThreadPostHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadProcPost, this, 0, &m_nThreadPostId);
#else
            m_bQuit = FALSE;
            pthread_create(&m_hThreadPostHandle, NULL, ThreadProcPost, this);
#endif
		}
	}

	/**************************************************************************
	* 函数名: Stop
	* 功能: 停止事件通知
	* 时间: 2015/08/27 10:32
	* 作者: ZDJ
	*/
	void zxEventNotify::Stop()
	{
		ReleaseQueue();
#ifdef Q_OS_WIN
        //ResetEvent(m_hRunProcEvent);
        //SetEvent(m_hQuitEvent);

		WaitForSingleObject(m_hThreadPostHandle, INFINITE);
		SAFE_CLOSEHANDLE(m_hThreadPostHandle);
        //SAFE_CLOSEHANDLE(m_hRunProcEvent);
        //SAFE_CLOSEHANDLE(m_hQuitEvent);
#else
        m_bQuit = TRUE;
        pthread_join(m_hThreadPostHandle, NULL);
        m_hThreadPostHandle = NULL;
#endif
	}	

	/**************************************************************************
	* 函数名: ReleaseQueue
	* 功能: 释放队列
	* 时间: 2015/08/27 10:45
	* 作者: ZDJ
	*/
	void zxEventNotify::ReleaseQueue()
	{
        m_criSec.Lock();
		while(m_queEvent.size() > 0)
		{
			zxEvent* pEvent = m_queEvent.front();
			Recycle(pEvent);
			m_queEvent.pop();
		}
        m_criSec.Unlock();
	}
//#pragma endregion
}
