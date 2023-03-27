/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：MyBaseWidget.h
* 创建时间：2016/05/20 15:19
* 文件标识：
* 文件摘要：对话框基类
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/05/20 15:19
* 版本摘要：
*/
#ifndef MYBASEWIDGET_H
#define MYBASEWIDGET_H

#include <QWidget>

namespace Ui {
class MyBaseWidget;
}

class MyBaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyBaseWidget(QWidget *parent = 0);
    ~MyBaseWidget();

protected:
    //基类关闭函数
    virtual void OnClose();
    virtual void paintEvent(QPaintEvent *);

    /************************************
    * 名称: setDefaultStyle
    * 功能: 设置窗体默认样式
    * 返回:   void
    * 时间:   2016/03/31
    * 作者:   hels
    ************************************/
    void setDefaultStyle();

private slots:
    void on_PushButton_close_clicked();

private:
    Ui::MyBaseWidget *ui;
};

#endif // MYBASEWIDGET_H
