/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cmarkline.cpp
* 创建时间：2016/12/09
* 文件标识：
* 文件摘要：标记线类
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/12/09
* 版本摘要：
*/

#include <QPainter>

#include "cmarkline.h"
#include "ubxundoredomanagerconst.h"

CMarkLine::CMarkLine(int nWidth, int nHeight, QObject *pParent) : QPixmap(nWidth, nHeight)
  , m_nLineSpaceWidth(nWidth)
  , m_nLineSpaceHeight(nHeight)
{
    this->fill(QColor(0, 0, 0, 0));  // 设置背景透明
}

CMarkLine::CMarkLine(Constants::emMarkLineType eLineType, double dPosMs, int nWidth, int nHeight, QObject *pParent) : QPixmap(nWidth, nHeight),
    m_dPosMs(dPosMs),
    m_eType(eLineType),
    m_eState(Constants::eMarkLineStateNone),
    m_nLineSpaceWidth(nWidth),
    m_nLineSpaceHeight(nHeight)
{
    this->fill(QColor(0, 0, 0, 0));  // 设置背景透明
}

CMarkLine::~CMarkLine()
{

}

void CMarkLine::clone(CMarkLine *pLine)
{
    if (!pLine || pLine == this)
    {
        return;
    }

    this->m_dPosMs = pLine->getPos();
//    this->m_nLineSpaceHeight = pLine->getLineHeight();
//    this->m_nLineSpaceWidth = pLine->getLineWidth();
    this->m_eType = pLine->getType();
    this->m_eState = pLine->getState();
}

double CMarkLine::getPos()
{
    if (m_dPosMs <= 0)
    {
        return 0;
    }
    return m_dPosMs;
}

/************************************
* 名称: move
* 功能: 移动线条
* 参数: [in] dOffset: 移动距离，正值表示右移，负值表示左移，以时间ms为单位
* 返回:  void
* 时间:   2016/12/12
* 作者:   hwx
************************************/
void CMarkLine::move(double dOffset)
{
    setPos(m_dPosMs + dOffset);
}

/************************************
* 名称: isInSelectedArea
* 功能: 判断线条是否在选择的矩形区域内
* 参数: [in] dStartPosMs: 绘制区域起始坐标位置，以ms为单位
* 参数: [in] dEndPosMs:  绘制区域结束坐标位置，以ms为单位
* 返回:  void
* 时间:   2016/12/17
* 作者:   hwx
************************************/
bool CMarkLine::isInSelectedArea(double dStartPosMs, double dEndPosMs)
{
    if (m_dPosMs >= dStartPosMs && m_dPosMs <= dEndPosMs)
    {
        return true;
    }

    return false;
}

void CMarkLine::init()
{
    drawLine(getColor(), getThick());
}

void CMarkLine::redraw()
{
    QPainter painter;
    painter.begin(this);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(this->rect(), Qt::transparent);
    painter.end();
    if (m_eState == Constants::eMarkLineStatePressed || m_eState == Constants::eMarkLineStateSelected)
    {
        drawLine(getColor(), 3 * getThick());
    }
    else
    {
        drawLine(getColor(), getThick());
    }
}

bool CMarkLine::isCursorHoverOn(double dCursorPos, double dPixelPerMs)
{
    if ((qAbs(m_dPosMs - dCursorPos)  * dPixelPerMs) < m_nLineSpaceWidth/2)
    {
        return true;
    }

    return false;
}

bool CMarkLine::drawLine(QColor color, int nLineThick)
{
    QPainter painter;
    painter.begin(this);
    //painter.setBackgroundMode(Qt::OpaqueMode);
    QPen penSelLine(color, nLineThick);
    painter.setPen(penSelLine);
    QPixmap pixmapPointer(":/res/images/icon_pointer.png");
    painter.drawPixmap(0, 0, pixmapPointer.width(), pixmapPointer.height(), pixmapPointer);
    painter.drawLine(m_nLineSpaceWidth/2, pixmapPointer.height(), m_nLineSpaceWidth/2, m_nLineSpaceHeight);
    painter.end();

    return true;
}

QColor CMarkLine::getColor()
{
    QColor lineColor;
    switch (m_eType) {
    case Constants::eMarkLineTypeFrame:
    {
        lineColor = Constants::COLOR_MARK_LINE;
        break;
    }
    case Constants::eMarkLineTypeBlock:
    {
        lineColor = Constants::COLOR_MARK_BLOCK;
        break;
    }
    default:
        break;
    }

    return lineColor;
}

int CMarkLine::getThick()
{
    return Constants::MARK_LINE_THIN_DEFAULT;
}
