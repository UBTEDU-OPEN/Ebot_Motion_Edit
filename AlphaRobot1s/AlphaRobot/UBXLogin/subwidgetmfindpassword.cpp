/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：subwidgetmfindpassword.h
* 创建时间：2016/04/16
* 文件标识：
* 文件摘要：登录模块邮箱找回密码子窗体源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#include "subwidgetmfindpassword.h"
#include "ui_subwidgetmfindpassword.h"

RETRANSLATEUIFUNCIMPL(SubWidgetMFindPassword)
SubWidgetMFindPassword::SubWidgetMFindPassword(QWidget *parent, loginLibMgr *pLoginLibMgr) :
    UBXWidget(parent),
    ui(new Ui::SubWidgetMFindPassword)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    m_pLoginLibMgr = pLoginLibMgr;
    m_pFindPassword = (WidgetFindPassword*)parent;
    ui->lEMailName->setPlaceholderText(tr("Please enter register mail account"));
    connect(ui->pBMailFindPassword, &QPushButton::clicked, this, &SubWidgetMFindPassword::onMailFindPasswordClicked);
}

SubWidgetMFindPassword::~SubWidgetMFindPassword()
{
    delete ui;
}

/*************
* 函数名:onMailFindPasswordClicked
* 功能:邮箱找回密码按钮响应槽
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void SubWidgetMFindPassword::onMailFindPasswordClicked()
{
    if(!m_pFindPassword)
    {
        return;
    }
    setEnabled(false);
    m_pFindPassword->setEnabled(false);
    g_crisec.Lock();
    do
    {
        if(!CheckUser())
            break;
        QString strUserName = ui->lEMailName->text();
        if(m_pFindPassword)
        {
            m_pFindPassword->SetUserName(strUserName);
        }
        //邮箱方式找回密码
        QString sFindPassWordURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/fopassword";
        bool bResult = false;       
        bResult = m_pLoginLibMgr->FindPassWord(sFindPassWordURL, strUserName, EMAILNAME);     
        if (bResult)
        {
            if(m_pFindPassword)
            {
                m_pFindPassword->setSubwidgetUserName(MAIL_LOGIN, strUserName);
                m_pFindPassword->showMailContent(ResetContent);
            }
        }
        else
        {
            ui->labelMailUserInfo->setText(tr("Email password failure"));
        }

    } while (0);
    g_crisec.Unlock();
    setEnabled(true);
    m_pFindPassword->setEnabled(true);
}

/**************************************************************************
* 函数名: CheckUser
* 功能: 检查用户名是否合法
* 时间: 2015/11/05 15:45
* 作者: zhouzp
*/
bool SubWidgetMFindPassword::CheckUser()
{
    bool bUserPass = false;
    if(!m_pFindPassword)
    {
        return false;
    }
    QString sQueryMailNameURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/check";
    do
    {
        QString sUserName = ui->lEMailName->text();
        if (sUserName.isEmpty())
        {
            ui->labelMailUserInfo->setText(tr("Email can not be empty"));
            break;
        }
        //邮箱方式找回
        int nIndex = sUserName.indexOf('@');
        if (nIndex <= 0 || nIndex >= sUserName.length() - 1)
        {
            ui->labelMailUserInfo->setText(tr("Email is error"));
            break;
        }

//        int nIndex2 = sUserName.indexOf('.');
//        if (nIndex2 <= nIndex + 1 || nIndex2 >= sUserName.length() - 1)
//        {
//            ui->labelMailUserInfo->setText(tr("Email is error"));
//            break;
//        }
        int nQueryStatus = m_pLoginLibMgr->QueryUser(sQueryMailNameURL, sUserName, EMAILNAME);
        if(nQueryStatus == QUERY_USER_IS_EXIST)
        {
            //ui->labelMailUserInfo->setText(tr("Email is available"));
            bUserPass = true;
            break;
        }
        else if(nQueryStatus == REQUEST_CONNECT_FAIL)
        {
            ui->labelMailUserInfo->setText(tr("Request connection failure"));
            break;
        }
        else if(nQueryStatus == REQUEST_NO_RESPONSES)
        {
            ui->labelMailUserInfo->setText(tr("The request didn't respond"));
            break;
        }
        else
        {
            ui->labelMailUserInfo->setText(tr("Email is not registered"));
            break;
        }

    } while (0);
    return bUserPass;
}

