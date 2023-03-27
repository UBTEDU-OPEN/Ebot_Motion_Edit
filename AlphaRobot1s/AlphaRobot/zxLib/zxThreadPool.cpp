/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxThreadPool.cpp
* 创建时间：2015/09/01 10:18
* 文件标识：
* 文件摘要：线程池
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/09/01 10:18
* 版本摘要：
*/
#include "StdAfx.h"
#include "zxThreadPool.h"

namespace zxLib
{
	static zxCriticalSection     g_criWorkThread;       //工作线程集合同步临界区
	static zxCriticalSection     g_criThreadTask;       //线程任务集合同步临界区

//#pragma region 工作线程实现

	/**************************************************************************
	* 函数名: zxWorkThread构造函数
	* 功能: 初始化工作线程
	* 参数: 
	*    @[in ] pTaskQueue: 任务队列	
	* 时间: 2015/09/01 13:35
	* 作者: ZDJ
	*/
	zxWorkThread::zxWorkThread(QueueThreadTaskType* pTaskQueue)
	{
        m_pTaskQueue = pTaskQueue;
#ifdef Q_OS_WIN
		m_hRunEvent = NULL;
		m_hQuitEvent = NULL;
		m_hProcessThreadHandle = NULL;	
#else
        //sem_init(&m_hRunEvent, 0, 1);
        //sem_init(&m_hQuitEvent, 0, 0);
        m_hProcessThreadHandle = 0;
        m_status = 0;
        m_bQuit = FALSE;
        //pthread_mutex_init(&m_mutex, NULL);
#endif
	}

	zxWorkThread::~zxWorkThread()
	{
		Stop();
	}

#ifdef Q_OS_WIN
	/**************************************************************************
	* 函数名: WorkThreadProc
	* 功能: 工作线程执行函数
	* 参数: 
	*    @[in ] lpParam: 函数参数
	* 返回值: 线程执行结果
	* 时间: 2015/09/01 13:55
	* 作者: ZDJ
	*/
	UINT STDCALL zxWorkThread::WorkThreadProc(LPVOID lpParam)
	{
		zxWorkThread* pWorkThread = (zxWorkThread*)lpParam;
		while(pWorkThread)
		{	
			HANDLE hHandleArr[] = {pWorkThread->m_hRunEvent, pWorkThread->m_hQuitEvent};
			int nHandleCount = sizeof(hHandleArr) / sizeof(HANDLE);
			DWORD dwRet = WaitForMultipleObjects(nHandleCount, hHandleArr, FALSE, 100);
			if(dwRet == WAIT_TIMEOUT)
			{
				//pause
			}
			else if(dwRet == WAIT_OBJECT_0)  
			{
				//run
				g_criThreadTask.Lock();

				if(pWorkThread->m_pTaskQueue && pWorkThread->m_pTaskQueue->size() > 0)
				{
					IThreadTask* pTask = pWorkThread->m_pTaskQueue->front();
					pTask->DoTask();
					SAFE_DELETE(pTask);
					pWorkThread->m_pTaskQueue->pop();
				}

				g_criThreadTask.Unlock();
			}
			else if(dwRet == WAIT_OBJECT_0 + 1)  
			{
				//quit
				g_criThreadTask.Lock();

				while(pWorkThread->m_pTaskQueue && pWorkThread->m_pTaskQueue->size() > 0)
				{
					IThreadTask* pTask = pWorkThread->m_pTaskQueue->front();					
					SAFE_DELETE(pTask);
					pWorkThread->m_pTaskQueue->pop();
				}

				g_criThreadTask.Unlock();

#ifndef QT_NO_DEBUG
                print_msg("WorkThreadProc %08X quit\n", pWorkThread);
#endif

				break;
			}
			else if(dwRet == WAIT_FAILED)  
			{
				//error
#ifndef QT_NO_DEBUG
                print_msg("WorkThreadProc wait quit error %08X\n", GetLastError());
#endif
				break;
			}
		}

		return 0;
	}
#else
    /**************************************************************************
    * 函数名: WorkThreadProc
    * 功能: 工作线程执行函数
    * 参数:
    *    @[in ] lpParam: 函数参数
    * 返回值: 线程执行结果
    * 时间: 2015/09/01 13:55
    * 作者: ZDJ
    */
    void* zxWorkThread::WorkThreadProc(LPVOID lpParam)
    {
        zxWorkThread* pWorkThread = (zxWorkThread*)lpParam;
        while(pWorkThread)
        {
            if(pWorkThread->m_bQuit)
                break;

            IThreadTask* pTask = NULL;

            g_criThreadTask.Lock();

            if(pWorkThread->m_pTaskQueue && pWorkThread->m_pTaskQueue->size() > 0)
            {
                pTask = pWorkThread->m_pTaskQueue->front();
                pWorkThread->m_pTaskQueue->pop();
            }

            g_criThreadTask.Unlock();

            if(pTask)
            {
                pTask->DoTask();
                SAFE_DELETE(pTask);
            }

            Sleep(50);
        }
//#ifndef QT_NO_DEBUG
//        print_msg("WorkThreadProc %08X quit\n", pWorkThread);
//#endif
        return 0;
    }

#endif

