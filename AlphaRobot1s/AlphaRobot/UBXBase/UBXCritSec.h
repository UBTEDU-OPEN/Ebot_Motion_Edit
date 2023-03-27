//UBXCritSec.h
//
#ifndef __UBX_CRITSEC_H__
#define __UBX_CRITSEC_H__

#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>

#ifndef WIN32

#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

#endif

#include "stable.h"

class UBXBASESHARED_EXPORT UBXCritSec
{
public:
    UBXCritSec();
    virtual ~UBXCritSec();

public:
    void Lock();
    void Unlock();

protected:
#ifdef WIN32
    CRITICAL_SECTION m_CritSec;
#else
	pthread_mutex_t		m_pMutex;
#endif
};

class UBXBASESHARED_EXPORT UBXAutoLock
{
public:
    UBXAutoLock(UBXCritSec * plock);
    virtual ~UBXAutoLock();

protected:
    UBXCritSec * m_pLock;
};

class UBXBASESHARED_EXPORT UBXEvent
{
public:
    UBXEvent(bool bManualReset = false);
    virtual ~UBXEvent();

public:
    void Set(void);
    bool Wait(unsigned long ulTimeout);
    void Reset(void);

protected:
#ifdef WIN32
	HANDLE m_hEvent;
#else
	pthread_mutex_t m_mutex;
	pthread_cond_t	m_cond;
	int				m_nEvent;
	bool			m_bManualReset;
#endif
};

#endif
