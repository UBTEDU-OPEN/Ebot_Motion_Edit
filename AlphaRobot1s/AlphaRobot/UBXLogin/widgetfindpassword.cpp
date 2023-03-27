/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：widgetfindpassword.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块找回密码主窗体源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#include "widgetfindpassword.h"
#include "ui_widgetfindpassword.h"
#include "subwidgetmfindpassword.h"
#include "subwidgetmhasfindpassword.h"
#include "subwidgetpfindpassword.h"
#include "subwidgetphasfindpassword.h"

SubWidgetPFindPassword* g_pSubWidgetPFindPassword = NULL;
SubWidgetPHasFindpassword* g_pSubWidgetPHasFindPassword = NULL;
SubWidgetMFindPassword * g_pSubWidgetMFindPassword = NULL;
SubWidgetMHasFindPassword* g_pSubWidgetMHasFindPassword = NULL;


RETRANSLATEUIFUNCIMPL(WidgetFindPassword)
WidgetFindPassword::WidgetFindPassword(QWidget *parent, loginLibMgr *pLoginLibMgr) :
    UBXWidget(parent),
    ui(new Ui::WidgetFindPassword)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    m_pLoginPage = (DialogLoginPage*)parent;
    m_pLoginLibMgr= pLoginLibMgr;
    m_bIsFindPassword = false;
    m_nUserType = USERNAME;

    g_pSubWidgetPFindPassword = new SubWidgetPFindPassword(this, pLoginLibMgr);
    g_pSubWidgetPHasFindPassword = new SubWidgetPHasFindpassword(this,pLoginLibMgr);
    g_pSubWidgetMFindPassword = new SubWidgetMFindPassword(this, pLoginLibMgr);
    g_pSubWidgetMHasFindPassword = new SubWidgetMHasFindPassword(this,pLoginLibMgr);

    ui->verticalLayoutPFind->addWidget(g_pSubWidgetPFindPassword);
    ui->verticalLayoutPFind->addWidget(g_pSubWidgetPHasFindPassword);
    ui->verticalLayoutMFind->addWidget(g_pSubWidgetMFindPassword);
    ui->verticalLayoutMFind->addWidget(g_pSubWidgetMHasFindPassword);

    connect(ui->pBReturnLogin, &QPushButton::clicked, this,&WidgetFindPassword::onReturnLoginClicked);
    connect(ui->pBGotoRegister, &QPushButton::clicked, this,&WidgetFindPassword::onGotoRegisterClicked);
    connect(this, &WidgetFindPassword::SigStartTime, g_pSubWidgetPHasFindPassword, &SubWidgetPHasFindpassword::onStartTime);
    showPhoneContent(FindContent);
    showMailContent(FindContent);
    ui->tabWidgetGetPassword->setCurrentIndex(0);
}

WidgetFindPassword::~WidgetFindPassword()
{
    disconnect();
    delete ui;
}

/*************
* 函数名:updateLoginWidget
* 功能: 返回登录界面时更新界面数据
* 参数:
* 返回值:
* 时间: 2016/07/07
* 作者: hwx
*****************/
void WidgetFindPassword::updateLoginWidget(const QString &strUserName, const QString &strPassword, emAccountType eAccountType)
{
    emit sigUpdateLoginWidget(strUserName, strPassword, eAccountType);
}

/*************
* 函数名:showPhoneContent
* 功能:手机找回密码页面显示状态
* 参数:nContent  目前有两种:1、重置状态 2、验证短信校验码状态
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetFindPassword::showPhoneContent(int nContent)
{
    if(nContent == FindContent)
    {
        g_pSubWidgetPFindPassword->show();
        g_pSubWidgetPHasFindPassword->hide();
    }
    else if(nContent == ResetContent)
    {
        g_pSubWidgetPFindPassword->hide();
        g_pSubWidgetPHasFindPassword->show();
        emit SigStartTime();
    }
}

/*************
* 函数名:setUserName
* 功能:找回密码成功时，设置界面中用户名（可能是手机号码或邮箱）
* 参数:LOGINWAY: 手机或邮箱方式
* 参数:strUserName：手机号码或邮箱账号
* 返回值:
* 时间: 2016/06/23
* 作者: hwx
*****************/
void WidgetFindPassword::setSubwidgetUserName(LOGINWAY eLoginWay, const QString &strUserName)
{
    switch (eLoginWay) {
    case PHONE_LOGION:
        g_pSubWidgetPHasFindPassword->setPhoneNumber(strUserName);
        break;
    case MAIL_LOGIN:
        g_pSubWidgetMHasFindPassword->setMailAccount(strUserName);
        break;
    default:
        break;
    }
}

/*************
* 函数名:showMailContent
* 功能:邮箱找回密码页面显示状态
* 参数:nContent  目前有两种:1、重置状态 2、找回成功状态
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetFindPassword::showMailContent(int nContent)
{
    if(nContent == FindContent)
    {
        g_pSubWidgetMFindPassword->show();
        g_pSubWidgetMHasFindPassword->hide();

    }
    else if( nContent == ResetContent)
    {
        g_pSubWidgetMFindPassword->hide();
        g_pSubWidgetMHasFindPassword->show();
    }
}

/*************
* 函数名:onReturnLoginClicked
* 功能:返回登录
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetFindPassword::onReturnLoginClicked()
{
    if(m_pLoginPage)
    {
        m_pLoginPage->ChangeChildWidget(LOGIN_WIDGET);
        updateLoginWidget(m_userName, "", m_nUserType);
    }
}

/*************
* 函数名:onGotoRegisterClicked
* 功能:去注册
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetFindPassword::onGotoRegisterClicked()
{
    if(m_pLoginPage)
    {
        m_pLoginPage->ChangeChildWidget(REGISTER_WIDGET);
        emit sigSetCurrentTab(m_nUserType);
    }
}

/*************
* 函数名:SetUserName
* 功能:设置用户名
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetFindPassword::SetUserName(QString strUserName)
{
    m_userName = strUserName;
}

/*************
* 函数名:GetUserName
* 功能:获取用户名
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
QString WidgetFindPassword::GetUserName()
{
    return m_userName;
}

/*************
* 函数名:changetoResetPage
* 功能:转到重置页面
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetFindPassword::changetoResetPage(QString strUserName, QString strVerifyCode)
{
    if(m_pLoginPage)
    {
        m_pLoginPage->SetResetWidgetInfo(strUserName, strVerifyCode);
        m_pLoginPage->ChangeChildWidget(RESETPASSWORD_WIDGET);
    }
}

/*************
* 函数名:SetFindPassWordType
* 功能:设置找回密码类型
* 参数:nType 1、手机找回 2、邮箱找回
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetFindPassword::SetFindPassWordType(int nType)
{
    if(nType == PHONE_LOGION)
    {
        ui->tabWidgetGetPassword->setCurrentIndex(0); //手机找回
    }
    else if(nType == MAIL_LOGIN)
    {
        ui->tabWidgetGetPassword->setCurrentIndex(1); //邮箱找回
    }
}

/*************
* 函数名:on_tabWidgetGetPassword_currentChanged
* 功能:获取找回密码当前table
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetFindPassword::on_tabWidgetGetPassword_currentChanged(int index)
{
        m_nUserType = (emAccountType) index;
}
