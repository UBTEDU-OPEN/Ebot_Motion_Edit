/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：WidgetUploadAddCover.cpp
* 创建时间：2016/05/18 17:15
* 文件标识：
* 文件摘要：动作上传添加封面界面
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/05/18 17:15
* 版本摘要：
*/
#include "widgetuploadaddcover.h"
#include "ui_widgetuploadaddcover.h"


RETRANSLATEUIFUNCIMPL(WidgetUploadAddCover)
WidgetUploadAddCover::WidgetUploadAddCover(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::WidgetUploadAddCover)
{
    ui->setupUi(this);
}

WidgetUploadAddCover::~WidgetUploadAddCover()
{
    delete ui;
}
