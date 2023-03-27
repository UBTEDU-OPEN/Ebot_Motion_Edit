/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：chandlemotiondata.h
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：修改动作数据
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#ifndef CCHANGEDMOTIONDATA_H
#define CCHANGEDMOTIONDATA_H

#include "ioperation.h"
#include "ubxundoredomanagerconst.h"
#include "datamodel/ActionBlockDataManager.h"

struct  STMotionData
{
public:
    quint64 m_nCurSelectTime;  // 当前选中的时间（粒度）
    CActionBlockDataManager *m_pMotionData;   // 动作数据

public:
    STMotionData()
    {
        m_nCurSelectTime = 0;
        m_pMotionData =NULL;
    }

    ~STMotionData()
    {
        SAFE_DELETE(m_pMotionData);
    }

    void cloneData(int nTime, CActionBlockDataManager *pData)
    {
        if (!pData)
        {
            return;
        }

        m_nCurSelectTime = nTime;
        SAFE_DELETE(m_pMotionData);
        m_pMotionData= new CActionBlockDataManager;
        m_pMotionData->cloneData(pData);
    }
};

class UBXUNDOREDOMANAGER_API CHandleMotionData : public IOperation
{
public:
    explicit CHandleMotionData(Constants::emOperateCode eCode);
    virtual ~CHandleMotionData();

    /************************************
    * 名称: execute
    * 功能: 执行redo操作
    * 参数:
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    virtual bool execute() override;

    /************************************
    * 名称: undo
    * 功能: 撤销操作
    * 参数:
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    virtual bool undo() override;
    
    /************************************
    * 名称: cloneDataBeforeChanged
    * 功能: 拷贝修改前的数据，作为历史数据，以便进行撤销操作
    * 参数: [in ] lParam: 参数1
                 [in ] wParam: 参数2
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    virtual bool cloneDataBeforeChanged(LPARAM lParam, WPARAM wParam) override;
    
    /************************************
    * 名称: cloneDataAfterChanged
    * 功能: 拷贝修改后的数据，以便进行重做操作
    * 参数:[in ] lParam: 参数1
                [in ] wParam: 参数2
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    virtual bool cloneDataAfterChanged(LPARAM lParam, WPARAM wParam) override;

private:
    /************************************
    * 名称: cloneMotionData
    * 功能: 深拷贝所有动作数据
    * 参数: [in] nSelTime: 记录当前选中的时间，单位是粒度
    * 参数: [in] pDataSrc: 源数据
    *           [in] pDataDest: 目的数据
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void  cloneMotionData(quint64 nSelTime, CActionBlockDataManager *pDataSrc, STMotionData **pDataDest);

private:
    STMotionData *m_pDataAferChanged;   // 修改后的动作数据
    STMotionData *m_pDataBeforeChanged;  // 修改前的动作数据
};

#endif // CCHANGEDMOTIONDATA_H
