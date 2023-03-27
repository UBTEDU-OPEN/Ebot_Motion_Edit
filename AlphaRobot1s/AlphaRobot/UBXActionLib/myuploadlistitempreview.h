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

#ifndef MYUPLOADLISTITEMPREVIEW_H
#define MYUPLOADLISTITEMPREVIEW_H

#include "iactionlistitempreview.h"

class MyUploadListItemPreview : public IActionListItemPreview
{
public:
    explicit MyUploadListItemPreview(QWidget *parent = 0);
    ~MyUploadListItemPreview();

public:  

public slots:
    //on first button, for on delete
    virtual void onFirstBtn();

    //delete user upload action
    void onDelete();

protected slots:
    /************************************
    * 名称: OnCellEntered
    * 功能: 响应鼠标进入item项，实现hover效果
    * 参数: [in]int row 当前Item行
    * 参数: [in]int column 当前Item列
    * 返回:   void
    * 时间:   20160608
    * 作者:   asu
    ************************************/
    void OnCellEntered(int row, int column);

};

#endif // MYUPLOADLISTITEMPREVIEW_H
