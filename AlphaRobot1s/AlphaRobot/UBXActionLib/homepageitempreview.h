/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：homepageitempreview.h
* 创建时间：2016/03/10
* 文件标识：
* 文件摘要：首页下载展示项控件，用于提供下载项的显示
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/10
* 版本摘要：
*/


#ifndef HOMEPAGEITEMPREVIEW_H
#define HOMEPAGEITEMPREVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QButtonGroup>
#include "actupdatemangr.h"
#include "UBXPublic.h"
#include "iactionlistitempreview.h"
#include "../zxLib/widget/ubxwidget.h"


namespace Ui {
class HomePageItemPreview;
}

class HomePageItemPreview : public UBXWidget
{
    Q_OBJECT

public:
    explicit HomePageItemPreview(emActionGroup eActGroup, QWidget *parent = 0);
    ~HomePageItemPreview();

public:
    //修改标题文字
    void SetTitle(const QString& title);

    void UpdateActionListItems(int nActType);

    // 获取当前选中的动作类型
    emActionType getCurrentActionType() { return m_emActionType; }

    RETRANSLATEUIFUNC

signals:
    //以下为控件标题栏按钮信号，供外部监听处理
    //基本动作按钮点击信号
    void SigBasicMotionsClicked();
    //舞蹈动作按钮点击信号
    void SigDancesClicked();
    //故事按钮点击信号
    void SigStoriesClicked();
    //换一换按钮点击信号
    void SigNextClicked();
    //下载按钮点击信号
    void SigDownLoadClicked(const QString& itemPath);

    /************************************
    * 名称: SigMoreBtnClicked
    * 功能: 更多按钮信号
    * 参数: [in]actGroup 动作查询组类型
    * 参数: [in]actType 动作类型
    * 返回:   void
    * 时间:   2016/04/12
    * 作者:   hels
    ************************************/
    void SigMoreBtnClicked(emActionGroup actGroup, emActionType actType);


    /************************************
    * 名称: SigActionPreviewClicked
    * 功能: 转发DownloadItemDelegate信号，点击预览图时发送，通知外部处理
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   20160608
    * 作者:   asu
    ************************************/
    void SigActionPreviewClicked(ActItemData itemData);

public slots:
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
    /************************************
    * 名称: InitActionListItems
    * 功能: 初始化动作列表项
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void InitActionListItems();


protected slots:
    /************************************
    * 名称: OnCellEntered
    * 功能: 响应鼠标进入item项，实现hover效果
    * 参数: [in]int row 当前Item行
    * 参数: [in]int column 当前Item列
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void OnCellEntered(int row, int column);

    /************************************
    * 名称: OnButtonToggled
    * 功能: 按钮组槽函数
    * 参数: [in]int-id 发送信号的按钮id
    * 参数: [in]bool-bChecked 按钮是否选中
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void OnButtonToggled(int id, bool bChecked);

    /************************************
    * 名称: OnTotalPage
    * 功能: 总页数处理槽函数
    * 参数: [in]requestID 请求ID
    * 参数: [in]nTotalPage 页码总数
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void OnTotalPage(int requestID, int nTotalPage);

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
    void OnItemDate(int requestID, ActItemData itemData, int nTotalPage);

    void OnItemUpdate();

    /************************************
    * 名称: OnDownload
    * 功能: 响应下载操作
    * 返回:   void
    * 时间:   2016/04/12
    * 作者:   hels
    ************************************/
    void OnDownload();

    /************************************
    * 名称: OnCollection
    * 功能: 响应收藏操作
    * 返回:   void
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    void OnCollection();

    /************************************
    * 名称: OnNextBtnClicked
    * 功能: 换一换按钮槽函数
    * 返回:   void
    * 时间:   2016/04/12
    * 作者:   hels
    ************************************/
    void OnNextBtnClicked();

    /************************************
    * 名称: OnMoreBtnClicked
    * 功能: 更多按钮槽函数
    * 返回:   void
    * 时间:   2016/04/12
    * 作者:   hels
    ************************************/
    void OnMoreBtnClicked();


private:

private:
    IActionListItemPreview *m_pIActionListItemPreview;

    QPushButton* m_pBtnFavor;//收藏按钮
    QPushButton* m_pBtnDownLoad;//下载按钮

    int m_nCurPage;//记录当前页
    int m_nTotalPages;
    int m_nRequestActListID;
    int m_nCollectStatusRequestID; // 动作收藏状态的请求ID

    bool m_bCurIsCollected;//当前项是否已经收藏

    emActionGroup m_emActionGroup;//动作列表组类型
    emActionType m_emActionType;//动作类型

    QButtonGroup m_btnGroup;//按钮组

    ActItemData m_CurItemData;

private:
    Ui::HomePageItemPreview *ui;
};

#endif // HOMEPAGEITEMPREVIEW_H
