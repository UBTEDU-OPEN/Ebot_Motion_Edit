// UBXBitrate.h: interfUBX for the UBXBitrate class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __UBXBitrate_H__
#define __UBXBitrate_H__

class UBXBitrate 
{
public:
	UBXBitrate();
	virtual ~UBXBitrate();
public:
	void Reset(void);
	void Deliver(unsigned long ulSize);
	double kbpsCurrent(void);//kbps
	double kbpsAverage(void);//kbps
	unsigned long KBTotal(void);//in KB
	unsigned long tsDuration(void);//in second
protected:
	unsigned long	m_ulTotal;
	unsigned long	m_ulTotalTemp;
	long			m_lDurationTemp;
	long			m_lStartTimeTemp;
	long			m_lStartTime;
	double			m_dbl_kbps;
};

#endif 