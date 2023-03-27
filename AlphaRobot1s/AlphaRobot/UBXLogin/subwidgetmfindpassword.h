/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：subwidgetmfindpassword.h
* 创建时间：2016/04/16
* 文件标识：
* 文件摘要：登录模块邮箱找回密码子窗体头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef SUBWIDGETMFINDPASSWORD_H
#define SUBWIDGETMFINDPASSWORD_H

#include <QWidget>
#include "loginlibmgr.h"
#include "widgetfindpassword.h"
#include "../zxLib/widget/ubxwidget.h"

namespace Ui {
class SubWidgetMFindPassword;
}
//邮箱找回密码类
class SubWidgetMFindPassword : public UBXWidget
{
    Q_OBJECT

public:
    explicit SubWidgetMFindPassword(QWidget *parent = 0 , loginLibMgr * pLoginLibMgr = NULL);
    ~SubWidgetMFindPassword();

public:
    RETRANSLATEUIFUNC

private:
    Ui::SubWidgetMFindPassword *ui;
private:

    /**************************************************************************
    * 函数名: CheckUser
    * 功能: 检查用户名是否合法
    * 时间: 2015/11/05 15:45
    * 作者: zhouzp
    */
    bool CheckUser();

private slots:

    /*************
    * 函数名:onMailFindPasswordClicked
    * 功能:邮箱找回密码按钮响应槽
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void onMailFindPasswordClicked();

private:
    loginLibMgr* m_pLoginLibMgr;
    WidgetFindPassword* m_pFindPassword;
};

#endif // SUBWIDGETMFINDPASSWORD_H
