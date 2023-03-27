/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ActionLibWidget.h
* 创建时间：2016/03/03 18:20
* 文件标识：
* 文件摘要：动作库界面
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/03/03 18:20
* 版本摘要：
*/
#ifndef ACTIONLIBWIDGET_H
#define ACTIONLIBWIDGET_H

#include <QWidget>
#include <QMutex>
#include <QMutexLocker>

#include "ubxactionlib_global.h"
#include "pictureflow.h"
#include "homepageitempreview.h"
#include "actionrecommendpage.h"
#include <datamodel/businesspromotedata.h>

namespace Ui {
class ActionLibWidget;
}

class UBXACTIONLIBSHARED_EXPORT ActionLibWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActionLibWidget(QWidget *parent = 0);
    ~ActionLibWidget();

    /************************************
    * 名称: requestRecommendActionList
    * 功能: 请求推荐动作数据
    * 返回:   void
    * 时间:   2016/08/30
    * 作者:   hwx
    ************************************/
    void requestRecommendActionList();

    /************************************
    * 名称: requestBusinessPromoteList
    * 功能: 请求运营活动推广数据
    * 返回:   void
    * 时间:   2016/08/30
    * 作者:   hwx
    ************************************/
    void requestBusinessPromoteList();

    /************************************
    * 名称: setDefaultStyle
    * 功能: 设置窗体默认样式
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void setDefaultStyle();

    void timerEvent(QTimerEvent* event);

    /************************************
    * 名称: UpdatePreviewItemList
    * 功能: 更新预览动作项列表
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void UpdatePreviewItemList();

signals:
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
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    void SigActionPreviewClicked(ActItemData itemData);

    /************************************
    * 名称: sigRecommendImageClicked
    * 功能: flowcover推荐动作图被点击时触发
    * 参数: [in]pRecommendItem：推荐主题数据
    * 返回:   void
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    void sigRecommendImageClicked(RecommendActItemData *pRecommendItem);

    /************************************
    * 名称: sigBusinessPromoteImageClicked
    * 功能: flowcover运营推广活动图片点击时触发信号
    * 参数: [in]pPromoteItem：运营活动数据
    * 返回:   void
    * 时间:   2016/08/17
    * 作者:   hwx
    ************************************/
    void sigBusinessPromoteImageClicked(BusinessPromoteData *pPromoteItem);

    /************************************
    * 名称: sigOnUpdatePopUpImage
    * 功能: 活动推广图片更新
    * 参数: [in]sImageFile 图片文件
    * 返回:   void
    * 时间:   2016/08/31
    * 作者:   hwx
    ************************************/
    void sigOnUpdatePopUpImage(const QString &sImageFile);

public slots:
     void OnDownLoadClicked(const QString& itemPath);

     /************************************
     * 名称: slotOnAddRecommendAction
     * 功能: 添加推荐主题
     * 参数：pRecommendItem，推荐动作数据
     * 返回:   void
     * 时间:   2016/04/11
     * 作者:   hwx
     ************************************/
     void slotOnAddRecommendAction(RecommendActItemData *pRecommendItem);

     /************************************
     * 名称: slotOnAddBusinessPromoteData
     * 功能: 添加运营推广活动数据
     * 参数：pPromoteItem，运营推广活动数据
     * 返回:   void
     * 时间:   2016/08/17
     * 作者:   hwx
     ************************************/
     void slotOnAddBusinessPromoteData(BusinessPromoteData *pPromoteItem);

     /************************************
     * 名称: slotOnCurrentFlowPictureChanged
     * 功能: 当前推荐动作图片选中的槽函数
     * 返回:   void
     * 时间:   2016/06/17
     * 作者:   hwx
     ************************************/
     void slotOnCenterImageClicked();

     /************************************
     * 名称: slotOnFlowCoverClicked
     * 功能: 点击flow流图指定Id页
     * 参数：[in]：nFlowCoverId 当前点击的图片id
     * 返回:   void
     * 时间:   2016/09/01
     * 作者:   hwx
     ************************************/
     void slotOnFlowCoverClicked(int nFlowCoverId);

     /************************************
     * 名称: slotOnClickPopUpPage
     * 功能: 点击运营活动退送页的槽函数
     * 参数：
     * 返回:   void
     * 时间:   2016/03/01
     * 作者:   hwx
     ************************************/
     void slotOnClickPopUpPage();

     /************************************
     * 名称: slotOnChangedFlowCoverImageTimerTimeOut
     * 功能: flowcover定时器槽函数
     * 返回:   void
     * 时间:   2016/06/21
     * 作者:   hwx
     ************************************/
     void slotOnChangedFlowCoverImageTimerTimeOut();

