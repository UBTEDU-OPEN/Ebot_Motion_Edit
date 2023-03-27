/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actionrecommendpage.cpp
* 创建时间：2016/06/17
* 文件标识：
* 文件摘要：推荐动作列表，用户点击主界面的flow图片时，弹出该推荐动作界面
*
* 当前版本：1.0.0.0
* 作    者：   hwx
* 完成时间：2016/06/17
* 版本摘要：
*/

#include <QCheckBox>
#include <QPainter>

#include "actionrecommendpage.h"
#include "ui_actionrecommendpage.h"
#include "ubxactionlibconstants.h"
#include "loginlibmgr.h"
#include "alpharobotlog.h"


RETRANSLATEUIFUNCIMPL(ActionRecommendPage)
ActionRecommendPage::ActionRecommendPage(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::ActionRecommendPage)
{
    m_pRecommendItem = NULL;
    m_pFlowImage  = NULL;
    m_pActionListTable1 = NULL;
    m_pActionListTable2 = NULL;
    m_pActionListTable3 = NULL;
    m_pMultiPageWidget = NULL;

    ui->setupUi(this);

    setUpUI();

    setDefaultStyle();

    AddSubscribe(g_actupdatemgr);
}

ActionRecommendPage::~ActionRecommendPage()
{
    delete ui;
}

/************************************
* 名称: updateData
* 功能: 更新界面
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionRecommendPage::updateData(RecommendActItemData *pRecommendItem)
{
    if (NULL == pRecommendItem)
    {
        return;
    }

    m_pRecommendItem = pRecommendItem;

    resetUI();

    // 设置图片
    setImage(pRecommendItem->m_strRecommendImage);

    // 默认为第1页
    updatePage(1);

    // 设置当前页
    m_pMultiPageWidget->setCurrentPage(1);

    // 更新多页显示控件
    m_pMultiPageWidget->UpdatePages(getTotalPageCount());
}

/************************************
* 名称: OnNotify
* 功能: 实现zxLib::zxObserverBase接口，接收通知消息
* 参数: [in]zxObserverSubjectBase* pSubject 被观察者
* 参数: [in]int nMsgID 消息ID
* 参数: [in]DWORD dwContext 消息上下文
* 参数: [in]WPARAM wParam 消息参数
* 参数: [in]LPARAM lParam 消息参数
* 返回:   void
* 时间:   2016/03/24
* 作者:   hels
************************************/
void ActionRecommendPage::OnNotify(const zxObserverSubjectBase *pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    zxEvent* pEvent = new zxEvent(nMsgID);
    pEvent->eEventType = zxEventTypeWindow;
    pEvent->lReceiver = (long)this;
    pEvent->dwMsgId = nMsgID;
    pEvent->wParam = wParam;
    pEvent->lParam = lParam;
    QCoreApplication::postEvent(this, pEvent);
}

