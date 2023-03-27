/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：chandletemplatefile.cpp
* 创建时间：2017/01/18
* 文件标识：
* 文件摘要：处理动作模板文件撤销重做功能
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/18
* 版本摘要：
*/

#include "chandletemplatefile.h"
#include "ubxundoredomanager.h"

CHandleTemplateFile::CHandleTemplateFile(Constants::emOperateCode eCode) : IOperation(eCode)
  , m_pDataBeforeChanged(NULL)
  , m_pDataAfterChanged(NULL)
{

}

CHandleTemplateFile::~CHandleTemplateFile()
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
bool CHandleTemplateFile::execute()
{
    UBXUndoRedoManager::getInstance()->notifyOberservers(eExecuteRedo, m_eCode, (LPARAM)(m_pDataAfterChanged), NULL);
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
bool CHandleTemplateFile::undo()
{
    UBXUndoRedoManager::getInstance()->notifyOberservers(eExecuteRedo, m_eCode, (LPARAM)(m_pDataBeforeChanged), NULL);
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
bool CHandleTemplateFile::cloneDataBeforeChanged(LPARAM lParam, WPARAM wParam)
{
    CStringListContainer *pTemplateFiles = (CStringListContainer *)lParam;
    cloneData(pTemplateFiles, &m_pDataBeforeChanged);
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
bool CHandleTemplateFile::cloneDataAfterChanged(LPARAM lParam, WPARAM wParam)
{
    CStringListContainer *pTemplateFiles = (CStringListContainer *)lParam;
    cloneData(pTemplateFiles, &m_pDataAfterChanged);
    return true;
}

/************************************
* 名称: cloneDataAfterChanged
* 功能: 拷贝数据
* 参数:[in ] pSrcData: 拷贝源数据
            [in ] pDesData: 拷贝目的数据
* 返回: void
* 时间:   2017/01/18
* 作者:   hwx
************************************/
void CHandleTemplateFile::cloneData(CStringListContainer *pSrcData, CStringListContainer **pDesData)
{
    if (!pSrcData)
    {
        return;
    }

    SAFE_DELETE(*pDesData);
    *pDesData = new CStringListContainer;
    (*pDesData)->setFiles(pSrcData->getFiles());
}
