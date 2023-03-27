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

#ifndef CCOMMENTTREEVIEW_H
#define CCOMMENTTREEVIEW_H

#include <QTreeView>
#include <QWheelEvent>

class CCommentTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit CCommentTreeView(QWidget *parent = 0);
    ~CCommentTreeView();

signals:

public slots:

protected:
    virtual void scrollContentsBy(int dx, int dy) override;
    virtual void wheelEvent(QWheelEvent * event) override;
};

#endif // CCOMMENTTREEVIEW_H
