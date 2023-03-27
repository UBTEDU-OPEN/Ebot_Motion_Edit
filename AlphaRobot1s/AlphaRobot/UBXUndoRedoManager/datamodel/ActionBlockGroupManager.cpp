
#include "ActionBlockGroupManager.h"

CActionBlockGroupManager::CActionBlockGroupManager(void)
{
	m_fStartTime = 0;
    m_fEndTime = 100;
    m_pBlockData = NULL;
    m_nUnUseBaseID = 0;
    m_nBlockDataLen = 0;
    m_bSelected = false;
    memset(m_strDescription, 0, MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
}


CActionBlockGroupManager::~CActionBlockGroupManager(void)
{
	if (m_pBlockData)
	{
		delete [] m_pBlockData;
		m_pBlockData = NULL;
	}
}

bool CActionBlockGroupManager::isSelected() const
{
    return m_bSelected;
}

void CActionBlockGroupManager::setSelected(bool bSelected)
{
    m_bSelected = bSelected;
}

/**************************************************************************
* 函数名: GetData
* 功能: 获取动作组数据
* 参数: 
*    @[out] ppData: 动作组数据
*    @[out] nLen: 动作组数据长度
* 返回值: 成功返回true,失败返回false
* 时间: 2015/08/10 14:01
* 作者: ZDJ
*/
bool CActionBlockGroupManager::GetData(char** ppData, int &nLen)
{
	vector<char> DataSave;
	// DataLen（int） + BlockNUM(INT) + [BLOCKDATALEN + BALOCKDATA]
	AddVectorData(&DataSave, (char*)&nLen, sizeof(nLen));

	//nt m_ID;			// 模块ID
	AddVectorData(&DataSave, (char*)&m_nID, sizeof(m_nID));
	//int m_StartTime;	// 起始时间
	AddVectorData(&DataSave, (char*)&m_fStartTime, sizeof(m_fStartTime));
	//int m_EndTime;		// 终止时间
	AddVectorData(&DataSave, (char*)&m_fEndTime, sizeof(m_fEndTime));
	//WCHAR	m_strDescription[MAX_DESCRIPTION_SIZE];	// 描述
	AddVectorData(&DataSave, (char*)m_strDescription, sizeof(m_strDescription));


    int nBlockCount = m_ListActionBlockData.count();
	AddVectorData(&DataSave, (char*)&nBlockCount, sizeof(nBlockCount));

	for(int i=0; i<nBlockCount; i++)
	{
        CActionBlockData* pBlockData = m_ListActionBlockData.at(i);
		if (pBlockData == NULL)
			continue;

		char* pBlock = NULL;
		int nBlockDataLen = 0;
		pBlockData->GetData(&pBlock, nBlockDataLen);
		// data lens
		AddVectorData(&DataSave, (char*)&nBlockDataLen, sizeof(nBlockDataLen));
		if (pBlock && nBlockDataLen)
		{
			// data
			AddVectorData(&DataSave, pBlock, nBlockDataLen);
			delete [] pBlock;
		}
	}

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

	return true;
}

/**************************************************************************
* 函数名: SetData
* 功能: 设置动作组数据
* 参数: 
*    @[in ] pData: 动作组数据
*    @[in ] nLen: 动作组数据长度
* 返回值: 成功返回true,失败返回false
* 时间: 2015/08/10 14:01
* 作者: ZDJ
*/
bool CActionBlockGroupManager::SetData(char* pData, int nLen)
{
	// 数据长度（INT） + DATA
    char *pTravelData = pData;

	int nTotalLen = 0;
    UBX_MEMCPY_INC(nTotalLen, pTravelData);
	if (nTotalLen != nLen)
		return false;

    UBX_MEMCPY_INC(m_nID, pTravelData);
	//int n1 = sizeof(m_fStartTime);
	//int n2 = sizeof(int);
    UBX_MEMCPY_INC(m_fStartTime, pTravelData);
    UBX_MEMCPY_INC(m_fEndTime, pTravelData);


    int nSize = sizeof(m_strDescription);  //12/23新加
    memcpy(m_strDescription, pTravelData, nSize);
    pTravelData += nSize;

    QString stTmp = WCHARToQString(m_strDescription,nSize);

	// Block num
	int nBlockNum = 0;
    UBX_MEMCPY_INC(nBlockNum, pTravelData);

	for (int i=0; i<nBlockNum; i++)
	{
		char* pBlockData = NULL;
		int nBlockDataLen = 0;
		// block data len
        UBX_MEMCPY_INC(nBlockDataLen, pTravelData);
		if (nBlockDataLen)
		{
			pBlockData = new char[nBlockDataLen];
            memcpy(pBlockData, pTravelData, nBlockDataLen);
            pTravelData += nBlockDataLen;

			CActionBlockData* pActionData =  new CActionBlockData();
			if (pActionData)
			{
				pActionData->SetData(pBlockData, nBlockDataLen);
				AddLayerBlock(pActionData);

			}
			delete [] pBlockData;
		}
	}

    UBX_MEMCPY_INC(m_nBlockDataLen, pTravelData);//6
	if (m_nBlockDataLen)
	{
		// char* m_pBlockData;
		//AddVectorData(&DataSave, m_pBlockData, m_nBlockDataLen);
		if (m_pBlockData)
		{
			delete [] m_pBlockData;
		}

		m_pBlockData = new char[m_nBlockDataLen];
        memcpy(m_pBlockData, pTravelData, m_nBlockDataLen); //"motion"

//		AddVectorData(&DataSaveTest, m_pBlockData, m_nBlockDataLen);
        pTravelData += m_nBlockDataLen;
	}

    SetUnUseBaseID();

    return true;
}

int m_nID;			// 模块ID
float m_fStartTime;	// 起始时间
float m_fEndTime;		// 终止时间
WCHAR  m_strDescription[MAX_DESCRIPTION_SIZE];	// 描述

char* m_pBlockData;
int m_nBlockDataLen;
int m_nUnUseBaseID;     //动作帧未使用ID基数；
/**************************************************************************
* 函数名: cloneData
* 功能: 拷贝动作组数据
* 参数:
*    @[in ] pActionGroupData: 动作组数据
* 返回值: void
* 时间: 2017/01/09
* 作者: hwx
*/
void CActionBlockGroupManager::cloneData(CActionBlockGroupManager *pActionGroupData)
{
    if (pActionGroupData == this || !pActionGroupData)
    {
        return;
    }

    this->m_bSelected = pActionGroupData->m_bSelected;
    this->m_nID = pActionGroupData->GetID();
    this->m_fStartTime = pActionGroupData->GetStartTime();
    this->m_fEndTime = pActionGroupData->GetEndTime();

    this->m_nUnUseBaseID = pActionGroupData->GetUnUserID();
    this->SetDescription(pActionGroupData->GetDescription());
    pActionGroupData->GetExtBlockData(&m_pBlockData, m_nBlockDataLen);

    // 需要进行深拷贝
    vector<CActionBlockData*> blockDataList;
    pActionGroupData->GetAllBlockData(blockDataList);
    for (int i = 0; i < blockDataList.size(); i++)
    {
        CActionBlockData *pBlockData = new CActionBlockData;
        pBlockData->cloneData(blockDataList[i]);
        AddLayerBlock(pBlockData);
    }
}

/**************************************************************************
* 函数名: SetExtBlockData
* 功能: 设置附加数据
* 参数: 
*    @[in ] nLen: 附加数据长度
*    @[in ] pData: 附加数据缓冲区
* 返回值: void
* 时间: 2015/08/07 13:26
* 作者: ZDJ
*/
void CActionBlockGroupManager::SetExtBlockData(int nLen, const char* pData)
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
* 返回值: 
* 时间: 2015/08/07 13:28
* 作者: ZDJ
*/
void CActionBlockGroupManager::GetExtBlockData(char** ppData, int& nLen)
{
	nLen = m_nBlockDataLen;
	if (nLen)
	{
		*ppData = new char[nLen];
		memcpy(*ppData, m_pBlockData, nLen);
	}
}

/**************************************************************************
* 函数名: AddLayerBlock
* 功能: 增加块
* 参数: 
*    @[in ] pBlockData: 块数据
* 返回值: 成功返回true，失败返回false
* 时间: 2015/08/10 14:14
* 作者: ZDJ
*/
bool CActionBlockGroupManager::AddLayerBlock(CActionBlockData* pBlockData)
{	
	CActionBlockData* itemFound = NULL; 

	if (pBlockData == NULL)
		return false;

    //if (IsExistBlockData(pBlockData->GetID()))
    //	return false;

    m_ListActionBlockData.append(pBlockData);

	return true;
}

/**************************************************************************
* 函数名: DeleteLayerBlock
* 功能: 删除指定块
* 参数: 
*    @[in ] nID: 块ID
* 返回值: 成功返回true，失败返回false
* 时间: 2015/08/10 14:11
* 作者: ZDJ
*/
bool CActionBlockGroupManager::DeleteLayerBlock(int nID)
{

	//Modified Flag
	CActionBlockData* itemFound = NULL; 

    for (int i=0; i<m_ListActionBlockData.count(); i++)
	{
        itemFound = m_ListActionBlockData.at(i);
		if (itemFound && itemFound->GetID() == nID)
		{
            m_ListActionBlockData.removeAt(i);
			delete itemFound;
			return true;
		}
	}

	return false;
}

/**************************************************************************
* 函数名: ClearBlockDataList
* 功能: 清除动作列表
* 时间: 2015/08/10 14:17
* 作者: ZDJ
*/
void CActionBlockGroupManager::ClearBlockDataList()
{
	CActionBlockData* itemFound = NULL; 

    for (int i=0; i<m_ListActionBlockData.count(); i++)
	{
        itemFound = m_ListActionBlockData.at(i);
		if (itemFound )
		{
			delete itemFound;
		}
	}
    while(!m_ListActionBlockData.isEmpty())
    {
        m_ListActionBlockData.removeFirst();
    }
}

///**************************************************************************
//* 函数名: IsExistBlockData
//* 功能: 是否存在指定块
//* 参数:
//*    @[in ] nID: 块ID
//* 返回值: 存在返回true,失败返回false
//* 时间: 2015/08/10 14:19
//* 作者: ZDJ
//*/
//bool CActionBlockGroupManager::IsExistBlockData(int nID)
//{
//	CActionBlockData* itemFound = NULL;

//    for (int i=0; i<m_ListActionBlockData.count(); i++)
//	{
//        itemFound = m_ListActionBlockData.at(i);
//		if (itemFound && itemFound->GetID() == nID)
//		{
//			return true;
//		}
//	}

//	return false;
//}

/**************************************************************************
* 函数名: FindBlockData
* 功能: 查找指定块
* 参数: 
*    @[in ] nID: 块ID
* 返回值: 存在返回块指针,不存在返回NULL
* 时间: 2015/08/10 14:20
* 作者: ZDJ
*/
CActionBlockData* CActionBlockGroupManager::FindBlockData(int nID)
{
	CActionBlockData* itemFound = NULL; 

    for (int i=0; i<m_ListActionBlockData.count(); i++)
	{
        itemFound = m_ListActionBlockData.at(i);
		if (itemFound && itemFound->GetID() == nID)
		{
			return itemFound;
		}
	}

	return NULL;
}

/**************************************************************************
* 函数名: GetActionCount
* 功能: 获取动作数量
* 时间: 2015/08/06 18:17
* 作者: ZDJ
*/
int CActionBlockGroupManager::GetActionCount()
{
    return m_ListActionBlockData.count();
}

bool CActionBlockGroupManager::Connect(void)
{
	return true;
}

/**************************************************************************
* 函数名: ReleaseConnection
* 功能: 清理块
* 时间: 2015/08/10 15:08
* 作者: ZDJ
*/
void CActionBlockGroupManager::ReleaseConnection()
{
	ClearBlockDataList();
}

/**************************************************************************
* 函数名: SetStartTime
* 功能: 设置开始时刻
* 参数: 
*    @[in ] fStartTime: 开始时刻
* 返回值: void
* 时间: 2015/08/10 14:27
* 作者: ZDJ
*/
void CActionBlockGroupManager::SetStartTime(float fStartTime)
{
	m_fStartTime = fStartTime;
}

/**************************************************************************
* 函数名: GetStartTime
* 功能: 获取开始时刻
* 返回值: 开始时刻
* 时间: 2015/08/10 14:27
* 作者: ZDJ
*/
float CActionBlockGroupManager::GetStartTime()
{
	return m_fStartTime;
}

/**************************************************************************
* 函数名: GetEndTime
* 功能: 获取结束时刻
* 参数: void
* 返回值: 结束时刻
* 时间: 2015/08/07 13:30
* 作者: ZDJ
*/
float CActionBlockGroupManager::GetEndTime()
{
 	/*float fEndTime = GetStartTime();
 	for (int i = 0;i<GetActionCount();i++)
 	{
 		CActionBlockData *pData = GetAt(i);
 		if (pData)
 		{
 			fEndTime += pData->GetAllTime();
 		}
 	}
 	if (fEndTime > m_fEndTime)
 	{
 		m_fEndTime = fEndTime;
 	}*/
	return m_fEndTime;
}

/**************************************************************************
* 函数名: SetEndTime
* 功能: 设置结束时刻
* 参数: 
*    @[in ] fEndTime: 结束时刻
* 返回值: void
* 时间: 2015/08/07 13:29
* 作者: ZDJ
*/
void CActionBlockGroupManager::SetEndTime(float fEndTime)
{
	m_fEndTime = fEndTime;
}

///**************************************************************************
//* 函数名: GetCurrentActionTimeSum
//* 功能: 获取当前分组的总时间
//* 返回值: 分组的总时间
//* 时间: 2015/08/10 14:29
//* 作者: ZDJ
//*/
//float CActionBlockGroupManager::GetCurrentActionTimeSum()
//{
//	return this->GetActionAllTimeSum();
//}

/**************************************************************************
* 函数名: GetActionStartTimeByIndex
* 功能: 获取指定块的开始时刻
* 参数: 
*    @[in ] nActionIndex: 指定块的索引
* 返回值: 开始时刻
* 时间: 2015/08/10 14:31
* 作者: ZDJ
*/
float  CActionBlockGroupManager::GetActionStartTimeByIndex(int nActionIndex)
{
    float fActionTime = GetStartTime();
    if (nActionIndex < 0 || nActionIndex > m_ListActionBlockData.count())
	{
        return fActionTime;
	}

	CActionBlockData *pActionData = NULL;
	for (int i = 0 ; i< nActionIndex ; i++)
	{
		pActionData = GetAt(i);
		if (pActionData)
		{
			fActionTime += pActionData->GetAllTime();
		}
	}
	return fActionTime;
}

/**************************************************************************
* 函数名: SetDescription
* 功能: 设置描述信息
* 参数: 
*    @[in ] strDescription: 描述信息
* 返回值: void
* 时间: 2015/08/07 13:22
* 作者: ZDJ
*/
void CActionBlockGroupManager::SetDescription(WCHAR* strDescription)
{
    memset(m_strDescription,0, sizeof(m_strDescription));
    memcpy(m_strDescription, strDescription ,sizeof(m_strDescription));
//    wcsncpy(m_strDescription, strDescription, MAX_DESCRIPTION_SIZE-1);
}

/**************************************************************************
* 函数名: GetDescription
* 功能: 获取描述信息
* 参数: void
* 返回值: 描述信息
* 时间: 2015/08/07 13:23
* 作者: ZDJ
*/
WCHAR* CActionBlockGroupManager::GetDescription()
{
	return m_strDescription;
}

/**************************************************************************
* 函数名: GetAt
* 功能: 获取指定索引的块
* 参数: 
*    @[in ] nIndex: 块索引
* 返回值: 成功返回块数据，失败返回NULL
* 时间: 2015/08/10 14:22
* 作者: ZDJ
*/
CActionBlockData* CActionBlockGroupManager::GetAt(int nIndex)
{
    if(nIndex < 0 || nIndex >= m_ListActionBlockData.count())
	{
		return NULL;
	}
    return m_ListActionBlockData.at(nIndex);
}

/**************************************************************************
* 函数名: indexAt
* 功能: 根据指定动作帧获取索引
* 参数:
*    @[in ] pBlockData: 指定动作帧
* 返回值: 块索引
* 时间: 2017/03/25
* 作者: hwx
*/
int CActionBlockGroupManager::indexAt(CActionBlockData *pBlockData)
{
    for (int i = 0; i < m_ListActionBlockData.size(); i++)
    {
        if (m_ListActionBlockData.at(i) == pBlockData)
        {
            return i;
        }
    }

    return -1;
}

/**************************************************************************
* 函数名: indexAt
* 功能: 根据动作帧ID获取索引
* 参数:
*    @[in ] nID: 指定动作帧ID
* 返回值: 帧索引
* 时间: 2017/03/25
* 作者: hwx
*/
int CActionBlockGroupManager::indexAt(int nID)
{
    CActionBlockData *pBlockData = FindBlockData(nID);
    if (!pBlockData)
    {
        return -1;
    }

    return indexAt(pBlockData);
}

/**************************************************************************
* 函数名: GetID
* 功能: 获取分组ID
* 返回值: 分组ID
* 时间: 2015/08/10 14:25
* 作者: ZDJ
*/
int CActionBlockGroupManager::GetID()
{
	return m_nID;
}

/**************************************************************************
* 函数名: SetID
* 功能: 设置分组ID
* 参数: 
*    @[in ] nID: 分组ID
* 时间: 2015/08/10 14:26
* 作者: ZDJ
*/
void CActionBlockGroupManager::SetID(int nID)
{
	m_nID = nID;
}

/**************************************************************************
* 函数名: GetAllBlockData
* 功能: 获取所有动作
* 参数: 
*    @[out] blockData: 返回的动作列表 
* 返回值: void
* 时间: 2015/08/07 13:23
* 作者: ZDJ
*/
void CActionBlockGroupManager::GetAllBlockData(vector<CActionBlockData*>& blockDataList)
{
	CActionBlockData* itemFound = NULL;
    for (int i=0; i < m_ListActionBlockData.count(); i++)
	{
        itemFound = m_ListActionBlockData.at(i);
		blockDataList.push_back(itemFound);
	}	
}

/**************************************************************************
* 函数名: GetUnUserID
* 功能: 获取一个没有使用过的ID
* 时间: 2015/08/10 14:24
* 作者: ZDJ
*/
int CActionBlockGroupManager::GetUnUserID()
{

    return ++m_nUnUseBaseID;
}

/**************************************************************************
* 函数名: RecalculateTime
* 功能: 重新计算分组的结束时刻
* 时间: 2015/08/06 16:22
* 作者: ZDJ
*/
void CActionBlockGroupManager::RecalculateTime()
{
	float fEndTime = GetStartTime();
	CActionBlockData* itemFound = NULL; 
    for (int i=0; i<m_ListActionBlockData.count(); i++)
	{
        itemFound = m_ListActionBlockData.at(i);
		if (itemFound)
		{
//			QString strDescription = itemFound->GetDescription();
// 			if (strDescription.Compare(L"Recoup"))
// 			{
				fEndTime += itemFound->GetAllTime();
//			}			
		}
	}

	SetEndTime(fEndTime);
}

/**************************************************************************
* 函数名: InsertBeforeId
* 功能: 在当前动作前插入新动作
* 参数: 
*    @[in ] nID: 当前动作id
*    @[in ] pActionBlock: 新动作数据
* 返回值: void
* 时间: 2015/08/07 13:31
* 作者: ZDJ
*/
void CActionBlockGroupManager::InsertBeforeId(int nID, CActionBlockData* pActionBlock)
{
	for (int i=0; i<GetActionCount(); i++)
	{
		CActionBlockData* pBlock = GetAt(i);
		if (pBlock->GetID() == nID)
		{
			if (pActionBlock->GetID() == -1)
			{
				pActionBlock->SetID(GetUnUserID());
			}

            int  pos = m_ListActionBlockData.indexOf(pBlock);
            if (pos < 0)
			{
                m_ListActionBlockData.prepend(pActionBlock);
			}
			else
			{
                m_ListActionBlockData.insert(pos, pActionBlock);
			}
			return;
		}
	}
}

/**************************************************************************
* 函数名: InsertAfterId
* 功能: 在当前动作后插入新动作
* 参数: 
*    @[in ] nID: 当前动作id
*    @[in ] pActionBlock: 新动作数据
* 返回值: void
* 时间: 2015/08/07 13:32
* 作者: ZDJ
*/
void CActionBlockGroupManager::InsertAfterId(int nID, CActionBlockData* pActionBlock)
{
	for (int i=0; i<GetActionCount(); i++)
	{
		CActionBlockData* pBlock = GetAt(i);
		if (pBlock->GetID() == nID)
		{
			if (pActionBlock->GetID() == -1)
			{
				pActionBlock->SetID(GetUnUserID());
			}

            int  pos = m_ListActionBlockData.indexOf(pBlock);
            if (pos < 0)
            {
                m_ListActionBlockData.prepend(pActionBlock);
            }
            else
            {
                m_ListActionBlockData.insert(pos + 1, pActionBlock);
            }
			return;
		}
	}
}

/**************************************************************************
* 函数名: 获取所有的动作的运动时间之和
* 功能: 
* 时间: 2015/08/13 10:52
* 作者: ZDJ
*/
float CActionBlockGroupManager::GetActionRunTimeSum()
{
	float fRunTime = 0;
	for (int i = 0;i<GetActionCount();i++)
	{
		CActionBlockData *pData = GetAt(i);
		if (pData)
		{
			fRunTime += pData->GetRunTime();
		}
	}
	return fRunTime;
}

/**************************************************************************
* 函数名: 获取所有的动作的总时间之和
* 功能: 
* 时间: 2015/08/13 10:52
* 作者: ZDJ
*/
float CActionBlockGroupManager::GetActionAllTimeSum()
{
	float fRunTime = 0;
	for (int i = 0;i<GetActionCount();i++)
	{
		CActionBlockData *pData = GetAt(i);
		if (pData)
		{
			fRunTime += pData->GetAllTime();
		}
	}
    return fRunTime;
}

/**************************************************************************
* 函数名: SetUnUseBaseID
* 功能:设置动作帧未使用ID基数
* 时间: 2016/03/16 20:52
* 作者: 周志平
*/
void CActionBlockGroupManager::SetUnUseBaseID()
{
    int nBaseID = 0;
    CActionBlockData* itemFound = NULL;
    for (int i=0; i<m_ListActionBlockData.count(); i++)
    {
        itemFound = m_ListActionBlockData.at(i);
        if(itemFound->GetID() > nBaseID)
        {
            nBaseID = itemFound->GetID();
        }
    }
    m_nUnUseBaseID = nBaseID;
}
