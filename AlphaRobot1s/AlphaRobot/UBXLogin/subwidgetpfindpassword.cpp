/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：subwidgetpfindpassword.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块手机找回密码子窗体源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#include "subwidgetpfindpassword.h"
#include "ui_subwidgetpfindpassword.h"

#include "clanguagemgr.h"

RETRANSLATEUIFUNCIMPL(SubWidgetPFindPassword)
SubWidgetPFindPassword::SubWidgetPFindPassword(QWidget *parent, loginLibMgr *pLoginLibMgr) :
    UBXWidget(parent),
    ui(new Ui::SubWidgetPFindPassword)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    m_pLoginLibMgr = pLoginLibMgr;
    m_pFindPassword = (WidgetFindPassword*)parent;
    ui->lEPhoneNum->setPlaceholderText(tr("Please enter register phone number"));

    // 为了使Country右对齐，QComboBox没有设置右对齐的方式
    QLineEdit *letNationComboBox  = new QLineEdit(ui->cbxCountryCode);
    ui->cbxCountryCode->setLineEdit(letNationComboBox);
    letNationComboBox->setAlignment(Qt::AlignRight);
    letNationComboBox->setEnabled(false);
    letNationComboBox->setFocusPolicy(Qt::NoFocus);

    ui->lEPhoneNum->setFocus();

    // 初始化国家码
    InitNationCode();

    connect(ui->pBPhoneFindPassword, &QPushButton::clicked, this,&SubWidgetPFindPassword::onPhoneFindPasswordClicked);
    connect(ui->cbxCountryCode, &QComboBox::editTextChanged, this, &SubWidgetPFindPassword::slotOnCurrentNationCodeChanged);

    // 设置默认的国家码
    ui->cbxCountryCode->setCurrentText(m_pFindPassword->getDialogLoginPage()->getDefaultNationText());
}

SubWidgetPFindPassword::~SubWidgetPFindPassword()
{
    delete ui;
}

/*************
* 函数名:onPhoneFindPasswordClicked
* 功能:手机找回密码按钮响应槽
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void SubWidgetPFindPassword::onPhoneFindPasswordClicked()
{
    if(!m_pFindPassword)
    {
        return;
    }
    setEnabled(false);
    m_pFindPassword->setEnabled(false);
    do
    {
        if(!CheckUser())
            break;
        QString strUserName = ui->lEPhoneNum->text();
        if(m_pFindPassword)
        {
            m_pFindPassword->SetUserName(strUserName);
        }
        //手机方式找回密码
        QString sRegisterURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/system/getSmsCode";
        QString sPhoneNum = m_ZoneCode + strUserName;    //中国地区发短信要加86;
        g_crisec.Lock();
        const QString strNetLanguageKeyword = CLanguageMgr::getNetLanguageKeyword(CLanguageMgr::getInstance()->getLanguageKeyword());
        bool bRet = m_pLoginLibMgr->GetSmsCode(sRegisterURL, sPhoneNum, strNetLanguageKeyword);
        g_crisec.Unlock();
        if(bRet)
        {
            m_pFindPassword->setSubwidgetUserName(PHONE_LOGION,  strUserName);
            m_pFindPassword->showPhoneContent(ResetContent);
        }
        else
        {
            ui->labelPhoneUserInfo->setText(tr("Message authentication code sent failure"));
        }

    } while (0);
    setEnabled(true);
    m_pFindPassword->setEnabled(true);

}

/*************
* 函数名:slotOnCurrentNationCodeChanged
* 功能:国家码选择的槽函数
* 参数:text 当前国家码
* 返回值:
* 时间: 2016/07/02
* 作者: hwx
*****************/
void SubWidgetPFindPassword::slotOnCurrentNationCodeChanged(const QString &text)
{
    QString selectCountry =text;
    if (!selectCountry.isEmpty())
    {
        ui->IENactionCode->setEnabled(true);        
        const int nNationID = DialogLoginPage::getNationID(selectCountry);
        ui->IENactionCode->setText("+" + QString::number(nNationID));
        m_ZoneCode = QString::number(nNationID);  // 获取用户密码后，因为要返回登录界面，因此国家码要更新
        ui->IENactionCode->setEnabled(false);
        ui->lEPhoneNum->setFocus();
    }
}

/**************************************************************************
* 函数名:InitNationCode
* 功能:初始化国家码
* 参数:
* 返回值:
* 时间:2016/07/02
* 作者: hwx
* 改动日志
**************************************************************************/
void SubWidgetPFindPassword::InitNationCode()
{
    if (m_pFindPassword)
    {
        ui->cbxCountryCode->addItems(m_pFindPassword->getDialogLoginPage()->getDefaultNationPairList());
    }
}

/**************************************************************************
* 函数名: CheckUser
* 功能: 检查用户名是否合法
* 时间: 2015/11/05 15:45
* 作者: ZDJ
*/
bool SubWidgetPFindPassword::CheckUser()
{
    bool bUserPass = false;
    QString sQueryPhoneNameURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/check";
    do
    {
        //手机号方式找回
        QString sUserName = ui->lEPhoneNum->text();
        if (sUserName.isEmpty())
        {
            ui->labelPhoneUserInfo->setText(tr("Mobile phone can not be empty"));
            break;
        }
//        if(sUserName.length() != 11)
//        {
//            ui->labelPhoneUserInfo->setText(tr("Mobile phone number wrong"));
//            break;
//        }

        sUserName = m_ZoneCode + sUserName;
        g_crisec.Lock();
        int nQueryStatus = m_pLoginLibMgr->QueryUser(sQueryPhoneNameURL, sUserName, PHONENAME);
        g_crisec.Unlock();
        if(nQueryStatus == QUERY_USER_IS_EXIST)
        {
            //ui->labelPhoneUserInfo->setText(tr("Mobile phone number available"));
            bUserPass = true;
        }
        else if(nQueryStatus == REQUEST_CONNECT_FAIL)
        {
            ui->labelPhoneUserInfo->setText(tr("Request connection failure"));
        }
        else if(nQueryStatus == REQUEST_NO_RESPONSES)
        {
            ui->labelPhoneUserInfo->setText(tr("The request didn't respond"));
        }
        else
        {
            if (m_ZoneCode.isEmpty())
            {
                ui->labelPhoneUserInfo->setText(tr("Country code is empty"));
            }
            else
            {
                ui->labelPhoneUserInfo->setText(tr("Mobile phone number is not registered"));
            }
            break;
        }

    } while (0);
    return bUserPass;
}

