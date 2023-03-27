#include "stable.h"
#include "FlowChartBlock.h"


CFlowChartBlock::CFlowChartBlock(void)
{
	m_nBlockType = BLOCK_FLOWCHART;
}


CFlowChartBlock::~CFlowChartBlock(void)
{
}

void CFlowChartBlock::OnOutPut(char* pData, int nLen)
{
	// TODO: CHENLIN
}
