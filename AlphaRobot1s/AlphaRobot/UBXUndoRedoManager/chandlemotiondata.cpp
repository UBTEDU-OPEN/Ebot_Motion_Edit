/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cchangemotiondata.cpp
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：修改动作数据
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#include "chandlemotiondata.h"
#include "ubxundoredomanager.h"

CHandleMotionData::CHandleMotionData(Constants::emOperateCode eCode) : IOperation(eCode)
  , m_pDataAferChanged(NULL)
  , m_pDataBeforeChanged(NULL)
{

}

CHandleMotionData::~CHandleMotionData()
{
    SAFE_DELETE(m_pDataBeforeChanged);
    SAFE_DELETE(m_pDataAferChanged);
}

/************************************
* 名称: execute
* 功能: 执行操作
* 参数:
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
bool CHandleMotionData::execute()
{
    UBXUndoRedoManager::getInstance()->notifyOberservers(eExecuteRedo, m_eCode, (LPARAM)m_pDataAferChanged, NULL);
    return true;
}

/************************************
* 名称: undo
* 功能: 撤销操作
* 参数:
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
bool CHandleMotionData::undo()
{
    UBXUndoRedoManager::getInstance()->notifyOberservers(eExecuteUndo, m_eCode, (LPARAM)m_pDataBeforeChanged, NULL);
    return true;
}

/************************************
* 名称: cloneDataBeforeChanged
* 功能: 拷贝修改前的数据，作为历史数据，以便进行撤销操作
* 参数:
    *    [in ] lParam: 参数1
    *    [in ] wParam: 参数2
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
bool CHandleMotionData::cloneDataBeforeChanged(LPARAM lParam, WPARAM wParam)
{
    cloneMotionData(wParam, (CActionBlockDataManager *)lParam, &m_pDataBeforeChanged);
    return true;
}

/************************************
* 名称: cloneDataAfterChanged
* 功能: 拷贝修改后的数据，以便进行重做操作
* 参数:
* 参数:
    *    [in ] lParam: 参数1
    *    [in ] wParam: 参数2
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
bool CHandleMotionData::cloneDataAfterChanged(LPARAM lParam, WPARAM wParam)
{
    cloneMotionData(wParam, (CActionBlockDataManager *)lParam, &m_pDataAferChanged);
    return true;
}

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
void CHandleMotionData::cloneMotionData(quint64 nSelTime, CActionBlockDataManager *pDataSrc, STMotionData **pDataDest)
{
    if (!pDataSrc)
    {
        return;
    }

    SAFE_DELETE(*pDataDest);
    *pDataDest = new STMotionData();
    (*pDataDest)->cloneData(nSelTime, pDataSrc);
}
