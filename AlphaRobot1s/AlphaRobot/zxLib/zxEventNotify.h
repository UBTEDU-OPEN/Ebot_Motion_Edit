/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxEventNotify.h
* 创建时间：2015/08/27 9:01
* 文件标识：
* 文件摘要：事件通知类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/08/27 9:01
* 版本摘要：
*/
#ifndef ZXEVENTNOTIFY_H
#define ZXEVENTNOTIFY_H

#include <QEvent>
#include <QCoreApplication>

namespace zxLib
{ 
	//事件类型
	enum zxEventType
	{
		zxEventTypeThread,	 //线程类型事件
		zxEventTypeWindow,	 //窗口类型事件
	};

	//事件结构
    typedef struct zxEvent : public QEvent
	{
		zxEventType   eEventType;  //事件类型
		long          lReceiver;   //事件接收方, 线程或窗口
        DWORD         dwContext;  //消息上下文
		DWORD         dwMsgId;     //消息id
		WPARAM        wParam;      //参数1
		LPARAM        lParam;      //参数2
        QString       strParam;    //param

        zxEvent(int nEvent) : QEvent((QEvent::Type)nEvent)
        {
            eEventType = zxEventTypeWindow;
            lReceiver = 0;
            dwContext = 0;
            dwMsgId = 0;
            wParam = 0;
            lParam = 0;
            strParam = "";
        }

	}zxEvent;


//#pragma region 事件通知类声明
    class ZXLIB_DLL zxEventNotify
	{
	private:
		friend class zxSingletonT<zxEventNotify>;

		zxEventNotify(void);
		virtual ~zxEventNotify(void);

	public:

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
		zxEvent* Alloc(DWORD dwThreadId, DWORD dwMsgId, WPARAM wParam, LPARAM lParam);

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
		zxEvent* Alloc(HWND hDestWnd, DWORD dwMsgId, WPARAM wParam, LPARAM lParam);

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
        zxEvent* Alloc(QObject* pDestObj, DWORD dwMsgId, WPARAM wParam, LPARAM lParam);
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
		void Recycle(zxEvent* pEvent);

		/**************************************************************************
		* 函数名: PostEvent
		* 功能: 投递事件
		* 参数: 
		*    @[in ] pEvent: 事件地址
		* 返回值: 成功返回TRUE，失败返回FALSE
		* 时间: 2015/08/27 9:18
		* 作者: ZDJ
		*/
		BOOL PostEvent(zxEvent* pEvent);

		/**************************************************************************
		* 函数名: Start
		* 功能: 启动事件通知
		* 时间: 2015/08/27 9:30
		* 作者: ZDJ
		*/
		void Start();

		/**************************************************************************
		* 函数名: Stop
		* 功能: 停止事件通知
		* 时间: 2015/08/27 10:32
		* 作者: ZDJ
		*/
        void Stop();

	private:
		/**************************************************************************
		* 函数名: ReleaseQueue
		* 功能: 释放队列
		* 时间: 2015/08/27 10:45
		* 作者: ZDJ
		*/
		void ReleaseQueue();

		/**************************************************************************
		* 函数名: ThreadProcPost
		* 功能: 投递执行线程
		* 参数: 
		*    @[in ] lpParam: 参数
		* 返回值: 执行完成返回0
		* 时间: 2015/08/27 11:12
		* 作者: ZDJ
		*/
#ifdef Q_OS_WIN
		static UINT STDCALL ThreadProcPost(LPVOID lpParam);
#else
        static void* ThreadProcPost(LPVOID lpParam);
#endif

	private:
        queue<zxEvent*>      m_queEvent;          //事件队列

#ifdef Q_OS_WIN
		HANDLE             m_hThreadPostHandle; //投递线程句柄
        UINT                m_nThreadPostId;	    //投递线程id
#else
        pthread_t              m_hThreadPostHandle;
#endif
	public:
        BOOL                m_bQuit;


	};
//#pragma endregion
}

#endif