/************************************
* 名称: slotOnCurrentPageChanged
* 功能: 当前页变化的槽函数
* 参数：nCurPage: 当前页
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionRecommendPage::slotOnCurrentPageChanged(int nCurPage)
{
   if (nCurPage > getTotalPageCount())
   {
       return;
   }
   updatePage(nCurPage);
}

/************************************
* 名称: slotOnPreviousPageClicked
* 功能: 点击前一页
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionRecommendPage::slotOnPreviousPageClicked()
{
    int nCurrentPage = m_pMultiPageWidget->getCurrentPage();
    if (nCurrentPage < 1)
    {
        return;
    }
    updatePage(nCurrentPage);
}

/************************************
* 名称: slotOnNextPageClicked
* 功能: 点击下一页
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionRecommendPage::slotOnNextPageClicked()
{
    int nCurrentPage = m_pMultiPageWidget->getCurrentPage();
    if (nCurrentPage > getTotalPageCount())
    {
        return;
    }
    updatePage(nCurrentPage);
}

/************************************
* 名称: paintEvent
* 功能: 摘抄自Qt帮助文档：If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionRecommendPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ActionRecommendPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 自适应屏幕分辨率
    if (m_pFlowImage)
    {
        m_pFlowImage->setFixedHeight(Constants::FLOW_WIDGET_HEIGHT_RATIO * this->height());
        m_pFlowImage->setSlideSize(QSize(m_pFlowImage->width() * Constants::FLOW_IMAGE_WIDTH_RATIO, Constants::FLOW_IMAGE_HEIGHT_RATIO * m_pFlowImage->height()));
    }

//    int nTableWidgetWidth = this->width() * Constants::HOME_PREVIEW_WIDGET_RATIO;
//    int nTableWidgetMaxWidth = DownLoadItemDelegate::getItemMaxWidth();
//    if (nTableWidgetWidth > nTableWidgetMaxWidth)
//    {
//        nTableWidgetWidth = nTableWidgetMaxWidth;
//    }
//    m_pActionListTable1->setFixedWidth(nTableWidgetWidth);
//    m_pActionListTable2->setFixedWidth(nTableWidgetWidth);
//    m_pActionListTable3->setFixedWidth(nTableWidgetWidth);
}

/************************************
* 名称: setUpUI
* 功能: 构建界面
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionRecommendPage::setUpUI()
{
    // flowcover图
    m_pFlowImage = new PictureFlow(this);
    //m_pFlowImage->setBackgroundColor(QColor("#404040"));
    m_pFlowImage->setFixedHeight(Constants::WINDOW_HEIGHT);
    m_pFlowImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pFlowImage->setSlideSize(QSize(Constants::COVER_FLOW_WIDGET_WIDTH, Constants::COVER_FLOW_WIDGET_HEIGHT));
    ui->layoutFlowImage->addWidget(m_pFlowImage);

    m_pActionListTable1 = new ActionListItemPreview(this);
    m_pActionListTable1->GetTableWidget()->setColumnCount(TABLE_COL_DEFAULT_NUMBER);
    m_pActionListTable1->GetTableWidget()->setColumnWidth(TABLE_COL_CHECKBOX, Constants::BTN_CHECOBOX_CONTAINER_WIDTH);
    m_pActionListTable1->GetTableWidget()->setColumnWidth(TABLE_COL_OPERATE, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    connect(m_pActionListTable1, &ActionListItemPreview::SigActionPreviewClicked, this, &ActionRecommendPage::slotOnActionDetailedClicked);

    m_pActionListTable2 = new ActionListItemPreview(this);
    m_pActionListTable2->GetTableWidget()->setColumnCount(TABLE_COL_DEFAULT_NUMBER);
    m_pActionListTable2->GetTableWidget()->setColumnWidth(TABLE_COL_CHECKBOX, Constants::BTN_CHECOBOX_CONTAINER_WIDTH);
    m_pActionListTable2->GetTableWidget()->setColumnWidth(TABLE_COL_OPERATE, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    connect(m_pActionListTable2, &ActionListItemPreview::SigActionPreviewClicked, this, &ActionRecommendPage::slotOnActionDetailedClicked);

    m_pActionListTable3 = new ActionListItemPreview(this);
    m_pActionListTable3->GetTableWidget()->setColumnCount(TABLE_COL_DEFAULT_NUMBER);
    m_pActionListTable3->GetTableWidget()->setColumnWidth(TABLE_COL_CHECKBOX, Constants::BTN_CHECOBOX_CONTAINER_WIDTH);
    m_pActionListTable3->GetTableWidget()->setColumnWidth(TABLE_COL_OPERATE, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    connect(m_pActionListTable3, &ActionListItemPreview::SigActionPreviewClicked, this, &ActionRecommendPage::slotOnActionDetailedClicked);

    m_pMultiPageWidget = new UBXMultiPage(this);
    ui->horizontalLayoutMultiPage->addWidget(m_pMultiPageWidget);
    connect(m_pMultiPageWidget, &UBXMultiPage::sigPrevPageClicked, this, &ActionRecommendPage::slotOnPreviousPageClicked);
    connect(m_pMultiPageWidget, &UBXMultiPage::sigNextPageClicked, this, &ActionRecommendPage::slotOnNextPageClicked);
    connect(m_pMultiPageWidget, &UBXMultiPage::sigCurrentPageChanged, this, &ActionRecommendPage::slotOnCurrentPageChanged);

    ui->horizontalLayoutCol1->insertWidget(1, m_pActionListTable1);
    ui->horizontalLayoutCol2->insertWidget(3, m_pActionListTable2);
    ui->horizontalLayoutCol3->insertWidget(5, m_pActionListTable3);

    connect(ui->pbGoBack, &QPushButton::clicked, this, &ActionRecommendPage::sigGoBack);
//    connect(ui->cbxSelectAll, &QCheckBox::stateChanged, this, &ActionRecommendPage::slotOnSelectAll);
//    connect(ui->pbBatchDownload, &QPushButton::clicked, this, &ActionRecommendPage::slotOnBatchDownloadClicked);
}

/************************************
* 名称: resetData
* 功能: 清空数据
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionRecommendPage::resetUI()
{
    if (m_pFlowImage)
    {
        m_pFlowImage->clear();
    }

    clearActionItems();
}

/************************************
* 名称: clearActionItems
* 功能: 清空表格中的动作数据
* 返回:   void
* 时间:   2016/06/20
* 作者:   hwx
************************************/
void ActionRecommendPage::clearActionItems()
{
    if (m_pActionListTable1)
    {
        m_pActionListTable1->ClearItems();
    }

    if (m_pActionListTable2)
    {
        m_pActionListTable2->ClearItems();
    }

    if (m_pActionListTable3)
    {
        m_pActionListTable3->ClearItems();
    }
}

