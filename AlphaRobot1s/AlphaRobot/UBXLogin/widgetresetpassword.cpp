/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：widgetresetpassword.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块重置密码主窗体源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#include "widgetresetpassword.h"
#include "ui_widgetresetpassword.h"
#include "loginconst.h"


RETRANSLATEUIFUNCIMPL(WidgetResetPassword)
WidgetResetPassword::WidgetResetPassword(QWidget *parent, loginLibMgr *pLoginLibMgr) :
    UBXWidget(parent),
    ui(new Ui::WidgetResetPassword)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    m_pLoginPage = (DialogLoginPage*)parent;
    m_pLoginLibMgr = pLoginLibMgr;
    m_bPwdPass = false;
    m_bPwdPassAgain = false;
    m_UserNames = "";
    m_strFirstPassword = "";
    m_strSecondPassword = "";
    m_VerifyCode = "";

    connect(ui->pBReturnLogin, &QPushButton::clicked, this, &WidgetResetPassword::slotOnReturnLoginClicked);
}

WidgetResetPassword::~WidgetResetPassword()
{
    delete ui;
}

/*************
* 函数名:on_pBResetPassword_clicked
* 功能:重置密码
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetResetPassword::on_pBResetPassword_clicked()
{
    if(!m_pLoginPage)
    {
        return;
    }
    setEnabled(false);
    m_pLoginPage->setEnabled(false);
    g_crisec.Lock();
    do
    {
        OnEnKillfocusFirstPassword();
        OnEnKillfocusAgainPassword();

        if(!m_bPwdPass)
        {
            break;
        }

        if(!m_bPwdPassAgain)
        {
            break;
        }

        QString sRegisterURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/system/verifySms";
        QString sPhoneNum = m_ZoneCode + m_UserNames;    //中国地区发短信要加86;
        bool bRegisterStatus = false;
        bRegisterStatus = m_pLoginLibMgr->VerifySmsCode(sRegisterURL, sPhoneNum, m_VerifyCode);

        QString sFindPassWordURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/rpasswordPhone";
        bool result = m_pLoginLibMgr->SetPassWord(sFindPassWordURL, sPhoneNum, m_strFirstPassword, PHONENAME, m_VerifyCode);

//        ////////////////////////
//        //// test huangwuxian
//        result = true;
//        ////////////////////////

        if (result)
        {
            g_userinfo = USERINFO(g_userinfo.m_nUserID, sPhoneNum.toUtf8().data(), m_strFirstPassword.toUtf8().data(), false);
            //CMessageBox::information(NULL,"",tr("Reset Password Success!"));
            ui->labelResetPasswordInfo->setText(tr("Reset Password Success!"));
            QTimer::singleShot(2000, this, &WidgetResetPassword::slotOnReturnLoginClicked);
            //切换到登陆窗口
           // m_pLoginPage->ChangeChildWidget(LOGIN_WIDGET);
            //emit sigUpdateLoginWidget(m_UserNames, "", PHONENAME);
        }
        else
        {
            ui->labelResetPasswordInfo->setText(tr("Change password failed"));
        }

    } while (0);

    g_crisec.Unlock();

    setEnabled(true);
    m_pLoginPage->setEnabled(true);
}

/*************
* 函数名:slotOnReturnLoginClicked
* 功能:点击返回登录时的槽函数
* 时间: 2016/06/22
* 作者: hwx
*****************/
void WidgetResetPassword::slotOnReturnLoginClicked()
{
    if (m_pLoginPage)
    {
        m_pLoginPage->ChangeChildWidget(LOGIN_WIDGET);
        emit sigUpdateLoginWidget(m_UserNames, "", PHONENAME);
    }
}

