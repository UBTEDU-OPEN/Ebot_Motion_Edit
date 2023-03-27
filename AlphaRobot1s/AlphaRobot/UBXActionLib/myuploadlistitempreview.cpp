/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：MyUploadListItemPreview.h
* 创建时间：2016/06/08
* 文件标识：
* 文件摘要：upload action preview
*
* 当前版本：1.0.0.0
* 作    者：asu
*/

#include "myuploadlistitempreview.h"
#include "loginlibmgr.h"

MyUploadListItemPreview::MyUploadListItemPreview(QWidget *parent) :
    IActionListItemPreview(parent)
{
    if(m_pBtnFirst)
    {
        //for set qss
        m_pBtnFirst->setObjectName(QStringLiteral("pbMyUploadDelete"));
    }

    getItemDelegate()->setShowAppoveStatus(true);
}

MyUploadListItemPreview::~MyUploadListItemPreview()
{

}

void MyUploadListItemPreview::onFirstBtn()
{
    onDelete();
}

//delete user upload action
void MyUploadListItemPreview::onDelete()
{
    if(-1 == g_actupdatemgr->GetUserID())//未登录时不能delete
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);

        return;
    }
    g_actupdatemgr->DeleteCreateActions(m_CurItemData.m_nItemID);
}

/************************************
* 名称: OnCellEntered
* 功能: 响应鼠标进入item项，实现hover效果
* 参数: [in]int row 当前Item行
* 参数: [in]int column 当前Item列
* 返回:   void
* 时间:   20160608
* 作者:   asu
************************************/
void MyUploadListItemPreview::OnCellEntered(int row, int column)
{
    IActionListItemPreview::OnCellEntered( row, column);

    if(m_pBtnFirst)
    {
        m_pBtnFirst->show();
    }
}