	/**************************************************************************
	* 函数名: Stop
	* 功能: 停止工作线程
	* 时间: 2015/09/01 13:56
	* 作者: ZDJ
	*/
	void zxWorkThread::Stop()
	{
#ifdef Q_OS_WIN
		ResetEvent(m_hRunEvent);
		SetEvent(m_hQuitEvent);

		WaitForSingleObject(m_hProcessThreadHandle, INFINITE);
		SAFE_CLOSEHANDLE(m_hProcessThreadHandle);		
		SAFE_CLOSEHANDLE(m_hRunEvent);
		SAFE_CLOSEHANDLE(m_hQuitEvent);
#else
        m_bQuit = TRUE;
        pthread_join(m_hProcessThreadHandle, &m_status);
        m_hProcessThreadHandle = NULL;

#endif

#ifndef QT_NO_DEBUG
        print_msg("workthread %08X stop\n", this);
#endif
	}

	/**************************************************************************
	* 函数名: Start
	* 功能: 启动工作线程
	* 时间: 2015/09/01 13:56
	* 作者: ZDJ
	*/
	void zxWorkThread::Start()
	{
		if(!m_hProcessThreadHandle)
		{
#ifdef Q_OS_WIN
			m_hRunEvent = CreateEvent(NULL, TRUE, TRUE, NULL);			
			m_hQuitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);			
            m_hProcessThreadHandle = (HANDLE)_beginthreadex(NULL, 0, WorkThreadProc, this, 0, &m_nProcessThreadId);
#else
            m_bQuit = FALSE;
            pthread_create(&m_hProcessThreadHandle, NULL, WorkThreadProc, this);
#endif
		}

#ifndef QT_NO_DEBUG
        print_msg("workthread %08X start\n", this);
#endif
    }

//#pragma endregion