/************************************
* 名称: setImage
* 功能: 设置页面flow的图片
* 返回:  void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionRecommendPage::setImage(const QString &strImageFile)
{
    QImage imgAction;
    if(!imgAction.load(strImageFile))
    {
        AlphaRobotLog::getInstance()->printError(QString("Failed to load image file %1").arg(strImageFile));
        return;
    }

    m_pFlowImage->addSlide(imgAction);
    m_pFlowImage->setCenterIndex(m_pFlowImage->slideCount() / 2);
    m_pFlowImage->setBackgroundColor(QColor("#404040"));
}

/************************************
* 名称: setDefaultStyle
* 功能: 设置样式
* 返回:  void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionRecommendPage::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/ubxactionlib.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

/************************************
* 名称: getPageActionItems
* 功能: 获取指定页数据
* 参数：[input] nPage：指定的页编号
* 返回:  QList<ActItemData *>: 返回的指定页的数据
* 时间:   2016/06/20
* 作者:   hwx
************************************/
QList<ActItemData *> ActionRecommendPage::getPageActionItems(int nPage)
{
    QList<ActItemData *> lstPageActionItems;
    if (NULL == m_pRecommendItem)
    {
        return lstPageActionItems;
    }

    int nActionNumPerPage = (Constants::RECOMMEND_PAGE_ACTION_COUNT_PER_LIST) * 3;
    int nTotalPage = getTotalPageCount();

    // 输入的页数大于总页数或者输入页数小于1
    // 视为非法输入
    if (nPage > nTotalPage || nPage < 1)
    {
        AlphaRobotLog::getInstance()->printError(QString("Input invalid page number: %1").arg(nPage));
        return lstPageActionItems;
    }

    // 如果不是最后一页，遍历到nActionNumPerPage * nPage即可获取到指定页的动作
    // 如果是最后一页（最后一页动作数码不足填满整页），此时只要遍历到m_pRecommendItem->m_lstActionItems.size()
    int nMaxValue = qMin(nActionNumPerPage * nPage, m_pRecommendItem->m_lstActionItems.size());
    for (int i = nActionNumPerPage * (nPage - 1); i < nMaxValue; i++)
    {
        ActItemData *pActItem = m_pRecommendItem->m_lstActionItems.at(i);
        if (NULL == pActItem)
        {
            continue;
        }

        if (lstPageActionItems.contains(pActItem))
        {
            continue;
        }

        lstPageActionItems.append(pActItem);
    }

    return lstPageActionItems;
}