protected:
     /************************************
     * 名称: paintEvent
     * 功能: 摘抄自Qt帮助文档：If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
     * 返回:   void
     * 时间:   2016/03/22
     * 作者:   hels
     ************************************/
     void paintEvent(QPaintEvent *event);

     /************************************
     * 名称: showEvent
     * 功能: 显示事件
     * 返回:   void
     * 时间:   2016/06/23
     * 作者:   hwx
     ************************************/
     void showEvent(QShowEvent * event);

     /************************************
     * 名称: hideEvent
     * 功能: 隐藏事件
     * 返回:   void
     * 时间:   2016/06/23
     * 作者:   hwx
     ************************************/
     void hideEvent(QHideEvent * event);

    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    /************************************
    * 名称: startChangedFlowImageTimer
    * 功能: 启动flowcover中切换图片的定时器
    * 返回:   void
    * 时间:   2016/06/21
    * 作者:   hwx
    ************************************/
    void startChangedFlowImageTimer();

    /************************************
    * 名称: stopChangedFlowImageTimer
    * 功能: 停止flowcover中切换图片的定时器
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void stopChangedFlowImageTimer();

    /************************************
    * 名称: setDefaultCoverFlowImage
    * 功能: 设置默认flow图片
    * 返回:   void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void setDefaultCoverFlowImage();

    /************************************
    * 名称: InitPreviewItemList
    * 功能: 初始化预览动作项列表
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void InitPreviewItemList();

    /************************************
    * 名称: getRecommendActionRequestLanguage
    * 功能: 获取后台请求数据语言类型，CN或EN
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hwx
    ************************************/
    QString getRecommendActionRequestLanguage();

    /************************************
    * 名称: addRecommendData
    * 功能: 添加主题数据，自动根据order排序
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hwx
    ************************************/
    void addRecommendData(RecommendActItemData *pData);

    /************************************
    * 名称: updateFlowCover
    * 功能: 根据新的顺序重新刷新banner页
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hwx
    ************************************/
    void updateFlowCover();

    // 判断是否所有banner图片都下载完成了
    bool hasBannerImagesAllDownloaded();

private:

    //根据指定路径返回文件列表
    QStringList FindPictures(const QString& Path);

    PictureFlow* m_pCoverFlow;//封面流图片控件

    QTimer  *m_pChangedFlowImageTimer;  // 定时切换flowpicture中中间图片
    QMutex  m_Mutex;   // 互斥锁

    HomePageItemPreview* m_pLatestPreview;
    HomePageItemPreview* m_pHotestPreview;
    HomePageItemPreview* m_pCommentsPreview;

    int m_nNewHotestPreviewTimeID;
    int m_nNewCommentsPreviewTimeID;
    int m_nUpdateHotestPreviewTimeID;
    int m_nUpdateCommentsPreviewTimeID;
    int m_nTime;
private:
    Ui::ActionLibWidget *ui;

private:
    bool m_bHasClearDefaultRecommendImage; // 指示是否已经清除了默认图片

    QList<RecommendActItemData*> m_lstRecommendItemData; // 推荐动作数据，List的下标为动作数据在flowpicture中的id

};

#endif // ACTIONLIBWIDGET_H
