/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：homepageitempreview.cpp
* 创建时间：2016/03/10
* 文件标识：
* 文件摘要：首页下载展示项控件，用于提供下载项的显示
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/10
* 版本摘要：
*/
#include "homepageitempreview.h"
#include "ui_homepageitempreview.h"
#include "downloaditemdelegate.h"
#include "ubxactionlibconstants.h"
#include <QHeaderView>
#include "loginlibmgr.h"


RETRANSLATEUIFUNCIMPL(HomePageItemPreview)
HomePageItemPreview::HomePageItemPreview(emActionGroup eActGroup, QWidget *parent)
    : m_emActionGroup(eActGroup),
      UBXWidget(parent),
      ui(new Ui::HomePageItemPreview),
      m_pIActionListItemPreview(NULL),
      m_pBtnFavor(NULL),
      m_pBtnDownLoad(NULL)
{
    ui->setupUi(this);

    connect(ui->pbHomePageDances, &QPushButton::clicked, this, &HomePageItemPreview::SigDancesClicked);
    connect(ui->pbHomePageStory, &QPushButton::clicked, this, &HomePageItemPreview::SigStoriesClicked);
    connect(ui->pbMore, &QPushButton::clicked, this, &HomePageItemPreview::OnMoreBtnClicked);
    connect(ui->pbNext, &QPushButton::clicked, this, &HomePageItemPreview::OnNextBtnClicked);

    m_pIActionListItemPreview = ui->iActionListItemPreview;
    m_pIActionListItemPreview->setMinimumHeight(Constants::HOME_PREVIEW_MIN_HEIGHT);
    m_pIActionListItemPreview->setMaximumHeight(Constants::HOME_PREVIEW_MAX_HEIGHT);

    this->setMaximumWidth(DownLoadItemDelegate::getItemMaxWidth());

    m_pBtnFavor = m_pIActionListItemPreview->getFirstBtn();
    m_pBtnDownLoad = m_pIActionListItemPreview->getSecondBtn();

    m_pBtnFavor->hide();
    m_pBtnFavor->setObjectName(QStringLiteral("pbActionListItemFavor"));
    m_pBtnDownLoad->setObjectName(QStringLiteral("pbActionListItemDownLoad"));
    m_pBtnDownLoad->hide();

    connect(m_pIActionListItemPreview, &IActionListItemPreview::SigActionPreviewClicked,
            this, &HomePageItemPreview::SigActionPreviewClicked);

    connect(m_pIActionListItemPreview, &IActionListItemPreview::sigCellEntered, this, &HomePageItemPreview::OnCellEntered);

    connect(m_pBtnDownLoad, &QPushButton::clicked, this, &HomePageItemPreview::OnDownload);
    connect(m_pBtnFavor, &QPushButton::clicked, this, &HomePageItemPreview::OnCollection);

    m_nTotalPages = -1;
    m_nCurPage = -1;
    m_nRequestActListID = -1;
    m_nCollectStatusRequestID = -1;
    m_emActionType = eActTypeDance;

    m_bCurIsCollected = false;

    connect(g_actupdatemgr, &CActUpdateMangr::SigItemDate, this, &HomePageItemPreview::OnItemDate);
    connect(g_actupdatemgr, &CActUpdateMangr::SigItemUpdate, this, &HomePageItemPreview::OnItemUpdate);
    connect(g_actupdatemgr, &CActUpdateMangr::SigTotalPage, this, &HomePageItemPreview::OnTotalPage);
    connect(g_actupdatemgr, &CActUpdateMangr::SigIsCollectResult, this, &HomePageItemPreview::OnIsCollectResult);

    InitActionListItems();
}

HomePageItemPreview::~HomePageItemPreview()
{
    delete ui;
}


void HomePageItemPreview::SetTitle(const QString &title)
{
    ui->labelTitle->setText(title);
}

