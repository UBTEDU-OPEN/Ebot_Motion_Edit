/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：chandleruntime.cpp
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：动作编辑模块配置动作帧运行时间
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#include "chandleint.h"
#include "ubxundoredomanager.h"

CHandleInt::CHandleInt(Constants::emOperateCode eType) : IOperation(eType)
  , m_nDataAfterChanged(0)
  , m_nDataBeforeChanged(0)
{

}

CHandleInt::~CHandleInt()
{

}

/************************************
* 名称: execute
* 功能: 执行redo操作
* 参数:
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
bool CHandleInt::execute()
{
    UBXUndoRedoManager::getInstance()->notifyOberservers(eExecuteRedo, m_eCode, m_nDataAfterChanged, NULL);
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
bool CHandleInt::undo()
{
    UBXUndoRedoManager::getInstance()->notifyOberservers(eExecuteRedo, m_eCode, m_nDataBeforeChanged, NULL);
    return true;
}

/************************************
* 名称: cloneDataBeforeChanged
* 功能: 拷贝修改前的数据，作为历史数据，以便进行撤销操作
* 参数: [in ] lParam: 参数1
             [in ] wParam: 参数2
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
bool CHandleInt::cloneDataBeforeChanged(LPARAM lParam, WPARAM wParam)
{
    m_nDataBeforeChanged = (int)lParam;
    return true;
}

/************************************
* 名称: cloneDataAfterChanged
* 功能: 拷贝修改后的数据，以便进行重做操作
* 参数:[in ] lParam: 参数1
            [in ] wParam: 参数2
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
bool CHandleInt::cloneDataAfterChanged(LPARAM lParam, WPARAM wParam)
{
    m_nDataAfterChanged = (int)lParam;
    return true;
}
