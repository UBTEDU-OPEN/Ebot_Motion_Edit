//// UBXThread.h: interface for the ACEThreadclass.
////
////////////////////////////////////////////////////////////////////////

//#ifndef __ACE_THREAD_H__
//#define __ACE_THREAD_H__

////#include <stdio.h>
////#include <stdlib.h>

//#ifndef WIN32
//#include <semaphore.h>
//#endif

//class UBXThread
//{
//public:
//	UBXThread();
//	virtual ~UBXThread();
//public:
//	bool StartThread(void);
//	void WaitForStop(void);
//	unsigned long ThreadProc(void);

//#ifdef WIN32
//	static DWORD WINAPI UBXThread::InitThreadProc(PVOID pObj){
//		return	((UBXThread*)pObj)->ThreadProc();
//	}
//#else
//	static void* UBXThread::InitThreadProc(void*pObj){
//		((UBXThread*)pObj)->ThreadProc();
//		return 0;
//	}
//#endif

//protected:
//	virtual void ThreadProcMain(void)=0;

//protected:
//#ifdef WIN32
//	DWORD	m_dwThreadID;		// 线程标识
//	HANDLE	m_hThread;			// 线程句柄
//	HANDLE	m_evStop;
//#else
//    pthread_t	m_thread;
//    sem_t		m_semWaitForStop;
//    bool		m_bThreadStopped;
//#endif
//};

//#endif