/************************************
* 名称: OnIsCollectResult
* 功能: 响应查询是否收藏结果
* 参数: [in]requestID 请求ID
* 参数: [in]nActionID 收藏总数
* 参数: [in]nRet 请求结果
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void HomePageItemPreview::OnIsCollectResult(int requestID, int nActionID, int nRet)
{
    if (requestID != m_nCollectStatusRequestID)
    {
        return;
    }

    if(1 == nRet)
    {
        m_pBtnFavor->setStyleSheet("QPushButton{background: transparent;"
                                   "border-image: url(:/res/images/btn_collection_press.png);"
                                   "width:18px;"
                                   "height:17px;}");
        m_bCurIsCollected = true;
    }
    else
    {
        m_pBtnFavor->setStyleSheet("QPushButton{background: transparent;"
                                   "border-image: url(:/res/images/btn_collection.png);"
                                   "width:18px;"
                                   "height:17px;}");
        m_bCurIsCollected = false;
    }

    QMap<int, ActItemData> mapDownloadedItems;
    vector<ActItemData> vecDownloadingItems;
    g_actupdatemgr->GetDownloadedActList(mapDownloadedItems);
    g_actupdatemgr->GetDownloadingActList(vecDownloadingItems);
    if(mapDownloadedItems.contains(m_CurItemData.m_nItemID) || containItem(m_CurItemData, vecDownloadingItems))
    {
        m_pBtnDownLoad->setStyleSheet("QPushButton{background: transparent;"
                                   "border-image: url(:/res/images/btn_download_press.png);}");
    }
    else
    {
        m_pBtnDownLoad->setStyleSheet("QPushButton{background: transparent;"
                                   "border-image: url(:/res/images/btn_download.png);}");
    }

    if (m_pIActionListItemPreview->getPreHoverRow() != -1)
    {
        m_pBtnFavor->show();
        m_pBtnDownLoad->show();
    }
}

/************************************
* 名称: InitActionListItems
* 功能: 初始化动作列表项
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void HomePageItemPreview::InitActionListItems()
{
    m_btnGroup.addButton(ui->pbHomePageDances, eActTypeDance);
    m_btnGroup.addButton(ui->pbHomePageStory, eActTypeStory);
    m_btnGroup.addButton(ui->pbHomePageSports, eActTypeSport);
    m_btnGroup.addButton(ui->pbHomePageMusics, eActTypeMusic);
    m_btnGroup.addButton(ui->pbHomePageEducation, eActTypeEducation);
//    AddRecommendActionItems();
    connect(&m_btnGroup, static_cast<void (QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled),
            this, &HomePageItemPreview::OnButtonToggled);
    ui->pbHomePageDances->setChecked(true);
}


/************************************
* 名称: UpdateActionListItems
* 功能: 更新动作列表项
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void HomePageItemPreview::UpdateActionListItems(int nActType)
{
    if(eActTypeDance == nActType) //所有动作类型eActTypeDance
    {
        if(ui->pbHomePageDances->isChecked())
        {
            OnButtonToggled(eActTypeDance, true);
        }
        ui->pbHomePageDances->setChecked(true);
    }
    else if(eActTypeStory == nActType)//eActTypeStory
    {
        if(ui->pbHomePageStory->isChecked())
        {
            OnButtonToggled(eActTypeStory, true);
        }
        ui->pbHomePageStory->setChecked(true);
    }
    else if(eActTypeSport == nActType) //eActTypeSport
    {
        if(ui->pbHomePageSports->isChecked())
        {
            OnButtonToggled(eActTypeSport, true);
        }
        ui->pbHomePageSports->setChecked(true);
    }
    else if(eActTypeMusic == nActType)//eActTypeMusic
    {
        if(ui->pbHomePageMusics->isChecked())
        {
            OnButtonToggled(eActTypeMusic, true);
        }
        ui->pbHomePageMusics->setChecked(true);
    }
    else if(eActTypeEducation == nActType) // eActTypeEducation
    {
        if(ui->pbHomePageEducation->isChecked())
        {
            OnButtonToggled(eActTypeEducation, true);
        }
        ui->pbHomePageEducation->setChecked(true);
    }
}

/************************************
* 名称: OnCellEntered
* 功能: 响应鼠标进入item项，实现hover效果
* 参数: [in]int row 当前Item行
* 参数: [in]int column 当前Item列
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void HomePageItemPreview::OnCellEntered(int row, int column)
{
    m_pBtnFavor->hide();
    m_pBtnDownLoad->hide();

    m_CurItemData = m_pIActionListItemPreview->getCurItemData();

    m_nCollectStatusRequestID =  g_actupdatemgr->QueryActFavour(g_actupdatemgr->GetUserID(), m_CurItemData.m_nItemID);
}

/************************************
* 名称: OnButtonToggled
* 功能: 按钮组槽函数
* 参数: [in]int-id 发送信号的按钮id
* 参数: [in]bool-bChecked 按钮是否选中
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void HomePageItemPreview::OnButtonToggled(int id, bool bChecked)
{
    // id是将Button添加到ButtonGroup时给的编号
    if (id < 1 || id > 5)
    {
        return;
    }

    if(bChecked && g_actupdatemgr)
    {
        m_nCurPage = 1;
        m_pIActionListItemPreview->ClearItems();
        if(eActTypeDance == id)// eActTypeDance
        {
            m_emActionType = eActTypeDance;
        }
        else if(eActTypeStory == id)// eActTypeStory
        {
            m_emActionType = eActTypeStory;
        }
        else if (eActTypeSport == id) // eActTypeSport
        {
            m_emActionType = eActTypeSport;
        }
        else if (eActTypeMusic == id) // eActTypeMusic
        {
            m_emActionType = eActTypeMusic;
        }
        else if (eActTypeEducation == id) // eActTypeEducation
        {
            m_emActionType = eActTypeEducation;
        }

        m_nRequestActListID = g_actupdatemgr->QueryActList(m_emActionGroup, m_emActionType, m_nCurPage, Constants::ACTION_PREVIEW_PAGE_SIZE, m_nRequestActListID, m_ZoneCode.toInt());
    }
}

/************************************
* 名称: OnTotalPage
* 功能: 总页数处理槽函数
* 参数: [in]requestID 请求ID
* 参数: [in]nTotalPage 页码总数
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void HomePageItemPreview::OnTotalPage(int requestID, int nTotalPage)
{
    if(requestID == m_nRequestActListID)
    {
        m_nTotalPages = nTotalPage;
    }
}

/************************************
* 名称: OnItemDate
* 功能: 列表项数据处理槽函数
* 参数: [in]requestID 请求ID
* 参数: [in]itemData 列表项数据
* 参数: [in]nTotalPage 总页数
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void HomePageItemPreview::OnItemDate(int requestID, ActItemData itemData, int nTotalPage)
{
    if(m_nRequestActListID == requestID)
    {
        QTableWidget* pTableWidget = m_pIActionListItemPreview->GetTableWidget();
        if(pTableWidget && (pTableWidget->rowCount() < Constants::ACTION_PREVIEW_PAGE_SIZE))
        {
            if(m_pIActionListItemPreview)
            {
                m_pIActionListItemPreview->addItemData(itemData);
            }
        }
    }
}

void HomePageItemPreview::OnItemUpdate()
{
    QTableWidget* pTableWidget = m_pIActionListItemPreview->GetTableWidget();
    if(pTableWidget)
    {
        pTableWidget->update();
    }
}

/************************************
* 名称: OnDownload
* 功能: 响应下载操作
* 返回:   void
* 时间:   2016/04/12
* 作者:   hels
************************************/
void HomePageItemPreview::OnDownload()
{
    if(-1 == g_actupdatemgr->GetUserID())//未登录时不能下载
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);

        return;
    }

    g_actupdatemgr->DownloadAct(m_CurItemData);
    if(m_pBtnDownLoad)
    {
        m_pBtnDownLoad->setStyleSheet("QPushButton{background: transparent;"
                                      "border-image: url(:/res/images/btn_download_press.png);}");
//        m_pBtnDownLoad->setGeometry(m_pBtnDownLoad->geometry().x(), m_pBtnDownLoad->geometry().y(), 20, 14);
    }
}

