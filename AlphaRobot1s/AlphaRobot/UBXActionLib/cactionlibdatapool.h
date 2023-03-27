/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cactionlibdatapool.h
* 创建时间：2016/09/09
* 文件标识：
* 文件摘要：动作库数据资源池
*
* 当前版本：1.0.0.0
* 作    者：   hwx
* 完成时间：2016/09/09
* 版本摘要：
*/

#ifndef CACTIONLIBDATAPOOL_H
#define CACTIONLIBDATAPOOL_H

#include <QMap>

#include "zxSingletonT.h"
#include "datamodel/businesspromotedata.h"

using namespace zxLib;

class CActionLibDataPool : public zxSingletonT<CActionLibDataPool>
{
public:

    //////////////运营推广活动数据接口//////////////////////////////////////
    // 添加推广活动数据
    void addBusinessPromoteData(BusinessPromoteData *pItem);

    // 根据运营活动id获取运营活动数据
    BusinessPromoteData *getBusinessPromoteItemById(int nId);

    // 根据活动id获取活动名称
    QString getBusinessPromoteName(int nId);

    // 判断是否包含指定key的项
    bool isBusinessPromoteItemExist(BusinessPromoteData *pItem);

    // 获取所有运营活动数据
    QList<BusinessPromoteData*> &getAllBusinessPromoteData() { return m_lstBusinessPromoteData; }
    //////////////运营推广活动数据接口//////////////////////////////////////

protected:
    explicit CActionLibDataPool();
    ~CActionLibDataPool();

private:
    friend CActionLibDataPool* zxSingletonT<CActionLibDataPool>::GetInstance();
    friend void zxSingletonT<CActionLibDataPool>::Release();

    QList<BusinessPromoteData *> m_lstBusinessPromoteData;  // 运营推广活动数据
};

#endif // CACTIONLIBDATAPOOL_H
