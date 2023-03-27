/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：WidgetUploadAddCover.h
* 创建时间：2016/05/18 17:15
* 文件标识：
* 文件摘要：动作上传添加封面界面
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/05/18 17:15
* 版本摘要：
*/
#ifndef WIDGETUPLOADADDCOVER_H
#define WIDGETUPLOADADDCOVER_H

#include <QWidget>
#include "../zxLib/widget/ubxwidget.h"


namespace Ui {
class WidgetUploadAddCover;
}

class UBXACTIONLIBSHARED_EXPORT WidgetUploadAddCover : public UBXWidget
{
    Q_OBJECT

public:
    explicit WidgetUploadAddCover(QWidget *parent = 0);
    ~WidgetUploadAddCover();

    RETRANSLATEUIFUNC

private:
    Ui::WidgetUploadAddCover *ui;
};

#endif // WIDGETUPLOADADDCOVER_H
