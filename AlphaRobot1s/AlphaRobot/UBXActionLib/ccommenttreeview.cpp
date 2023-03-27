/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ccommenttreeview.h
* 创建时间：2016/10/14
* 文件标识：
* 文件摘要：评论树控件
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/09
* 版本摘要：
*/

#include <QDebug>

#include "ccommenttreeview.h"

CCommentTreeView::CCommentTreeView(QWidget *parent) : QTreeView(parent)
{

}

CCommentTreeView::~CCommentTreeView()
{

}

void CCommentTreeView::scrollContentsBy(int dx, int dy)
{
    QTreeView::scrollContentsBy(dx, dy);
}

void CCommentTreeView::wheelEvent(QWheelEvent *event)
{
    QTreeView::wheelEvent(event);
}

