/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actionrecommendpage.h
* 创建时间：2016/06/17
* 文件标识：
* 文件摘要：推荐动作列表，用户点击主界面的flow图片时，弹出该推荐动作界面
*
* 当前版本：1.0.0.0
* 作    者：   hwx
* 完成时间：2016/06/17
* 版本摘要：
*/

#ifndef ACTIONRECOMMENDPAGE_H
#define ACTIONRECOMMENDPAGE_H

#include <QDialog>
#include <QCloseEvent>
#include <QPoint>
#include <QButtonGroup>

#include "pictureflow.h"
#include "actionlistitempreview.h"
#include "widget/ubxmultipage.h"
#include "../zxLib/widget/ubxwidget.h"

namespace Ui {
class ActionRecommendPage;
}

class UBXACTIONLIBSHARED_EXPORT ActionRecommendPage : public UBXWidget, public zxObserverBase
{
    Q_OBJECT

public:
    explicit ActionRecommendPage(QWidget *parent = 0);
    ~ActionRecommendPage();

    /************************************
    * 名称: updateData
    * 功能: 更新界面
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void updateData(RecommendActItemData *pRecommendItem);

    /************************************
    * 名称: OnNotify
    * 功能: 实现zxLib::zxObserverBase接口，接收通知消息
    * 参数: [in]zxObserverSubjectBase* pSubject 被观察者
    * 参数: [in]int nMsgID 消息ID
    * 参数: [in]DWORD dwContext 消息上下文
    * 参数: [in]WPARAM wParam 消息参数
    * 参数: [in]LPARAM lParam 消息参数
    * 返回:   void
    * 时间:   2016/03/24
    * 作者:   hels
    ************************************/
    void OnNotify(const zxObserverSubjectBase *pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

    RETRANSLATEUIFUNC

protected slots:

    /************************************
    * 名称: slotOnCurrentPageChanged
    * 功能: 当前页变化的槽函数
    * 参数：nCurPage: 当前页
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void slotOnCurrentPageChanged(int nCurPage);

    /************************************
    * 名称: slotOnPreviousPageClicked
    * 功能: 点击前一页
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void slotOnPreviousPageClicked();

    /************************************
    * 名称: slotOnNextPageClicked
    * 功能: 点击下一页
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void slotOnNextPageClicked();

    /************************************
    * 名称: slotOnSelectAll
    * 功能: 选中/取消选中所有动作
    * 参数：checked: 是否选中
    * 时间:   2016/06/20
    * 作者:   hwx
    ************************************/
    void slotOnSelectAll(bool checked);

    /************************************
    * 名称: slotOnItemCheckBoxClicked
    * 功能: 选中/取消选中列表中的动作
    * 时间:   2016/06/20
    * 作者:   hwx
    ************************************/
    void slotOnItemCheckBoxClicked();

    /************************************
    * 名称: slotOnBatchDownloadClicked
    * 功能: 点击批量下载的槽函数
    * 时间:   2016/06/20
    * 作者:   hwx
    ************************************/
    void slotOnBatchDownloadClicked();

    /************************************
    * 名称: slotOnActionDetailedClicked
    * 功能: 预览动作详细信息
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/06/18
    * 作者:   hwx
    ************************************/
    void slotOnActionDetailedClicked( ActItemData itemData);

signals:
    /************************************
    * 名称: sigGoBack
    * 功能: 返回上一次打开的界面
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void sigGoBack();

    /************************************
    * 名称: sigActionPreviewClicked
    * 功能: 预览动作详细信息
    * 参数: [in]nPageId 页标识
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/06/18
    * 作者:   hwx
    ************************************/
    void sigActionDetailedClicked(int nPageId, ActItemData itemData);

protected:

    /************************************
    * 名称: paintEvent
    * 功能: 摘抄自Qt帮助文档：If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void paintEvent(QPaintEvent *event);
     virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
private:

    /************************************
    * 名称: setUpUI
    * 功能: 构建界面
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void setUpUI();

    /************************************
    * 名称: resetData
    * 功能: 清空数据
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void resetUI();

    /************************************
    * 名称: clearActionItems
    * 功能: 清空表格中的动作数据
    * 返回:   void
    * 时间:   2016/06/20
    * 作者:   hwx
    ************************************/
    void clearActionItems();

    /************************************
    * 名称: setImage
    * 功能: 设置页面flow的图片
    * 返回:  void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void setImage(const QString &strImageFile);

    /************************************
    * 名称: setDefaultStyle
    * 功能: 设置样式
    * 返回:  void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void setDefaultStyle();

    /************************************
    * 名称: getPageActionItems
    * 功能:  获取指定页数据
    * 参数：[input] nPage：指定的页编号
    * 返回:  QList<ActItemData *>: 返回的指定页的数据
    * 时间:   2016/06/20
    * 作者:   hwx
    ************************************/
    QList<ActItemData *> getPageActionItems(int nPage);

    /************************************
    * 名称: getTotalPageCount
    * 功能: 获取总页数
    * 返回:  int：总页数
    * 时间:   2016/06/20
    * 作者:   hwx
    ************************************/
    int getTotalPageCount();

    /************************************
    * 名称: updatePage
    * 功能: 更新到指定页数据
    * 参数:  int：指定页
    * 时间:   2016/06/20
    * 作者:   hwx
    ************************************/
    void updatePage(int nPage);

    /************************************
    * 名称: downLoadItems
    * 功能: 下载指定表格数据
    * 参数:  pTable：指定的表格
    * 时间:   2016/06/20
    * 作者:   hwx
    ************************************/
    void downLoadItems(QTableWidget *pTable);

private:
    RecommendActItemData *m_pRecommendItem; // 推荐动作数据
    UBXMultiPage *m_pMultiPageWidget;  // 多页控件

    QPoint   m_dragPosition;
    PictureFlow *m_pFlowImage;  // 显示的Flow图片

    ActionListItemPreview *m_pActionListTable1;   // 推荐动作第一张列表
    ActionListItemPreview *m_pActionListTable2;   // 推荐动作第二张列表
    ActionListItemPreview *m_pActionListTable3;   // 推荐动作第三张列表

private:
    Ui::ActionRecommendPage *ui;
};

#endif // ACTIONRECOMMENDPAGE_H