/************************************
* 名称: getTotalPageCount
* 功能: 获取总页数
* 返回:  int：总页数
* 时间:   2016/06/20
* 作者:   hwx
************************************/
int ActionRecommendPage::getTotalPageCount()
{
    if (NULL == m_pRecommendItem)
    {
        return -1;
    }

    int nActionNumPerPage = (Constants::RECOMMEND_PAGE_ACTION_COUNT_PER_LIST) * 3;
    int nTotalActionCount = m_pRecommendItem->m_lstActionItems.size();
    if (nTotalActionCount != 0 && nTotalActionCount%nActionNumPerPage == 0)
    {
        return nTotalActionCount/nActionNumPerPage;
    }

    return nTotalActionCount/nActionNumPerPage + 1;
}

/************************************
* 名称: updatePage
* 功能: 更新到指定页数据
* 参数:  int：指定页
* 时间:   2016/06/20
* 作者:   hwx
************************************/
void ActionRecommendPage::updatePage(int nPage)
{
    if (nPage < 1)
    {
        return;
    }

    clearActionItems();

    // 根据数据刷新表格
    QList<ActItemData *> lstActionItems = getPageActionItems(nPage);
    for (int nId = 0; nId < lstActionItems.size(); nId++)
    {
        ActItemData *itemData = lstActionItems.at(nId);
        if (NULL == itemData)
        {
            continue;
        }

        QTableWidget* pTableWidget = NULL;
        if(m_pActionListTable1->GetTableWidget()->rowCount() < Constants::RECOMMEND_PAGE_ACTION_COUNT_PER_LIST) // 每页只显示5个
        {
            pTableWidget = m_pActionListTable1->GetTableWidget();
        }
        else if(m_pActionListTable2->GetTableWidget()->rowCount() < Constants::RECOMMEND_PAGE_ACTION_COUNT_PER_LIST)
        {
            pTableWidget = m_pActionListTable2->GetTableWidget();
        }
        else if (m_pActionListTable3->GetTableWidget()->rowCount() < Constants::RECOMMEND_PAGE_ACTION_COUNT_PER_LIST)
        {
            pTableWidget = m_pActionListTable3->GetTableWidget();
        }

        if(!pTableWidget)
        {
            return;
        }

        int row = pTableWidget->rowCount();
        pTableWidget->insertRow(row);

        //复选框
        QTableWidgetItem *item0 = new QTableWidgetItem();//插入空对象，占位符
        pTableWidget->setItem(row, 0, item0);
        QCheckBox* pCheckBox = new QCheckBox();
        pTableWidget->setCellWidget(row, 0, pCheckBox);
        connect(pCheckBox, &QCheckBox::clicked, this, &ActionRecommendPage::slotOnItemCheckBoxClicked);

        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData(Constants::DownLoadItemType, Constants::CUSTOM_DOWNLOAD_ITEM_TYPE);
        QVariant var;
        var.setValue(*itemData);
        item->setData(Constants::ActionItemData, var);
        pTableWidget->setItem(row, 1, item);

        QTableWidgetItem *item2 = new QTableWidgetItem();//插入空对象，占位符，提供给收藏和下载按钮位置
        pTableWidget->setItem(row, 2, item2);

        pTableWidget->resizeRowToContents(row);
        pTableWidget->resizeColumnToContents(1);
    }
}

/************************************
* 名称: downLoadItems
* 功能: 下载指定表格数据
* 参数:  pTable：指定的表格
* 时间:   2016/06/20
* 作者:   hwx
************************************/
void ActionRecommendPage::downLoadItems(QTableWidget *pTable)
{
    if (NULL == pTable)
    {
        return;
    }

    QMap<int, ActItemData> mapItem;
    g_actupdatemgr->GetDownloadedActList(mapItem);
    for(int i = 0; i < pTable->rowCount(); i++)
    {
        QCheckBox* pChkbox = (QCheckBox*)pTable->cellWidget(i, 0);
        if (NULL == pChkbox)
        {
            continue;
        }

        if(pChkbox->isChecked())
        {
            //第二列为数据列
             QTableWidgetItem*  pItem = pTable->item(i, 1);
            if (NULL == pItem)
            {
                continue;
            }

            ActItemData actItem = pItem->data(Constants::ActionItemData).value<ActItemData>();
            if(mapItem.contains(actItem.m_nItemID))
            {
                continue;
            }
            g_actupdatemgr->DownloadAct(actItem);
        }
    }
}

