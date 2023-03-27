/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：MyBaseWidget.cpp
* 创建时间：2016/05/20 15:19
* 文件标识：
* 文件摘要：对话框基类
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/05/20 15:19
* 版本摘要：
*/
#include "mybasewidget.h"
#include "ui_mybasewidget.h"
#include <QPainter>

MyBaseWidget::MyBaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyBaseWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setDefaultStyle();
}

MyBaseWidget::~MyBaseWidget()
{
    delete ui;
}

void MyBaseWidget::on_PushButton_close_clicked()
{
    OnClose();
}

//基类关闭函数，允许子类重写
void MyBaseWidget::OnClose()
{
    this->close();
}

void MyBaseWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/************************************
* 名称: setDefaultStyle
* 功能: 设置窗体默认样式
* 返回:   void
* 时间:   2016/03/31
* 作者:   hels
************************************/
void MyBaseWidget::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/ubxactionlib.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}
