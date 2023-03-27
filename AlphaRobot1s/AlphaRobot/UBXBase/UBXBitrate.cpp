#ifdef WIN32
	#include <winsock2.h>
#endif

//#include "UBXBase.h"
#include "UBXBitrate.h"
#include <time.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
UBXBitrate::UBXBitrate()
{
	Reset();
}

UBXBitrate::~UBXBitrate()
{

}

void UBXBitrate::Reset(void)
{
	m_ulTotalTemp=0;
	m_lDurationTemp=0;
	m_lStartTimeTemp=clock();
	m_dbl_kbps=0;
	m_lStartTime=clock();
	m_ulTotal=0;
}

void UBXBitrate::Deliver(unsigned long ulSize)
{
	m_ulTotalTemp+=ulSize;
	m_ulTotal+=ulSize;
}

double UBXBitrate::kbpsCurrent(void)//kbps
{
	m_lDurationTemp=clock()-m_lStartTimeTemp;

	if (m_lDurationTemp)
	{
		m_dbl_kbps=(double)m_ulTotalTemp/(double)m_lDurationTemp*8.000;

		//reset temp
		m_lStartTimeTemp=clock();
		m_ulTotalTemp=0;
	}

	return m_dbl_kbps;
}

double UBXBitrate::kbpsAverage(void)//kbps
{
	long lDuration=tsDuration();

	double dbl_kbps=0;

	if (lDuration>1000)
	{
		dbl_kbps=(double)m_ulTotal/(double)lDuration*8.000;
	}
	return dbl_kbps;
}

unsigned long UBXBitrate::KBTotal(void)//in KB
{
	return m_ulTotal/1024;
}

unsigned long UBXBitrate::tsDuration(void)//in second
{
	return clock()-m_lStartTime;
}