/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：favoritespage.h
* 创建时间：2016/06/08
* 文件标识：
* 文件摘要：my upload page ui，show my upload
*
* 当前版本：1.0.0.0
* 作    者：asu
*/
#include "myuploadpage.h"
//#include "ui_myuploadpage.h"

#include "ui_favoritespage.h"

#include <QPainter>
#include <QCheckBox>

#include "ubxactionlibconstants.h"
#include "../zxLib/widget/ubxwidget.h"
#include "loginlibmgr.h"


const int PAGE_SIZE_MY_UPLOAD_PAGE = 3 * Constants::ACTION_LIST_COUNT_PER_LIST;

RETRANSLATEUIFUNCIMPL(MyUploadPage)
MyUploadPage::MyUploadPage(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::FavoritesPage)
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

    connect(m_pMultiPages, &UBXMultiPage::sigPrevPageClicked, this, &MyUploadPage::OnPrevPageClicked);
    connect(m_pMultiPages, &UBXMultiPage::sigNextPageClicked, this, &MyUploadPage::OnNextPageClicked);
    connect(m_pMultiPages, &UBXMultiPage::sigCurrentPageChanged, this, &MyUploadPage::onCurrentPageChanged);

    connect(ui->pbPrevPage, &QPushButton::clicked, this, &MyUploadPage::OnPrevPageClicked);
    connect(ui->pbNextPage, &QPushButton::clicked, this, &MyUploadPage::OnNextPageClicked);

    //由于服务器暂时不支持对我的收藏进行分页和分类查询，因此自己计算分页和分类，后续支持了记得改过来
    connect(g_actupdatemgr, &CActUpdateMangr::sigRecordCount, this, &MyUploadPage::OnTotalMyUpload);
    connect(g_actupdatemgr, &CActUpdateMangr::SigItemDate, this, &MyUploadPage::OnItemDate);
    connect(g_actupdatemgr, &CActUpdateMangr::SigItemUpdate, this, &MyUploadPage::OnItemUpdate);
    connect(g_actupdatemgr, &CActUpdateMangr::SigCancelMyUpload, this, &MyUploadPage::onCancelMyUpload);

    InitActionListControl();
    InitBtns();

    UBXWidget::setDefaultStyle(this, ":/res/qss/ubxactionlib.qss");
}

MyUploadPage::~MyUploadPage()
{
    delete ui;
}

/************************************
* 名称: requestUploadActions
* 功能: 请求上传动作
* 参数: [in]eActionType 动作类型
* 返回:   void
* 时间:   2016/12/22
* 作者:   hwx
************************************/
void MyUploadPage::requestUploadActions(emActionType eActionType)
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
void MyUploadPage::InitActionListControl()
{
    //我的收藏页不需要显示check box，因此首列设置为0，但不要去除，因为使用的是专门设计的ActionListItemPreview控件
    //保持内部的逻辑判断等不需要修改
    m_pActListPreviewCol1 = new MyUploadListItemPreview(this);
    m_pActListPreviewCol1->GetTableWidget()->setColumnCount(3);
    m_pActListPreviewCol1->GetTableWidget()->setColumnWidth(0, 0);
    m_pActListPreviewCol1->GetTableWidget()->setColumnWidth(2, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    connect(m_pActListPreviewCol1, &MyUploadListItemPreview::SigActionPreviewClicked, this, &MyUploadPage::SigActionPreviewClicked);

    m_pActListPreviewCol2 = new MyUploadListItemPreview(this);
    m_pActListPreviewCol2->GetTableWidget()->setColumnCount(3);
    m_pActListPreviewCol2->GetTableWidget()->setColumnWidth(0, 0);
    m_pActListPreviewCol2->GetTableWidget()->setColumnWidth(2, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    connect(m_pActListPreviewCol2, &MyUploadListItemPreview::SigActionPreviewClicked, this, &MyUploadPage::SigActionPreviewClicked);

    m_pActListPreviewCol3 = new MyUploadListItemPreview(this);
    m_pActListPreviewCol3->GetTableWidget()->setColumnCount(3);
    m_pActListPreviewCol3->GetTableWidget()->setColumnWidth(0, 0);
    m_pActListPreviewCol3->GetTableWidget()->setColumnWidth(2, Constants::BTN_FAV_DOWN_CONTAINER_WIDTH);
    connect(m_pActListPreviewCol3, &MyUploadListItemPreview::SigActionPreviewClicked, this, &MyUploadPage::SigActionPreviewClicked);

    ui->horizontalLayoutCol1->insertWidget(1, m_pActListPreviewCol1);
    ui->horizontalLayoutCol2->insertWidget(3, m_pActListPreviewCol2);
    ui->horizontalLayoutCol3->insertWidget(5, m_pActListPreviewCol3);
}

/************************************
* 名称: onCancelMyUpload
* 功能: action to cancel result
* 参数: [in]requestID 请求ID
* 参数: [in]nActionID 收藏总数
* 参数: [in]nRet 请求结果
* 返回:   void
* 时间:   2016/06/08
* 作者:   asu
************************************/
void MyUploadPage::onCancelMyUpload(int /*requestID*/, int nActionID, int nRet)
{
    if(Constants::HTTP_RESULT_FAIL != nRet)
    {
        ClearItems();
        m_nRequestID = g_actupdatemgr->QueryCreateActions(m_nRequestID, m_emActionType, m_pMultiPages->getCurrentPage(), PAGE_SIZE_MY_UPLOAD_PAGE);
    }
}

/************************************
* 名称: InitBtns
* 功能: 初始化按钮组
* 返回:   void
* 时间:   2016/04/13
* 作者:   hels
************************************/
void MyUploadPage::InitBtns()
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
            &MyUploadPage::OnBtnsButtonToggled);

    ui->pbFavAllActions->setChecked(true);//init the ui data
}