/************************************
* 名称: OnCollection
* 功能: 响应收藏操作
* 返回:   void
* 时间:   2016/04/13
* 作者:   hels
************************************/
void HomePageItemPreview::OnCollection()
{
    if(-1 == g_actupdatemgr->GetUserID())//未登录时不能下载
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);

        return;
    }

    if(m_bCurIsCollected)
    {
        m_nCollectStatusRequestID = g_actupdatemgr->UnfavourAction(g_actupdatemgr->GetUserID(), m_CurItemData.m_nItemID);
    }
    else
    {
        m_nCollectStatusRequestID = g_actupdatemgr->FavourAction(g_actupdatemgr->GetUserID(), m_CurItemData.m_nItemID);
    }
}

/************************************
* 名称: OnNextBtnClicked
* 功能: 换一换按钮槽函数
* 返回:   void
* 时间:   2016/04/12
* 作者:   hels
************************************/
void HomePageItemPreview::OnNextBtnClicked()
{
    if(m_nCurPage < m_nTotalPages)
    {
        m_nCurPage++;
    }
    else
    {
        m_nCurPage = 1;
    }

    m_pIActionListItemPreview->ClearItems();
    m_nRequestActListID = g_actupdatemgr->QueryActList(m_emActionGroup,
                                                m_emActionType,
                                                m_nCurPage,
                                                Constants::ACTION_PREVIEW_PAGE_SIZE,
                                                m_nRequestActListID,
                                                m_ZoneCode.toInt());
}

/************************************
* 名称: OnMoreBtnClicked
* 功能: 更多按钮槽函数
* 返回:   void
* 时间:   2016/04/12
* 作者:   hels
************************************/
void HomePageItemPreview::OnMoreBtnClicked()
{
    emit SigMoreBtnClicked(m_emActionGroup, m_emActionType);
}

