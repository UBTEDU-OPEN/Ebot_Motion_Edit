/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：widgetregister.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块注册主窗体头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef WIDGETREGISTER_H
#define WIDGETREGISTER_H

#include <QWidget>
#include "loginlibmgr.h"
#include "dialogloginpage.h"
#include <QPainter>
#include <QKeyEvent>

#include "../zxLib/widget/ubxwidget.h"

namespace Ui {
class Widgetregister;
}

//注册主窗体类
class Widgetregister : public UBXWidget
{
    Q_OBJECT

public:
    explicit Widgetregister(QWidget *parent = 0, loginLibMgr * pLoginLibMgr = NULL);
    ~Widgetregister();

    /**************************************************************************
    * 函数名:timerEvent
    * 功能:时钟事件
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    virtual void timerEvent(QTimerEvent *event);

    /**************************************************************************
    * 函数名:setDefaultFocus
    * 功能:设置默认取得焦点的控件
    * 参数:
    * 返回值:
    * 时间:2016/06/18
    * 作者: hwx
    * 改动日志
    **************************************************************************/
    void setDefaultFocus();

    /**************************************************************************
    * 函数名:setCurrentRegisterTabIndex
    * 功能:设置当前注册方式
    * 参数:eAccountType 账号方式
    * 返回值:
    * 时间:2016/07/02
    * 作者: hwx
    * 改动日志
    **************************************************************************/
    void setCurrentRegisterTabIndex(emAccountType eAccountType);

    RETRANSLATEUIFUNC

protected:

    /**************************************************************************
    * 函数名:keyPressEvent
    * 功能:键盘按键事件
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    virtual void keyPressEvent(QKeyEvent * event);

signals:

    /*************
    * 函数名:
    * 功能:
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    //void SigRegisterResult(const QString &strUserName, const QString &strPassword, emAccountType eAccountType, bool bRslt);

    /*************
    * 函数名:sigUpdateLoginWidget
    * 功能: 注册完成后返回登录界面时，自动记录用户名
    * 参数: strUserName：用户名 strPassword：密码
    * 返回值:
    * 时间: 2016/07/06
    * 作者: hwx
    *****************/
    void sigUpdateLoginWidget(const QString &strUserName, const QString &strPassword, emAccountType eAccountType);

public slots:

    /*************
    * 函数名:slotSetCurrentTabIndex
    * 功能: 设置当前tab
    * 参数:
    * 返回值:
    * 时间: 2016/07/011
    * 作者: hwx
    *****************/
    void slotSetCurrentTabIndex(int nIndex);

private slots:

    /**************************************************************************
    * 函数名:OnUserResgisterResult
    * 功能:注册结果事件
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void OnUserResgisterResult(const QString &strUserName, const QString &strPassword,  emAccountType eAccountType, bool bRslt);

    /**************************************************************************
    * 函数名:onReturnLogin
    * 功能:返回登录
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onReturnLogin();

    /**************************************************************************
    * 函数名:onGetMessageCodeClicked
    * 功能:获取手机验证码
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onGetMessageCodeClicked();

    /**************************************************************************
    * 函数名:onPhoneRegisterClicked
    * 功能:手机注册
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onPhoneRegisterClicked();

    /**************************************************************************
    * 函数名:onMailRegisterClicked
    * 功能:邮箱注册
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onMailRegisterClicked();

    /**************************************************************************
    * 函数名:slotOnCurrentNationCodeChanged
    * 功能:获取当前国家码
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void slotOnCurrentNationCodeChanged(const QString &strNation);

    /**************************************************************************
    * 函数名:onTabWidgetRegisterCurrentChanged
    * 功能:注册方式切换
    * 参数:[@int ] index 0、手机注册 1、邮箱注册
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void onTabWidgetRegisterCurrentChanged(int index);

protected:

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
    * 函数名: OnEnKillfocusPassword
    * 功能: 验证密码是否有效
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void  OnEnKillfocusPassword(int nType);

    /**************************************************************************
    * 函数名:OnPasswordAgain
    * 功能:再次输入密码
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void  OnPasswordAgain(int nType);

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
    * 函数名:OnEnKillfocusVerifyMsg
    * 功能:短信校验
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void OnEnKillfocusVerifyMsg();

    /**************************************************************************
    * 函数名:InitNationCode
    * 功能:初始化国家码
    * 参数:
    * 返回值:
    * 时间:2016/04/01
    * 作者: 周志平
    * 改动日志
    **************************************************************************/
    void InitNationCode();

private:
    Ui::Widgetregister *ui;
private:
    loginLibMgr* m_pLoginLibMgr;
    DialogLoginPage* m_pLoginPage;

    QString m_strUserName;
    QString m_userPassword;
    QString m_userPasswordAgain;
    emAccountType  m_nUserType;

    bool m_bIsPassVerify;    //验证码正确
    bool m_bIsPassPassword;  //密码通过验证
    bool m_bIsPassPwdAgain;  //重复密码通过验证
    bool m_bIsPassUser;      //用户名通过验证
    //CFont m_font;
    int    m_nTimeoutID;  //超时时钟id
    int    m_nTimeout;    //超时时间
    int    m_nTimeCount;  //已统计时间

    enum emWndCtrlID
    {
        IDC_EDIT_PHONE_USERS = 1030,
        IDC_EDIT_PHONE_PASSWORDS,
        IDC_EDIT_AGAIN_PASSWORDS,
        IDC_EDIT_PHONE_VERIFYS,
        IDC_EDIT_PHONE_COUNTRYS,
        IDC_BTN_GET_VERIFYS,
        IDC_BTN_PHONE_REGISTERSS,
        IDC_COMBO_PHONE_COUNTRYS,
        IDC_STATIC_PHONE_COUNTRYS,
        IDC_STATIC_PHONE_REGISTERS,
        IDC_STATIC_PHONE_NAMES,
        IDC_STATIC_PHONE_PASSWORDS,
        IDC_STATIC_PWD_AGAIN,
        IDC_STATIC_PWD_AGAINOK,
        IDC_STATIC_RETURN,
        IDC_STATIC_RETURNLOGIN
    };
protected:

    /*************
    * 函数名:
    * 功能:
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

#endif // WIDGETREGISTER_H
