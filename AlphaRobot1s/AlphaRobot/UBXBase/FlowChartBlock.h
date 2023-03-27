#ifndef _FLOWCHARTBLOCK_H_
#define _FLOWCHARTBLOCK_H_

#include "BaseBlock.h"
#include "PortConnetMap.h"
#include "ProcessHandle.h"

class CFlowChartBlock: public CProcessHandle, CBaseBlock
{
public:
	CFlowChartBlock(void);
	~CFlowChartBlock(void);

protected:
    QList<CBaseBlock*> m_BlockList;
    QList<CPortConnetMap*> m_ConnectList;

public:
	virtual void OnOutPut(char* pData, int nLen);
};
#endif  //_FLOWCHARTBLOCK_H_
