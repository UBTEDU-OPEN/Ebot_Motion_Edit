#pragma once
#include "actionblockdata.h"
class CMusicBlockData :
	public CActionBlockData
{
public:
	CMusicBlockData(void);
	~CMusicBlockData(void);
	int  GetActualStartTime();
	int  GetActualEndTime();
	void SetActualStartTime(int nActStartTime);
	void SetActualEndTime(int nActEndTime);
protected:
	// ʵ�ʵ���ʼʱ��
	int m_ActualStartTime;
	// ʵ�ʵĽ�ֹʱ��
	int m_ActualEndTime;
};

