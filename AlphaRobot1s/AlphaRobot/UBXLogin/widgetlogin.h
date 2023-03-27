/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：widgetlogin.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块登录主窗体头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef WIDGETLOGIN_H
#define WIDGETLOGIN_H

#include "loginlibmgr.h"
#include "dialogloginpage.h"
#include <QDialog>
#include <QPainter>
#include <QKeyEvent>

#include "simplecrypt.h"
#include "../zxLib/widget/ubxwidget.h"


namespace Ui {
class WidgetLogin;
}

class WidgetLogin : public UBXWidget
{
    Q_OBJECT

public:
    explicit WidgetLogin(QDialog *parent = 0 , loginLibMgr * pLoginLibMgr = NULL);
    ~WidgetLogin();

    /*************
    * 函数名:eventFilter
    * 功能:事件过滤器
    * 参数:
    * 返回值:
    * 时间: 2016/06/24
    * 作者: hwx
    *****************/
    virtual bool eventFilter(QObject * watched, QEvent * event);

    /*************
    * 函数名:setDefaultFocus
    * 功能:设置默认获得焦点的控件
    * 参数:
    * 返回值:
    * 时间: 2016/07/13
    * 作者: hwx
    *****************/
    void setDefaultFocus();

    RETRANSLATEUIFUNC

protected:
    /*************
    * 函数名:keyPressEvent
    * 功能:键盘按键事件
    * 参数:
    * 返回值:
    * 时间: 2016/04/01
    * 作者: 周志平
    *****************/
    virtual void keyPressEvent(QKeyEvent * event);

private:
    Ui::WidgetLogin *ui;

private:

    /*************
    * 函数名:setUpWidget
    * 功能:初始化窗体
    * 参数:
    * 返回值:
    * 时间: 2016/04/01
    * 作者: 周志平
    *****************/
    void setUpWidget();

    /*************
    * 函数名:initWidget
    * 功能:初始化窗体
    * 参数:
    * 返回值:
    * 时间: 2016/06/27
    * 作者: hwx
    *****************/
    void initWidget();

    /*************
    * 函数名:initPhoneLoginWidget
    * 功能:初始化手机登录窗体
    * 参数:
    * 返回值:
    * 时间: 2016/06/27
    * 作者: hwx
    *****************/
    void initPhoneLoginWidget();

    /*************
    * 函数名:initMailLoginWidget
    * 功能:初始化邮箱登录窗体
    * 参数:
    * 返回值:
    * 时间: 2016/06/27
    * 作者: hwx
    *****************/
    void initMailLoginWidget();

    /**************************************************************************
    * 函数名:UpdateCfgContent
    * 功能:更新配置文件
    * 参数:int nType 用户类型
    * 返回值:无
    * 时间:2016/03/15 15:40
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void UpdateCfgContent(int nType);

    /**************************************************************************
    * 函数名: OnEnKillfocusPassword
    * 功能: 验证密码是否有效
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void OnEnKillfocusPassword(int nType);

    /**************************************************************************
    * 函数名: OnEnKillfocusUser
    * 功能:验证用户名
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void OnEnKillfocusUser(int nType);

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
    * 函数名:setZoneNum
    * 功能:设置国家码
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void setZoneNum();

public slots:

    /*************
    * 函数名:on_UpdateMessage
    * 功能:更新信息
    * 参数:strName 用户名
    * 参数:strPassword 密码
    * 返回值:
    * 时间: 2016/04/01
    * 作者: 周志平
    *****************/
    void on_UpdateMessage(QString strName, QString strPassword);

    /*************
    * 函数名:slotUpdateLoginInfo
    * 功能:设置用户信息
    * 参数:strUserName：用户名，可能是邮箱或手机号码
    * 参数：strPassword：用户登录密码
    * 返回值:
    * 时间: 2016/06/27
    * 作者: hwx
    *****************/
    void slotUpdateLoginInfo(const QString &strUserName, const QString &strPassword, emAccountType eAccountType);

private slots:

    /*************
    * 函数名:onPhoneLoginClicked
    * 功能:手机登录
    * 参数:
    * 返回值:
    * 时间: 2016/04/01
    * 作者: 周志平
    *****************/
    void onPhoneLoginClicked();

    /**************************************************************************
    * 函数名:onMaiLoginClicked
    * 功能:邮箱登录
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onMaiLoginClicked();

    /**************************************************************************
    * 函数名:onLoginCurrentChanged
    * 功能:用户登录方式切换
    * 参数:index  0、手机登录 1、邮箱登录
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onLoginCurrentChanged(int index);

    /**************************************************************************
    * 函数名:onBPhoneResgisterClicked
    * 功能:手机用户注册
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onBPhoneResgisterClicked();

    /**************************************************************************
    * 函数名:onMailRegisterClicked
    * 功能:邮箱用户注册
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onMailRegisterClicked();

    /**************************************************************************
    * 函数名:onLostMailPassWordClicked
    * 功能:邮箱用户找回密码
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onLostMailPassWordClicked();

    //    void on_checkBox_2_stateChanged(int arg1);

    /**************************************************************************
    * 函数名:onLostPhonePasswordClicked
    * 功能:手机用户找回密码
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onLostPhonePasswordClicked();

    /**************************************************************************
    * 函数名:slotOnCurrentNationCodeChanged
    * 功能:国家码变化的槽函数
    * 参数:
    * 返回值:
    * 时间:2016/07/04
    * 作者: hwx
    * 改动日志
    **************************************************************************/
    void slotOnCurrentNationCodeChanged(const QString &strNation);

private:
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    loginLibMgr* m_pLoginLibMgr;
    DialogLoginPage* m_pLoginPage;
    SimpleCrypt *m_pSimpleCrypt;
    QString m_sUserName;
    QString m_sPassword;
    int m_nUserType;
    bool m_bIsSave;
    bool m_bIsPassUser;
    bool m_bIsPassPassword;

protected:
    /*************
    * 函数名:
    * 功能:
    * 参数:
    * 返回值:
    * 时间: 2016/01/01
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

#endif // WIDGETLOGIN_H
