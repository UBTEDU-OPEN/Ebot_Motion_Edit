/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actionlistitempreview.cpp
* 创建时间：2016/03/23
* 文件标识：
* 文件摘要：动作列表下载项预览控件
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/23
* 版本摘要：
*/
#include "actionlistitempreview.h"
#include "ui_actionlistitempreview.h"
#include "downloaditemdelegate.h"
#include "ubxactionlibconstants.h"
#include "actupdatemangr.h"
#include "../zxLib/zxObserverBaseEx.h"
#include "loginlibmgr.h"

#include <QFile>


ActionListItemPreview::ActionListItemPreview(QWidget *parent) :
    IActionListItemPreview(parent),
//    ui(new Ui::ActionListItemPreview),
    m_pBtnFavor(NULL),
    m_nCollectStatusRequestID(-1),
    m_pBtnDownLoad(NULL)
{

    m_pBtnFavor = getFirstBtn();
    m_pBtnDownLoad = getSecondBtn();

    m_bCurIsCollected = false;

    connect(g_actupdatemgr, &CActUpdateMangr::SigIsCollectResult,
            this, &ActionListItemPreview::OnIsCollectResult);

    OnIsCollectResult(-1,  -1,  -1);
}

ActionListItemPreview::~ActionListItemPreview()
{
//    delete ui;
}

void ActionListItemPreview::onFirstBtn()
{
    OnCollection();
}

void ActionListItemPreview::onSecondBtn()
{
    OnDownload();
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
void ActionListItemPreview::OnIsCollectResult(int requestID, int nActionID, int nRet)
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
//        m_pBtnDownLoad->setGeometry(m_ptDownLoad.x(), m_ptDownLoad.y(), 20, 14);
    }
    else
    {
        m_pBtnDownLoad->setStyleSheet("QPushButton{background: transparent;"
                                   "border-image: url(:/res/images/btn_download.png);}");
//        m_pBtnDownLoad->setGeometry(m_ptDownLoad.x(), m_ptDownLoad.y(), 18, 18);
    }

    if(-1 != m_nRowPrevHover)
    {
        m_pBtnFavor->show();
        m_pBtnDownLoad->show();
    }
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
void ActionListItemPreview::OnCellEntered(int row, int column)
{
    if (m_nRowPrevHover == row)
    {
        return;
    }

    m_pBtnFavor->hide();
    m_pBtnDownLoad->hide();
    IActionListItemPreview::OnCellEntered(row, column);

    m_nCollectStatusRequestID = g_actupdatemgr->QueryActFavour(g_actupdatemgr->GetUserID(), m_CurItemData.m_nItemID);
}

/************************************
* 名称: OnDownload
* 功能: 响应下载操作
* 返回:   void
* 时间:   2016/04/01
* 作者:   hels
************************************/
void ActionListItemPreview::OnDownload()
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
* 时间:   2016/04/14
* 作者:   hels
************************************/
void ActionListItemPreview::OnCollection()
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
