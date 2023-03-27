/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：iactionlistitempreview.h
* 创建时间：2016/06/08
* 文件标识：
* 文件摘要：动作列表下载项abstract 预览控件
*
* 当前版本：1.0.0.0
* 作    者： asu
*/

#ifndef IACTIONLISTITEMPREVIEW_H
#define IACTIONLISTITEMPREVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>

#include "actupdatemangr.h"
#include "downloaditemdelegate.h"



const int TABLE_COL_DEFAULT_NUMBER = 3;
const int TABLE_COL_CHECKBOX = 0;
const int TABLE_COL_DATA = 1;
const int TABLE_COL_OPERATE = 2;


namespace Ui {
//class IActionListItemPreview;
class ActionListItemPreview;
}

class IActionListItemPreview : public QWidget
{
    Q_OBJECT

public:
    explicit IActionListItemPreview(QWidget *parent = 0);
    virtual ~IActionListItemPreview();

public:
    /************************************
    * 名称: GetTableWidget
    * 功能: 返回控件内部的QTableWidget对象指针(由Designer创建，一定会有值，但外部不能删除)
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    QTableWidget* GetTableWidget();

    // 获取上次hover的行
    int getPreHoverRow() { return m_nRowPrevHover; }

    /************************************
    * 名称: getCurItemData
    * 功能: get the current item data
    * 参数: [in]
    * 返回:   ActItemData
    * 时间:   20160919
    * 作者:   asu
    ************************************/
    ActItemData getCurItemData()const;

    /************************************
    * 名称: addItemData
    * 功能: add item data to table
    * 参数: [in]itemData
    * 返回:
    * 时间:   20160919
    * 作者:   asu
    ************************************/
    void addItemData(ActItemData itemData);

    /************************************
    * 名称: ClearItems
    * 功能: 清空列表内容
    * 返回:   void
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    void ClearItems();
    
    /************************************
    * 名称: setItemBackground
    * 功能: set item background
    * 返回:   void
    * 时间:   2016/06/08
    * 作者:   asu
    ************************************/
    void setItemBackground(const int& row, const QColor& color);

    DownLoadItemDelegate* getItemDelegate()const;
    //get first button
    QPushButton* getFirstBtn()const;
    //get second button
    QPushButton* getSecondBtn()const;

    //set the check box col visual
    void setCheckVisual(bool bVisual);

    /************************************
    * 名称: isAllCheck
    * 功能: whether is all check of table check boxs
    * 参数: [in]
    * 返回: bool
    * 时间:   20160919
    * 作者:   asu
    ************************************/
    bool isAllCheck();

    /************************************
    * 名称: setAllCheckStatus
    * 功能: set all check of table check boxs to status bChecked
    * 参数: [in]bChecked
    * 返回:
    * 时间:   20160919
    * 作者:   asu
    ************************************/
    void setAllCheckStatus(bool bChecked);

signals:
    /************************************
    * 名称: SigActionPreviewClicked
    * 功能: 转发DownloadItemDelegate信号，点击预览图时发送，通知外部处理
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   20160608
    * 作者:   asu
    ************************************/
    void SigActionPreviewClicked(ActItemData itemData);

    /************************************
    * 名称: sigOnActionPraiseIconClicked
    * 功能: 点赞信号
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/10/20
    * 作者:   hwx
    ************************************/
    void sigOnActionPraiseIconClicked(ActItemData itemData);

    /************************************
    * 名称: sigCellEntered
    * 功能: sent the sig when enter the cell of table
    * 参数: [in]
    * 返回:   void
    * 时间:   20160919
    * 作者:   asu
    ************************************/
    void sigCellEntered(int row, int column);

    /************************************
    * 名称: sigCheckBoxClicked
    * 功能: sent the sig when click the checkbox
    * 参数: [in]
    * 返回:   void
    * 时间:   20160919
    * 作者:   asu
    ************************************/
    void sigCheckBoxClicked();

public slots:
    /************************************
    * 名称: OnCellEntered
    * 功能: 响应鼠标进入item项，实现hover效果
    * 参数: [in]int row 当前Item行
    * 参数: [in]int column 当前Item列
    * 返回:   void
    * 时间:   2016/03/23
    * 作者:   hels
    ************************************/
    virtual void OnCellEntered(int row, int column);

protected:
    /************************************
    * 名称: eventFilter
    * 功能: 重写父类函数，当鼠标离开tablewidget时，将hover状态清空
    * 返回:   void
    * 时间:   2016/03/23
    * 作者:   hels
    ************************************/
    bool eventFilter(QObject *obj, QEvent *ev);

    virtual void resizeEvent(QResizeEvent *);


protected slots:
    //on first button
    virtual void onFirstBtn();

    //click on second btn
    virtual void onSecondBtn();

public:
    QTableWidget *m_pTableWidgetPreview;
    QPushButton* m_pBtnFirst;//
    QPushButton* m_pBtnSecond;//

    DownLoadItemDelegate *m_pItemDelegate;

    int m_nRowPrevHover;//记录上一次hover的行，当次要清除hover背景

    ActItemData m_CurItemData;
    QPoint m_ptBtnSecond;//second按钮起始位置

private:
    Ui::ActionListItemPreview *ui;

};
#endif // IACTIONLISTITEMPREVIEW_H