//#pragma region 线程池实现

	zxThreadPool::zxThreadPool(void)
	{
		Init();
	}

	zxThreadPool::~zxThreadPool(void)
	{
		Release();
	}

	/**************************************************************************
	* 函数名: Init
	* 功能: 初始化线程池
	* 时间: 2015/09/01 10:32
	* 作者: ZDJ
	*/
	void zxThreadPool::Init()
	{
		m_nWorkThreadMinCount = 1;
		m_nWorkThreadMaxCount = 20;	

		for(int i = 0; i < m_nWorkThreadMinCount; i++)
		{
			zxWorkThread* pNewWorkThread = new zxWorkThread(&m_queThreadTask);		
			m_setWorkThread.insert(pNewWorkThread);		
		}

		m_bThreadWorkMgrQuit = FALSE;
#ifdef Q_OS_WIN
		m_hThreadWorkMgr = (HANDLE)_beginthreadex(NULL, 0, ThreadProcMgr, this, 0, NULL);
#else
        m_status = 0;
        pthread_create(&m_hThreadWorkMgr, NULL, ThreadProcMgr, this);
#endif
	}

	/**************************************************************************
	* 函数名: Release
	* 功能: 释放线程池
	* 时间: 2015/09/01 10:32
	* 作者: ZDJ
	*/
	void zxThreadPool::Release()
	{
		//退出线程管理线程
		m_bThreadWorkMgrQuit = TRUE;
#ifdef Q_OS_WIN
		WaitForSingleObject(m_hThreadWorkMgr, INFINITE);
		SAFE_CLOSEHANDLE(m_hThreadWorkMgr);		
#else
        pthread_join(m_hThreadWorkMgr, &m_status);
        m_hThreadWorkMgr = NULL;
#endif
        //删除所有任务
        g_criThreadTask.Lock();
        while(m_queThreadTask.size() > 0)
        {
            IThreadTask* pTask = m_queThreadTask.front();
            SAFE_DELETE(pTask);
            m_queThreadTask.pop();
        }
        g_criThreadTask.Unlock();

        //删除所有工作线程
        g_criWorkThread.Lock();
        while(m_setWorkThread.size() > 0)
        {
            SetWorkThread::iterator itor = m_setWorkThread.begin();

            zxWorkThread* pThread = *itor;
            SAFE_DELETE(pThread);
            m_setWorkThread.erase(itor);
        }
        g_criWorkThread.Unlock();
	}

	/**************************************************************************
	* 函数名: AddTask
	* 功能: 添加任务
	* 参数: 
	*    @[in ] pTask: 要添加的任务
	*    @[in ] lpParam: 回调函数参数
	* 返回值: void
	* 时间: 2015/09/01 11:19
	* 作者: ZDJ
	*/
	void zxThreadPool::AddTask(IThreadTask* pTask)
	{
        //g_criThreadTask.Lock();
		m_queThreadTask.push(pTask);
        //g_criThreadTask.Unlock();

#ifndef QT_NO_DEBUG
        //printf("addtask %08X\n", pTask);
#endif
	}

	/**************************************************************************
	* 函数名: Run
	* 功能: 执行任务
	* 时间: 2015/09/01 11:19
	* 作者: ZDJ
	*/
	void zxThreadPool::Run()
	{
		SetWorkThread::iterator itor = m_setWorkThread.begin();
		for(; itor != m_setWorkThread.end(); itor++)
		{
			zxWorkThread* pThread = *itor;
			pThread->Start();
		}	
    }

	/**************************************************************************
	* 函数名: Stop
	* 功能: 停止执行任务
	* 时间: 2015/09/01 11:19
	* 作者: ZDJ
	*/
	void zxThreadPool::Stop()
    {
        //删除所有任务
        g_criThreadTask.Lock();
        while(m_queThreadTask.size() > 0)
        {
            IThreadTask* pTask = m_queThreadTask.front();
            SAFE_DELETE(pTask);
            m_queThreadTask.pop();
        }
        g_criThreadTask.Unlock();

        //停止所有工作线程
		SetWorkThread::iterator itor = m_setWorkThread.begin();
		for(; itor != m_setWorkThread.end(); itor++)
		{
			zxWorkThread* pThread = *itor;
			pThread->Stop();			
		}        
	}	

	/**************************************************************************
	* 函数名: ThreadProcMgr
	* 功能: 工作线程管理处理函数
	* 参数: 
	*    @[in ] lpParam: 函数参数
    * 返回值: 线程处理结果
	* 时间: 2015/09/01 16:57
	* 作者: ZDJ
	*/
#ifdef Q_OS_WIN
	UINT STDCALL zxThreadPool::ThreadProcMgr(LPVOID lpParam)
#else
    void* zxThreadPool::ThreadProcMgr(LPVOID lpParam)
#endif
	{
		zxThreadPool* pThreadPool = (zxThreadPool*)lpParam;		

		while(1)
		{
			if(pThreadPool->m_bThreadWorkMgrQuit)
				break;

			g_criWorkThread.Lock();

			int nThreadCount = pThreadPool->m_setWorkThread.size(); //工作线程数
			int nTaskCount = pThreadPool->m_queThreadTask.size();   //任务数
			if(nTaskCount > 10)
			{
				if(nThreadCount < pThreadPool->m_nWorkThreadMaxCount)
				{
					zxWorkThread* pNewWorkThread = new zxWorkThread(&pThreadPool->m_queThreadTask);			
					pThreadPool->m_setWorkThread.insert(pNewWorkThread);
					pNewWorkThread->Start();
				}
			}
			else
			{
				if(nThreadCount > pThreadPool->m_nWorkThreadMinCount)
				{
					zxWorkThread* pWorkThread = *pThreadPool->m_setWorkThread.begin();				
					pThreadPool->m_setWorkThread.erase(pWorkThread);
					SAFE_DELETE(pWorkThread);
				}
			}		

			g_criWorkThread.Unlock();

#ifndef QT_NO_DEBUG
            print_msg("threadcount:%d  taskcount:%d\n", pThreadPool->m_setWorkThread.size(), pThreadPool->m_queThreadTask.size());
#endif
			Sleep(1000);
		}

		return 0;
	}

	/**************************************************************************
	* 函数名: GetTaskCount
	* 功能: 获取任务数量
	* 时间: 2015/09/24 21:31
	* 作者: zdj
	*/
	int zxThreadPool::GetTaskCount()
	{
		return m_queThreadTask.size();
	}

//#pragma endregion

}
