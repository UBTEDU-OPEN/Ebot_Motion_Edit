/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actionlistitempreview.h
* 创建时间：2016/03/23
* 文件标识：
* 文件摘要：动作列表下载项预览控件
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/23
* 版本摘要：
*/

#ifndef ACTIONLISTITEMPREVIEW_H
#define ACTIONLISTITEMPREVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include "actupdatemangr.h"
#include "iactionlistitempreview.h"


namespace Ui {
class ActionListItemPreview;
}

class ActionListItemPreview : public IActionListItemPreview
{
    Q_OBJECT

public:
    explicit ActionListItemPreview(QWidget *parent = 0);
    ~ActionListItemPreview();

    
public slots:
    //on first button
    virtual void onFirstBtn();

    //click on second btn
    virtual void onSecondBtn();

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
    void OnIsCollectResult(int requestID, int nActionID, int nRet);

protected:

protected slots:
    /************************************
    * 名称: OnCellEntered
    * 功能: 响应鼠标进入item项，实现hover效果
    * 参数: [in]int row 当前Item行
    * 参数: [in]int column 当前Item列
    * 返回:   void
    * 时间:   2016/03/23
    * 作者:   hels
    ************************************/
    void OnCellEntered(int row, int column);

    /************************************
    * 名称: OnDownload
    * 功能: 响应下载操作
    * 返回:   void
    * 时间:   2016/04/01
    * 作者:   hels
    ************************************/
    void OnDownload();

    /************************************
    * 名称: OnCollection
    * 功能: 响应收藏操作
    * 返回:   void
    * 时间:   2016/04/14
    * 作者:   hels
    ************************************/
    void OnCollection();

private:
    QPushButton* m_pBtnFavor;
    QPushButton* m_pBtnDownLoad;

    int   m_nCollectStatusRequestID;  // 动作收藏状态请求ID
    bool m_bCurIsCollected;//当前项是否已经收藏

//private:
//    Ui::ActionListItemPreview *ui;
};

#endif // ACTIONLISTITEMPREVIEW_H
