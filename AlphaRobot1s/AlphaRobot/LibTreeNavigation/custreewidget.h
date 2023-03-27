#ifndef CUSTREEWIDGET_H
#define CUSTREEWIDGET_H
#include <QTreeWidget>
#include <QWheelEvent>
#include <QApplication>
#include <QDrag>
#include <qobject.h>
#include <QPainter>
#include <QDir>
#include <QStyleOption>
#include <QMimeData>


#include "ctemplateacttreewdg.h"

class CusTreeWidget : public CTemplateActTreeWdg
{
    Q_OBJECT
public:
    CusTreeWidget(QWidget *parent = 0);
    ~CusTreeWidget();

public:

//protected:
//  void mouseMoveEvent(QMouseEvent *event);
//  void mousePressEvent(QMouseEvent *event);
//private:
//  bool ParseData(char* pData, int nLens);

//private:
//  QPoint      m_startDragPoint;
//   void       performDrag();
//   bool       m_CtrlPressed;
//   QTreeWidgetItem * m_DragItem;
//   QByteArray m_DragByteArray;
//   QString    m_strActionName;

};

#endif // CUSTREEWIDGET_H
