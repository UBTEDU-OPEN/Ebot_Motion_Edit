/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：favoritespage.h
* 创建时间：2016/04/13
* 文件标识：
* 文件摘要：我的收藏页界面，展示我收藏的动作，并提供取消收藏和下载功能
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/04/13
* 版本摘要：
*/
#include "favoritespage.h"
#include "ui_favoritespage.h"

#include <QPainter>
#include <QCheckBox>

#include "ubxactionlibconstants.h"
#include "../zxLib/widget/ubxwidget.h"
#include "widget/messagebox.h"


RETRANSLATEUIFUNCIMPL(FavoritesPage)
FavoritesPage::FavoritesPage(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::FavoritesPage),
    m_pMultiPages(NULL)
{
    ui->setupUi(this);
    m_listActIDHadShow.clear();

    ui->scrollAreaItemPreview->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollAreaItemPreview->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_nRequestID = REQUSET_ID_DEFAULT_INVALID;
    m_nPagebtns = 10;
    //m_emActionGroup = eActFavorites;
    m_emActionType = eActTypeAll;

    m_pMultiPages = new UBXMultiPage(this);
    ui->horizontalLayoutMultiPage->addWidget(m_pMultiPages);

    connect(m_pMultiPages, &UBXMultiPage::sigPrevPageClicked, this, &FavoritesPage::OnPrevPageClicked);
    connect(m_pMultiPages, &UBXMultiPage::sigNextPageClicked, this, &FavoritesPage::OnNextPageClicked);
    connect(m_pMultiPages, &UBXMultiPage::sigCurrentPageChanged, this, &FavoritesPage::onCurrentPageChanged);


    connect(ui->pbPrevPage, &QPushButton::clicked, this, &FavoritesPage::OnPrevPageClicked);
    connect(ui->pbNextPage, &QPushButton::clicked, this, &FavoritesPage::OnNextPageClicked);

    //由于服务器暂时不支持对我的收藏进行分页和分类查询，因此自己计算分页和分类，后续支持了记得改过来
    connect(g_actupdatemgr, &CActUpdateMangr::SigTotalFavorites, this, &FavoritesPage::OnTotalFavorites);
    connect(g_actupdatemgr, &CActUpdateMangr::SigItemDate, this, &FavoritesPage::OnItemDate);
    connect(g_actupdatemgr, &CActUpdateMangr::SigItemUpdate, this, &FavoritesPage::OnItemUpdate);
    connect(g_actupdatemgr, &CActUpdateMangr::SigIsCollectResult, this, &FavoritesPage::OnIsCollectResult);

    InitActionListControl();
    InitBtns();

    UBXWidget::setDefaultStyle(this, ":/res/qss/ubxactionlib.qss");
}

FavoritesPage::~FavoritesPage()
{
    delete ui;
}

/************************************
* 名称: requestFavouiteActions
* 功能: 请求收藏动作
* 参数: [in]eActionType 动作类型
* 返回:   void
* 时间:   2016/12/22
* 作者:   hwx
************************************/
void FavoritesPage::requestFavouiteActions(emActionType eActionType)
{
    OnBtnsButtonToggled(eActionType, true);
}

