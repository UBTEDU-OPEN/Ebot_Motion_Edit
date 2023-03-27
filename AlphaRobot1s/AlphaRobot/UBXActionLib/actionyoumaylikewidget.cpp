/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actionyoumaylikewidget.cpp
* 创建时间：2016/06/17
* 文件标识：
* 文件摘要：猜你喜欢动作控件
*
* 当前版本：1.0.0.0
* 作    者：   hwx
* 完成时间：2016/06/17
* 版本摘要：
*/

#include <QTableWidget>
#include <QCheckBox>

#include "actionyoumaylikewidget.h"
#include "ui_actionyoumaylikewidget.h"
#include "actionlibmgr.h"
#include "ubxactionlibconstants.h"
#include "loginlibmgr.h"
#include "actiondetailwidget.h"


RETRANSLATEUIFUNCIMPL(ActionYouMayLikeWidget)
ActionYouMayLikeWidget::ActionYouMayLikeWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::ActionYouMayLikeWidget),
    m_nRequestID(-1),
    m_pActionList(NULL)
{
    ui->setupUi(this);

    initUI();
}

ActionYouMayLikeWidget::~ActionYouMayLikeWidget()
{
    delete ui;
}

/************************************
* 名称: requestForYouMayLikeAction
* 功能: 请求猜你喜欢的动作
* 参数：[in] eGroup：动作组类，如最热的
* 参数：[in] eType：动作类型，如舞蹈
* 返回:   void
* 时间:   2016/06/19
* 作者:   hwx
************************************/
void ActionYouMayLikeWidget::requestForYouMayLikeAction(emActionGroup eGroup, emActionType eType)
{
//    // 在发起请求前清理界面
//    resetUI();

    if (g_actupdatemgr)
    {
        ///////////////////////////////Log/////////////////////////////////////
        QString strLog = QString("[requestForYouMayLikeAction]request for action list: action group: %1, action type: %2, page: %3, action/page: %4, request ID: %5, zone: %6").arg(eGroup)
                .arg(eType).arg(1).arg(Constants::ACTION_YOU_MAY_LIKE_NUMBER).arg(m_nRequestID).arg(m_ZoneCode);
        AlphaRobotLog::getInstance()->printDebug(strLog);
        //////////////////////////////Log//////////////////////////////////////
        // 由于请求返回的动作可能已经在动作详情页显示，此时不会在猜你喜欢的动作列表中重复显示，
        // 因此需要多请求一个数据
        m_nRequestID = g_actupdatemgr->QueryActList(eGroup, eType, 1, Constants::ACTION_YOU_MAY_LIKE_NUMBER + 1, m_nRequestID, m_ZoneCode.toInt());
    }
}

/************************************
* 名称: slotOnItemDate
* 功能: 列表项数据处理槽函数
* 参数: [in]requestID 请求ID
* 参数: [in]itemData 列表项数据
* 参数: [in]nTotalPage 总页数
* 返回:   void
* 时间:   2016/04/11
* 作者:   hwx
************************************/
void ActionYouMayLikeWidget::slotOnItemDate(int requestID, ActItemData itemData, int nTotalPage)
{
    if(m_nRequestID == requestID)
    {
        // 如果请求返回的动作是当前动作详情页的动作，则不会在列表中显示
        if (m_CurItemData.m_nItemID == itemData.m_nItemID)
        {
            return;
        }

        QTableWidget *pTableWidget = m_pActionList->GetTableWidget();
        if(pTableWidget->rowCount() < Constants::ACTION_YOU_MAY_LIKE_NUMBER)
        {            
            m_pActionList->addItemData(itemData);
            m_pActionList->setCheckVisual(false);
        }
    }
}

/************************************
* 名称: slotOnMoreClicked
* 功能: 点击more按钮的槽函数
* 返回:   void
* 时间:   2016/06/19
* 作者:   hwx
************************************/
void ActionYouMayLikeWidget::slotOnMoreClicked()
{
    emit sigOnMoreClicked();
}

void ActionYouMayLikeWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ActionYouMayLikeWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    int nTableWidgetWidth = this->width();
    int nTableWidgetMaxWidth = DownLoadItemDelegate::getItemMaxWidth();
    if (nTableWidgetWidth > nTableWidgetMaxWidth)
    {
        nTableWidgetWidth = nTableWidgetMaxWidth;
    }
    m_pActionList->setFixedWidth(nTableWidgetWidth);
}

/************************************
* 名称: setCurrentItemData
* 功能: 设置当前动作详情对应的动作
* 返回:   void
* 时间:   2016/06/19
* 作者:   hwx
************************************/
void ActionYouMayLikeWidget::setCurrentItemData(const ActItemData &itemData)
{
    m_CurItemData = itemData;
}

/************************************
* 名称: initUI
* 功能: 初始化
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionYouMayLikeWidget::initUI()
{
    m_pActionList = new ActionListItemPreview(this);
    m_pActionList->GetTableWidget()->setColumnCount(TABLE_COL_DEFAULT_NUMBER);
    m_pActionList->GetTableWidget()->setColumnWidth(TABLE_COL_CHECKBOX, Constants::BTN_CHECOBOX_CONTAINER_WIDTH);
    m_pActionList->GetTableWidget()->hideColumn(TABLE_COL_CHECKBOX); // 隐藏checkbox选中列
    m_pActionList->GetTableWidget()->setColumnWidth(TABLE_COL_OPERATE, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    connect(m_pActionList, &ActionListItemPreview::SigActionPreviewClicked, this, &ActionYouMayLikeWidget::sigOnActionPreviewClicked);

    ui->verticalLayoutActList->addWidget(m_pActionList);
    ui->verticalLayoutActList->addStretch(1);

    connect(ui->pbMore, &QPushButton::clicked, this, &ActionYouMayLikeWidget::slotOnMoreClicked);

    resetUI();
}

/************************************
* 名称: resetUI
* 功能: 复位界面
* 返回:   void
* 时间:   2016/06/19
* 作者:   hwx
************************************/
void ActionYouMayLikeWidget::resetUI()
{
    m_pActionList->ClearItems();
}
