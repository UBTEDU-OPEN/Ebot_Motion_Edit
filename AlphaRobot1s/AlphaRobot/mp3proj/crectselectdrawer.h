/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：crectselectdrawer.h
* 创建时间：2016/12/14
* 文件标识：
* 文件摘要：矩形选择框绘制
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/12/14
* 版本摘要：
*/

#ifndef CRECTSELECTDRAWER_H
#define CRECTSELECTDRAWER_H

#include <QObject>
#include <QColor>
#include <QPixmap>

class CRectSelectDrawer : public QObject
{
    Q_OBJECT
public:
    explicit CRectSelectDrawer(QObject *parent = 0);
    ~CRectSelectDrawer();

public:
    // 设置画笔、画刷颜色
    void setPenColor(QColor color);
    void setBrushColor(QColor color);

    // 绘制矩形框
    bool drawRect(const QRect & rectangle, QPixmap &pixmap);

signals:

public slots:

private:
    QColor m_penColor; // 画笔颜色
    QColor m_brushColor; // 填充颜色
};

#endif // CRECTSELECTDRAWER_H
