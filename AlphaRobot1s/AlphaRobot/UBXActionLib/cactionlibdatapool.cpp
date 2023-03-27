/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cactionlibdatapool.cpp
* 创建时间：2016/09/09
* 文件标识：
* 文件摘要：动作库数据资源池
*
* 当前版本：1.0.0.0
* 作    者：   hwx
* 完成时间：2016/09/09
* 版本摘要：
*/

#include "cactionlibdatapool.h"

void CActionLibDataPool::addBusinessPromoteData(BusinessPromoteData *pItem)
{
    if (m_lstBusinessPromoteData.contains(pItem))
    {
        return;
    }

    m_lstBusinessPromoteData.push_back(pItem);
}

BusinessPromoteData *CActionLibDataPool::getBusinessPromoteItemById(int nId)
{
    for (int i = 0; i < m_lstBusinessPromoteData.size(); i++)
    {
        BusinessPromoteData *pItem = m_lstBusinessPromoteData.at(i);
        if (!pItem)
        {
            continue;
        }

        if (pItem->getId() == nId)
        {
            return pItem;
        }
    }

    return NULL;
}

QString CActionLibDataPool::getBusinessPromoteName(int nId)
{
    BusinessPromoteData *pItem = getBusinessPromoteItemById(nId);
    if (!pItem)
    {
        return "";
    }

    return pItem->getName();
}

bool CActionLibDataPool::isBusinessPromoteItemExist(BusinessPromoteData *pItem)
{
    for (int i = 0; i < m_lstBusinessPromoteData.size(); i++)
    {
        if (m_lstBusinessPromoteData[i] == pItem)
        {
            return true;
        }
    }

    return false;
}

CActionLibDataPool::CActionLibDataPool()
{

}

CActionLibDataPool::~CActionLibDataPool()
{
    for (int i = 0; i < m_lstBusinessPromoteData.size(); i++)
    {
        SAFE_DELETE(m_lstBusinessPromoteData[i]);
    }
    m_lstBusinessPromoteData.clear();
}
