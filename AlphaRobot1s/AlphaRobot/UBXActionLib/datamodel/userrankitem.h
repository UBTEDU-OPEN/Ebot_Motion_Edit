/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：userrankitem.h
* 创建时间：2016/08/09
* 文件标识：
* 文件摘要：用户推广活动数据结构
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/08
* 版本摘要：
*/

#ifndef USERRANKITEM_H
#define USERRANKITEM_H

#include <QString>

#include "UBXPublic.h"

class UserRankItem
{
public:
    explicit UserRankItem();
    UserRankItem(const UserRankItem &item);
    virtual ~UserRankItem();

    // 设置、获取用户排名
    void setRankOrder(int nRankOrder) { m_nRankOrder = nRankOrder; }
    int getRankOrder() const { return m_nRankOrder; }

    // 获取、设置热度值
    int getActionHotValue() const;
    void setActionHotValue(int nActionHotValue);

    // 获取动作数据
    ActItemData &getActionItemData() { return m_stActionItemData; }

    /************************************
    * 名称: operator =
    * 功能:  复制运算符重载
    * 参数: [in]itemobj 赋值对象
    * 返回:   UserRankItem 返回赋值结果
    * 时间:   2016/08/09
    * 作者:  hwx
    ************************************/
    UserRankItem& operator = (const UserRankItem& itemobj);

private:
    int m_nRankOrder; // 用户热度排名
    int m_nActionHotValue;  // 动作热度值
    ActItemData m_stActionItemData;  // 动作数据
};

Q_DECLARE_METATYPE(UserRankItem)

#endif // USERRANKITEM_H
