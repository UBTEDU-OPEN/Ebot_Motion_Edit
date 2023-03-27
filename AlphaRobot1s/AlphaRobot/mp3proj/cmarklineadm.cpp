/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cmarklineadm.cpp
* 创建时间：2016/12/09
* 文件标识：
* 文件摘要：标记线管理类
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/12/09
* 版本摘要：
*/

#include <QPainter>
#include <QDebug>

#include "mp3projconstants.h"
#include "cmarklineadm.h"
#include "UBXPublic.h"

CMarkLineAdm::CMarkLineAdm(QObject *parent) : QObject(parent)
  , m_pMarkLineContainer(NULL)
{
    m_nLineSpaceWidth = Constants::DEFAULT_LINE_THICK;
    m_nLineSpaceHeight = Constants::DEFAULT_LINE_HEIGHT;

    m_pMarkLineContainer = new CMarkLineContainer;
}

CMarkLineAdm::~CMarkLineAdm()
{
    SAFE_DELETE(m_pMarkLineContainer);
}

void CMarkLineAdm::updateMarkLines(CMarkLineContainer *pContainer)
{
    if (!pContainer)
    {
        return;
    }

    // 需要进行深拷贝，执行新操作后清空undo列表时会释放内存
    SAFE_DELETE(m_pMarkLineContainer);
    m_pMarkLineContainer = new CMarkLineContainer;
    m_pMarkLineContainer->cloneLines(pContainer);
}

/************************************
* 名称: createMarkLine
* 功能: 新建标记线
* 参数: [in]eLineType: 标记线类型
* 参数: [in]dPosMs: 标记线在坐标系中的位置，以时间ms为单位
* 返回:  bool
* 时间:   2016/12/10
* 作者:   hwx
************************************/
bool CMarkLineAdm::createMarkLine(Constants::emMarkLineType eLineType, double dPosMs, QObject *pParent)
{
    if (!m_pMarkLineContainer)
    {
        return false;
    }

    IOperation *pHandleMarkLine = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMarkLine);
    if (!pHandleMarkLine)
    {
        return false;
    }

    cloneMarkLineBeforeChanged(pHandleMarkLine);
    CMarkLine *pLine = new CMarkLine(eLineType, dPosMs, m_nLineSpaceWidth, m_nLineSpaceHeight, pParent);
    pLine->init();
    m_pMarkLineContainer->addLine(pLine);

    cloneMarkLineAfterChanged(pHandleMarkLine);
    return true;
}

/************************************
* 名称: deleteMarkLine
* 功能: 删除标记线
* 参数:
* 返回:  bool
* 时间:   2017/1/4
* 作者:   hwx
************************************/
bool CMarkLineAdm::deleteMarkLine(CMarkLine *pLine)
{
    if (!m_pMarkLineContainer)
    {
        return false;
    }

    m_pMarkLineContainer->deleteLine(pLine);

    return true;
}

/************************************
* 名称: deleteSelectedMarkLines
* 功能: 删除选中的标记线
* 参数:
* 返回:  void
* 时间:   2017/1/4
* 作者:   hwx
************************************/
void CMarkLineAdm::deleteSelectedMarkLines()
{
    if (!m_pMarkLineContainer)
    {
        return;
    }

    IOperation *pHandleMarkLine = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMarkLine);
    if (!pHandleMarkLine)
    {
        return;
    }

    cloneMarkLineBeforeChanged(pHandleMarkLine);
    QList<CMarkLine *> lstNeedDelete;
    QList<CMarkLine *> lstMarkLines = m_pMarkLineContainer->getAllLines();
    for (int i = 0; i < lstMarkLines.size(); i++)
    {
        CMarkLine *pMarkLine = lstMarkLines[i];
        if (!pMarkLine)
        {
            continue;
        }

        // 只删除选中的标记线
        if (pMarkLine->getState() != Constants::eMarkLineStateSelected)
        {
            continue;
        }

        lstNeedDelete.append(pMarkLine);
    }

    for (int i = 0; i < lstNeedDelete.size(); i++)
    {
        m_pMarkLineContainer->deleteLine(lstNeedDelete[i]);
    }

    cloneMarkLineAfterChanged(pHandleMarkLine);
}

