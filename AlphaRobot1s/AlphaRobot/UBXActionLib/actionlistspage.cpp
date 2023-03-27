/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actionlistspage.cpp
* 创建时间：2016/03/22
* 文件标识：
* 文件摘要：动作列表界面，按类别显示动作列表，并可进行动作的收藏、下载等操作
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/22
* 版本摘要：
*/
#include "actionlistspage.h"
#include "ui_actionlistspage.h"

#include <QPainter>


#include "actionlistitempreview.h"
#include "ActUpdateMangr.h"
#include "ubxactionlibconstants.h"
#include "loginlibmgr.h"
#include "../zxLib/widget/ubxwidget.h"

const int BTN_INDEX_ACT_ALL = 0;
const int BTN_INDEX_ACT_OFFICAL = BTN_INDEX_ACT_ALL + 1;

const int BTN_INDEX_ACT_LAST_UPDATE = 0;
const int BTN_INDEX_ACT_HOTTEST = BTN_INDEX_ACT_LAST_UPDATE + 1;
const int BTN_INDEX_ACT_MOST_COMMENT = BTN_INDEX_ACT_HOTTEST + 1;


RETRANSLATEUIFUNCIMPL(ActionListsPage)
ActionListsPage::ActionListsPage(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::ActionListsPage)
{
    ui->setupUi(this);

    m_nCurrentIndexFirstLevelBtn = BTN_INDEX_ACT_ALL;
    m_nCurrentIndexThreeLevelBtn = BTN_INDEX_ACT_LAST_UPDATE;

    m_pActListPreviewCol1 = NULL;
    m_pActListPreviewCol2 = NULL;
    m_pActListPreviewCol3 = NULL;
    m_pMultiPages = NULL;
    m_emActionGroup = eActGroupOfficalLastUpdate;//默认为所有动作组
    m_emActionType = eActTypeAll;//默认为所有动作
    m_nRequestID = -1;

    setUpUi();

    InitActionListItem();

    UBXWidget::setDefaultStyle(this, ":/res/qss/ubxactionlib.qss");

    AddSubscribe(g_actupdatemgr);
    connect(g_actupdatemgr, &CActUpdateMangr::SigItemDate, this, &ActionListsPage::OnItemDate);
    connect(g_actupdatemgr, &CActUpdateMangr::SigItemUpdate, this, &ActionListsPage::OnItemUpdate);
    connect(g_actupdatemgr, &CActUpdateMangr::SigTotalPage, this, &ActionListsPage::OnTotalPage);

    InitFirstLevelBtns();
    InitSecondLevelBtns();
    InitThreeLevelBtns();
}

ActionListsPage::~ActionListsPage()
{
    delete ui;
}