/************************************
* 名称: InitActionListItem
* 功能: 初始化收藏项列表
* 返回:   void
* 时间:   2016/04/13
* 作者:   hels
************************************/
void FavoritesPage::InitActionListControl()
{
    //我的收藏页不需要显示check box，因此首列设置为0，但不要去除，因为使用的是专门设计的ActionListItemPreview控件
    //保持内部的逻辑判断等不需要修改
    m_pActListPreviewCol1 = new ActionListItemPreview(this);
    m_pActListPreviewCol1->GetTableWidget()->setColumnCount(3);
    m_pActListPreviewCol1->GetTableWidget()->setColumnWidth(0, 0);
    m_pActListPreviewCol1->GetTableWidget()->setColumnWidth(2, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    connect(m_pActListPreviewCol1, &ActionListItemPreview::SigActionPreviewClicked, this, &FavoritesPage::SigActionPreviewClicked);

    m_pActListPreviewCol2 = new ActionListItemPreview(this);
    m_pActListPreviewCol2->GetTableWidget()->setColumnCount(3);
    m_pActListPreviewCol2->GetTableWidget()->setColumnWidth(0, 0);
    m_pActListPreviewCol2->GetTableWidget()->setColumnWidth(2, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    connect(m_pActListPreviewCol2, &ActionListItemPreview::SigActionPreviewClicked, this, &FavoritesPage::SigActionPreviewClicked);

    m_pActListPreviewCol3 = new ActionListItemPreview(this);
    m_pActListPreviewCol3->GetTableWidget()->setColumnCount(3);
    m_pActListPreviewCol3->GetTableWidget()->setColumnWidth(0, 0);
    m_pActListPreviewCol3->GetTableWidget()->setColumnWidth(2, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    connect(m_pActListPreviewCol3, &ActionListItemPreview::SigActionPreviewClicked, this, &FavoritesPage::SigActionPreviewClicked);

    ui->horizontalLayoutCol1->insertWidget(1, m_pActListPreviewCol1);
    ui->horizontalLayoutCol2->insertWidget(3, m_pActListPreviewCol2);
    ui->horizontalLayoutCol3->insertWidget(5, m_pActListPreviewCol3);
}

/************************************
* 名称: OnIsCollectResult
* 功能: 响应查询是否收藏结果
* 参数: [in]requestID 请求ID
* 参数: [in]nActionID 收藏总数
* 参数: [in]nRet 请求结果
* 返回:   void
* 时间:   2016/04/15
* 作者:   hels
************************************/
void FavoritesPage::OnIsCollectResult(int requestID, int nActionID, int nRet)
{
    if(1 != nRet)
    {
        if(m_mapActItem.contains(nActionID))
        {
            m_mapActItem.remove(nActionID);
            ClearItems();
            SearchItem(m_emActionType, m_pMultiPages->getCurrentPage());
        }
    }
}

/************************************
* 名称: InitBtns
* 功能: 初始化按钮组
* 返回:   void
* 时间:   2016/04/13
* 作者:   hels
************************************/
void FavoritesPage::InitBtns()
{
    m_btnGroupForButtons.addButton(ui->pbFavAllActions, eActTypeAll);
    m_btnGroupForButtons.addButton(ui->pbFavDances, eActTypeDance);
    m_btnGroupForButtons.addButton(ui->pbFavStories, eActTypeStory);
    m_btnGroupForButtons.addButton(ui->pbFavSports, eActTypeSport);
    m_btnGroupForButtons.addButton(ui->pbFavChildSong, eActTypeMusic);
    m_btnGroupForButtons.addButton(ui->pbFavEducation, eActTypeEducation);

    connect(&m_btnGroupForButtons,
            static_cast<void(QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled),
            this,
            &FavoritesPage::OnBtnsButtonToggled);

    ui->pbFavAllActions->setChecked(true);
}

//If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
void FavoritesPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void FavoritesPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

//    int nTableWidgetWidth = this->width() * Constants::HOME_PREVIEW_WIDGET_RATIO;
//    int nTableWidgetMaxWidth = DownLoadItemDelegate::getItemMaxWidth();
//    if (nTableWidgetWidth > nTableWidgetMaxWidth)
//    {
//        nTableWidgetWidth = nTableWidgetMaxWidth;
//    }
//    m_pActListPreviewCol1->setFixedWidth(nTableWidgetWidth);
//    m_pActListPreviewCol2->setFixedWidth(nTableWidgetWidth);
//    m_pActListPreviewCol3->setFixedWidth(nTableWidgetWidth);

//    update();
}

/************************************
* 名称: ClearItems
* 功能: 清空所有数据
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void FavoritesPage::ClearItems()
{
    m_listActIDHadShow.clear();

    if(m_pActListPreviewCol1)
    {
        m_pActListPreviewCol1->ClearItems();
    }

    if(m_pActListPreviewCol2)
    {
        m_pActListPreviewCol2->ClearItems();
    }

    if(m_pActListPreviewCol3)
    {
        m_pActListPreviewCol3->ClearItems();
    }
}

/************************************
* 名称: OnBtnsButtonToggled
* 功能: 第一排按钮组槽函数
* 参数: [in]int-id 发送信号的按钮id
* 参数: [in]bool-bChecked 按钮是否选中
* 返回:   void
* 时间:   2016/04/13
* 作者:   hels
************************************/
void FavoritesPage::OnBtnsButtonToggled(int id, bool bChecked)
{
    //由于服务器对我的收藏不支持分类和分页查询（注：动作列表是支持分页和分类查询的），因此暂时处理为每次都是全部查询，自己计算分类和分页
    //以后等服务器支持了再改回与动作列表页一样的处理方式
    if(bChecked)
    {
        this->setEnabled(false);

        m_pMultiPages->setCurrentPage(1);

        ClearItems();
        m_emActionType = (emActionType)id;

        switch (id)
        {
        case eActTypeAll:
            m_nRequestID = g_actupdatemgr->QueryFavorites(m_nRequestID);
            break;

        case eActTypeDance:
        case eActTypeStory:
        case eActTypeSport:
        case eActTypeMusic:
        case eActTypeEducation:
        {
            if(m_mapActItem.isEmpty())
            {
                m_nRequestID = g_actupdatemgr->QueryFavorites(m_nRequestID);
            }
            else
            {
                SearchItem(m_emActionType, m_pMultiPages->getCurrentPage());
            }
        }
            break;

        default:
        {
            m_emActionType = eActTypeAll;
            m_nRequestID = g_actupdatemgr->QueryFavorites(m_nRequestID);
        }
            break;

        }

        this->setEnabled(true);
    }
}

/************************************
* 名称: OnTotalFavorites
* 功能: 响应我的收藏总数
* 参数: [in]requestID 请求ID
* 参数: [in]nTotals 收藏总数
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void FavoritesPage::OnTotalFavorites(int requestID, int nTotals)
{
    if(requestID == m_nRequestID)
    {
        int nTotalPage = qCeil((float)nTotals / (float)(3 * Constants::ACTION_LIST_COUNT_PER_LIST));//总个数 / (3col * 每列个数)

        int nCurrentPage = m_pMultiPages->getCurrentPage();
        QString strPage = QString::number(nCurrentPage);
        strPage += "/" + QString::number(nTotalPage);
        ui->labelPages->setText(strPage);

        m_pMultiPages->UpdatePages(nTotalPage);

        if (nTotals <= 0)
        {
            CMessageBox::information(this, tr(""), tr("You have not collected any actions."));
        }
    }
}

/************************************
* 名称: OnItemDate
* 功能: 列表项数据处理槽函数
* 参数: [in]requestID 请求ID
* 参数: [in]itemData 列表项数据
* 参数: [in]nTotalPage 总页数
* 返回:   void
* 时间:   2016/04/13
* 作者:   hels
************************************/
void FavoritesPage::OnItemDate(int requestID, ActItemData itemData, int /*nTotalPage*/)
{
    if((REQUSET_ID_DEFAULT_INVALID != m_nRequestID) && (m_nRequestID == requestID))
    {
//        QTableWidget* pTableWidget = NULL;
        ActionListItemPreview* pActionListItemPreview = NULL;
        if(m_pActListPreviewCol1->GetTableWidget()->rowCount() < Constants::ACTION_LIST_COUNT_PER_LIST)
        {
            pActionListItemPreview = m_pActListPreviewCol1;
        }
        else if(m_pActListPreviewCol2->GetTableWidget()->rowCount() < Constants::ACTION_LIST_COUNT_PER_LIST)
        {
            pActionListItemPreview = m_pActListPreviewCol2;
        }
        else if(m_pActListPreviewCol3->GetTableWidget()->rowCount() < Constants::ACTION_LIST_COUNT_PER_LIST)
        {
            pActionListItemPreview = m_pActListPreviewCol3;
        }

        if(pActionListItemPreview && !m_listActIDHadShow.contains(itemData.m_nItemID))
        {
            if((eActTypeAll == m_emActionType) || (itemData.m_nActType == m_emActionType))
            {
                m_listActIDHadShow.append(itemData.m_nItemID);

                pActionListItemPreview->addItemData(itemData);
            }
        }

        if(!m_mapActItem.contains(itemData.m_nItemID))
        {
            m_mapActItem.insert(itemData.m_nItemID, itemData);
        }
    }
}

void FavoritesPage::OnItemUpdate()
{
    QTableWidget* pTableWidget = NULL;
    if(m_pActListPreviewCol1->GetTableWidget()->rowCount() < Constants::ACTION_LIST_COUNT_PER_LIST)
    {
        pTableWidget = m_pActListPreviewCol1->GetTableWidget();
    }
    else if(m_pActListPreviewCol2->GetTableWidget()->rowCount() < Constants::ACTION_LIST_COUNT_PER_LIST)
    {
        pTableWidget = m_pActListPreviewCol2->GetTableWidget();
    }
    else if(m_pActListPreviewCol3->GetTableWidget()->rowCount() < Constants::ACTION_LIST_COUNT_PER_LIST)
    {
        pTableWidget = m_pActListPreviewCol3->GetTableWidget();
    }
    if(pTableWidget)
    {
        pTableWidget->update();
    }
}

void FavoritesPage::onCurrentPageChanged(int nCurPage)
{
    if (nCurPage < 0 || (nCurPage > m_pMultiPages->getTotalPage()))
    {
        return;
    }

    ClearItems();
    SearchItem(m_emActionType, nCurPage);
}

/************************************
* 名称: OnPrevPageClicked
* 功能: 上一页按钮槽函数
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void FavoritesPage::OnPrevPageClicked()
{    
    int nCurrentPage = m_pMultiPages->getCurrentPage();
    if(nCurrentPage > 0)
    {
        ClearItems();
        SearchItem(m_emActionType, nCurrentPage);
    }
}

/************************************
* 名称: OnNextPageClicked
* 功能: 下一页按钮槽函数
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void FavoritesPage::OnNextPageClicked()
{    
    int nCurrentPage = m_pMultiPages->getCurrentPage();
    if(nCurrentPage <= m_pMultiPages->getTotalPage())
    {
        ClearItems();
        SearchItem(m_emActionType, nCurrentPage);
    }
}

/************************************
* 名称: SearchItem
* 功能: 根据已经保存的所有收藏项，选择指定类型（由于服务器不支持收藏的分页和分类查询，因此自己实现，后续支持了记得改掉）
* 参数: [in]emActType 动作类型
* 参数: [in]nPage 第几页
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void FavoritesPage::SearchItem(emActionType emActType, int nPage)
{
    //先统计类型个数
    QVector<ActItemData> vec;
    vec.clear();

    int nItems = 0;

    //get the emActType items.
    foreach (ActItemData item, m_mapActItem.values())
    {        
        if(eActTypeAll == emActType)
        {
            vec.append(item);
            nItems++;
        }
        else if(emActType == item.m_nActType)
        {
            vec.append(item);
            nItems++;
        }
    }


    //计算此类型页数
    const int nTotalPage = qCeil((float)nItems / (float)(3 * Constants::ACTION_LIST_COUNT_PER_LIST));//总个数 / (3col * 每列个数)
    m_pMultiPages->UpdatePages(nTotalPage);


    //update the current page's item
    if(nPage > 0 && nPage <= nTotalPage)
    {
        int nFirstIndex = (nPage - 1) * (3 * Constants::ACTION_LIST_COUNT_PER_LIST);//get the current page first item index
        int nEndIndex = nFirstIndex + (3 * Constants::ACTION_LIST_COUNT_PER_LIST);////get the current page end item index
        for(int nIndex = nFirstIndex; nIndex < nItems && nIndex < nEndIndex; nIndex++)
        {
            OnItemDate(m_nRequestID, vec.at(nIndex), -1);
        }
    }

    int nCurrentPage = m_pMultiPages->getCurrentPage();

    //update the top right lable
    QString strPage = QString::number(nCurrentPage);
    strPage += "/" + QString::number(nTotalPage);
    ui->labelPages->setText(strPage);
}