/************************************
* 名称: isAnyMarkLinePressed
* 功能: 判断是否存在任何一条线被点击
* 返回:  bool
* 时间:   2016/12/10
* 作者:   hwx
************************************/
bool CMarkLineAdm::isAnyMarkLinePressed()
{
    QList<CMarkLine *> lstMarkLines = m_pMarkLineContainer->getAllLines();
    for (int i = 0; i < lstMarkLines.size(); i++)
    {
        CMarkLine *pMarkLine = lstMarkLines[i];
        if (!pMarkLine)
        {
            continue;
        }

        if (pMarkLine->getState() == Constants::eMarkLineStatePressed)
        {
            return true;
        }
    }

    return false;
}

/************************************
* 名称: resetLineState
* 功能: 设置标记线为默认状态
* 返回:  void
* 时间:   2016/12/10
* 作者:   hwx
************************************/
void CMarkLineAdm::resetLineState()
{
    QList<CMarkLine *> lstMarkLines = m_pMarkLineContainer->getAllLines();
    for (int i = 0; i < lstMarkLines.size(); i++)
    {
        CMarkLine *pMarkLine = lstMarkLines[i];
        if (!pMarkLine)
        {
            continue;
        }

        pMarkLine->setState(Constants::eMarkLineStateNone);
    }
}

/************************************
* 名称: redrawAllMarkLine
* 功能: 绘制所有标记线到缓冲的pixmap
* 参数: [in] dStartPosMs: 绘制区域起始坐标位置，以ms为单位
* 参数: [in] dEndPosMs:  绘制区域结束坐标位置，以ms为单位
* 参数: [out]pixmap: 缓冲的pixmap
* 返回:  bool
* 时间:   2016/12/10
* 作者:   hwx
************************************/
void CMarkLineAdm::redrawAllMarkLine(double dStartPosMs, double dEndPosMs, QPixmap &pixmap)
{
    if (!m_pMarkLineContainer)
    {
        return;
    }

    QPainter painter;
    painter.begin(&pixmap);
    QList<CMarkLine *> lstMarkLines = m_pMarkLineContainer->getAllLines();
    for (int i = 0; i < lstMarkLines.size(); i++)
    {
        CMarkLine *pMarkLine = lstMarkLines[i];
        if (!pMarkLine)
        {
            continue;
        }

        // 如果线条时间位置没有落在绘制区域内，则不会绘制
        double dLineMs = pMarkLine->getPos();
        if (dLineMs < dStartPosMs || dLineMs > dEndPosMs)
        {
            continue;
        }

        pMarkLine->redraw();

        // 时间坐标映射为像素坐标
        double dLinePos = (dLineMs - dStartPosMs )*  pixmap.width()/(dEndPosMs - dStartPosMs + 1);

        // 因为画线所占空间宽度为m_nLineSpaceWidth，因此使线所在位置是点击的位置，需要平移m_nLineSpaceWidth/2
        painter.drawPixmap(dLinePos - m_nLineSpaceWidth/2, 0, pMarkLine->width(), pMarkLine->height(), *pMarkLine);
    }
    painter.end();
}

