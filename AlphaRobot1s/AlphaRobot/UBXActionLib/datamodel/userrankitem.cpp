/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：userrankitem.cpp
* 创建时间：2016/03/14
* 文件标识：
* 文件摘要：用户推广活动数据结构
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/08
* 版本摘要：
*/

#include "userrankitem.h"

UserRankItem::UserRankItem()
{
    m_nRankOrder = 0;
    m_nActionHotValue = 0;
}

UserRankItem::UserRankItem(const UserRankItem &item)
{
    this->m_nRankOrder = item.getRankOrder();
    this->m_nActionHotValue = item.getActionHotValue();
    this->m_stActionItemData = item.m_stActionItemData;
}

UserRankItem::~UserRankItem()
{
}

int UserRankItem::getActionHotValue() const
{
    return m_nActionHotValue;
}

void UserRankItem::setActionHotValue(int nActionHotValue)
{
    m_nActionHotValue = nActionHotValue;
}

/************************************
* 名称: operator =
* 功能:  复制运算符重载
* 参数: [in]itemobj 赋值对象
* 返回:   UserRankItem 返回赋值结果
* 时间:   2016/08/09
* 作者:  hwx
************************************/
UserRankItem &UserRankItem::operator =(const UserRankItem &itemobj)
{
    if (&itemobj == this)
    {
        return *this;
    }

    this->m_nRankOrder = itemobj.getRankOrder();
    this->m_nActionHotValue = itemobj.getActionHotValue();
    this->m_stActionItemData = itemobj.m_stActionItemData;

    return *this;
}
