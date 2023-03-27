/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：businesspromotedata.h
* 创建时间：2016/08/17
* 文件标识：
* 文件摘要：运营推广活动数据结构
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/17
* 版本摘要：
*/

#include "datamodel/businesspromotedata.h"
#include "UBXPublic.h"

BusinessPromoteData::BusinessPromoteData() : m_nId(-1), m_nType(-1)
{
}

BusinessPromoteData::~BusinessPromoteData()
{
    m_vecUserRankItems.clear();
}

void BusinessPromoteData::addUserRankItem(UserRankItem item)
{
    m_vecUserRankItems.append(item);
}

void BusinessPromoteData::clearRankActionList()
{
    m_vecUserRankItems.clear();
}
