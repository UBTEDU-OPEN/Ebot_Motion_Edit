/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：chandlemusicfile.cpp
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：处理音乐文件加载撤销重做功能
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#include "chandlestring.h"
#include "ubxundoredomanager.h"

CHandleString::CHandleString(Constants::emOperateCode eCode) : IOperation(eCode)
{

}

CHandleString::~CHandleString()
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
bool CHandleString::execute()
{
    QByteArray byteName = m_strDataAfterChanged.toUtf8();
    char *pData = byteName.data();
    char *pFileName = new char[byteName.size() + 1];
    memset(pFileName, 0, byteName.size() + 1);
    memcpy(pFileName, pData, byteName.size());

    UBXUndoRedoManager::getInstance()->notifyOberservers(eExecuteRedo, m_eCode, (LPARAM)(pFileName), NULL);
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
bool CHandleString::undo()
{
    QByteArray byteName = m_strDataBeforeChanged.toUtf8();
    char *pData = byteName.data();
    char *pFileName = new char[byteName.size() + 1];
    memset(pFileName, 0, byteName.size() + 1);
    memcpy(pFileName, pData, byteName.size());
    UBXUndoRedoManager::getInstance()->notifyOberservers(eExecuteRedo, m_eCode, (LPARAM) (pFileName), NULL);
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
bool CHandleString::cloneDataBeforeChanged(LPARAM lParam, WPARAM wParam)
{
    QString strFile((char *)lParam);
    m_strDataBeforeChanged = strFile;
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
bool CHandleString::cloneDataAfterChanged(LPARAM lParam, WPARAM wParam)
{
    QString strFile((char *)lParam);
    m_strDataAfterChanged = strFile;
    return true;
}

