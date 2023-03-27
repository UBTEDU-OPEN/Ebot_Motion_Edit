/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：subwidgetmhasfindpassword.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块邮箱找回密码子窗体头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef SUBWIDGETMHASFINDPASSWORD_H
#define SUBWIDGETMHASFINDPASSWORD_H

#include <QWidget>
#include "loginlibmgr.h"
#include "widgetfindpassword.h"
#include "../zxLib/widget/ubxwidget.h"


namespace Ui {
class SubWidgetMHasFindPassword;
}

class SubWidgetMHasFindPassword : public UBXWidget
{
    Q_OBJECT

public:
    explicit SubWidgetMHasFindPassword(QWidget *parent = 0 , loginLibMgr * pLoginLibMgr = NULL);
    ~SubWidgetMHasFindPassword();

    /*************
    * 函数名:setMailAccount
    * 功能:设置邮箱账号
    * 参数:strMailAccount 邮箱账号
    * 返回值:
    * 时间: 2016/06/23
    * 作者: hwx
    *****************/
    void setMailAccount(const QString &strMailAccount);

    RETRANSLATEUIFUNC

private slots:

    /*************
    * 函数名:onFindMailPasswordOkClicked
    * 功能:通过邮箱找回密码按钮响应槽
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void onFindMailPasswordOkClicked();

private:
    Ui::SubWidgetMHasFindPassword *ui;

private:
    loginLibMgr* m_pLoginLibMgr;
    WidgetFindPassword* m_pFindPassword;

    QString m_strMailAccount;
};

#endif // SUBWIDGETMHASFINDPASSWORD_H
