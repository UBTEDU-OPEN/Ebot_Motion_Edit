/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actionyoumaylikewidget.h
* 创建时间：2016/06/17
* 文件标识：
* 文件摘要：猜你喜欢动作控件
*
* 当前版本：1.0.0.0
* 作    者：   hwx
* 完成时间：2016/06/17
* 版本摘要：
*/

#ifndef ACTIONYOUMAYLIKEWIDGET_H
#define ACTIONYOUMAYLIKEWIDGET_H

#include <QWidget>
#include <QPushButton>

#include "UBXPublic.h"
#include "actionlistitempreview.h"
#include "../zxLib/widget/ubxwidget.h"


class ActionDetailWidget;

namespace Ui {
class ActionYouMayLikeWidget;
}

class ActionYouMayLikeWidget : public UBXWidget
{
    Q_OBJECT

public:
    explicit ActionYouMayLikeWidget(QWidget *parent = 0);
    ~ActionYouMayLikeWidget();

    /************************************
    * 名称: resetUI
    * 功能: 复位界面
    * 返回:   void
    * 时间:   2016/06/19
    * 作者:   hwx
    ************************************/
    void resetUI();

    /************************************
    * 名称: requestForYouMayLikeAction
    * 功能: 请求猜你喜欢的动作
    * 参数：[in] eGroup：动作组类，如最热的
    * 参数：[in] eType：动作类型，如舞蹈
    * 返回:   void
    * 时间:   2016/06/19
    * 作者:   hwx
    ************************************/
    void requestForYouMayLikeAction(emActionGroup eGroup, emActionType eType);

    /************************************
    * 名称: setCurrentItemData
    * 功能: 设置当前动作详情对应的动作
    * 返回:   void
    * 时间:   2016/06/19
    * 作者:   hwx
    ************************************/
    void setCurrentItemData(const ActItemData &itemData);

    RETRANSLATEUIFUNC

public slots:
    /************************************
    * 名称: slotOnItemDate
    * 功能: 列表项数据处理槽函数
    * 参数: [in]requestID 请求ID
    * 参数: [in]itemData 列表项数据
    * 参数: [in]nTotalPage 总页数
    * 返回:   void
    * 时间:   2016/08/19
    * 作者:   hwx
    ************************************/
    void slotOnItemDate(int requestID, ActItemData itemData, int nTotalPage);

    /************************************
    * 名称: slotOnMoreClicked
    * 功能: 点击more按钮的槽函数
    * 返回:   void
    * 时间:   2016/06/19
    * 作者:   hwx
    ************************************/
    void slotOnMoreClicked();

signals:
    /************************************
    * 名称: sigOnMoreClicked
    * 功能: 点击更多动作
    * 返回:   void
    * 时间:   2016/06/19
    * 作者:   hwx
    ************************************/
    void sigOnMoreClicked();

    /************************************
    * 名称: sigOnActionPreviewClicked
    * 功能: 点击动作语言
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/08/22
    * 作者:   hwx
    ************************************/
    void sigOnActionPreviewClicked(ActItemData itemData);

protected:
    /************************************
    * 名称: paintEvent
    * 功能: 摘抄自Qt帮助文档：If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
    * 返回:   void
    * 时间:   2016/03/22
    * 作者:   hels
    ************************************/
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
private:
    /************************************
    * 名称: initUI
    * 功能: 初始化
    * 返回:   void
    * 时间:   2016/06/19
    * 作者:   hwx
    ************************************/
    void initUI();

private:
    int m_nRequestID;  // 请求ID
    ActItemData m_CurItemData; // 当前动作详情页对应的动作

    ActionListItemPreview *m_pActionList;  // 动作列表

private:
    Ui::ActionYouMayLikeWidget *ui;
};

#endif // ACTIONYOUMAYLIKEWIDGET_H
