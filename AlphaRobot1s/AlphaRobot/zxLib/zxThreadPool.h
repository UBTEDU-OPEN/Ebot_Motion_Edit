/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxThreadPool.h
* 创建时间：2015/09/01 10:17
* 文件标识：
* 文件摘要：线程池
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/09/01 10:17
* 版本摘要：
*/
#ifndef ZXTHREADPOOL_H
#define ZXTHREADPOOL_H

namespace zxLib
{
	class IThreadTask;
	typedef queue<IThreadTask*>   QueueThreadTaskType; //线程任务队列

//#pragma region 线程任务接口

	//任务接口，用户任务需继承此接口
	class ZXLIB_DLL IThreadTask
	{
	public:
		/**************************************************************************
		* 函数名: DoTask
		* 功能: 执行任务
		* 时间: 2015/09/01 13:32
		* 作者: ZDJ
		*/
		virtual void DoTask() = 0;
	};

//#pragma endregion

//#pragma region 工作线程声明

	class zxWorkThread
	{
	public:
		/**************************************************************************
		* 函数名: zxWorkThread构造函数
		* 功能: 初始化工作线程
		* 参数: 
		*    @[in ] pTaskQueue: 任务队列	
		* 时间: 2015/09/01 13:35
		* 作者: ZDJ
		*/
		zxWorkThread(QueueThreadTaskType* pTaskQueue);

		virtual ~zxWorkThread();

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
		static UINT STDCALL WorkThreadProc(LPVOID lpParam);
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
        static void* WorkThreadProc(LPVOID lpParam);
#endif

		/**************************************************************************
		* 函数名: Stop
		* 功能: 停止工作线程
		* 时间: 2015/09/01 13:56
		* 作者: ZDJ
		*/
		void Stop();

		/**************************************************************************
		* 函数名: Start
		* 功能: 启动工作线程
		* 时间: 2015/09/01 13:56
		* 作者: ZDJ
		*/
		void Start();

		/**************************************************************************
		* 函数名: Pause
		* 功能: 暂停工作线程
		* 时间: 2015/09/01 13:56
		* 作者: ZDJ
		*/
		void Pause();

		/**************************************************************************
		* 函数名: Resume
		* 功能: 恢复工作线程
		* 时间: 2015/09/14 2:27
		* 作者: zdj
		*/
		void Resume();

	public:
		QueueThreadTaskType* m_pTaskQueue;              //任务队列地址
#ifdef Q_OS_WIN
		HANDLE               m_hRunEvent;               //启动暂停事件
		HANDLE               m_hQuitEvent;              //退出事件
		HANDLE               m_hProcessThreadHandle;    //处理线程句柄
#else
        //sem_t                  m_hRunEvent;               //启动暂停事件
        //sem_t                  m_hQuitEvent;              //退出事件
        BOOL                   m_bQuit;
        pthread_t               m_hProcessThreadHandle;    //处理线程句柄
        //pthread_mutex_t          m_mutex;
        void*                   m_status;
#endif
		UINT                 m_nProcessThreadId;        //处理线程ID
	};

//#pragma endregion

//#pragma region 线程池声明

	class ZXLIB_DLL zxThreadPool
	{
		friend class  zxSingletonT<zxThreadPool>;
	public:
		zxThreadPool(void);
		virtual ~zxThreadPool(void);

		/**************************************************************************
		* 函数名: Init
		* 功能: 初始化线程池
		* 时间: 2015/09/01 10:32
		* 作者: ZDJ
		*/
		void Init();

		/**************************************************************************
		* 函数名: Release
		* 功能: 释放线程池
		* 时间: 2015/09/01 10:32
		* 作者: ZDJ
		*/
		void Release();

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
		void AddTask(IThreadTask* pTask);

		/**************************************************************************
		* 函数名: Run
		* 功能: 执行任务
		* 时间: 2015/09/01 11:19
		* 作者: ZDJ
		*/
		void Run();		

		/**************************************************************************
		* 函数名: Stop
		* 功能: 停止执行任务
		* 时间: 2015/09/01 11:19
		* 作者: ZDJ
		*/
		void Stop();		

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
		static UINT STDCALL ThreadProcMgr(LPVOID lpParam);
#else
        static void* ThreadProcMgr(LPVOID lpParam);
#endif
		/**************************************************************************
		* 函数名: GetTaskCount
		* 功能: 获取任务数量
		* 时间: 2015/09/24 21:31
		* 作者: zdj
		*/
		int GetTaskCount();

	private:
		QueueThreadTaskType   m_queThreadTask;       //线程任务队列

		typedef set<zxWorkThread*>  SetWorkThread; 
		SetWorkThread         m_setWorkThread;        //工作线程集合
#ifdef Q_OS_WIN
		HANDLE                m_hThreadWorkMgr;      //工作线程管理线程
#else
        pthread_t                m_hThreadWorkMgr;
        //pthread_mutex_t          m_mutex;
        void*                   m_status;
#endif
		BOOL                  m_bThreadWorkMgrQuit;  //工作线程管理线程退出标识

		int                   m_nWorkThreadMinCount; //最少工作线程数
		int                   m_nWorkThreadMaxCount; //最多工作线程数

	};

//#pragma endregion
}

#endif