/*************
* 函数名:OnEnKillfocusFirstPassword
* 功能:验证第一次输入密码是否合法
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetResetPassword::OnEnKillfocusFirstPassword()
{
    m_bPwdPass = false;

    m_strFirstPassword = ui->lEPhonePassword->text();
    if (m_strFirstPassword.isEmpty())
    {
        ui->labelPasswordInfo->setText(tr("Incorrect password!"));
        return;
    }

    int results = VerifyPasswordCounter(m_strFirstPassword);

    if (results == 1)
    {
        ui->labelPasswordInfo->setText(tr("Please enter 15 to 6 bit password!"));
    }
    else if (results == 2)
    {
        ui->labelPasswordInfo->setText(tr("Password may not contain characters other than letters or Numbers!"));
    }
    else if (results == 3)
    {
        //ui->labelPasswordInfo->setText(tr("The password can be used!"));
        m_bPwdPass = true;
    }
}

/*************
* 函数名:OnEnKillfocusAgainPassword
* 功能:验证再次输入密码是否合法
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetResetPassword::OnEnKillfocusAgainPassword()
{
    m_bPwdPassAgain = false;

    if(!m_bPwdPass)
        return;
    m_strSecondPassword = ui->lEConfirmPhonePassword->text();
    if (m_strFirstPassword.compare(m_strSecondPassword) == 0)
    {
        //ui->labelConfirmPasswordInfo->setText(tr("The two passwords are identical!"));
        m_bPwdPassAgain = true;
    }
    else
    {
        ui->labelConfirmPasswordInfo->setText(tr("The two passwords are not identical!"));
    }
}

/**************************************************************************
* 函数名:VerifyPasswordCounter
* 功能:校验密码
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
int WidgetResetPassword::VerifyPasswordCounter(QString strPassword)
{
    int nCount = strPassword.length();
    if (nCount < 6 || nCount > 15)
    {
        return 1;
    }

    for(int i = 0; i < strPassword.length(); i++)
    {
        if((strPassword.at(i) >= '0' && strPassword.at(i) <= '9')
                || (strPassword.at(i) >= 'a' && strPassword.at(i) <= 'z')
                || (strPassword.at(i) >= 'A' && strPassword.at(i) <= 'Z'))
        {
        } else {
            return 2;
        }
    }
    return 3;
}

/**************************************************************************
* 函数名:getShowUserName
* 功能:获取用于界面显示的手机号码
* 参数:strPhoneNumber 手机号码
* 返回值:
* 时间:2016/06/23
* 作者: hwx
* 改动日志
**************************************************************************/
QString WidgetResetPassword::getShowUserName(const QString &strPhoneNumber)
{
    QString strOriginalPhoneNumber = strPhoneNumber;
    if (strOriginalPhoneNumber.size() <= (LoginConstants::LEFT_PHONE_NUMBER_WIDTH + LoginConstants::RIGHT_PHONE_NUMBER_WIDTH))
    {
        return strOriginalPhoneNumber;
    }

    QString strLeft = strOriginalPhoneNumber.left(LoginConstants::LEFT_PHONE_NUMBER_WIDTH);
    QString strRight = strOriginalPhoneNumber.right(LoginConstants::RIGHT_PHONE_NUMBER_WIDTH);
    QString strMiddle = strOriginalPhoneNumber.fill('*').remove(0, LoginConstants::LEFT_PHONE_NUMBER_WIDTH + LoginConstants::RIGHT_PHONE_NUMBER_WIDTH);

    return strLeft + strMiddle + strRight;
}

/**************************************************************************
* 函数名:SetUserName
* 功能:设置用户名
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void WidgetResetPassword::SetUserName(QString userName)
{
    m_UserNames = userName;
    ui->labelCountryCode->setText("+" + m_ZoneCode);
    ui->labelPhoneNum->setText(getShowUserName(m_UserNames));
}

/*************
* 函数名:SetVerifyCode
* 功能:设置当前手机校验码
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void WidgetResetPassword::SetVerifyCode(QString verifyCode)
{
    m_VerifyCode = verifyCode;
}

//void WidgetResetPassword::OnEnChangePassword()
//{
//    m_bPwdPass = false;
//    ui->lEConfirmPhonePassword->setText("");
//    m_bPwdPassAgain = false;
//}
