/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：chandlemarkline.cpp
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：修改标记线
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#include "chandlemarkline.h"
#include "ubxundoredomanager.h"

CHandleMarkLine::CHandleMarkLine(Constants::emOperateCode eType) : IOperation(eType)
  , m_pMarkLinesBeforeChanged(NULL)
  , m_pMarkLinesAfterChanged(NULL)
{

}

CHandleMarkLine::~CHandleMarkLine()
{
    SAFE_DELETE(m_pMarkLinesAfterChanged);
    SAFE_DELETE(m_pMarkLinesBeforeChanged);
}

///************************************
//* 名称: clone
//* 功能: 拷贝数据
//* 参数: [in] item 待拷贝的数据
//* 返回: void
//* 时间:   2017/01/09
//* 作者:   hwx
//************************************/
//bool CHandleMarkLine::clone(CHandleMarkLine &item)
//{
//    if (this == &item)
//    {
//        return true;
//    }

//    cloneDataBeforeChanged((LPARAM)(item.getDataBeforeChanged()), NULL);
//    cloneDataAfterChanged((LPARAM)(item.getDataAfterChanged()), NULL);
//}

/************************************
* 名称: execute
* 功能: 执行redo操作
* 参数:
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
bool CHandleMarkLine::execute()
{
    UBXUndoRedoManager::getInstance()->notifyOberservers(eExecuteRedo, m_eCode, (LPARAM)m_pMarkLinesAfterChanged, NULL);
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
bool CHandleMarkLine::undo()
{
    UBXUndoRedoManager::getInstance()->notifyOberservers(eExecuteUndo, m_eCode, (LPARAM)m_pMarkLinesBeforeChanged, NULL);
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
bool CHandleMarkLine::cloneDataBeforeChanged(LPARAM lParam, WPARAM wParam)
{
    CMarkLineContainer *pLines = (CMarkLineContainer *)lParam;
    cloneMarkLines(pLines, &m_pMarkLinesBeforeChanged);
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
bool CHandleMarkLine::cloneDataAfterChanged(LPARAM lParam, WPARAM wParam)
{
    CMarkLineContainer *pLines = (CMarkLineContainer *)lParam;
    cloneMarkLines(pLines, &m_pMarkLinesAfterChanged);
    return true;
}

/************************************
* 名称: cloneMarkLines
* 功能: 深拷贝所有动作数据
* 参数: [in] pLinesSrc: 源数据
*           [in] pLinesDes: 目的数据
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
void CHandleMarkLine::cloneMarkLines(CMarkLineContainer  *pLinesSrc, CMarkLineContainer  **pLinesDes)
{
    if (!pLinesSrc)
    {
        return;
    }

    SAFE_DELETE(*pLinesDes);
    (*pLinesDes) = new CMarkLineContainer;
    (*pLinesDes)->cloneLines(pLinesSrc);
}

CMarkLineContainer::CMarkLineContainer()
{

}

CMarkLineContainer::~CMarkLineContainer()
{
    removeAllLines();
}

/************************************
* 名称: addLine
* 功能: 添加标记线
* 参数: [in] pLine 标记线
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
void CMarkLineContainer::addLine(CMarkLine *pLine)
{
    if (m_lstMarkLines.contains(pLine))
    {
        return;
    }

    m_lstMarkLines.append(pLine);
}

/************************************
* 名称: deleteLine
* 功能: 删除标记线
* 参数: [in] pLine 标记线
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
void CMarkLineContainer::deleteLine(CMarkLine *pLine)
{
    if (m_lstMarkLines.contains(pLine))
    {
        m_lstMarkLines.removeAll(pLine);
        SAFE_DELETE(pLine);
        pLine = NULL;
    }
}

/************************************
* 名称: cloneLines
* 功能: 深拷贝所有动作数据
* 参数: [in] pSrcLines: 源数据
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
void CMarkLineContainer::cloneLines(CMarkLineContainer *pSrcLines)
{
    if (!pSrcLines)
    {
        return;
    }

    removeAllLines();
    QList<CMarkLine *> lstLinesSrc = pSrcLines->getAllLines();
    for (int i = 0; i < lstLinesSrc.size(); i++)
    {
        CMarkLine *pSrc = lstLinesSrc[i];
        if (!pSrc)
        {
            continue;
        }

        CMarkLine *pLine = new CMarkLine(pSrc->getLineWidth(), pSrc->getLineHeight());
        pLine->clone(pSrc);
        m_lstMarkLines.append(pLine);
    }
}

/************************************
* 名称: removeAllLines
* 功能: 删除所有标记线
* 参数:
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
void CMarkLineContainer::removeAllLines()
{
    for (int i = 0; i < m_lstMarkLines.size(); i++)
    {
        SAFE_DELETE(m_lstMarkLines[i]);
    }
    m_lstMarkLines.clear();
}

