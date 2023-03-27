/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ubxundoredomanager.h
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：撤销重做管理模块
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#include <QDebug>

#include "UBXPublic.h"
#include "ubxundoredomanager.h"


const UINT WM_MSG_EXECUTE_UNDO = QEvent::registerEventType();
const UINT WM_MSG_EXECUTE_REDO = QEvent::registerEventType();

/************************************
* 名称: getInstance
* 功能: 获取单例
* 参数：
* 返回:   撤销管理模块对象指针
* 时间:   2017/01/09
* 作者:   hwx
************************************/
UBXUndoRedoManager *UBXUndoRedoManager::getInstance()
{
    static UBXUndoRedoManager instance;
    return &instance;
}

/************************************
* 名称: notifyOberservers
* 功能: 通知观察者
* 参数: [in] eType: 操作类型
             [in] eCode: 操作码
             [in] lParam: 参数1
             [in] wParam: 参数2
* 返回:
* 时间:   2017/01/13
* 作者:   hwx
************************************/
void UBXUndoRedoManager::notifyOberservers(emExecuteType eType, emOperateCode eCode, LPARAM lParam, WPARAM wParam)
{
    switch (eType) {
    case eExecuteUndo:
    {
        NotifyObservers(WM_MSG_EXECUTE_UNDO, eCode, wParam, lParam);
        break;
    }
    case eExecuteRedo:
    {
        NotifyObservers(WM_MSG_EXECUTE_REDO, eCode, wParam, lParam);
        break;
    }
    default:
        break;
    }
}

/************************************
* 名称: createOperation
* 功能: 工厂模式，根据类型码创建操作对象
* 参数: [in] eType: 操作类型码
* 返回: operation对象
* 时间:   2017/01/13
* 作者:   hwx
************************************/
IOperation *UBXUndoRedoManager::createOperation(emOperateCode eCode)
{
    IOperation *pHandle = NULL;
    switch (eCode) {
    case eCodeHandleMotionData:
    {
        pHandle = new CHandleMotionData(eCode);
        break;
    }
    case eCodeHandleMarkLine:
    {
        pHandle = new CHandleMarkLine(eCode);
        break;
    }
    case eCodeHandleFrameRunTime:
    {
        pHandle = new CHandleInt(eCode);
        break;
    }
    case eCodeHandleLoadMusicFile:
    case eCodeHandleSearchText:
    {
        pHandle = new CHandleString(eCode);
        break;
    }
    case eCodeHandleTemplateFile:
    {
        pHandle = new CHandleTemplateFile(eCode);
        break;
    }
    default:
        break;
    }

    if (pHandle)
    {
        int nId = createID();
//        qDebug() << "Create id = " << nId << endl;
        pHandle->setID(nId);
        clearRedoList();  // 执行新操作后，原来的redo操作不可恢复
        pushUndoOperation(pHandle);
    }

    return pHandle;
}

int UBXUndoRedoManager::createID()
{
    return m_nID++;
}

/************************************
* 名称: pushUndoOperation
* 功能: 添加undo操作
* 参数: [in] pItem: 操作项
* 返回:  添加后的size
* 时间:   2017/01/09
* 作者:   hwx
************************************/
int UBXUndoRedoManager::pushUndoOperation(IOperation *pItem)
{
    // 如果已经达到支持操作的最大值，则删除最早的操作
    if (m_lstUndoOperation.size() >= Constants::MAX_REDO_UNDO_NUMBER)
    {
        IOperation *pFirstItem = m_lstUndoOperation.first();
        m_lstUndoOperation.removeFirst();
        SAFE_DELETE(pFirstItem);
    }

    m_lstUndoOperation.push_back(pItem);
    emit sigUndoListSizeChanged(m_lstUndoOperation.size());
    return m_lstUndoOperation.size();
}

