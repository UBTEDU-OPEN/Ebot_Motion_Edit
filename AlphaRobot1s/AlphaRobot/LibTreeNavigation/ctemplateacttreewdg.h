/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ubxtreewidget.h
* 创建时间：2017/09/04 14:00
* 文件标识：
* 文件摘要：动作模板库导航树图界面设计类
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：2017/09/04 14:00
* 版本摘要：
*/

#ifndef CTEMPLATEACTTREEWDG_H
#define CTEMPLATEACTTREEWDG_H


#include <QTreeWidget>
#include <QWheelEvent>
#include <QApplication>
#include <QDrag>
#include <qobject.h>
#include <QPainter>
#include <QDir>
#include <QMimeData>
#include <QStyleOption>



class CTemplateActTreeWdg : public QTreeWidget
{
    Q_OBJECT

public:
    CTemplateActTreeWdg(QWidget *parent = 0);
    virtual ~CTemplateActTreeWdg();

    QString templateDir() const;
    void setTemplateDir(const QString &strTemplateDir);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    bool ParseData(char *pData, int nLens);
    void performDrag();

private:

    QPoint      m_startDragPoint;
    bool       m_CtrlPressed;
    QTreeWidgetItem * m_DragItem;
    QByteArray m_DragByteArray;
    QString    m_strActionName;
    QString m_strTemplateDir;
};

#endif // CTEMPLATEACTTREEWDG_H