void ActionListsPage::InitActionListItem()
{
    m_pActListPreviewCol1 = new ActionListItemPreview(this);
    connect(m_pActListPreviewCol1, &ActionListItemPreview::SigActionPreviewClicked, this, &ActionListsPage::SigActionPreviewClicked);
    m_pActListPreviewCol1->GetTableWidget()->setColumnCount(3);
    m_pActListPreviewCol1->GetTableWidget()->setColumnWidth(TABLE_COL_CHECKBOX, Constants::BTN_CHECOBOX_CONTAINER_WIDTH);
    m_pActListPreviewCol1->GetTableWidget()->setColumnWidth(TABLE_COL_OPERATE, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    m_pActListPreviewCol1->setCheckVisual(true);

    m_pActListPreviewCol2 = new ActionListItemPreview(this);
    connect(m_pActListPreviewCol2, &ActionListItemPreview::SigActionPreviewClicked, this, &ActionListsPage::SigActionPreviewClicked);
    m_pActListPreviewCol2->GetTableWidget()->setColumnCount(3);
    m_pActListPreviewCol2->GetTableWidget()->setColumnWidth(TABLE_COL_CHECKBOX, Constants::BTN_CHECOBOX_CONTAINER_WIDTH);
    m_pActListPreviewCol2->GetTableWidget()->setColumnWidth(TABLE_COL_OPERATE, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    m_pActListPreviewCol2->setCheckVisual(true);

    m_pActListPreviewCol3 = new ActionListItemPreview(this);
    connect(m_pActListPreviewCol3, &ActionListItemPreview::SigActionPreviewClicked, this, &ActionListsPage::SigActionPreviewClicked);
    m_pActListPreviewCol3->GetTableWidget()->setColumnCount(3);
    m_pActListPreviewCol3->GetTableWidget()->setColumnWidth(TABLE_COL_CHECKBOX, Constants::BTN_CHECOBOX_CONTAINER_WIDTH);
    m_pActListPreviewCol3->GetTableWidget()->setColumnWidth(TABLE_COL_OPERATE, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    m_pActListPreviewCol3->setCheckVisual(true);

    ui->horizontalLayoutCol1->insertWidget(1, m_pActListPreviewCol1);
    ui->horizontalLayoutCol2->insertWidget(3, m_pActListPreviewCol2);
    ui->horizontalLayoutCol3->insertWidget(5, m_pActListPreviewCol3);
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
void ActionListsPage::OnNotify(const zxObserverSubjectBase *pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
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
* 名称: SetQueryAction
* 功能: 设置查询类型
* 参数: [in]actGroup 动作查询组类型
* 参数: [in]actType 动作类型
* 返回:   void
* 时间:   2016/04/12
* 作者:   hels
************************************/
void ActionListsPage::SetQueryAction(emActionGroup actGroup, emActionType actType)
{
    switch(actGroup)
    {
    case eActGroupAllLastUpdate:
    case eActGroupAllHotest:
    case eActGroupAllMostComment:
        ui->pbAllActions->setChecked(true);
        break;

    case eActGroupOfficalLastUpdate:
    case eActGroupOfficalHotest:
    case eActGroupOfficalMostComment:
        ui->pbOfficialActions->setChecked(true);
        break;

    default:
        break;
    }

    switch(actGroup)
    {
    case eActGroupAllLastUpdate:
    case eActGroupOfficalLastUpdate:
        ui->pbLatest->setChecked(true);
        break;

    case eActGroupAllHotest:
    case eActGroupOfficalHotest:
        ui->pbHottest->setChecked(true);
        break;

    case eActGroupAllMostComment:
    case eActGroupOfficalMostComment:
        ui->pbComments->setChecked(true);
        break;

    default:
        break;
    }



    switch(actType)
    {
    case eActTypeAll:
        ui->pbActionPageAll->setChecked(true);
        break;
    case eActTypeDance:
        ui->pbActionPageDances->setChecked(true);
        break;
    case eActTypeStory:
        ui->pbActionPageStories->setChecked(true);
        break;
    case eActTypeSport:
        ui->pbActionPageSports->setChecked(true);
        break;
    case eActTypeMusic:
        ui->pbActionPageMusics->setChecked(true);
        break;
    case eActTypeEducation:
        ui->pbActionPageEducation->setChecked(true);
        break;

    default:
        break;
    }
}

/************************************
* 名称: SetUserLogIn
* 功能: 设置用户登录状态
* 参数: [in]bLogin 用户是否登录
* 返回:   void
* 时间:   2016/04/15
* 作者:   hels
************************************/
void ActionListsPage::SetUserLogIn(bool bLogin)
{
//    ui->pbBatchDownload->setVisible(bLogin);
    ui->pbBatchDownload->setVisible(true);
    onUserLoginStatusChanged(bLogin);
}

/************************************
* 名称: setButtonCurrentChecked
* 功能: 设置当前选中的三层菜单按钮
* 参数: [in]eActionGroup 动作组
* 参数: [in]eActionType 动作类型
* 返回:   void
* 时间:   2016/08/22
* 作者:   hwx
************************************/
void ActionListsPage::setButtonCurrentChecked(emActionGroup eActionGroup, emActionType eActionType)
{
    setFirstLevelButtonCurrentChecked(eActionGroup);
    setSecondLevelButtonCurrentChecked(eActionType);
    setThirdLevelButtonCurrentChecked(eActionGroup);
}

/************************************
* 名称: InitFirstLevelBtns
* 功能: 初始化第一排按钮组（所有动作，官方动作按钮）
* 返回:   void
* 时间:   2016/03/22
* 作者:   hels
************************************/
void ActionListsPage::InitFirstLevelBtns()
{
    m_btnGroupForFirstLevel.addButton(ui->pbAllActions, BTN_INDEX_ACT_ALL);
    m_btnGroupForFirstLevel.addButton(ui->pbOfficialActions, BTN_INDEX_ACT_OFFICAL);

    connect(&m_btnGroupForFirstLevel, static_cast<void(QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled),
            this, &ActionListsPage::OnFirstLevelButtonToggled);

    ui->pbOfficialActions->setChecked(true);
}

/************************************
* 名称: InitSecondLevelBtns
* 功能: 初始化第二排按钮组（所有、基本、舞蹈、讲故事）
* 返回:   void
* 时间:   2016/03/22
* 作者:   hels
************************************/
void ActionListsPage::InitSecondLevelBtns()
{
    m_btnGroupForSencondLevel.addButton(ui->pbActionPageAll, eActTypeAll);
    m_btnGroupForSencondLevel.addButton(ui->pbActionPageDances, eActTypeDance);
    m_btnGroupForSencondLevel.addButton(ui->pbActionPageStories, eActTypeStory);
    m_btnGroupForSencondLevel.addButton(ui->pbActionPageSports, eActTypeSport);
    m_btnGroupForSencondLevel.addButton(ui->pbActionPageMusics, eActTypeMusic);
    m_btnGroupForSencondLevel.addButton(ui->pbActionPageEducation, eActTypeEducation);

    connect(&m_btnGroupForSencondLevel, static_cast<void (QButtonGroup:: *)(int, bool)>(&QButtonGroup::buttonToggled),
            this, &ActionListsPage::OnSencondLevelButtonToggled);

    ui->pbActionPageAll->setChecked(true);
}

/************************************
* 名称: InitThreeLevelBtns
* 功能: 初始化第三排按钮组（最新、最热、下载量、评论数）
* 返回:   void
* 时间:   2016/03/22
* 作者:   hels
************************************/
void ActionListsPage::InitThreeLevelBtns()
{
    m_btnGroupForThreeLevel.addButton(ui->pbLatest, BTN_INDEX_ACT_LAST_UPDATE);
    m_btnGroupForThreeLevel.addButton(ui->pbHottest, BTN_INDEX_ACT_HOTTEST);

    //由于服务器不支持按下载量排行，暂时先注释，以后支持了再开启
    //m_btnGroupForThreeLevel.addButton(ui->pbDownloads, 2);
    ui->pbDownloads->hide();
    m_btnGroupForThreeLevel.addButton(ui->pbComments, BTN_INDEX_ACT_MOST_COMMENT);
    connect(&m_btnGroupForThreeLevel, static_cast<void (QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled),
            this, &ActionListsPage::OnThreeLevelButtonToggled);

    connect(ui->pbPrevPage, &QPushButton::clicked, this, &ActionListsPage::OnPrevPageClicked);
    connect(ui->pbNextPage, &QPushButton::clicked, this, &ActionListsPage::OnNextPageClicked);

    ui->pbLatest->setChecked(true);//init the ui data
}

/************************************
* 名称: setUpUi
* 功能: 构建界面
* 返回:   void
* 时间:   2016/03/28
* 作者:   hels
************************************/
void ActionListsPage::setUpUi()
{
    m_pMultiPages = new UBXMultiPage(this);
    ui->horizontalLayoutMultiPage->addWidget(m_pMultiPages);

    connect(m_pMultiPages, &UBXMultiPage::sigPrevPageClicked, this, &ActionListsPage::OnPrevPageClicked);
    connect(m_pMultiPages, &UBXMultiPage::sigNextPageClicked, this, &ActionListsPage::OnNextPageClicked);
    connect(m_pMultiPages, &UBXMultiPage::sigCurrentPageChanged, this, &ActionListsPage::onCurrentPageChanged);

    connect(ui->pbBatchDownload, &QPushButton::clicked, this, &ActionListsPage::OnBatchDownload);
    connect(ui->checkBoxSelectAll, &QCheckBox::clicked, this, &ActionListsPage::OnSelAllCheckBoxClicked);
}

/************************************
* 名称: setFirstLevelButtonCurrentChecked
* 功能: 设置当前选中的一级按钮
* 参数：[in]eActionGroup:动作组
* 返回:   void
* 时间:   2016/08/22
* 作者:   hwx
************************************/
void ActionListsPage::setFirstLevelButtonCurrentChecked(emActionGroup eActionGroup)
{
    switch(eActionGroup)
    {
    case eActGroupAllLastUpdate:
    case eActGroupAllHotest:
    case eActGroupAllMostComment:
        ui->pbAllActions->setChecked(true);
        break;
    case eActGroupOfficalLastUpdate:
    case eActGroupOfficalHotest:
    case eActGroupOfficalMostComment:
        ui->pbOfficialActions->setChecked(true);
        break;
    default:
        break;
    }
}

/************************************
* 名称: setSecondLevelButtonCurrentChecked
* 功能: 设置当前选中的二级按钮
* 参数：[in]eActionType:动作类型
* 返回:   void
* 时间:   2016/08/22
* 作者:   hwx
************************************/
void ActionListsPage::setSecondLevelButtonCurrentChecked(emActionType eActionType)
{
    switch (eActionType) {
    case eActTypeAll:
    {
        ui->pbActionPageAll->setChecked(true);
        break;
    }
    case eActTypeDance:
    {
        ui->pbActionPageDances->setChecked(true);
        break;
    }
    case eActTypeStory:
    {
        ui->pbActionPageStories->setChecked(true);
        break;
    }
    case eActTypeMusic:
    {
        ui->pbActionPageMusics->setChecked(true);
        break;
    }
    case eActTypeSport:
    {
        ui->pbActionPageSports->setChecked(true);
        break;
    }
    case eActTypeEducation:
    {
        ui->pbActionPageEducation->setChecked(true);
        break;
    }
    default:
        break;
    }
}

/************************************
* 名称: setThirdLevelButtonCurrentChecked
* 功能: 设置当前选中的三级按钮
* 参数：[in]eActionGroup:动作组
* 返回:   void
* 时间:   2016/08/22
* 作者:   hwx
************************************/
void ActionListsPage::setThirdLevelButtonCurrentChecked(emActionGroup eActionGroup)
{
    switch(eActionGroup)
    {
    case eActGroupAllLastUpdate:
    case eActGroupOfficalLastUpdate:
    {
        ui->pbLatest->setChecked(true);
        break;
    }
    case eActGroupAllHotest:
    case eActGroupOfficalHotest:
    {
        ui->pbHottest->setChecked(true);
        break;
    }
    case eActGroupAllMostComment:
    case eActGroupOfficalMostComment:
    {
        ui->pbComments->setChecked(true);
        break;
    }
    default:
        break;
    }
}

//If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
void ActionListsPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ActionListsPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

//    // 根据屏幕分辨率自动标志显示，
//    // 当分辨率很高时，限制动作预览表格最大宽度，否则显示会很难看
//    int nTableWidgetWidth = this->width() * Constants::HOME_PREVIEW_WIDGET_RATIO;
//    int nTableWidgetMaxWidth = DownLoadItemDelegate::getItemMaxWidth();
//    if (nTableWidgetWidth > nTableWidgetMaxWidth)
//    {
//        nTableWidgetWidth = nTableWidgetMaxWidth;
//    }
//    m_pActListPreviewCol1->setFixedWidth(nTableWidgetWidth);
//    m_pActListPreviewCol2->setFixedWidth(nTableWidgetWidth);
//    m_pActListPreviewCol3->setFixedWidth(nTableWidgetWidth);
}

/************************************
* 名称: customEvent
* 功能: 自定义事件处理函数
* 参数: [in]QEvent *event事件对象指针
* 返回:   void
* 时间:   2016/03/24
* 作者:   hels
************************************/
void ActionListsPage::customEvent(QEvent *event)
{
    zxEvent* pEvent = (zxEvent*)event;
    DWORD nMsgID = pEvent->dwMsgId;
    WPARAM wParam = pEvent->wParam;
    LPARAM lParam = pEvent->lParam;

    if(nMsgID == NOTIFY_MSG_ACTLIB_QUERYLIST_RECV)
    {

    }
}

/************************************
* 名称: ClearItems
* 功能: 清空所有数据
* 返回:   void
* 时间:   2016/03/25
* 作者:   hels
************************************/
void ActionListsPage::ClearItems()
{
    //ui->labelPages->setText("0\/0");
    if(m_pActListPreviewCol1)
    {
        m_pActListPreviewCol1->GetTableWidget()->clear();
        m_pActListPreviewCol1->GetTableWidget()->setRowCount(0);
    }

    if(m_pActListPreviewCol2)
    {
        m_pActListPreviewCol2->GetTableWidget()->clear();
        m_pActListPreviewCol2->GetTableWidget()->setRowCount(0);
    }

    if(m_pActListPreviewCol3)
    {
        m_pActListPreviewCol3->GetTableWidget()->clear();
        m_pActListPreviewCol3->GetTableWidget()->setRowCount(0);
    }

    ui->checkBoxSelectAll->setChecked(false);
}

/************************************
* 名称: OnFirstLevelButtonToggled
* 功能: 第一排按钮组槽函数
* 参数: [in]int-id 发送信号的按钮id
* 参数: [in]bool-bChecked 按钮是否选中
* 返回:   void
* 时间:   2016/03/22
* 作者:   hels
************************************/
void ActionListsPage::OnFirstLevelButtonToggled(int id, bool bChecked)
{
    if(bChecked)
    {
        m_nCurrentIndexFirstLevelBtn = id;
        if((BTN_INDEX_ACT_ALL == id) ||
                (BTN_INDEX_ACT_OFFICAL == id))
        {
            ClearItems();
            m_pMultiPages->setCurrentPage(1); // 默认显示第一页
            m_emActionGroup = (emActionGroup)getActionGroup(m_nCurrentIndexFirstLevelBtn, m_nCurrentIndexThreeLevelBtn);
            m_nRequestID = g_actupdatemgr->QueryActList(m_emActionGroup, m_emActionType, 1, Constants::ACTION_LIST_PAGE_SIZE, m_nRequestID, m_ZoneCode.toInt());
        }
    }
}

/************************************
* 名称: OnSencondLevelButtonToggled
* 功能: 第二排按钮组槽函数
* 参数: [in]int-id 发送信号的按钮id
* 参数: [in]bool-bChecked 按钮是否选中
* 返回:   void
* 时间:   2016/03/22
* 作者:   hels
************************************/
void ActionListsPage::OnSencondLevelButtonToggled(int id, bool bChecked)
{
    // id是将Button添加到ButtonGroup时给的编号
    if (id < 0 || id >5)
    {
        return;
    }

    if(bChecked)
    {
        ClearItems();
        if(eActTypeAll == id)//所有动作
        {
            m_emActionType = eActTypeAll;
            ui->labelSelWhat->setText(tr("All"));
        }
        else if(eActTypeDance == id) // Dance
        {
            m_emActionType = eActTypeDance;
            ui->labelSelWhat->setText(tr("Dance"));
        }
        else if(eActTypeStory == id)//Story
        {
            m_emActionType = eActTypeStory;
            ui->labelSelWhat->setText(tr("Story "));
        }
        else if(eActTypeSport == id) // Sport
        {
            m_emActionType = eActTypeSport;
            ui->labelSelWhat->setText(tr("Sport"));
        }
        else if (eActTypeMusic == id) // Chant
        {
            m_emActionType = eActTypeMusic;
            ui->labelSelWhat->setText(tr("Chant"));
        }
        else if (eActTypeEducation == id) // education
        {
            m_emActionType = eActTypeEducation;
            ui->labelSelWhat->setText(tr("Education"));
        }

        m_pMultiPages->setCurrentPage(1); // 默认显示第一页
         m_nRequestID = g_actupdatemgr->QueryActList(m_emActionGroup, m_emActionType, 1, Constants::ACTION_LIST_PAGE_SIZE, m_nRequestID, m_ZoneCode.toInt());
    }
}

/************************************
* 名称: OnThreeLevelButtonToggled
* 功能: 第三排按钮组槽函数
* 参数: [in]int-id 发送信号的按钮id
* 参数: [in]bool-bChecked 按钮是否选中
* 返回:   void
* 时间:   2016/03/22
* 作者:   hels
************************************/
void ActionListsPage::OnThreeLevelButtonToggled(int id, bool bChecked)
{
    if(bChecked)
    {
        if((BTN_INDEX_ACT_LAST_UPDATE == id) ||
                (BTN_INDEX_ACT_HOTTEST == id) ||
                (BTN_INDEX_ACT_MOST_COMMENT == id) )
        {

            m_nCurrentIndexThreeLevelBtn = id;

            ClearItems();
            m_pMultiPages->setCurrentPage(1); // 默认显示第一页
            m_emActionGroup = (emActionGroup)getActionGroup(m_nCurrentIndexFirstLevelBtn, m_nCurrentIndexThreeLevelBtn);
            m_nRequestID = g_actupdatemgr->QueryActList(m_emActionGroup, m_emActionType, 1, Constants::ACTION_LIST_PAGE_SIZE, m_nRequestID, m_ZoneCode.toInt());
        }
    }
}

/************************************
* 名称: onCurrentPageChanged
* 功能: 当前页发生变化
* 参数: [in]nCurPage 当前页
* 返回:   void
* 时间:   2016/03/28
* 作者:   hels
************************************/
void ActionListsPage::onCurrentPageChanged(int nCurPage)
{
    if (nCurPage < 0 || (nCurPage > m_pMultiPages->getTotalPage()))
    {
        return;
    }

    ClearItems();
    m_nRequestID = g_actupdatemgr->QueryActList(m_emActionGroup, m_emActionType, nCurPage, Constants::ACTION_LIST_PAGE_SIZE, m_nRequestID, m_ZoneCode.toInt());
}

/************************************
* 名称: OnItemDate
* 功能: 列表项数据处理槽函数
* 参数: [in]requestID 请求ID
* 参数: [in]itemData 列表项数据
* 参数: [in]nTotalPage 总页数
* 返回:   void
* 时间:   2016/03/25
* 作者:   hels
************************************/
void ActionListsPage::OnItemDate(int requestID, ActItemData itemData, int /*nTotalPage*/)
{
    if((REQUSET_ID_DEFAULT_INVALID != m_nRequestID) && (m_nRequestID == requestID))
    {
        //        QTableWidget* pTableWidget = NULL;
        ActionListItemPreview* pActListPreview = NULL;

        if(m_pActListPreviewCol1->GetTableWidget()->rowCount() < Constants::ACTION_LIST_COUNT_PER_LIST)
        {
            pActListPreview = m_pActListPreviewCol1;
        }
        else if(m_pActListPreviewCol2->GetTableWidget()->rowCount() < Constants::ACTION_LIST_COUNT_PER_LIST)
        {
            pActListPreview = m_pActListPreviewCol2;
        }
        else if(m_pActListPreviewCol3->GetTableWidget()->rowCount() < Constants::ACTION_LIST_COUNT_PER_LIST)
        {
            pActListPreview = m_pActListPreviewCol3;
        }

        if(pActListPreview)
        {
            pActListPreview->addItemData(itemData);
        }
    }
}

void ActionListsPage::OnItemUpdate()
{
    QTableWidget* pTableWidget = NULL;
    pTableWidget = m_pActListPreviewCol1->GetTableWidget();
    pTableWidget->update();

    pTableWidget = m_pActListPreviewCol2->GetTableWidget();
    pTableWidget->update();

    pTableWidget = m_pActListPreviewCol3->GetTableWidget();
    pTableWidget->update();
}

/************************************
* 名称: OnTotalPage
* 功能: 总页数处理槽函数
* 参数: [in]requestID 请求ID
* 参数: [in]nTotalPage 页码总数
* 返回:   void
* 时间:   2016/03/25
* 作者:   hels
************************************/
void ActionListsPage::OnTotalPage(int requestID, int nTotalPage)
{
    if(requestID == m_nRequestID)
    {
        int nCurrentPage = m_pMultiPages->getCurrentPage();
        QString strPage = QString::number(nCurrentPage);
        strPage += "/" + QString::number(nTotalPage);
        ui->labelPages->setText(strPage);

        // 如果不先隐藏，显示的时候按钮会被拖长，很那看
        m_pMultiPages->hide();
        m_pMultiPages->UpdatePages(nTotalPage);
        m_pMultiPages->show();
    }
}

/************************************
* 名称: OnPrevPageClicked
* 功能: 上一页按钮槽函数
* 返回:   void
* 时间:   2016/03/25
* 作者:   hels
************************************/
void ActionListsPage::OnPrevPageClicked()
{
    int nCurrentPage = m_pMultiPages->getCurrentPage();
    if(nCurrentPage > 0)
    {
        ClearItems();
        m_nRequestID = g_actupdatemgr->QueryActList(m_emActionGroup, m_emActionType, nCurrentPage, Constants::ACTION_LIST_PAGE_SIZE, m_nRequestID, m_ZoneCode.toInt());
    }
}

/************************************
* 名称: OnNextPageClicked
* 功能: 下一页按钮槽函数
* 返回:   void
* 时间:   2016/03/25
* 作者:   hels
************************************/
void ActionListsPage::OnNextPageClicked()
{
    int nCurrentPage = m_pMultiPages->getCurrentPage();
    if(nCurrentPage <= m_pMultiPages->getTotalPage())
    {
        ClearItems();

        m_nRequestID = g_actupdatemgr->QueryActList(m_emActionGroup, m_emActionType, nCurrentPage, Constants::ACTION_LIST_PAGE_SIZE, m_nRequestID, m_ZoneCode.toInt());
    }
}

/************************************
* 名称: OnItemCheckBoxClicked
* 功能: 响应下载项checkbox槽函数
* 参数: [in]checked 按钮check状态
* 返回:   void
* 时间:   2016/03/29
* 作者:   hels
************************************/
void ActionListsPage::OnItemCheckBoxClicked(bool checked)
{
    bool bAllSelect = true;

    ActionListItemPreview *pItemPreview = NULL;

    for(int index = 0; (index < 3) && bAllSelect; index++)
    {
        pItemPreview = getItemPreview(index);

        if(pItemPreview)
        {
            bAllSelect &= pItemPreview->isAllCheck();
            if(!bAllSelect)
            {
                break;
            }
        }
    }

    ui->checkBoxSelectAll->setChecked(bAllSelect);
}

/************************************
* 名称: OnSelAllCheckBoxClicked
* 功能: 响应全选按钮槽函数
* 参数: [in]checked 按钮check状态
* 返回:   void
* 时间:   2016/03/29
* 作者:   hels
************************************/
void ActionListsPage::OnSelAllCheckBoxClicked(bool bChecked)
{    
    for(int index = 0; index < 3; index++)
    {
        getItemPreview(index)->setAllCheckStatus(bChecked);
    }
}

/************************************
* 名称: OnBatchDownload
* 功能: 批量下载
* 返回:   void
* 时间:   2016/04/08
* 作者:   hels
************************************/
void ActionListsPage::OnBatchDownload()
{
    if(NULL == g_actupdatemgr)
    {
        return;
    }

    if(-1 == g_actupdatemgr->GetUserID())//未登录时不能下载
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);
    }

    // add by huangwuxian 2016.7.6 禅道2592
    // 弹出登录界面后，如果用户没有登录，则返回
    // 如果用户选择了登录，则进入下载状态
    if (-1 == g_actupdatemgr->GetUserID())
    {
        return;
    }

//    // 已经下载的动作
//    QMap<int, ActItemData> mapItem;
//    g_actupdatemgr->GetDownloadedActList(mapItem);

    // add by huangwuxian 2016.06.28 禅道1790
    // 解决批量下载后，点击暂停，再次使用批量下载时，下载进度不会删掉
    // 同时不会断点续传的问题。原因是：本地已经下载了一部分数据，但是
    // 在actionlistpage中actionitem的数据来自服务器，没有记录已经下载的数
    // 据信息，因此需要读取本地正在下载的数据，进行断点续传

    QTableWidgetItem* pItem = NULL;
    ActItemData actItem;
    QTableWidget *pTabelWidget = NULL;

    for(int index = 0; index < 3; index++)
    {
        pTabelWidget = getTableWidget(index);
        if(pTabelWidget)
        {
            for(int i = 0; i < pTabelWidget->rowCount(); i++)
            {
                QCheckBox* pChkbox = dynamic_cast<QCheckBox*>(pTabelWidget->cellWidget(i, TABLE_COL_CHECKBOX));
                if(pChkbox && pChkbox->isChecked())
                {
                    //第二列为数据列
                    pItem = pTabelWidget->item(i, TABLE_COL_DATA);
                    actItem = pItem->data(Constants::ActionItemData).value<ActItemData>();

                    g_actupdatemgr->DownloadAct(actItem);
                }
            }
        }
    }
}

/************************************
* 名称: onUserLoginStatusChanged
* 功能: 用户登录状态发生改变
* 返回:   void
* 时间:   2017/03/29
* 作者:   hwx
************************************/
void ActionListsPage::onUserLoginStatusChanged(bool bLogin)
{
    OnThreeLevelButtonToggled(m_nCurrentIndexThreeLevelBtn, true);
}

/************************************
* 名称: getActionGroup
* 参数: [in]indexFirstLevelBtn
*       : [in]indexThreeLevelBtn
* 返回:   int
* 时间:   2016/06/20
* 作者:   asu
************************************/
int ActionListsPage::getActionGroup(int indexFirstLevelBtn, int indexThreeLevelBtn) const
{
    int nActionGroup = eActGroupAllLastUpdate;

    if((BTN_INDEX_ACT_ALL == indexFirstLevelBtn) && (BTN_INDEX_ACT_LAST_UPDATE == indexThreeLevelBtn))
    {
        nActionGroup = eActGroupAllLastUpdate;
    }

    else  if((BTN_INDEX_ACT_ALL == indexFirstLevelBtn) && (BTN_INDEX_ACT_HOTTEST == indexThreeLevelBtn))
    {
        nActionGroup = eActGroupAllHotest;
    }

    else  if((BTN_INDEX_ACT_ALL == indexFirstLevelBtn) && (BTN_INDEX_ACT_MOST_COMMENT == indexThreeLevelBtn))
    {
        nActionGroup = eActGroupAllMostComment;
    }

    else  if((BTN_INDEX_ACT_OFFICAL == indexFirstLevelBtn) && (BTN_INDEX_ACT_LAST_UPDATE == indexThreeLevelBtn))
    {
        nActionGroup = eActGroupOfficalLastUpdate;
    }

    else  if((BTN_INDEX_ACT_OFFICAL == indexFirstLevelBtn) && (BTN_INDEX_ACT_HOTTEST == indexThreeLevelBtn))
    {
        nActionGroup = eActGroupOfficalHotest;
    }

    else  if((BTN_INDEX_ACT_OFFICAL == indexFirstLevelBtn) && (BTN_INDEX_ACT_MOST_COMMENT == indexThreeLevelBtn))
    {
        nActionGroup = eActGroupOfficalMostComment;
    }

    return nActionGroup;
}

/************************************
* 名称: getTableWidget
* 参数: [in]index   index of tablewidget
* 返回:   QTableWidget
* 时间:   2016/06/28
* 作者:   asu
************************************/
QTableWidget *ActionListsPage::getTableWidget(const int index)
{
    QTableWidget* pTableWidget = NULL;    
    pTableWidget = getItemPreview(index)->GetTableWidget();

    return pTableWidget;
}

/************************************
* 名称: getItemPreview
* 参数: [in]index   index of ActionListItemPreview
* 返回:   ActionListItemPreview
* 时间:   20160919
* 作者:   asu
************************************/
ActionListItemPreview *ActionListsPage::getItemPreview(const int index) const
{
    ActionListItemPreview* pItemPreview = NULL;
    switch (index)
    {
    case 0:
        pItemPreview = m_pActListPreviewCol1;
        break;

    case 1:
        pItemPreview = m_pActListPreviewCol2;
        break;

    case 2:
        pItemPreview = m_pActListPreviewCol3;
        break;

    default:
        break;
    }

    return pItemPreview;
}
