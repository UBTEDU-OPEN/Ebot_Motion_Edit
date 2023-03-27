/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：crectselectdrawer.cpp
* 创建时间：2016/12/14
* 文件标识：
* 文件摘要：矩形选择框绘制
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/12/14
* 版本摘要：
*/

#include <QPainter>

#include "crectselectdrawer.h"
#include "mp3projconstants.h"

CRectSelectDrawer::CRectSelectDrawer(QObject *parent) : QObject(parent)
{
    m_penColor = QColor("#000000");
    m_brushColor = QColor(0, 0, 0, 0);
}

CRectSelectDrawer::~CRectSelectDrawer()
{

}

void CRectSelectDrawer::setPenColor(QColor color)
{
    m_penColor = color;
}

void CRectSelectDrawer::setBrushColor(QColor color)
{
    m_brushColor = color;
}

bool CRectSelectDrawer::drawRect(const QRect &rectangle, QPixmap &pixmap)
{
    QPainter painter;
    painter.begin(&pixmap);
    //painter.setBackgroundMode(Qt::OpaqueMode);
    QPen penSelLine(m_penColor, Constants::DEFAULT_LINE_THICK);
    painter.setPen(penSelLine);
    painter.drawRect(rectangle);
    painter.end();

    return true;
}

