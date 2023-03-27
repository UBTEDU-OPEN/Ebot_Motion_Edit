#include "ubxundoredomanagerconst.h"
#include "ActionBlockDataManager.h"

bool CActionBlockDataManager::GetData(char** ppData, int &nLen)
{
    vector<char> DataSave;
    // DataLen（int） + BlockNUM(INT) + [BLOCKDATALEN + BALOCKDATA]
    AddVectorData(&DataSave, (char*)&nLen, sizeof(nLen));
    int nBlockCount = m_setActionBlockData.size();
    AddVectorData(&DataSave, (char*)&nBlockCount, sizeof(nBlockCount));

    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();
    //for(int i=0; i<nBlockCount; i++)
    for(; itor != m_setActionBlockData.end(); itor++)
    {
        CActionBlockGroupManager* pBlockData = *itor;
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
bool CActionBlockDataManager::SetData(char* pData, int nLen)
{
    if (!pData || nLen <= 0)
    {
        return false;
    }

    // 数据长度（INT） + DATA
    int nTotalLen = 0;
    UBX_MEMCPY_INC(nTotalLen, pData);
    if (nTotalLen != nLen)
        return false;

    // Block num
    int nBlockNum = 0;
    UBX_MEMCPY_INC(nBlockNum, pData);

    for (int i=0; i<nBlockNum; i++)
    {
        char* pBlockData = NULL;
        int nBlockDataLen = 0;
        // block data len
        UBX_MEMCPY_INC(nBlockDataLen, pData);
        if (nBlockDataLen)
        {
            pBlockData = new char[nBlockDataLen];
            memcpy(pBlockData, pData, nBlockDataLen);
            pData += nBlockDataLen;

            CActionBlockGroupManager* pActionData =  new CActionBlockGroupManager();
            if (pActionData)
            {
                pActionData->SetData(pBlockData, nBlockDataLen);
                AddLayerBlock(pActionData);                
            }
            delete [] pBlockData;
        }
    }

    SetUnUseBaseID();

    return true;
}

void CActionBlockDataManager::cloneData(CActionBlockDataManager *pBlockDataManager)
{
    if (!pBlockDataManager || pBlockDataManager == this)
    {
        return;
    }

    this->m_nUnUseBaseID = pBlockDataManager->GetUnUserID();

    // 需要进行深拷贝
    vector<CActionBlockGroupManager*> vecAllActionGroup;
    pBlockDataManager->GetAllBlockData(vecAllActionGroup);
    for (int i = 0; i < vecAllActionGroup.size(); i++)
    {
        CActionBlockGroupManager *pGroupData = new CActionBlockGroupManager;
        pGroupData->cloneData(vecAllActionGroup[i]);
        AddLayerBlock(pGroupData);
    }
}

//******************************************************************************//
//添加块
bool CActionBlockDataManager::AddLayerBlock(CActionBlockGroupManager* pBlock)
{
    //Modified Flag
    CActionBlockGroupManager* itemFound = NULL;

    if (pBlock == NULL)
        return false;

    //if (IsExistBlockData(pBlock->GetID()))
    //    return false;

    m_setActionBlockData.insert(pBlock);

    // 排序
    //SortBlockDataManager();

    return true;
}

//bool CActionBlockDataManager::AddLayerBlockNoSort(CActionBlockGroupManager* pLayer)
//{
//    CActionBlockGroupManager* itemFound = NULL;

//    if (pLayer == NULL)
//        return false;

//    //if (IsExistBlockData(pLayer->GetID()))
//    //    return false;

//    m_setActionBlockData.insert(pLayer);

//    return true;
//}

int CActionBlockDataManager::GetIndexByID(int nID)
{
    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();
    for(int i = 0; itor != m_setActionBlockData.end(); itor++, i++)
    {
        CActionBlockGroupManager* pBlock = *itor;
        if(pBlock->GetID() == nID)
            return i;
    }

    return -1;
}

SetActionBlockDataType::const_iterator CActionBlockDataManager::FirstGroupIndex(float fStartTime)
{
    CActionBlockGroupManager temp;
    temp.SetStartTime(fStartTime);
    SetActionBlockDataType::const_iterator iter = m_setActionBlockData.lower_bound(&temp);
    if(iter != m_setActionBlockData.begin())
    {
        iter--;
    }
    return iter;
}

SetActionBlockDataType::const_iterator CActionBlockDataManager::EndIndex()
{
    return m_setActionBlockData.end();
}

//删除块
bool CActionBlockDataManager::DeleteLayerBlock(int nID)
{
    //Modified Flag
    CActionBlockGroupManager* itemFound = NULL;

    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();
    for(; itor != m_setActionBlockData.end(); itor++)
    {
        itemFound = *itor;
        if (itemFound && itemFound->GetID() == nID)
        {
            itemFound->ReleaseConnection();
            delete itemFound;
            m_setActionBlockData.erase(itor);
            return true;
        }
    }

    return false;
}


//-------------------------------------------------------------------------------------------------
CActionBlockDataManager::CActionBlockDataManager(void)
{
    m_nUnUseBaseID = 0;
}


CActionBlockDataManager::~CActionBlockDataManager(void)
{

}

void CActionBlockDataManager::ClearBlockDataList()
{
    CActionBlockGroupManager* itemFound = NULL;
    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();
    for(; itor != m_setActionBlockData.end(); itor++)
    {
        itemFound = *itor;
        if (itemFound )
        {
            itemFound->ReleaseConnection();
            delete itemFound;
        }
    }
    m_setActionBlockData.clear();
}

//bool CActionBlockDataManager::IsExistBlockData(int nID)
//{
//    CActionBlockGroupManager* itemFound = NULL;

//    for (int i=0; i<m_ListActionBlockData.count(); i++)
//    {
//        itemFound = m_ListActionBlockData.at(i);
//        if (itemFound && itemFound->GetID() == nID)
//        {
//            return true;
//        }
//    }

//    return false;
//}

CActionBlockGroupManager* CActionBlockDataManager::FindBlockData(int nID)
{
    CActionBlockGroupManager* itemFound = NULL;

    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();
    for(; itor != m_setActionBlockData.end(); itor++)
    {
        itemFound = *itor;
        if (itemFound && itemFound->GetID() == nID)
        {
            return itemFound;
        }
    }

    return NULL;
}

CActionBlockGroupManager* CActionBlockDataManager::GetFinalBlockData()
{
    CActionBlockGroupManager* itemFound = NULL;
    if (m_setActionBlockData.size() <= 0)
    {
        return NULL;
    }
    SetActionBlockDataType::iterator itor = m_setActionBlockData.end();
    itor--;

    itemFound = *itor;
    if (itemFound != NULL)
    {
        return itemFound;
    }

    return NULL;
}


bool CActionBlockDataManager::GetAllBlockData(vector<CActionBlockGroupManager*> &blockData)
{
    CActionBlockGroupManager* itemFound = NULL;
    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();
    for(; itor != m_setActionBlockData.end(); itor++)
    {
        itemFound = *itor;
        blockData.push_back(itemFound);
    }

    return true;
}

int CActionBlockDataManager::GetCount()
{
    return m_setActionBlockData.size();
}

CActionBlockGroupManager* CActionBlockDataManager::GetAt(int nIndex)
{
    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();
    for(int i = 0; itor != m_setActionBlockData.end(); itor++, i++)
    {
        if(i == nIndex)
            return *itor;
    }
    return NULL;
}

int CActionBlockDataManager::getIndex(const CActionBlockGroupManager *pGroupMgr)
{
    int nIndex = INVALID_INTEGER;
    for (int i = 0;i < GetCount();i++)
    {
        CActionBlockGroupManager *pManager = GetAt(i);
        if (!pManager)
        {
            continue;
        }

        if (pManager == pGroupMgr)
        {
            nIndex = i;
            break;
        }
    }

    return nIndex;
}

CActionBlockGroupManager *CActionBlockDataManager::getNextActionBlockGroupManager(const CActionBlockGroupManager *pActionBlockGroupManager)
{
    if(!pActionBlockGroupManager)
    {
        return NULL;
    }

    const int nIndex = getIndex(pActionBlockGroupManager);
    CActionBlockGroupManager* pActionBlockGroupManagerNext = GetAt(nIndex + 1);
    return pActionBlockGroupManagerNext;
}

int CActionBlockDataManager::GetUnUserID()
{
    return ++m_nUnUseBaseID;
}

/**************************************************************************
* 函数名: SetUnUseBaseID
* 功能:设置动作帧未使用ID基数
* 时间: 2016/03/16 20:52
* 作者: 周志平
*/
void CActionBlockDataManager::SetUnUseBaseID()
{
    int nBaseID = 0;
    CActionBlockGroupManager* itemFound = NULL;
    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();
    for (; itor != m_setActionBlockData.end(); itor++)
    {
        itemFound = *itor;
        if(itemFound->GetID() > nBaseID)
        {
            nBaseID = itemFound->GetID();
        }
    }
    m_nUnUseBaseID = nBaseID;
}

/**************************************************************************
* 函数名:GetUnUseDescription
* 功能:获取没有使用的描述符
* 参数:
* 返回值:QString
* 时间: 2015/12/22 17:22
* 作者: 周志平
**************************************************************************/
QString CActionBlockDataManager::GetUnUseDescription()
{
    QString strDescription;
    int i = 0;
    while (true)
    {
        bool bFound = true;
        strDescription = QString("name %1").arg(i++);
        if (m_setActionBlockData.size() == 0)
            break;

        CActionBlockGroupManager* itemFound = NULL;
        SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();
        for (; itor != m_setActionBlockData.end(); itor++)
        {
            itemFound = *itor;
            if (itemFound == NULL)
                continue;

            WCHAR* pwstrName = itemFound->GetDescription();

            QString strName = WCHARToQString(pwstrName,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
            if (strDescription.compare(strName, Qt::CaseInsensitive) == 0)
            {
                bFound = false;
                break;
            }

        }

        if (bFound)
            break;
    }

    return strDescription;
}

bool CActionBlockDataManager::Connect(void)
{
    return true;
}

void CActionBlockDataManager::ReleaseConnection()
{
    ClearBlockDataList();
}

/**************************************************************************
* 函数名: MoveGroup
* 功能: 移动分组
* 参数:
*    @[in ] pBaseGroup: 起始分组
*    @[in ] fTimeSpan: 移动的时间间隔，正值向右，负值向左
* 返回值: 成功返回true，失败返回false
* 时间: 2016/03/19 12:13
* 作者: ZDJ
*/
bool CActionBlockDataManager::MoveGroup(CActionBlockGroupManager* pBaseGroup, float fTimeSpan)
{
    SetActionBlockDataType::iterator itor = m_setActionBlockData.find(pBaseGroup);
    if(itor != m_setActionBlockData.end())
    {
        for(; itor != m_setActionBlockData.end(); itor++)
        {
            CActionBlockGroupManager* pGroup = *itor;
            float fTimeFrame = pGroup->GetEndTime() - pGroup->GetStartTime(); //动作帧时间
            pGroup->SetStartTime(pGroup->GetStartTime() + fTimeSpan);
            pGroup->SetEndTime(pGroup->GetStartTime() + fTimeFrame);
        }

        return true;
    }

    return false;
}

/**************************************************************************
* 函数名: getSelectGroupFrameArrayIndex
* 功能: 获取指定时间对应的动作组ID，动作帧ID，时间偏移
* 参数:
*    @[in ] nSelectTime: 选中的时间，CActionBlockGroupManager中对应的m_fStartTime等对应，都除以了时间粒度
*    @[in ] nGroupIndex: nGroupIndex，返回的选中的时间所在的动作组数组下标
*    @[in ] nFrameIndex: 返回的选中的时间所在的动作组的动作帧数组下标
*    @[in ] nTimeOffset: 返回选中的时间所在动作帧中的时间偏移
* 返回值:
* 时间: 2016/12/08
* 作者:hwx
*/
void CActionBlockDataManager::getSelectGroupFrameArrayIndex(int nSelectTime, int &nGroupIndex, int &nFrameIndex, int &nTimeOffset)
{
    CActionBlockGroupManager * pManager = NULL;
    bool bCursorInGroup =false;

    for (int i = 0;i < GetCount();i++)
    {
        pManager = GetAt(i);
        if (pManager)
        {
            float fGroupEndTime = pManager->GetEndTime() ;
            if (fGroupEndTime > nSelectTime)
            {
                nGroupIndex = i;
                bCursorInGroup = true;
                break;
            }
        }
    }

    if(!bCursorInGroup)
    {
        nGroupIndex =  -1;
        nFrameIndex = -1;
        nTimeOffset =  -1;
        return;
    }
    CActionBlockData *pActionData = NULL;
    float fTimeSum = 0;
    if (pManager)
    {
        fTimeSum = pManager->GetStartTime();
        for (int i = 0;i<pManager->GetActionCount();i++)
        {
            pActionData = pManager->GetAt(i);
            if (pActionData)
            {
                float fActionEndTime = pActionData->GetAllTime();
                fTimeSum += fActionEndTime;
                if ( fTimeSum > nSelectTime )
                {
                    //nActionIndex
                    nFrameIndex = i;
                    nTimeOffset = nSelectTime - fTimeSum + fActionEndTime;
                    break;
                }
            }
        }
    }
}

/**************************************************************************
* 函数名: getSelectGroupFrameID
* 功能: 获取指定时间对应的动作组ID，动作帧ID，时间偏移
* 参数:
*    @[in ] nSelectTime: 选中的时间，CActionBlockGroupManager中对应的m_fStartTime等对应，都除以了时间粒度
*    @[in ] nGroupID: 返回的选中的时间所在的动作组id
*    @[in ] nFrameID: 返回的选中的时间所在的动作帧id
*    @[in ] nTimeOffset: 返回选中的时间在动作帧中的时间偏移
* 返回值:
* 时间: 2016/12/08
* 作者:hwx
*/
void CActionBlockDataManager::getSelectGroupFrameID(int nSelectTime, int &nGroupID, int &nFrameID, int &nTimeOffset)
{
    int nGroupIndex = 0;
    int nFrameIndex = 0;
    getSelectGroupFrameArrayIndex(nSelectTime, nGroupIndex, nFrameIndex, nTimeOffset);

    CActionBlockGroupManager * pManager = GetAt(nGroupIndex);
    if (!pManager)
    {
        return;
    }

    nGroupID = pManager->GetID();
    CActionBlockData *pBlock = pManager->GetAt(nFrameIndex);
    if (!pBlock)
    {
        return;
    }

    nFrameID = pBlock->GetID();
}

/**************************************************************************
* 函数名: getSelectGroupFrame
* 功能: 获取指定时间对应的动作组ID，动作帧ID，时间偏移
* 参数:
*    @[in ] nSelectTime: 选中的时间，CActionBlockGroupManager中对应的m_fStartTime等对应，都除以了时间粒度
*    @[in ] fStartTime: 从指定的时间开始搜索（已经除以时间粒度）
*    @[in ] nGroupIndex: 返回的选中的时间所在的动作组数组下标
*    @[in ] nFrameIndex: 返回的选中的时间所在的动作组的动作帧数组下标
*    @[in ] nTimeOffset: 返回选中的时间在动作帧中的时间偏移
* 返回值:
* 时间: 2016/12/08
* 作者:hwx
*/
void CActionBlockDataManager::getSelectGroupFrameArrayIndex(int nSelectTime, float fStartTime, int &nGroupIndex, int &nFrameIndex, int &nTimeOffset)
{
    CActionBlockGroupManager* pManager = NULL;
    bool bCursorInGroup = false;

    SetActionBlockDataType::const_iterator iterStart = FirstGroupIndex(fStartTime);
    for(; iterStart != EndIndex(); iterStart++)
    {
        pManager = *iterStart;
        if(pManager)
        {
            float fGroupEndTime = pManager->GetEndTime();
            if(fGroupEndTime > nSelectTime)
            {
                nGroupIndex = GetIndexByID(pManager->GetID());
                bCursorInGroup = true;
                break;
            }
        }
    }

    if(!bCursorInGroup)
    {
        nGroupIndex = -1;
        nFrameIndex = -1;
        nTimeOffset = -1;
        return;
    }
    CActionBlockData* pActionData = NULL;
    float fTimeSum = 0;
    if(pManager)
    {
        fTimeSum = pManager->GetStartTime();
        for(int i = 0; i < pManager->GetActionCount(); i++)
        {
            pActionData = pManager->GetAt(i);
            if(pActionData)
            {
                float fActionEndTime = pActionData->GetAllTime();
                fTimeSum += fActionEndTime;
                if(fTimeSum > nSelectTime)
                {
                    nFrameIndex = i;
                    nTimeOffset = nSelectTime - fTimeSum + fActionEndTime;
                    break;
                }
            }
        }
    }
}

/**************************************************************************
* 函数名: getActionFrameStartTime
* 功能:根据动作组ID和动作帧ID获取动作帧起始时间（粒度，非真实时间）
* 参数:）
*    @[in ] nGroupID: 动作组ID
*    @[in ] nFrameID: 动作帧ID
* 返回值:
* 时间: 2017/03/25
* 作者:hwx
*/
float CActionBlockDataManager::getActionFrameStartTime(int nGroupID, int nFrameID)
{
    CActionBlockGroupManager *pGroupMgr = FindBlockData(nGroupID);
    if (!pGroupMgr)
    {
        return INVALID_INTEGER;
    }

    return pGroupMgr->GetActionStartTimeByIndex(pGroupMgr->indexAt(nFrameID));
}

/**************************************************************************
* 函数名: createDefaultGroupMgr
* 功能: 创建获取使用默认配置的动作组
* 参数:
* 返回值:
* 时间: 2017/03/23
* 作者:hwx
*/
CActionBlockGroupManager *CActionBlockDataManager::createDefaultGroupMgr()
{
    CActionBlockGroupManager *pActionGroupMgr = new CActionBlockGroupManager();
    pActionGroupMgr->SetID(GetUnUserID());

    WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
    memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
    QString strDescription = GetUnUseDescription();
    int nDescriptionLen = QStringToWCHAR(wstrDescription, strDescription);
    pActionGroupMgr->SetDescription(wstrDescription);

    const char *pdatas = Constants::KEY_WORD_MOTION.toStdString().c_str();
    int nLenss = strlen(pdatas);
    pActionGroupMgr->SetExtBlockData(nLenss, pdatas);
    pActionGroupMgr->SetStartTime(0);
    pActionGroupMgr->SetEndTime(0);

    return pActionGroupMgr;
}

QList<int> CActionBlockDataManager::getGroupIDSelected()
{
    QList<int> lstID;
    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();

    for(; itor != m_setActionBlockData.end(); itor++)
    {
        CActionBlockGroupManager* pBlockData = *itor;
        if(pBlockData ->isSelected())
        {
            lstID.push_back(pBlockData->GetID());
        }
    }
    return lstID;
}

int CActionBlockDataManager::getGroupIDSelectedFirst()
{
    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();

    for(; itor != m_setActionBlockData.end(); itor++)
    {
        CActionBlockGroupManager* pBlockData = *itor;
        if(pBlockData ->isSelected())
        {
            return pBlockData->GetID();
        }
    }
    return -1;
}

void CActionBlockDataManager::setGroupIDSelected(int nGroupIDSelect, bool bSelect)
{

    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();

    for(; itor != m_setActionBlockData.end(); itor++)
    {
        CActionBlockGroupManager* pBlockData = *itor;
        if(pBlockData->GetID() == nGroupIDSelect)
        {
            pBlockData->setSelected(bSelect);
        }
    }
}

void CActionBlockDataManager::clearAllGroupIDSelect()
{
    SetActionBlockDataType::iterator itor = m_setActionBlockData.begin();

    for(; itor != m_setActionBlockData.end(); itor++)
    {
        CActionBlockGroupManager* pBlockData = *itor;
        pBlockData->setSelected(false);
    }
}