/************************************
* 名称: moveMarkLines
* 功能: 移动选中的标记线
* 参数: [in] dOffset: 移动距离，正值表示右移，负值表示左移，以时间ms为单位
* 返回:  void
* 时间:   2016/12/10
* 作者:   hwx
************************************/
void CMarkLineAdm::moveMarkLines(double dOffset)
{
    if (qAbs(dOffset) < 1)
    {
        return;
    }

    if (!m_pMarkLineContainer)
    {
        return;
    }

    // 由于标记线是实时移动的，如果在这里记录信息，会记录不必要的数据
    // 移动标记线只需要记录起始移动位置和结束移动位置
    //CHandleMarkLine *pHandleMarkLine = cloneMarkLineBeforeChanged();

    QList<CMarkLine *> lstMarkLines = m_pMarkLineContainer->getAllLines();
    for (int i = 0; i < lstMarkLines.size(); i++)
    {
        CMarkLine *pMarkLine = lstMarkLines[i];
        if (!pMarkLine)
        {
            continue;
        }

        // 只移动选中的标记线
        if (pMarkLine->getState() == Constants::eMarkLineStatePressed || pMarkLine->getState() == Constants::eMarkLineStateSelected)
        {
            pMarkLine->move(dOffset);
        }
    }

    //cloneMarkLineAfterChanged(pHandleMarkLine);
}

/************************************
* 名称: selectMarkLines
* 功能: 矩形框选中标记线
* 参数: [in] dStartPosMs: 绘制区域起始坐标位置，以ms为单位
* 参数: [in] dEndPosMs:  绘制区域结束坐标位置，以ms为单位
* 返回:  void
* 时间:   2016/12/10
* 作者:   hwx
************************************/
void CMarkLineAdm::selectMarkLines(double dStartPosMs, double dEndPosMs)
{
    if (!m_pMarkLineContainer)
    {
        return;
    }

    QList<CMarkLine *> lstMarkLines = m_pMarkLineContainer->getAllLines();
    for (int i = 0; i < lstMarkLines.size(); i++)
    {
        CMarkLine *pMarkLine = lstMarkLines[i];
        if (!pMarkLine)
        {
            continue;
        }

        // 如果线条位置在矩形选择区内，则线条被选中
        if (pMarkLine->isInSelectedArea(dStartPosMs, dEndPosMs))
        {
            pMarkLine->setState(Constants::eMarkLineStateSelected);
        }
    }
}

/************************************
* 名称: getLineCursorHoverOn
* 功能: 获取光标悬停在上面的标记线
* 参数: [in] dCursorPos: 光标位置，已经转为为时间单位，ms
            [in] dPixelPerMs: 1ms占得像素值
* 返回:  标记线
* 时间:   2016/12/10
* 作者:   hwx
************************************/
CMarkLine *CMarkLineAdm::getLineCursorHoverOn(double dCursorPos,  double dPixelPerMs)
{
    QList<CMarkLine *> lstMarkLines = m_pMarkLineContainer->getAllLines();
    for (int i = 0; i < lstMarkLines.size(); i++)
    {
        CMarkLine *pMarkLine = lstMarkLines[i];
        if (!pMarkLine)
        {
            continue;
        }

        if (pMarkLine->isCursorHoverOn(dCursorPos, dPixelPerMs))
        {
            return pMarkLine;
        }
    }

    return NULL;
}

/************************************
* 名称: removeAllLines
* 功能: 删除所有标记线
* 参数:
* 返回:
* 时间:   2017/01/13
* 作者:   hwx
************************************/
void CMarkLineAdm::removeAllLines()
{
    if (!m_pMarkLineContainer)
    {
        return;
    }

    m_pMarkLineContainer->removeAllLines();
}

void CMarkLineAdm::cloneMarkLineBeforeChanged(IOperation *pHandleMarkLine)
{
    if (!pHandleMarkLine)
    {
        return;
    }

    pHandleMarkLine->cloneDataBeforeChanged((LPARAM)m_pMarkLineContainer, NULL);
    return;
}

void CMarkLineAdm::cloneMarkLineAfterChanged(IOperation *pHandleMarkLine)
{
    if (!pHandleMarkLine)
    {
        return;
    }

    pHandleMarkLine->cloneDataAfterChanged((LPARAM)m_pMarkLineContainer, NULL);
    return;
}

