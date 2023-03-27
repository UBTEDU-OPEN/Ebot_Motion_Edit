/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：widgetresetpassword.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块重置密码主窗体头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef WIDGETRESETPASSWORD_H
#define WIDGETRESETPASSWORD_H

#include <QWidget>
#include "loginlibmgr.h"
#include "dialogloginpage.h"
#include <QPainter>

#include "../zxLib/widget/ubxwidget.h"



namespace Ui {
class WidgetResetPassword;
}

class WidgetResetPassword : public UBXWidget
{
    Q_OBJECT

public:
    explicit WidgetResetPassword(QWidget *parent = 0, loginLibMgr * pLoginLibMgr = NULL);
    ~WidgetResetPassword();

public:
    RETRANSLATEUIFUNC

private slots:

    /*************
    * 函数名:on_pBResetPassword_clicked
    * 功能:重置密码
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void on_pBResetPassword_clicked();

    /*************
    * 函数名:slotOnReturnLoginClicked
    * 功能:点击返回登录时的槽函数
    * 时间: 2016/06/22
    * 作者: hwx
    *****************/
    void slotOnReturnLoginClicked();

signals:
    /*************
    * 函数名:sigUpdateLoginWidget
    * 功能: 通知登录界面更新登录信息（用户名，密码）
    * 时间: 2016/07/06
    * 作者: hwx
    *****************/
    void sigUpdateLoginWidget(const QString &strUserName, const QString &strPassword, emAccountType eAccountType);

public:

    /**************************************************************************
    * 函数名:SetUserName
    * 功能:设置用户名
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void SetUserName(QString strUserName);

    /*************
    * 函数名:SetVerifyCode
    * 功能:设置当前手机校验码
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void SetVerifyCode(QString StrVerifyCode);

private:
    Ui::WidgetResetPassword *ui;
    loginLibMgr* m_pLoginLibMgr;
    DialogLoginPage* m_pLoginPage;

private:

    /*************
    * 函数名:OnEnKillfocusFirstPassword
    * 功能:验证第一次输入密码是否合法
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
     void OnEnKillfocusFirstPassword();

     /*************
     * 函数名:OnEnKillfocusAgainPassword
     * 功能:验证再次输入密码是否合法
     * 参数:
     * 返回值:
     * 时间: 2016/06/01
     * 作者: 周志平
     *****************/
     void OnEnKillfocusAgainPassword();

     /**************************************************************************
     * 函数名:VerifyPasswordCounter
     * 功能:校验密码
     * 参数:
     * 返回值:
     * 时间:2016/04/01
     * 作者: 周志平
     * 改动日志
     **************************************************************************/
     int VerifyPasswordCounter(QString strPassword);

     /**************************************************************************
     * 函数名:getShowUserName
     * 功能:获取用于界面显示的手机号码
     * 参数: strPhoneNumber 手机号码
     * 返回值:
     * 时间:2016/06/23
     * 作者: hwx
     * 改动日志
     **************************************************************************/
     QString getShowUserName(const QString &strPhoneNumber);

private:
    QString m_UserNames;
    QString m_strFirstPassword;
    QString m_strSecondPassword;

    QString m_VerifyCode;
    //CFont m_font;
    bool m_bPwdPass;
    bool m_bPwdPassAgain;

    enum emWndCtrlID
    {
        IDC_STATIC_RESET_PASSWORES = 1051,
        IDC_STATIC_RESET_AGAINS,
        IDC_EDIT_RESET_PASSWORDS,
        IDC_EDIT_RESET_AGAINS,
        IDC_EDIT_RESET_USERNAMES,
        IDC_BTN_RESET_PASSWORDS
    };
protected:

    /*************
    * 函数名:paintEvent
    * 功能:绘制事件
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};

#endif // WIDGETRESETPASSWORD_H
