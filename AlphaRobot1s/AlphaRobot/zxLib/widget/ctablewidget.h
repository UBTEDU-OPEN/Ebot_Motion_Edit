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

#ifndef CTABLEWIDGET_H
#define CTABLEWIDGET_H


#include <QTableWidget>
#include <QCheckBox>

#include <QMap>
#include <QList>

const QColor COLOR_DEFAULT_ITEM_HOVER =  QColor("#5d5d5d");//列表Item Hover背景色
const QColor COLOR_DEFAULT_ITEM_HOVER_CLEAR =  Qt::transparent;//清空hover


class ZXLIB_DLL CTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit CTableWidget(QWidget *parent = NULL);
    ~CTableWidget();

public:
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
    static void setAllCheckBoxs(QTableWidget* pTab, int nCheckBoxColPosition, Qt::CheckState state);

    /************************************
    * 名称: isAllCheckBoxsChecked
    * 功能: get the checkboxs status
    * 参数: [in]pTab
    *      [in]nCheckBoxColPosition
    * 返回:   Qt::CheckState
    * 时间:   20161108
    * 作者:   asu
    ************************************/
    static Qt::CheckState getAllCheckBoxCheckedState(const QTableWidget* const pTab, const int nCheckBoxColPosition);

    void startRowChangeTimer(const int nMsec);

    /************************************
    * 名称: resetHoverRowBackground
    * 功能: reset Hover Row Background
    * 参数: [in]pTab
    * 返回:
    * 时间:   20170304
    * 作者:   asu
    ************************************/
    virtual void resetHoverRowBackground(int nRow, int nCol);


    QColor getColorHover() const;
    void setColorHover(const QColor &colorHover);

    QColor getColorClear() const;
    void setColorClear(const QColor &colorClear);

    //the item must have to be the CTableWidgetItem
    void addRetranslateColumn(int nColumn);
    virtual void retranslateUi();


signals:
    void sigRowCountChanged(int newCount);

public slots:
    void onSlotTableCellEnter(int row, int column);

protected:
    // 重写QAbstractItemView的timerEvent()需要调用父类接口，否则会导致Mac上CPU使用率暴涨
    //https://bugreports.qt.io/browse/QTBUG-11118
    virtual void timerEvent(QTimerEvent* event);


private:
    int m_nCurEnterCol;
    int m_nCurEnterRow;
    int m_nCurrentRowCount;
    int m_nTimerIDUpdateRowChange;

    QColor m_colorHover; //列表Item Hover背景色
    QColor m_colorClear;  //清空hover


    QList<int> m_listRetranslate;

};

#endif // CTABLEWIDGET_H
