/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：subwidgetpfindpassword.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块手机找回密码子窗体头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef SUBWIDGETPFINDPASSWORD_H
#define SUBWIDGETPFINDPASSWORD_H

#include <QWidget>
#include "loginlibmgr.h"
#include "widgetfindpassword.h"
#include "../zxLib/widget/ubxwidget.h"

namespace Ui {
class SubWidgetPFindPassword;
}

//手机找回密码类
class SubWidgetPFindPassword : public UBXWidget
{
    Q_OBJECT

public:
    explicit SubWidgetPFindPassword(QWidget *parent = 0 , loginLibMgr * pLoginLibMgr = NULL);
    ~SubWidgetPFindPassword();

public:
    RETRANSLATEUIFUNC

private:
    Ui::SubWidgetPFindPassword *ui;

private slots:

    /*************
    * 函数名:onPhoneFindPasswordClicked
    * 功能:手机找回密码按钮响应槽
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void onPhoneFindPasswordClicked();

    /*************
    * 函数名:slotOnCurrentNationCodeChanged
    * 功能:国家码选择的槽函数
    * 参数:text 当前国家码
    * 返回值:
    * 时间: 2016/07/02
    * 作者: hwx
    *****************/
    void slotOnCurrentNationCodeChanged(const QString &text);

private:

    /**************************************************************************
    * 函数名:InitNationCode
    * 功能:初始化国家码
    * 参数:
    * 返回值:
    * 时间:2016/07/02
    * 作者: hwx
    * 改动日志
    **************************************************************************/
    void InitNationCode();

    /**************************************************************************
    * 函数名: CheckUser
    * 功能: 检查用户名是否合法
    * 时间: 2015/11/05 15:45
    * 作者: ZDJ
    */
    bool CheckUser();

private:
   loginLibMgr* m_pLoginLibMgr;
   WidgetFindPassword* m_pFindPassword;

};

#endif // SUBWIDGETPFINDPASSWORD_H
