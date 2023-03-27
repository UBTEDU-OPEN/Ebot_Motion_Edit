//// UBXThread.cpp: implementation of the UBXThread class.
////
////////////////////////////////////////////////////////////////////////
//#ifdef WIN32
//	#include <winsock2.h>
//#else

//#include <pthread.h>
//#endif
////#include "UBXBase.h"
//#include "UBXThread.h"


////////////////////////////////////////////////////////////////////////
//// Construction/Destruction
////////////////////////////////////////////////////////////////////////

//UBXThread::UBXThread()
//{
//#ifndef WIN32
//	sem_init(&m_semWaitForStop,0,1);
//	m_thread = -1;
//	m_bThreadStopped=true;
//#else
//	m_dwThreadID=0;
//	m_hThread=NULL;
//	m_evStop = CreateEvent(NULL,TRUE,TRUE,NULL);
//	SetEvent(m_evStop);
//#endif
//}

//UBXThread::~UBXThread()
//{
//#ifndef WIN32
//	sem_destroy(&m_semWaitForStop);
//#else
//	CloseHandle(m_evStop);
//#endif
//}

//bool UBXThread::StartThread()
//{
//#ifndef WIN32
//	if (pthread_create(&m_thread,NULL,InitThreadProc,(void*)this)!=0)
//		return false;
//	sem_wait(&m_semWaitForStop);
//	m_bThreadStopped=false;
//	return true;
//#else
//	//如果线程已经创建，则不需要再创建
//	if (!m_hThread){
//		//创建并启动线程函数
//		m_hThread = CreateThread(
//					NULL,			//the handle cannot be inherited.
//                    0,				//the default Thread Stack Size is set
//                    InitThreadProc,	//线程函数
//                    this,			//线程函数的参数
//                    0,				//使线程函数创建完后立即启动
//                    &m_dwThreadID	//receives the thread identifier
//					);
                
//        }//end if (!m_hThread...

//	if (m_hThread)
//		ResetEvent(m_evStop);

//	return	m_hThread != NULL;
//#endif
//}

//void UBXThread::WaitForStop()
//{
//#ifndef WIN32
//	if (m_bThreadStopped==false)
//	{
//		sem_wait(&m_semWaitForStop);
//		m_bThreadStopped=true;
//		pthread_join(m_thread,NULL);
//	}
//#else
		
//	//WaitForSingleObject(m_evStop,INFINITE);

//	// 返回线程句柄
//	HANDLE hThread = (HANDLE)InterlockedExchange((LONG *)&m_hThread, 0);
//	if (hThread)
//	{
//		// 等待线程终止
//		//WaitForSingleObject(hThread, INFINITE);

//		while(hThread != NULL)
//		{
//			DWORD result = MsgWaitForMultipleObjects(1, &hThread, FALSE, INFINITE, QS_ALLINPUT);
//			if (result == WAIT_OBJECT_0)
//			{
//				//m_hRecvThread = NULL;
//				break;
//			}
//			else
//			{
//				MSG msg;
//				PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
//				TranslateMessage(&msg);
//				DispatchMessage(&msg);
//			}
//		}

//		// 关闭线程句柄
//		CloseHandle(hThread);
//		hThread = NULL;
//	}// end if (hThread...
//#endif
//}

//unsigned long  UBXThread::ThreadProc()
//{
//	ThreadProcMain();
//#ifndef WIN32
//	sem_post(&m_semWaitForStop);
//#else
//	SetEvent(m_evStop);
//#endif
//	return 0;
//}

