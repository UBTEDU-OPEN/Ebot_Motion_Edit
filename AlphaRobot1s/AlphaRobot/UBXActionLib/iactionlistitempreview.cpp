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
#include "iactionlistitempreview.h"
//#include "ui_iactionlistitempreview.h"
#include "ui_actionlistitempreview.h"

#include <QFile>
#include <QCheckBox>
#include <QResizeEvent>


#include "downloaditemdelegate.h"
#include "ubxactionlibconstants.h"
#include "actupdatemangr.h"
#include "../zxLib/zxObserverBaseEx.h"



const int TABLE_ROW_DEFAULT_INVALID = -1;

const int BTN_GEOMERTY_DEFAULT_WITH = 18;
const int BTN_GEOMERTY_DEFAULT_HEIGHT = 17;

IActionListItemPreview::IActionListItemPreview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActionListItemPreview),
    m_pTableWidgetPreview(NULL),
    m_pItemDelegate(NULL)
{
    ui->setupUi(this);

    m_pTableWidgetPreview = ui->tableWidgetPreview;

    m_pItemDelegate = new DownLoadItemDelegate(m_pTableWidgetPreview);
    connect(m_pItemDelegate, &DownLoadItemDelegate::SigActionPreviewClicked, this, &IActionListItemPreview::SigActionPreviewClicked);
    connect(m_pItemDelegate, &DownLoadItemDelegate::sigOnActionCommentIconClicked, this, &IActionListItemPreview::SigActionPreviewClicked);
    connect(m_pItemDelegate, &DownLoadItemDelegate::sigOnActionPraiseIconClicked, this, &IActionListItemPreview::sigOnActionPraiseIconClicked);

    m_pTableWidgetPreview->setItemDelegate(m_pItemDelegate);
    m_pTableWidgetPreview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pTableWidgetPreview->setSelectionMode(QAbstractItemView::NoSelection);
    m_pTableWidgetPreview->setFrameShape(QFrame::NoFrame);
    m_pTableWidgetPreview->setShowGrid(false);
    m_pTableWidgetPreview->verticalHeader()->hide();
    m_pTableWidgetPreview->horizontalHeader()->hide();
    m_pTableWidgetPreview->setMouseTracking(true);
    m_pTableWidgetPreview->installEventFilter(this);
    m_pTableWidgetPreview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTableWidgetPreview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_pTableWidgetPreview->setColumnCount(TABLE_COL_DEFAULT_NUMBER);
    m_pTableWidgetPreview->setColumnWidth(TABLE_COL_CHECKBOX, Constants::BTN_CHECOBOX_CONTAINER_WIDTH);
    m_pTableWidgetPreview->setColumnWidth(TABLE_COL_DATA, Constants::PREVIEW_ICON_WIDTH
                                          + m_pTableWidgetPreview->width() * Constants::DESCRIPTION_MAX_WIDTH_RATION
                                          + Constants::CONTENT_SPACE);
    m_pTableWidgetPreview->setColumnWidth(TABLE_COL_OPERATE, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH - Constants::CONTENT_SPACE);


    m_pTableWidgetPreview->horizontalHeader()->setStretchLastSection(true); //
    setCheckVisual(false);

    connect(m_pTableWidgetPreview, &QTableWidget::cellEntered, this, &IActionListItemPreview::OnCellEntered);

    m_nRowPrevHover = TABLE_ROW_DEFAULT_INVALID;

    m_pBtnFirst = new QPushButton(m_pTableWidgetPreview);
    m_pBtnFirst->hide();
//    m_pBtnFirst->setObjectName(QStringLiteral("pbFirst"));

    m_pBtnSecond = new QPushButton(m_pTableWidgetPreview);
//    m_pBtnSecond->setObjectName(QStringLiteral("pbSecond"));
    m_pBtnSecond->hide();

    connect(m_pBtnFirst, &QPushButton::clicked, this, &IActionListItemPreview::onFirstBtn);
    connect(m_pBtnSecond, &QPushButton::clicked, this, &IActionListItemPreview::onSecondBtn);
}

IActionListItemPreview::~IActionListItemPreview()
{
    delete ui;
}

QTableWidget *IActionListItemPreview::GetTableWidget()
{
    return m_pTableWidgetPreview;
}

/************************************
* 名称: getCurItemData
* 功能: get the current item data
* 参数: [in]
* 返回:   ActItemData
* 时间:   20160919
* 作者:   asu
************************************/
ActItemData IActionListItemPreview::getCurItemData() const
{
    return m_CurItemData;
}