/************************************
* 名称: slotOnSelectAll
* 功能: 选中/取消选中所有动作
* 参数：checked: 是否选中
* 时间:   2016/06/20
* 作者:   hwx
************************************/
void ActionRecommendPage::slotOnSelectAll(bool checked)
{
    for(int i = 0; i < m_pActionListTable1->GetTableWidget()->rowCount(); i++)
    {
        QCheckBox* pChkbox = (QCheckBox*)m_pActionListTable1->GetTableWidget()->cellWidget(i, 0);
        pChkbox->setChecked(checked);
    }

    for(int i = 0; i < m_pActionListTable2->GetTableWidget()->rowCount(); i++)
    {
        QCheckBox* pChkbox = (QCheckBox*)m_pActionListTable2->GetTableWidget()->cellWidget(i, 0);
        pChkbox->setChecked(checked);
    }

    for(int i = 0; i < m_pActionListTable3->GetTableWidget()->rowCount(); i++)
    {
        QCheckBox* pChkbox = (QCheckBox*)m_pActionListTable3->GetTableWidget()->cellWidget(i, 0);
        pChkbox->setChecked(checked);
    }
}

/************************************
* 名称: slotOnItemCheckBoxClicked
* 功能: 选中/取消选中列表中的动作
* 时间:   2016/06/20
* 作者:   hwx
************************************/
void ActionRecommendPage::slotOnItemCheckBoxClicked()
{
    bool bAllSelect = true;

    for(int i = 0; i < m_pActionListTable1->GetTableWidget()->rowCount(); i++)
    {
        QCheckBox* pCheckBox = (QCheckBox*)m_pActionListTable1->GetTableWidget()->cellWidget(i, 0);
        if(!pCheckBox->isChecked())
        {
            bAllSelect = false;
            break;
        }
    }

    if(bAllSelect)
    {
        for(int i = 0; i < m_pActionListTable2->GetTableWidget()->rowCount(); i++)
        {
            QCheckBox* pCheckBox = (QCheckBox*)m_pActionListTable2->GetTableWidget()->cellWidget(i, 0);
            if(!pCheckBox->isChecked())
            {
                bAllSelect = false;
                break;
            }
        }
    }

    if(bAllSelect)
    {
        for(int i = 0; i < m_pActionListTable3->GetTableWidget()->rowCount(); i++)
        {
            QCheckBox* pCheckBox = (QCheckBox*)m_pActionListTable3->GetTableWidget()->cellWidget(i, 0);
            if(!pCheckBox->isChecked())
            {
                bAllSelect = false;
                break;
            }
        }
    }

//    ui->cbxSelectAll->setChecked(bAllSelect);
}

/************************************
* 名称: slotOnBatchDownloadClicked
* 功能: 点击批量下载的槽函数
* 时间:   2016/06/20
* 作者:   hwx
************************************/
void ActionRecommendPage::slotOnBatchDownloadClicked()
{
    if(NULL == g_actupdatemgr)
    {
        return;
    }

    if(-1 == g_actupdatemgr->GetUserID())//未登录时不能下载
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);
        return;
    }

    downLoadItems(m_pActionListTable1->GetTableWidget());
    downLoadItems(m_pActionListTable2->GetTableWidget());
    downLoadItems(m_pActionListTable3->GetTableWidget());
}

/************************************
* 名称: slotOnActionDetailedClicked
* 功能: 预览动作详细信息
* 参数: [in]itemData 对应的动作数据
* 返回:   void
* 时间:   2016/06/18
* 作者:   hwx
************************************/
void ActionRecommendPage::slotOnActionDetailedClicked(ActItemData itemData)
{
    emit sigActionDetailedClicked(BUTTON_RECOMMEND_SUBJECT_PAGE, itemData);
}
