/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：businesspromotewidget.h
* 创建时间：2016/06/17
* 文件标识：
* 文件摘要：运营推广活动控件
*
* 当前版本：1.0.0.0
* 作    者：   hwx
* 完成时间：2016/06/17
* 版本摘要：
*/

#ifndef BUSINESSPROMOTEWIDGET_H
#define BUSINESSPROMOTEWIDGET_H

#include <QWidget>

#include "datamodel/businesspromotedata.h"
#include "pictureflow.h"
#include "userrankitemdeletegate.h"
#include "../zxLib/widget/ubxwidget.h"


namespace Ui {
class BusinessPromoteWidget;
}

class UBXACTIONLIBSHARED_EXPORT BusinessPromoteWidget : public UBXWidget
{
    Q_OBJECT

public:
    explicit BusinessPromoteWidget(QWidget *parent = 0);
    ~BusinessPromoteWidget();

    /************************************
    * 名称: requestRankActionList
    * 功能: 请求排行榜数据
    * 参数：
    * 返回:   void
    * 时间:   2016/10/30
    * 作者:   hwx
    ************************************/
    int requestRankActionList();

    RETRANSLATEUIFUNC

public slots:
    /************************************
    * 名称: slotUpdateData
    * 功能: 更新数据
    * 参数：[in] pData:运营活动数据
    * 返回:   void
    * 时间:   2016/08/08
    * 作者:   hwx
    ************************************/
    void slotUpdateData(BusinessPromoteData *pData);

signals:
    /************************************
    * 名称: sigOnGoBackClicked
    * 功能: 点击返回的槽函数
    * 返回:   void
    * 时间:   2016/09/01
    * 作者:   hwx
    ************************************/
    void sigOnGoBackClicked();

    /************************************
    * 名称: sigOnActionUpload
    * 功能: 上传动作信号
    * 参数: [out] nSchemId 活动id
    * 返回:   void
    * 时间:   2016/09/09
    * 作者:   hwx
    ************************************/
    void sigOnActionUpload(int nSchemId);

    /************************************
    * 名称: sigDetailedActionPreviewClicked
    * 功能: 预览动作详细信息
    * 参数: [in]nPageId 页标识
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/06/18
    * 作者:   hwx
    ************************************/
    void sigDetailedActionPreviewClicked(int nPageId, ActItemData item);

private slots:
    /************************************
    * 名称: sigDetailedActionPreviewClicked
    * 功能: 预览动作详细信息
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/09/01
    * 作者:   hwx
    ************************************/
    void slotOnDetailedActionPreviewClicked(ActItemData item);

    /************************************
    * 名称: slotOnJoinInClicked
    * 功能: 点击参加运营活动的槽函数
    * 返回:   void
    * 时间:   2016/09/02
    * 作者:   hwx
    ************************************/
    void slotOnJoinInClicked();

protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    virtual void paintEvent(QPaintEvent *) override;
    virtual void resizeEvent(QResizeEvent * event) override;

private:
    /************************************
    * 名称: setDefaultStyle
    * 功能: 设置样式
    * 返回:  void
    * 时间:   2016/06/17
    * 作者:   hwx
    ************************************/
    void setDefaultStyle();

    /************************************
    * 名称: setupWidget
    * 功能: 构建界面
    * 返回:   void
    * 时间:   2016/08/08
    * 作者:   hwx
    ************************************/
    void setupWidget();

    /************************************
    * 名称: initWidget
    * 功能: 初始化界面
    * 返回:   void
    * 时间:   2016/08/08
    * 作者:   hwx
    ************************************/
    void initWidget();

    /************************************
    * 名称: addItem
    * 功能: 单个用户工作数据
    * 参数：[in] itemData:个人用户数据
    * 返回:   void
    * 时间:   2016/08/08
    * 作者:   hwx
    ************************************/
    void addItem(const UserRankItem &itemData);

    /************************************
    * 名称: setCoverImage
    * 功能: 设置运营活动图片
    * 参数：[in] strFile: 图片文件
    * 返回:   void
    * 时间:   2016/08/18
    * 作者:   hwx
    ************************************/
    void setCoverImage(const QString &strFile);

    /************************************
    * 名称: resetUI
    * 功能: 复位界面
    * 返回:   void
    * 时间:   2016/08/18
    * 作者:   hwx
    ************************************/
    void resetUI();

private:
//    PictureFlow *m_pPictureFlow; // 流图
    UserRankItemDelegate *m_pTableDelegate;
    BusinessPromoteData *m_pBusinessPromoteData;

private:
    Ui::BusinessPromoteWidget *ui;
};

#endif // BUSINESSPROMOTEWIDGET_H
