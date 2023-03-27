/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CTableWidget.h
* 创建时间：20161107
* 文件标识：
* 文件摘要：CTableWidget
*
* 当前版本：1.0.0.0
* 作    者：asu
*/

#include "ctablewidget.h"

#include <QTimerEvent>
#include <QPainter>
#include <QApplication>

#include "ctablewidgetitem.h"



CTableWidget::CTableWidget(QWidget *parent): QTableWidget(parent)
  ,m_nCurEnterCol(-1)
  ,m_nCurEnterRow(-1)
  ,m_nCurrentRowCount(0)
  ,m_nTimerIDUpdateRowChange(-1)
{
    setColorHover(COLOR_DEFAULT_ITEM_HOVER);
    setColorClear(COLOR_DEFAULT_ITEM_HOVER_CLEAR);
    connect(this, &CTableWidget::cellEntered, this, &CTableWidget::onSlotTableCellEnter);
}

CTableWidget::~CTableWidget()
{

}

/************************************
* 名称: setAllCheckBoxs
* 功能:
* 参数: [in]pTab
*      [in]nCheckBoxColPosition
*      [in]state
* 返回:   void
* 时间:   20161107
* 作者:   asu
************************************/
void CTableWidget::setAllCheckBoxs(QTableWidget *pTab, int nCheckBoxColPosition, Qt::CheckState state)
{
    if(pTab)
    {
        for(int i = 0; i < pTab->rowCount(); i++)
        {
            QCheckBox* pChkbox = dynamic_cast<QCheckBox*>(pTab->cellWidget(i, nCheckBoxColPosition));
            if(pChkbox)
            {
                pChkbox->setCheckState(state);
            }
        }
    }
}

/************************************
* 名称: isAllCheckBoxsChecked
* 功能: get the checkboxs status
* 参数: [in]pTab
*      [in]nCheckBoxColPosition
* 返回:   bool
* 时间:   20161108
* 作者:   asu
************************************/
Qt::CheckState CTableWidget::getAllCheckBoxCheckedState(const QTableWidget * const pTab, const int nCheckBoxColPosition)
{
    if(!pTab || (pTab->rowCount() == 0))
    {
        return Qt::Unchecked;
    }

    bool bAllSelect = true;
    bool bAtLeastOneSelect = false;

    if(pTab)
    {
        for(int i = 0; i < pTab->rowCount(); i++)
        {
            QCheckBox* pCheckBox = dynamic_cast<QCheckBox*>(pTab->cellWidget(i, nCheckBoxColPosition));
            if(pCheckBox && !pCheckBox->isChecked())
            {
                bAllSelect = false;
            }
            else if (pCheckBox && pCheckBox->isChecked())
            {
                bAtLeastOneSelect = true;
            }
        }
    }

    if (bAllSelect)
    {
        return Qt::Checked;
    }
    else if (bAtLeastOneSelect)
    {
        return Qt::PartiallyChecked;
    }

    return Qt::Unchecked;
}

/************************************
* 名称: resetHoverRowBackground
* 功能: reset Hover Row Background
* 参数: [in]pTab
* 返回:
* 时间:   20170304
* 作者:   asu
************************************/
void CTableWidget::resetHoverRowBackground(int nRow, int nCol)
{
    if (m_nCurEnterRow == nRow)
    {
        return;
    }

    for(int col = 0; col < this->columnCount(); col++)
    {
        //清除上一次hover状态
        QTableWidgetItem* pItem = this->item(m_nCurEnterRow, col);
        if(pItem)
        {
            pItem->setBackground(m_colorClear);
        }
        //hover本次状态
        pItem = this->item(nRow, col);
        if(pItem)
        {
            pItem->setBackground(m_colorHover);
        }
    }
}

void CTableWidget::onSlotTableCellEnter(int row, int column)
{
    resetHoverRowBackground(row, column);

    m_nCurEnterRow = row;
    m_nCurEnterCol = column;
}

QColor CTableWidget::getColorClear() const
{
    return m_colorClear;
}

void CTableWidget::setColorClear(const QColor &colorClear)
{
    m_colorClear = colorClear;
}

void CTableWidget::addRetranslateColumn(int nColumn)
{
    if(!m_listRetranslate.contains(nColumn))
    {
       m_listRetranslate.push_back(nColumn);
    }
}

void CTableWidget::retranslateUi()
{
    int nSize = m_listRetranslate.size();
    int nRow = this->rowCount();
    for(int i = 0; i < nSize; i++)
    {
        for(int j = 0; j < nRow; j++)
        {
            int nColumnTemp = m_listRetranslate.at(i);
            QTableWidgetItem *pItem = this->item(j, nColumnTemp);
            if(pItem)
            {
                CTableWidgetItem* pTableItem = dynamic_cast<CTableWidgetItem*>(pItem);
                if(pTableItem && !pTableItem->isEmptyRetranslateKey())
                {
                    QString strKey = pTableItem->textKeyForRetranslate();
                    QString strClassName = pTableItem->retranslateContext();
                    pTableItem->setText(QApplication::translate(strClassName.toStdString().c_str(), strKey.toStdString().c_str(), 0));
                }
            }
        }
    }
}

QColor CTableWidget::getColorHover() const
{
    return m_colorHover;
}

void CTableWidget::setColorHover(const QColor &colorHover)
{
    m_colorHover = colorHover;
}

void CTableWidget::startRowChangeTimer(const int nMsec)
{
    if (m_nTimerIDUpdateRowChange > 0)
    {
        killTimer(m_nTimerIDUpdateRowChange);
    }

    m_nTimerIDUpdateRowChange = this->startTimer(nMsec);
}

void CTableWidget::timerEvent(QTimerEvent *event)
{
    if(event && (event->timerId() == m_nTimerIDUpdateRowChange))
    {
        if(m_nCurrentRowCount != this->rowCount())
        {
            m_nCurrentRowCount = this->rowCount();
            emit sigRowCountChanged(m_nCurrentRowCount);
        }
    }

    // 需要调用父类的timerEvent()，否则CPU使用率会暴涨
    // https://bugreports.qt.io/browse/QTBUG-11118
    QTableWidget::timerEvent(event);
}