//If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
void MyUploadPage::paintEvent(QPaintEvent *event)
{
    UBXWidget::paintEvent(this);
}

void MyUploadPage::resizeEvent(QResizeEvent *event)
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
void MyUploadPage::ClearItems()
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
void MyUploadPage::OnBtnsButtonToggled(int id, bool bChecked)
{
    //由于服务器对我的收藏不支持分类和分页查询（注：动作列表是支持分页和分类查询的），因此暂时处理为每次都是全部查询，自己计算分类和分页
    //以后等服务器支持了再改回与动作列表页一样的处理方式
    if(bChecked)
    {
        if(-1 == g_actupdatemgr->GetUserID())//未登录时
        {
            NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);

            return;
        }

        this->setEnabled(false);

        switch (id)
        {
        case eActTypeAll:
        case eActTypeDance:
        case eActTypeStory:
        case eActTypeSport:
        case eActTypeMusic:
        case eActTypeEducation:
        {
            m_emActionType = (emActionType)id;
        }
            break;

        default:
        {
            m_emActionType = eActTypeAll;
        }
            break;

        }

        ClearItems();
        m_pMultiPages->setCurrentPage(1);
        m_nRequestID = g_actupdatemgr->QueryCreateActions(m_nRequestID, m_emActionType, 1, PAGE_SIZE_MY_UPLOAD_PAGE);

        this->setEnabled(true);
    }
}

/************************************
* 名称: OnTotalMyUpload
* 功能: 响应我的收藏总数
* 参数: [in]requestID 请求ID
* 参数: [in]nRecordCount  records总数
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void MyUploadPage::OnTotalMyUpload(int requestID, int nRecordCount)
{
    if((REQUSET_ID_DEFAULT_INVALID != m_nRequestID) && (m_nRequestID == requestID))
    {
        const int nTotalPage = qCeil((float)nRecordCount / (float)(3 * Constants::ACTION_LIST_COUNT_PER_LIST));//总个数 / (3col * 每列个数)

        int nCurrentPage = m_pMultiPages->getCurrentPage();
        QString strPage = QString::number(nCurrentPage);
        strPage += "/" + QString::number(nTotalPage);
        ui->labelPages->setText(strPage);

        m_pMultiPages->UpdatePages(nTotalPage);
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
void MyUploadPage::OnItemDate(int requestID, ActItemData itemData, int /*nTotalPage*/)
{
    if((REQUSET_ID_DEFAULT_INVALID != m_nRequestID) && (m_nRequestID == requestID))
    {
//        QTableWidget* pTableWidget = NULL;
        MyUploadListItemPreview* pActListPreview = NULL;
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

        if(pActListPreview && !m_listActIDHadShow.contains(itemData.m_nItemID))
        {
            if((eActTypeAll == m_emActionType) || (itemData.m_nActType == m_emActionType))
            {
                m_listActIDHadShow.append(itemData.m_nItemID);

                pActListPreview->addItemData(itemData);
            }
        }
    }
}

void MyUploadPage::OnItemUpdate()
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

/************************************
* 名称: onCurrentPageChanged
* 功能: 当前页发生变化
* 参数: [in]nCurPage 当前页
* 返回:   void
* 时间:   20160826
* 作者:  asu
************************************/
void MyUploadPage::onCurrentPageChanged(int nCurPage)
{
    if (nCurPage < 0 || (nCurPage > m_pMultiPages->getTotalPage()))
    {
        return;
    }

    ClearItems();
    m_nRequestID = g_actupdatemgr->QueryCreateActions(m_nRequestID, m_emActionType,
                                                      nCurPage, PAGE_SIZE_MY_UPLOAD_PAGE);
}

/************************************
* 名称: OnPrevPageClicked
* 功能: 上一页按钮槽函数
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void MyUploadPage::OnPrevPageClicked()
{
    int nCurrentPage = m_pMultiPages->getCurrentPage();
    if(nCurrentPage > 0)
    {
        ClearItems();
        m_nRequestID = g_actupdatemgr->QueryCreateActions(m_nRequestID, m_emActionType,
                                                          nCurrentPage, PAGE_SIZE_MY_UPLOAD_PAGE);
    }
}

/************************************
* 名称: OnNextPageClicked
* 功能: 下一页按钮槽函数
* 返回:   void
* 时间:   2016/04/14
* 作者:   hels
************************************/
void MyUploadPage::OnNextPageClicked()
{
    int nCurrentPage = m_pMultiPages->getCurrentPage();
    if(nCurrentPage <= m_pMultiPages->getTotalPage())
    {
        ClearItems();
        m_nRequestID = g_actupdatemgr->QueryCreateActions(m_nRequestID, m_emActionType,
                                                          nCurrentPage, PAGE_SIZE_MY_UPLOAD_PAGE);
    }
}
