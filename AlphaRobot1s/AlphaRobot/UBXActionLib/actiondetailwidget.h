/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：actiondetailwidget.h
* 创建时间：2016/03/14
* 文件标识：
* 文件摘要：动作详情页控件
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/14
* 版本摘要：
*/

#ifndef ACTIONDETAILWIDGET_H
#define ACTIONDETAILWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QPushButton>
#include "actupdatemangr.h"
#include "actioncommentwidget.h"
#include "actionyoumaylikewidget.h"

#include "ubxvideoplayer.h"
#include "../zxLib/widget/ubxwidget.h"


namespace Ui {
class ActionDetailWidget;
}

class UBXACTIONLIBSHARED_EXPORT ActionDetailWidget : public UBXWidget
{
    Q_OBJECT

public:
    explicit ActionDetailWidget(QWidget *parent = 0);
    ~ActionDetailWidget();

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
    * 名称: requestForDescriptionImage
    * 功能: 请求动作描述图片
    * 参数：[in] itemData: 动作数据
    * 返回:   void
    * 时间:   2016/11/18
    * 作者:   hwx
    ************************************/
    void requestForDescriptionImage(ActItemData &itemData);

    /************************************
    * 名称: SetItemData
    * 功能: 设置显示Item详情
    * 参数: [in]itemData 动作项数据
    * 返回:   void
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    void SetItemData(const ActItemData& itemData);

    /************************************
    * 名称: setDefaultStyle
    * 功能: 设置窗体默认样式
    * 返回:   void
    * 时间:   2016/04/16
    * 作者:   hels
    ************************************/
    void setDefaultStyle();

    /************************************
    * 名称: SetUserLogIn
    * 功能: 设置用户登录状态
    * 参数: [in]bLogin 用户是否登录
    * 返回:   void
    * 时间:   2016/04/16
    * 作者:   hels
    ************************************/
    void SetUserLogIn(bool bLogin);

    RETRANSLATEUIFUNC

public slots:
    /************************************
    * 名称: OnIsCollectResult
    * 功能: 响应查询是否收藏结果
    * 参数: [in]requestID 请求ID
    * 参数: [in]nActionID 收藏总数
    * 参数: [in]nRet 请求结果
    * 返回:   void
    * 时间:   2016/04/16
    * 作者:   hels
    ************************************/
    void OnIsCollectResult(int requestID, int nActionID, int nRet);

    /************************************
    * 名称: slotOnYouMayLikeWidgetMoreClicked
    * 功能: 动作详情页猜你喜欢控件槽函数
    * 返回:   void
    * 时间:   2016/08/22
    * 作者:   hwx
    ************************************/
    void slotOnYouMayLikeWidgetMoreClicked();

    /************************************
    * 名称: slotOnActionPreviewClicked
    * 功能: 点击动作预览的槽函数
    * 返回:   void
    * 时间:   2016/08/22
    * 作者:   hwx
    ************************************/
    void slotOnActionPreviewClicked(ActItemData itemData);

signals:
    /************************************
    * 名称: SigGoBack
    * 功能: 详情页处理完毕，返回上一次住界面时发送信号
    * 返回:   void
    * 时间:   2016/04/16
    * 作者:   hels
    ************************************/
    void SigGoBack();

    /************************************
    * 名称: sigOnDetailedWidgetMoreClicked
    * 功能: 点击更多动作
    * 参数：[in] eGroup：动作组类，如最热的
    * 参数：[in] eType：动作类型，如舞蹈
    * 返回:   void
    * 时间:   2016/06/19
    * 作者:   hwx
    ************************************/
    void sigOnDetailedWidgetMoreClicked(emActionGroup eGroup, emActionType eType);

protected:
    /************************************
    * 名称: Init
    * 功能: 初始化
    * 返回:   void
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    void Init();

     /************************************
     * 名称: paintEvent
     * 功能: 摘抄自Qt帮助文档：If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
     * 返回:   void
     * 时间:   2016/04/16
     * 作者:   hels
     ************************************/
     void paintEvent(QPaintEvent *event);

     virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
     /************************************
     * 名称: OnBackBtnclicked
     * 功能: 返回按钮槽函数
     * 返回:   void
     * 时间:   2016/04/16
     * 作者:   hels
     ************************************/
     void OnBackBtnclicked();

     /************************************
     * 名称: OnDownload
     * 功能: 响应下载操作
     * 返回:   void
     * 时间:   2016/04/16
     * 作者:   hels
     ************************************/
     void OnDownload();

     /************************************
     * 名称: OnCollection
     * 功能: 响应收藏操作
     * 返回:   void
     * 时间:   2016/04/16
     * 作者:   hels
     ************************************/
     void OnCollection();

     /************************************
     * 名称: slotOndownloadDescImageDone
     * 功能: 完成动作详情页图片下载
    * 参数：[in] nRequestId 请求ID
     * 返回:   void
     * 时间:   2016/11/17
     * 作者:   hwx
     ************************************/
     void slotOndownloadDescImageDone(int nRequestId);

     /************************************
     * 名称: slotOnReplyClicked
    * 功能: 用户点击回复评论图标的槽函数，调整滚动条到评论区
    * 参数：
     * 返回:   void
     * 时间:   2016/11/25
     * 作者:   hwx
     ************************************/
     void slotOnReplyClicked();

private:
     /************************************
     * 名称: setVideoContent
     * 功能: 设置媒体播放器内容
     * 返回:   void
     * 时间:   2016/08/27
     * 作者:   hwx
     ************************************/
     void setVideoContent();

     /************************************
     * 名称: isVideoURLValid
     * 功能: 判断视频URL是否有效
     * 返回:   void
     * 时间:   2016/11/18
     * 作者:   hwx
     ************************************/
     bool isVideoURLValid(const QString &strUrl);

     /************************************
     * 名称: updateActionDescImage
     * 功能: 刷新动作描述图片显示
     * 返回:   void
     * 时间:   2016/11/18
     * 作者:   hwx
     ************************************/
     void updateActionDescImage();

     /************************************
     * 名称: resetUI
     * 功能: 复位界面
     * 返回:   void
     * 时间:   2016/09/26
     * 作者:   hwx
     ************************************/
     void resetUI();

     // 重新构建播放器
     void resetPlayer();

private:
    Ui::ActionDetailWidget *ui;

    int m_nDescImageRequestId; // 描述图片请求Id

    ActItemData m_CurItemData;
    bool m_bCurIsCollected;//当前项是否已经收藏

    ActionCommentWidget *m_pActionCommentWidget;
    ActionYouMayLikeWidget *m_pActionYouMayLikeWidget;

    UBXVideoPlayer *m_pVideoPlayer;
};

#endif // ACTIONDETAILWIDGET_H
