#ifndef DEFTREEWIDGET_H
#define DEFTREEWIDGET_H
#include <QTreeWidget>
#include <QWheelEvent>
#include <QApplication>
#include <QDrag>
#include <qobject.h>
#include <QPainter>
#include <QDir>
#include <QMimeData>
#include <QStyleOption>


#include "ctemplateacttreewdg.h"


class DefTreeWidget : public CTemplateActTreeWdg
{
    Q_OBJECT
public:
        DefTreeWidget(QWidget *parent = 0);
        ~DefTreeWidget();

public:

//protected:
//    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent *event);

//private:
//    bool ParseData(char *pData, int nLens);
//    QPoint      m_startDragPoint;
//    void       performDrag();
//    bool       m_CtrlPressed;
//    QTreeWidgetItem * m_DragItem;
//    QByteArray m_DragByteArray;
//    QString    m_strActionName;

};

#endif // DEFTREEWIDGET_H
