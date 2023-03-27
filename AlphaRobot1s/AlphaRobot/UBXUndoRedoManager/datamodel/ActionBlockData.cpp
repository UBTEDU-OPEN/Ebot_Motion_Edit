/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ActionBlockData.cpp
* 创建时间：2015/12/04 14:00
* 文件标识：
* 文件摘要：动作块数据类实现
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2015/12/04 14:00
* 版本摘要：
*/

#include "ActionBlockData.h"
#include "../Common/UBXPublic.h"


CActionBlockData::CActionBlockData(void)
{
	m_pBlockData = NULL;
	m_fRunTime = 0;
	//m_fFreezeTime = 0;
	m_fAllTime = 0;
	m_ID = -1;
//	memset(m_strDescription, 0, sizeof(m_strDescription));
	m_nBlockDataLen = 0;
}

CActionBlockData::~CActionBlockData(void)
{
	if (m_pBlockData)
	{
		delete [] m_pBlockData;
		m_pBlockData = NULL;
	}
}

/**************************************************************************
* 函数名: SetDescription
* 功能: 设置描述信息
* 参数: 
*    @[in ] strDescription: 描述信息
* 返回值: void
* 时间: 2015/08/07 16:02
* 作者: ZDJ
*/
void CActionBlockData::SetDescription(WCHAR* strDescription)
{
    memset(m_strDescription,0, MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
    memcpy(m_strDescription, strDescription ,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
//    memcpy(m_strDescription, strDescription ,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
//    memcpy(m_strDescription, strDescription, MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
}

/**************************************************************************
* 函数名: GetDescription
* 功能: 获取描述信息
* 参数: void	
* 返回值: 描述信息
* 时间: 2015/08/07 16:02
* 作者: ZDJ
*/
WCHAR* CActionBlockData::GetDescription()
{
	return m_strDescription;
}

/**************************************************************************
* 函数名: GetID
* 功能: 获取动作ID
* 时间: 2015/08/10 13:36
* 作者: ZDJ
*/
int CActionBlockData::GetID()
{
	return m_ID;
}

/**************************************************************************
* 函数名: SetID
* 功能: 设置动作ID
* 参数: 
*    @[in ] nID: 动作ID
* 返回值: void
* 时间: 2015/08/10 13:37
* 作者: ZDJ
*/
void CActionBlockData::SetID(int nID)
{
	m_ID = nID;
}

/**************************************************************************
* 函数名: SetExtBlockData
* 功能: 设置附加数据
* 参数: 
*    @[in ] nLen: 附加数据长度
*    @[in ] pData: 附加数据缓冲区
* 返回值: 
* 时间: 2015/08/07 16:03
* 作者: ZDJ
*/
void CActionBlockData::SetExtBlockData(int nLen, char* pData)
{
	m_nBlockDataLen = nLen;
	if (m_pBlockData)
	{
		delete [] m_pBlockData;
		m_pBlockData = NULL;
	}

	if (nLen)
	{
		m_pBlockData = new char[nLen];
        Q_ASSERT(m_pBlockData);
		memcpy(m_pBlockData, pData, nLen);
	}
}

/**************************************************************************
* 函数名: GetExtBlockData
* 功能: 获取附加数据
* 参数: 
*    @[out] ppData: 附加数据缓冲区
*    @[out] nLen: 附加数据长度	
* 返回值: void
* 时间: 2015/08/07 16:03
* 作者: ZDJ
*/
void CActionBlockData::GetExtBlockData(char** ppData, int& nLen)
{
	nLen = m_nBlockDataLen;
	if (nLen)
	{
		*ppData = new char[nLen];
		memcpy(*ppData, m_pBlockData, nLen);
    }
}

/**************************************************************************
* 函数名: cloneData
* 功能: 拷贝动作帧数据
* 参数:
*    @[in] pActionBlockData: 动作帧数据
* 返回值: void
* 时间: 2017/01/09
* 作者: hwx
*/
void CActionBlockData::cloneData(CActionBlockData *pActionBlockData)
{
    if (pActionBlockData == this || !pActionBlockData)
    {
        return;
    }

    this->m_ID = pActionBlockData->GetID();
    this->m_fRunTime = pActionBlockData->GetRunTime();
    this->m_fAllTime = pActionBlockData->GetAllTime();

    this->SetDescription(pActionBlockData->GetDescription());
     pActionBlockData->GetExtBlockData(&m_pBlockData, m_nBlockDataLen);
}

///**************************************************************************
//* 函数名: GetAllTime
//* 功能: 获取已经变换的总时间
//* 时间: 2015/08/06 13:51
//* 作者: ZDJ
//*/
//float CActionBlockData::GetAllTime()
//{
//	return m_fRunTime + m_fFreezeTime;
//}

/**************************************************************************
* 函数名: GetData
* 功能: 获取动作数据
* 参数: 
*    @[out] ppData: 动作数据缓冲区
*    @[out] nLen: 动作数据长度
* 返回值: 成功返回true，失败返回false
* 时间: 2015/08/10 13:42
* 作者: ZDJ
*/
bool CActionBlockData::GetData(char** ppData, int& nLen)
{
    std::vector<char> DataSave;
	//vector<char> DataSaveTest;
	// 数据长度（INT） + DATA
	AddVectorData(&DataSave, (char*)&nLen, sizeof(nLen));
	//nt m_ID;			// 模块ID
	AddVectorData(&DataSave, (char*)&m_ID, sizeof(m_ID));
	//int m_StartTime;	// 起始时间
	AddVectorData(&DataSave, (char*)&m_fRunTime, sizeof(m_fRunTime));//运行时间
	//int m_EndTime;		// 终止时间
	AddVectorData(&DataSave, (char*)&m_fAllTime, sizeof(m_fAllTime));//总时间
	//WCHAR	m_strDescription[MAX_DESCRIPTION_SIZE];	// 描述
	AddVectorData(&DataSave, (char*)m_strDescription, sizeof(m_strDescription));

	// int m_nBlockDataLen;
	AddVectorData(&DataSave, (char*)&m_nBlockDataLen, sizeof(m_nBlockDataLen));
	if (m_nBlockDataLen)
	{
		// char* m_pBlockData;
		AddVectorData(&DataSave, m_pBlockData, m_nBlockDataLen);
		//AddVectorData(&DataSaveTest, m_pBlockData, m_nBlockDataLen);
	}

	nLen = DataSave.size();
	ModifyVertorData(DataSave, 0, (char*)&nLen, sizeof(nLen));
	if (nLen)
	{
		*ppData = new char[nLen];
        Q_ASSERT(*ppData);
		CopyVertorToPointer(DataSave, *ppData);
	}

//------------------------------------------------------------------------
	/*char* ppDataTest;
	int nLenTest = DataSaveTest.size();
	ModifyVertorData(DataSaveTest, 0, (char*)&nLenTest, sizeof(nLenTest));
	if (nLenTest)
	{
		ppDataTest = new char[nLenTest];
		ASSERT(*ppDataTest);
		CopyVertorToPointer(DataSaveTest, ppDataTest);
	}

	//--------------------------
	if (ppDataTest==NULL || nLenTest <= 4)
		return false;

	int AngleArr[18];
	MOTOR_DATA* pMotor = (MOTOR_DATA*)(ppDataTest+sizeof(nLenTest));
	int nCount = (nLenTest-sizeof(nLenTest))/sizeof(MOTOR_DATA);
	for (int i=0; i<nCount; i++)
	{
		AngleArr[i] = pMotor[i].nAngle;
	}*/
//------------------------------------------------

	return true;
}

/**************************************************************************
* 函数名: SetData
* 功能: 设置动作数据
* 参数: 
*    @[in ] pData: 动作数据缓冲区
*    @[in ] nLen: 动作数据长度
* 返回值: void
* 时间: 2015/08/10 13:44
* 作者: ZDJ
*/
void CActionBlockData::SetData(char* pData, int nLen)
{
    char *pTravelData = pData;

	vector<char> DataSaveTest;

	// 数据长度（INT） + DATA
	int nTotalLen = 0;
    UBX_MEMCPY_INC(nTotalLen, pTravelData);
	if (nTotalLen != nLen)
		return;

	//nt m_ID;			// 模块ID
    UBX_MEMCPY_INC(m_ID, pTravelData);
	//int m_StartTime;	// 起始时间
    UBX_MEMCPY_INC(m_fRunTime, pTravelData); //运行时间
	//int m_EndTime;		// 终止时间
    UBX_MEMCPY_INC(m_fAllTime, pTravelData);//总时间
	//WCHAR	m_strDescription[MAX_DESCRIPTION_SIZE];	// 描述
    memcpy(m_strDescription, pTravelData, sizeof(m_strDescription));
    pTravelData += sizeof(m_strDescription);

	// int m_nBlockDataLen;
    UBX_MEMCPY_INC(m_nBlockDataLen, pTravelData);
	if (m_nBlockDataLen)
	{
		// char* m_pBlockData;
		//AddVectorData(&DataSave, m_pBlockData, m_nBlockDataLen);
		if (m_pBlockData)
		{
			delete [] m_pBlockData;
		}

		m_pBlockData = new char[m_nBlockDataLen];
        memcpy(m_pBlockData, pTravelData, m_nBlockDataLen);

		AddVectorData(&DataSaveTest, m_pBlockData, m_nBlockDataLen);

        pTravelData += m_nBlockDataLen;
	}
}

/**************************************************************************
* 函数名: GetMotorAngle
* 功能: 获取舵机角度
* 参数: 
*    @[in ] nMotroID: 舵机ID
* 返回值: 舵机角度
* 时间: 2015/08/10 13:38
* 作者: ZDJ
*/
int CActionBlockData::GetMotorAngle(int nMotroID)
{
	//char *pData = NULL;
	int nLen = m_nBlockDataLen;
	//GetExtBlockData()
	if (m_pBlockData == NULL || nLen <= 4)
		return -1;

	MOTOR_DATA* pMotor = (MOTOR_DATA*)(m_pBlockData+sizeof(nLen));
	int nCount = (nLen-sizeof(nLen))/sizeof(MOTOR_DATA);
	for (int i=0; i<nCount; i++)
	{
		//CUBXRobotMotorItemDialog* pItem = FindMotor(pMotor[i].nID);
		if (nMotroID == pMotor[i].nID)
		{
			return pMotor[i].nAngle;
		}
	}
	return -1;
}

/**************************************************************************
* 函数名: SetMotorAngle
* 功能: 设置舵机角度
* 参数: 
*    @[in ] pData: 角度数据
*    @[in ] nLen: 角度数据长度
*    @[in ] nID: 舵机ID
*    @[in ] fAngle: 设置角度
* 返回值: 
* 时间: 2015/08/10 13:40
* 作者: ZDJ
*/
void CActionBlockData::SetMotorAngle(char* pData, int nLen, int nID, float fAngle)
{
	int nCount = (nLen-4)/sizeof(MOTOR_DATA);
	MOTOR_DATA* p = (MOTOR_DATA*)(pData+4);

	p[nID].nAngle = (int)fAngle;
}

/**************************************************************************
* 函数名: SetRunTime
* 功能: 设置运行时间
* 参数: 
*    @[in ] fTime: 变换后的运行时间
* 返回值: void
* 时间: 2015/08/07 15:59
* 作者: ZDJ
*/
void CActionBlockData::SetRunTime(float fTime)
{
	m_fRunTime = fTime;
}

/**************************************************************************
* 函数名: GetRunTime
* 功能: 获取运行时间
* 参数: void	
* 返回值: 变换后的运行时间
* 时间: 2015/08/07 16:00
* 作者: ZDJ
*/
float CActionBlockData::GetRunTime()
{
	return m_fRunTime;
}

///**************************************************************************
//* 函数名: SetFreezeTime
//* 功能: 设置冻结时间
//* 参数: 
//*    @[in ] fTime: 变换后的运行时间
//* 返回值: void
//* 时间: 2015/08/07 16:01
//* 作者: ZDJ
//*/
//void CActionBlockData::SetFreezeTime(float fTime)
//{
//	m_fFreezeTime = fTime;
//}	
//
///**************************************************************************
//* 函数名: GetFreezeTime
//* 功能: 获取冻结时间
//* 参数: void	
//* 返回值: 变换后的冻结时间
//* 时间: 2015/08/07 16:00
//* 作者: ZDJ
//*/
//float CActionBlockData::GetFreezeTime()
//{
//	//return m_fFreezeTime;
//	return m_fAllTime - m_fRunTime;
//}

/**************************************************************************
	* 函数名: SetAllTime
	* 功能: 设置总时间
	* 参数: 
	*    @[in ] fTime: 变换后的总时间
	* 返回值: void
	* 时间: 2015/08/07 16:01
	* 作者: ZDJ
	*/
void CActionBlockData::SetAllTime(float fTime)
{
	m_fAllTime = fTime;
}

/**************************************************************************
* 函数名: GetAllTime
* 功能: 获取总时间
* 参数: void	
* 返回值: 变换后的总时间
* 时间: 2015/08/07 16:00
* 作者: ZDJ
*/
float CActionBlockData::GetAllTime()
{
	return m_fAllTime;
}
