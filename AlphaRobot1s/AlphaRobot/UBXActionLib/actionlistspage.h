/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actionlistspage.h
* 创建时间：2016/03/22
* 文件标识：
* 文件摘要：动作列表界面，按类别显示动作列表，并可进行动作的收藏、下载等操作
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/22
* 版本摘要：
*/

#ifndef ACTIONLISTSPAGE_H
#define ACTIONLISTSPAGE_H
#include "zxLib.h"

#include <QWidget>
#include <QTableWidget>
#include <QButtonGroup>
#include <QPushButton>


#include "actupdatemangr.h"
#include "widget/ubxmultipage.h"
#include "../zxLib/widget/ubxwidget.h"

using namespace zxLib;

class ActionListItemPreview;

namespace Ui {
class ActionListsPage;
}

class UBXACTIONLIBSHARED_EXPORT ActionListsPage : public UBXWidget, public zxObserverBase
{
    Q_OBJECT

public:
    explicit ActionListsPage(QWidget *parent = 0);
    ~ActionListsPage();

public:
    RETRANSLATEUIFUNC

    /************************************
    * 名称: InitActionListItem
    * 功能: 初始化下载项列表
    * 参数: [in]parameter-name parameter description
    * 返回:   void
    * 时间:   2016/03/23
    * 作者:   hels
    ************************************/
    void InitActionListItem();

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
    void OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

    /************************************
    * 名称: SetQueryAction
    * 功能: 设置查询类型
    * 参数: [in]actGroup 动作查询组类型
    * 参数: [in]actType 动作类型
    * 返回:   void
    * 时间:   2016/04/12
    * 作者:   hels
    ************************************/
    void SetQueryAction(emActionGroup actGroup, emActionType actType);

    /************************************
    * 名称: SetUserLogIn
    * 功能: 设置用户登录状态
    * 参数: [in]bLogin 用户是否登录
    * 返回:   void
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    void SetUserLogIn(bool bLogin);

    /************************************
    * 名称: setButtonCurrentChecked
    * 功能: 设置当前选中的三层菜单按钮
    * 参数: [in]eActionGroup 动作组
    * 参数: [in]eActionType 动作类型
    * 返回:   void
    * 时间:   2016/08/22
    * 作者:   hwx
    ************************************/
    void setButtonCurrentChecked(emActionGroup eActionGroup, emActionType eActionType);

signals:
    /************************************
    * 名称: SigActionPreviewClicked
    * 功能: 转发DownloadItemDelegate信号，点击预览图时发送，通知外部处理
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    void SigActionPreviewClicked(ActItemData itemData);

protected:
    /************************************
    * 名称: InitFirstLevelBtns
    * 功能: 初始化第一排按钮组（所有动作，官方动作按钮）
    * 返回:   void
    * 时间:   2016/03/22
    * 作者:   hels
    ************************************/
    void InitFirstLevelBtns();

    /************************************
    * 名称: InitSecondLevelBtns
    * 功能: 初始化第二排按钮组（所有、基本、舞蹈、讲故事）
    * 返回:   void
    * 时间:   2016/03/22
    * 作者:   hels
    ************************************/
    void InitSecondLevelBtns();

    /************************************
    * 名称: InitThreeLevelBtns
    * 功能: 初始化第三排按钮组（最新、最热、下载量、评论数）
    * 返回:   void
    * 时间:   2016/03/22
    * 作者:   hels
    ************************************/
    void InitThreeLevelBtns();

    /************************************
    * 名称: paintEvent
    * 功能: 摘抄自Qt帮助文档：If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
    * 返回:   void
    * 时间:   2016/03/22
    * 作者:   hels
    ************************************/
    void paintEvent(QPaintEvent *event);

     virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    /************************************
    * 名称: customEvent
    * 功能: 自定义事件处理函数
    * 参数: [in]QEvent *event事件对象指针
    * 返回:   void
    * 时间:   2016/03/24
    * 作者:   hels
    ************************************/
    void customEvent(QEvent *event);

    /************************************
    * 名称: ClearItems
    * 功能: 清空所有数据
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void ClearItems();

protected slots:
    /************************************
    * 名称: OnFirstLevelButtonToggled
    * 功能: 第一排按钮组槽函数
    * 参数: [in]int-id 发送信号的按钮id
    * 参数: [in]bool-bChecked 按钮是否选中
    * 返回:   void
    * 时间:   2016/03/22
    * 作者:   hels
    ************************************/
    void OnFirstLevelButtonToggled(int id, bool bChecked);

    /************************************
    * 名称: OnSencondLevelButtonToggled
    * 功能: 第二排按钮组槽函数
    * 参数: [in]int-id 发送信号的按钮id
    * 参数: [in]bool-bChecked 按钮是否选中
    * 返回:   void
    * 时间:   2016/03/22
    * 作者:   hels
    ************************************/
    void OnSencondLevelButtonToggled(int id, bool bChecked);

    /************************************
    * 名称: OnThreeLevelButtonToggled
    * 功能: 第三排按钮组槽函数
    * 参数: [in]int-id 发送信号的按钮id
    * 参数: [in]bool-bChecked 按钮是否选中
    * 返回:   void
    * 时间:   2016/03/22
    * 作者:   hels
    ************************************/
    void OnThreeLevelButtonToggled(int id, bool bChecked);

