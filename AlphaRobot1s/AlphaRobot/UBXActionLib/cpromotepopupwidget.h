/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cpromotepopupwidget.h
* 创建时间：2016/03/10
* 文件标识：
* 文件摘要：推广活动自动弹出界面
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/08/31
* 版本摘要：
*/


#ifndef CPROMOTEPOPUPWIDGET_H
#define CPROMOTEPOPUPWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QPaintEvent>

#include "widget/ubxwidget.h"

namespace Ui {
class CPromotePopUpWidget;
}

class UBXACTIONLIBSHARED_EXPORT CPromotePopUpWidget : public UBXWidget
{
    Q_OBJECT

public:
    explicit CPromotePopUpWidget(QWidget *parent = 0);
    ~CPromotePopUpWidget();

    /************************************
    * 名称: updateImage
    * 功能: 更新图片文件的槽函数
    * 参数: [in]strImageFIle 图片文件
    * 返回:   void
    * 时间:   2016/08/31
    * 作者:   hwx
    ************************************/
    void updateImage(const QString &strImageFIle);

    RETRANSLATEUIFUNC

signals:
    /************************************
    * 名称: sigOnImageClicked
    * 功能: 点击推广图片时的信号
    * 返回:   void
    * 时间:   2016/08/31
    * 作者:   hwx
    ************************************/
    void sigOnImageClicked();

    /************************************
    * 名称: sigSwitchTab
    * 功能: 切换到标题栏指定页的信号
    * 参数: [out]nTabId: Tab Id
    * 返回:   void
    * 时间:   2016/09/01
    * 作者:   hwx
    ************************************/
    void sigSwitchTab(int nTabId);

private slots:

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent * event) override;

    // 处理图片点击事件
    virtual bool eventFilter(QObject * watched, QEvent * event) override;

private:
    // 初始化
    void init();

    /************************************
    * 名称: showCloseButton
    * 功能: 显示关闭按钮
    * 返回:   void
    * 时间:   2016/09/06
    * 作者:   hwx
    ************************************/
    void showCloseButton();

private:
    // windows/mac关闭按钮
    QPushButton *m_pPushButtonCloseMac;
    QPushButton *m_pPushButtonCloseWin;

    QPixmap m_pixMapImage;

private:
    Ui::CPromotePopUpWidget *ui;
};

#endif // CPROMOTEPOPUPWIDGET_H
