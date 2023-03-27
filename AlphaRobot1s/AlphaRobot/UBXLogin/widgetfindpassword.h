/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：widgetfindpassword.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块找回密码主窗体头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef WIDGETFINDPASSWORD_H
#define WIDGETFINDPASSWORD_H

#include <QWidget>
#include "loginlibmgr.h"
#include "dialogloginpage.h"
#include <QPainter>

#include "../zxLib/widget/ubxwidget.h"

namespace Ui {
class WidgetFindPassword;
}

enum Content
{
    FindContent,  //查找状态
    ResetContent  //重置状态
};

class WidgetFindPassword : public UBXWidget
{
    Q_OBJECT

public:
    explicit WidgetFindPassword(QWidget *parent = 0, loginLibMgr * pLoginLibMgr = NULL);
    ~WidgetFindPassword();

    /*************
    * 函数名:getDialogLoginPage
    * 功能:获取DialogLoginPage
    * 参数:
    * 返回值:
    * 时间: 2016/07/02
    * 作者: hwx
    *****************/
    DialogLoginPage *getDialogLoginPage() { return m_pLoginPage; }

    /*************
    * 函数名:updateLoginWidget
    * 功能: 返回登录界面时更新界面数据
    * 参数:
    * 返回值:
    * 时间: 2016/07/07
    * 作者: hwx
    *****************/
    void updateLoginWidget(const QString &strUserName, const QString &strPassword, emAccountType eAccountType);
    RETRANSLATEUIFUNC

private:
    Ui::WidgetFindPassword *ui;

signals:

    /*************
    * 函数名:SigStartTime
    * 功能:开始计时信号
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void SigStartTime();

    /*************
    * 函数名:sigSetCurrentTab
    * 功能:设置当前tab页，找回密码界面返回注册界面时，注册界面要与
    *          找回密码界面对应，比如：邮箱找回密码时，返回的注册界面是
    *          邮箱注册界面
    * 参数:
    * 返回值:
    * 时间: 2016/07/11
    * 作者: hwx
    *****************/
    void sigSetCurrentTab(int nCurrentTab);

    /*************
    * 函数名:sigUpdateLoginWidget
    * 功能:返回登录界面时，根据当前账号更新登录界面
    * 参数:
    * 返回值:
    * 时间: 2016/07/06
    * 作者: hwx
    *****************/
    void sigUpdateLoginWidget(const QString &strUserName, const QString &strPassword, emAccountType eAccountType);

public slots:

    /*************
    * 函数名:onReturnLoginClicked
    * 功能:返回登录
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void onReturnLoginClicked();

    /*************
    * 函数名:onGotoRegisterClicked
    * 功能:去注册
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void onGotoRegisterClicked();

public:

    /*************
    * 函数名:showPhoneContent
    * 功能:手机找回密码页面显示状态
    * 参数:nContent  目前有两种:1、重置状态 2、验证短信校验码状态
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void showPhoneContent(int nContent);

    /*************
    * 函数名:setUserName
    * 功能:找回密码成功时，设置界面中用户名（可能是手机号码或邮箱）
    * 参数:LOGINWAY: 手机或邮箱方式
    * 参数:strUserName：手机号码或邮箱账号
    * 返回值:
    * 时间: 2016/06/23
    * 作者: hwx
    *****************/
    void setSubwidgetUserName(LOGINWAY eLoginWay, const QString &strUserName);

    /*************
    * 函数名:showMailContent
    * 功能:邮箱找回密码页面显示状态
    * 参数:nContent  目前有两种:1、重置状态 2、找回成功状态
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void showMailContent(int nContent);

    /*************
    * 函数名:SetUserName
    * 功能:设置用户名
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void SetUserName(QString strUserName);

    /*************
    * 函数名:GetUserName
    * 功能:获取用户名
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    QString GetUserName();

    /*************
    * 函数名:changetoResetPage
    * 功能:转到重置页面
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void changetoResetPage(QString strUserName, QString strVerifyCode);

    /*************
    * 函数名:SetFindPassWordType
    * 功能:设置找回密码类型
    * 参数:nType 1、手机找回 2、邮箱找回
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void SetFindPassWordType(int nType);

private:
    int m_nLoginWay;
    loginLibMgr* m_pLoginLibMgr;
    DialogLoginPage* m_pLoginPage;
    QString m_userName;
    bool m_bIsFindPassword;
    emAccountType m_nUserType;

    int m_nTimeoutID; //超时时钟ID
    int m_nTimeout;   //超时时间
    int m_nTimeCount;

protected:

    /*************
    * 函数名:paintEvent
    * 功能:重绘事件
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

private slots:
    /*************
    * 函数名:on_tabWidgetGetPassword_currentChanged
    * 功能:获取找回密码当前table
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void on_tabWidgetGetPassword_currentChanged(int index);
};

#endif // WIDGETFINDPASSWORD_H