    /************************************
    * 名称: onCurrentPageChanged
    * 功能: 当前页发生变化
    * 参数: [in]nCurPage 当前页
    * 返回:   void
    * 时间:   2016/03/28
    * 作者:   hels
    ************************************/
    void onCurrentPageChanged(int nCurPage);

    /************************************
    * 名称: OnItemDate
    * 功能: 列表项数据处理槽函数
    * 参数: [in]requestID 请求ID
    * 参数: [in]itemData 列表项数据
    * 参数: [in]nTotalPage 总页数
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void OnItemDate(int requestID, ActItemData itemData, int nTotalPage);

    void OnItemUpdate();

    /************************************
    * 名称: OnTotalPage
    * 功能: 总页数处理槽函数
    * 参数: [in]requestID 请求ID
    * 参数: [in]nTotalPage 页码总数
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void OnTotalPage(int requestID, int nTotalPage);

    /************************************
    * 名称: OnPrevPageClicked
    * 功能: 上一页按钮槽函数
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void OnPrevPageClicked();

    /************************************
    * 名称: OnNextPageClicked
    * 功能: 下一页按钮槽函数
    * 返回:   void
    * 时间:   2016/03/25
    * 作者:   hels
    ************************************/
    void OnNextPageClicked();

    /************************************
    * 名称: OnItemCheckBoxClicked
    * 功能: 响应下载项checkbox槽函数
    * 参数: [in]checked 按钮check状态
    * 返回:   void
    * 时间:   2016/03/29
    * 作者:   hels
    ************************************/
    void OnItemCheckBoxClicked(bool checked);

    /************************************
    * 名称: OnSelAllCheckBoxClicked
    * 功能: 响应全选按钮槽函数
    * 参数: [in]checked 按钮check状态
    * 返回:   void
    * 时间:   2016/03/29
    * 作者:   hels
    ************************************/
    void OnSelAllCheckBoxClicked(bool checked);

    /************************************
    * 名称: OnBatchDownload
    * 功能: 批量下载
    * 返回:   void
    * 时间:   2016/04/08
    * 作者:   hels
    ************************************/
    void OnBatchDownload();

    /************************************
    * 名称: onUserLoginStatusChanged
    * 功能: 用户登录状态发生改变
    * 返回:   void
    * 时间:   2017/03/29
    * 作者:   hwx
    ************************************/
    void onUserLoginStatusChanged(bool bLogin);

private:
    /************************************
    * 名称: getActionGroup
    * 参数: [in]indexFirstLevelBtn
    *    : [in]indexThreeLevelBtn
    * 返回:   int
    * 时间:   2016/06/20
    * 作者:   asu
    ************************************/
    int getActionGroup(int indexFirstLevelBtn, int indexThreeLevelBtn)const;


    /************************************
    * 名称: getTableWidget
    * 参数: [in]index   index of tablewidget
    * 返回:   QTableWidget
    * 时间:   2016/06/28
    * 作者:   asu
    ************************************/
    QTableWidget *getTableWidget(const int index);

    /************************************
    * 名称: getItemPreview
    * 参数: [in]index   index of ActionListItemPreview
    * 返回:   ActionListItemPreview
    * 时间:   20160919
    * 作者:   asu
    ************************************/
    ActionListItemPreview* getItemPreview(const int index)const;


    /************************************
    * 名称: setUpUi
    * 功能: 构建界面
    * 返回:   void
    * 时间:   2016/03/28
    * 作者:   hels
    ************************************/
    void setUpUi();

    /************************************
    * 名称: setFirstLevelButtonCurrentChecked
    * 功能: 设置当前选中的一级按钮
    * 参数：[in]eActionGroup:动作组
    * 返回:   void
    * 时间:   2016/08/22
    * 作者:   hwx
    ************************************/
    void setFirstLevelButtonCurrentChecked(emActionGroup eActionGroup);

    /************************************
    * 名称: setSecondLevelButtonCurrentChecked
    * 功能: 设置当前选中的二级按钮
    * 参数：[in]eActionType:动作类型
    * 返回:   void
    * 时间:   2016/08/22
    * 作者:   hwx
    ************************************/
    void setSecondLevelButtonCurrentChecked(emActionType eActionType);

    /************************************
    * 名称: setThirdLevelButtonCurrentChecked
    * 功能: 设置当前选中的三级按钮
    * 参数：[in]eActionGroup:动作组
    * 返回:   void
    * 时间:   2016/08/22
    * 作者:   hwx
    ************************************/
    void setThirdLevelButtonCurrentChecked(emActionGroup eActionGroup);


private:
    Ui::ActionListsPage *ui;

private:
    int m_nRequestID;

    int m_nCurrentIndexFirstLevelBtn;
    int m_nCurrentIndexThreeLevelBtn;
    emActionGroup m_emActionGroup;//动作列表组类型
    emActionType m_emActionType;//动作类型

    ActionListItemPreview* m_pActListPreviewCol1;//第一列动作列表控件
    ActionListItemPreview* m_pActListPreviewCol2;
    ActionListItemPreview* m_pActListPreviewCol3;

    QButtonGroup m_btnGroupForFirstLevel;//第一排按钮组，即所有动作和官方动作
    QButtonGroup m_btnGroupForSencondLevel;//第二排按钮组，即所有、基本、舞蹈、讲故事
    QButtonGroup m_btnGroupForThreeLevel;//第三排按钮组（最新、最热、下载量、评论数）

    UBXMultiPage *m_pMultiPages;  // 多页显示控件
};

#endif // ACTIONLISTSPAGE_H