/************************************
* 名称: executeUndoOperation
* 功能: 执行undo操作
* 参数:
* 返回:  bool
* 时间:   2017/01/09
* 作者:   hwx
************************************/
bool UBXUndoRedoManager::executeUndoOperation()
{
    if (m_lstUndoOperation.size() <= 0)
    {
        return NULL;
    }

    IOperation *pItem = m_lstUndoOperation.last();
    m_lstUndoOperation.pop_back();
    pItem->undo();
    pushRedoOperation(pItem);
    emit sigUndoListSizeChanged(m_lstUndoOperation.size());
    return true;
}

/************************************
* 名称: pushRedoOperation
* 功能: 添加redo操作
* 参数: [in] pItem: 操作项
* 返回:  添加后的size
* 时间:   2017/01/09
* 作者:   hwx
************************************/
int UBXUndoRedoManager::pushRedoOperation(IOperation *pItem)
{
    // 如果已经达到支持操作的最大值，则删除最早的操作
    if (m_lstRedoOperation.size() >= Constants::MAX_REDO_UNDO_NUMBER)
    {
        IOperation *pFirstItem = m_lstRedoOperation.first();
        m_lstRedoOperation.removeFirst();
        SAFE_DELETE(pFirstItem);
    }

    m_lstRedoOperation.push_back(pItem);
    emit sigRedoListSizeChanged(m_lstRedoOperation.size());
    return m_lstRedoOperation.size();
}

/************************************
* 名称: executeRedoOperation
* 功能: 执行redo操作
* 参数:
* 返回: bool
* 时间:   2017/01/09
* 作者:   hwx
************************************/
bool UBXUndoRedoManager::executeRedoOperation()
{
    if (m_lstRedoOperation.size() <= 0)
    {
        return NULL;
    }

    IOperation *pItem = m_lstRedoOperation.last();
    pItem->execute();
    m_lstRedoOperation.pop_back();
    pushUndoOperation(pItem);
    emit sigRedoListSizeChanged(m_lstRedoOperation.size());
    return true;
}

/************************************
* 名称: clearAllList
* 功能: 清空撤销重做列表
* 参数:
* 返回: bool
* 时间:   2017/01/09
* 作者:   hwx
************************************/
void UBXUndoRedoManager::clearAllList()
{
    clearUndoList();
    clearRedoList();
}

/************************************
* 名称: clearRedoList
* 功能: 清空重做列表
* 参数:
* 返回: bool
* 时间:   2017/01/09
* 作者:   hwx
************************************/
void UBXUndoRedoManager::clearRedoList()
{
    clearList(m_lstRedoOperation);
    emit sigRedoListSizeChanged(0);
}

/************************************
* 名称: clearUndoList
* 功能: 清空撤销列表
* 参数:
* 返回: bool
* 时间:   2017/01/09
* 作者:   hwx
************************************/
void UBXUndoRedoManager::clearUndoList()
{
    clearList(m_lstUndoOperation);
    emit sigUndoListSizeChanged(0);
}

bool UBXUndoRedoManager::isUndoListEmpty()
{
    return (m_lstUndoOperation.size() > 0);
}

bool UBXUndoRedoManager::isRedoListEmpty()
{
    return (m_lstRedoOperation.size() > 0);
}

UBXUndoRedoManager::UBXUndoRedoManager(QObject *parent) : QObject(parent)
  , m_nID(0)
{

}

UBXUndoRedoManager::~UBXUndoRedoManager()
{
//    qDebug() << "Clear redo list" << endl;
    clearList(m_lstRedoOperation);
//    qDebug() << "Clear undo list" << endl;
    clearList(m_lstUndoOperation);
//    qDebug() << "Clear done" << endl;
}

void UBXUndoRedoManager::clearList(QList<IOperation *> &lstOperation)
{
    for (int i = 0; i < lstOperation.size(); i++)
    {
//        qDebug() << "delete id = " << lstOperation[i]->getId() << endl;
        SAFE_DELETE(lstOperation[i]);
    }
    lstOperation.clear();
}