/************************************
* 名称: addItemData
* 功能: add item data to table
* 参数: [in]itemData
* 返回:
* 时间:   20160919
* 作者:   asu
************************************/
void IActionListItemPreview::addItemData(ActItemData itemData)
{
    if(m_pTableWidgetPreview)
    {
        int row = m_pTableWidgetPreview->rowCount();
        m_pTableWidgetPreview->insertRow(row);


        //复选框
        QTableWidgetItem *item0 = new QTableWidgetItem();//插入空对象，占位符
        m_pTableWidgetPreview->setItem(row, TABLE_COL_CHECKBOX, item0);

        QTableWidgetItem *item1 = new QTableWidgetItem();
        item1->setData(Constants::DownLoadItemType, Constants::CUSTOM_DOWNLOAD_ITEM_TYPE);
        QVariant var;
        var.setValue(itemData);
        item1->setData(Constants::ActionItemData, var);
        m_pTableWidgetPreview->setItem(row, TABLE_COL_DATA, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem();//插入空对象，占位符，提供给收藏和下载按钮位置
        m_pTableWidgetPreview->setItem(row, TABLE_COL_OPERATE, item2);

        m_pTableWidgetPreview->resizeRowToContents(row);
        m_pTableWidgetPreview->resizeColumnToContents(TABLE_COL_DATA);

        /////////have to after resizeRowToContents, or checkbox will not show at centre
        QCheckBox* pCheckBox = new QCheckBox(m_pTableWidgetPreview);
        m_pTableWidgetPreview->setCellWidget(row, TABLE_COL_CHECKBOX, pCheckBox);
        connect(pCheckBox, &QCheckBox::clicked, this, &IActionListItemPreview::sigCheckBoxClicked);

//        m_pTableWidgetPreview->resizeRowToContents(row);
//        m_pTableWidgetPreview->resizeColumnToContents(TABLE_COL_DATA);

    }
}

/************************************
* 名称: ClearItems
* 功能: 清空列表内容
* 返回:   void
* 时间:   2016/04/15
* 作者:   hels
************************************/
void IActionListItemPreview::ClearItems()
{
    m_pTableWidgetPreview->clear();
    m_pTableWidgetPreview->setRowCount(0);

    m_pBtnFirst->hide();
    m_pBtnSecond->hide();

    m_nRowPrevHover = TABLE_ROW_DEFAULT_INVALID;
}

/************************************
* 名称: setItemBackground(int row, const QString& strType)
* 功能: clear all items background
* 返回:   void
* 时间:   2016/06/08
* 作者:   asu
************************************/
void IActionListItemPreview::setItemBackground(const int& row, const QColor& color)
{
    for(int col = 0; col < m_pTableWidgetPreview->columnCount(); col++)
    {
        QTableWidgetItem* pItem = m_pTableWidgetPreview->item(row, col);
        if(pItem)
        {
            pItem->setBackground(color);
        }
    }
}

DownLoadItemDelegate *IActionListItemPreview::getItemDelegate() const
{
    return m_pItemDelegate;
}

//get first button
QPushButton *IActionListItemPreview::getFirstBtn() const
{
    return m_pBtnFirst;
}
//    //get second button
QPushButton *IActionListItemPreview::getSecondBtn() const
{
    return m_pBtnSecond;
}

 //set the check box col visual
void IActionListItemPreview::setCheckVisual(bool bVisual)
{
    if(bVisual)
    {
        m_pTableWidgetPreview->showColumn(TABLE_COL_CHECKBOX);
    }
    else
    {
        m_pTableWidgetPreview->hideColumn(TABLE_COL_CHECKBOX);
    }
}

/************************************
* 名称: isAllCheck
* 功能: whether is all check of table check boxs
* 参数: [in]
* 返回: bool
* 时间:   20160919
* 作者:   asu
************************************/
bool IActionListItemPreview::isAllCheck()
{
    bool bAllSelect = true;

    QTableWidget *pTabelWidget = m_pTableWidgetPreview;

    if(pTabelWidget)
    {
        for(int i = 0; (i < pTabelWidget->rowCount()) && bAllSelect; i++)
        {
            QCheckBox* pCheckBox =  dynamic_cast<QCheckBox*>(pTabelWidget->cellWidget(i, TABLE_COL_CHECKBOX));
            if(pCheckBox && !pCheckBox->isChecked())
            {
                bAllSelect = false;

                break;
            }
        }
    }

    return bAllSelect;
}

/************************************
* 名称: setAllCheckStatus
* 功能: set all check of table check boxs to status bChecked
* 参数: [in]bChecked
* 返回:
* 时间:   20160919
* 作者:   asu
************************************/
void IActionListItemPreview::setAllCheckStatus(bool bChecked)
{
    QTableWidget *pTabelWidget = m_pTableWidgetPreview;

    if(pTabelWidget)
    {
        for(int i = 0; i < pTabelWidget->rowCount(); i++)
        {
            QCheckBox* pChkbox = dynamic_cast<QCheckBox*>(pTabelWidget->cellWidget(i, TABLE_COL_CHECKBOX));
            if(pChkbox)
            {
                pChkbox->setChecked(bChecked);
            }
        }
    }
}

/************************************
* 名称: eventFilter
* 功能: 重写父类函数，当鼠标离开tablewidget时，将hover状态清空
* 返回:   void
* 时间:   2016/03/23
* 作者:   hels
************************************/
bool IActionListItemPreview::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == m_pTableWidgetPreview)
    {
        if((ev->type() == QEvent::Leave) ||
           (ev->type() == QEvent::HoverLeave))
        {
            if(m_nRowPrevHover >= 0)
            {
                setItemBackground(m_nRowPrevHover, Constants::COLOR_ITEM_HOVER_CLEAR);
                m_nRowPrevHover = TABLE_ROW_DEFAULT_INVALID;
            }

            if(m_pBtnFirst)
            {
                m_pBtnFirst->hide();
            }
            if(m_pBtnSecond)
            {
                m_pBtnSecond->hide();
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    return QWidget::eventFilter(obj, ev);
}

void IActionListItemPreview::resizeEvent(QResizeEvent *resizeEvent)
{
    m_pTableWidgetPreview->resizeColumnToContents(TABLE_COL_DATA);    
    QWidget::resizeEvent(resizeEvent);
}

/************************************
* 名称: OnCellEntered
* 功能: 响应鼠标进入item项，实现hover效果
* 参数: [in]int row 当前Item行
* 参数: [in]int column 当前Item列
* 返回:   void
* 时间:   2016/03/23
* 作者:   hels
************************************/
void IActionListItemPreview::OnCellEntered(int row, int column)
{
    if (m_nRowPrevHover == row)
    {
        return;
    }

    QTableWidgetItem* pItem = NULL;
    if(m_nRowPrevHover != row)
    {
        //清除上次hover状态
        setItemBackground(m_nRowPrevHover, Constants::COLOR_ITEM_HOVER_CLEAR);
         //设置本次hover状态
        setItemBackground(row, Constants::COLOR_ITEM_HOVER);
    }

    m_nRowPrevHover = row;

    //最后一列位置显示按钮
    pItem = m_pTableWidgetPreview->item(row, TABLE_COL_OPERATE);
    if(NULL == m_pBtnSecond || NULL == m_pBtnFirst || NULL == pItem)
    {
        return;
    }

    QRect itemRect = m_pTableWidgetPreview->visualItemRect(pItem);
    QPoint startsPt;
    startsPt.setX(itemRect.left() + Constants::CONTENT_SPACE);
    startsPt.setY(itemRect.top() + (itemRect.height() - m_pBtnFirst->height()) / 2);
    m_pBtnFirst->setGeometry(startsPt.x(), startsPt.y(), BTN_GEOMERTY_DEFAULT_WITH, BTN_GEOMERTY_DEFAULT_HEIGHT);

    m_ptBtnSecond.setX(itemRect.left() + m_pBtnFirst->width() + 2 * Constants::CONTENT_SPACE);
    m_ptBtnSecond.setY(itemRect.top() + (itemRect.height() - m_pBtnSecond->height()) / 2);    
    m_pBtnSecond->move(m_ptBtnSecond);
    m_pBtnSecond->setGeometry(m_ptBtnSecond.x(), m_ptBtnSecond.y(), BTN_GEOMERTY_DEFAULT_WITH, BTN_GEOMERTY_DEFAULT_HEIGHT);


    //第二列为数据列
    pItem = m_pTableWidgetPreview->item(row, TABLE_COL_DATA);
    if(pItem)
    {
        m_CurItemData = pItem->data(Constants::ActionItemData).value<ActItemData>();
    }

    emit sigCellEntered(row, column);
}

void IActionListItemPreview::onFirstBtn()
{

}

void IActionListItemPreview::onSecondBtn()
{

}
