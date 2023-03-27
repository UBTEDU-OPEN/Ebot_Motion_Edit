/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：subwidgetphasfindpassword.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块手机找回密码子窗体源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#include "subwidgetphasfindpassword.h"
#include "ui_subwidgetphasfindpassword.h"
#include "loginconst.h"
#include "clanguagemgr.h"

RETRANSLATEUIFUNCIMPL(SubWidgetPHasFindpassword)
SubWidgetPHasFindpassword::SubWidgetPHasFindpassword(QWidget *parent, loginLibMgr *pLoginLibMgr) :
    UBXWidget(parent),
    ui(new Ui::SubWidgetPHasFindpassword)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    m_pLoginLibMgr = pLoginLibMgr;
    m_pFindPassword = (WidgetFindPassword*)parent;
    m_nTimeoutID = 1;//超时时钟ID
    m_nTimeout = 60000;//超时时间60s
    m_nTimeCount = 0;
    connect(ui->pBResendMsg, &QPushButton::clicked, this , &SubWidgetPHasFindpassword::onResendMsgClicked);
    connect(ui->pBResetPassword, &QPushButton::clicked, this, &SubWidgetPHasFindpassword::onResetPasswordClicked);
}

SubWidgetPHasFindpassword::~SubWidgetPHasFindpassword()
{
    disconnect();
    killTimer(m_nTimeoutID);
    delete ui;
}

/*************
* 函数名:setPhoneNumber
* 功能:设置手机号码
* 参数:strPhoneNumber 手机号码
* 返回值:
* 时间: 2016/06/23
* 作者: hwx
*****************/
void SubWidgetPHasFindpassword::setPhoneNumber(const QString &strPhoneNumber)
{
    ui->textEditSendPhoneMessage->clear();
    QFont fontNormal;
    fontNormal.setPixelSize(FONT_PIXEL_SIZE_14);
    ui->textEditSendPhoneMessage->setFont(fontNormal);
    ui->textEditSendPhoneMessage->setTextColor(QColor(211, 211, 211)); // #d3d3d3
    ui->textEditSendPhoneMessage->insertPlainText(tr("PhoneNumberLeft"));

    QFont fontPhoneNumber;
    fontPhoneNumber.setPixelSize(FONT_PIXEL_SIZE_16);
    ui->textEditSendPhoneMessage->setFont(fontPhoneNumber);
    ui->textEditSendPhoneMessage->setTextColor(QColor(16, 181,229)); // "#10b5e5"
    ui->textEditSendPhoneMessage->insertPlainText(strPhoneNumber.right(LoginConstants::RIGHT_PHONE_NUMBER_WIDTH));

    ui->textEditSendPhoneMessage->setFont(fontNormal);
    ui->textEditSendPhoneMessage->setTextColor(QColor(211, 211, 211));
    ui->textEditSendPhoneMessage->insertPlainText(tr("PhoneNumberRight"));

    //ui->labelPhoneNumber->setText(strPhoneNumber.right(LoginConstants::RIGHT_PHONE_NUMBER_WIDTH));
}

/*************
* 函数名:onStartTime
* 功能:启动时钟
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void SubWidgetPHasFindpassword::onStartTime()
{
    //计时并使按钮变灰
    m_nTimeCount = m_nTimeout;

    QString sTip;
    sTip.sprintf("%ds", m_nTimeCount / 1000);
    ui->pBResendMsg->setText(sTip);

    ui->pBResendMsg->setEnabled(false);
    m_nTimeoutID = startTimer(1000);
}

/**************************************************************************
* 函数名:timerEvent
* 功能:时钟事件
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void SubWidgetPHasFindpassword::timerEvent(QTimerEvent *event)
{

    int nIDEvent = event->timerId();
    if(nIDEvent == m_nTimeoutID)
    {
        if(m_nTimeCount > 0)
        {
            m_nTimeCount -= 1000;
            QString sTip;
            sTip.sprintf("%ds", m_nTimeCount / 1000);
            ui->pBResendMsg->setText(sTip);
        }
        else
        {
            killTimer(nIDEvent);
            m_nTimeCount = m_nTimeout;
            ui->pBResendMsg->setEnabled(true);
            ui->pBResendMsg->setText(tr("Resend Message"));
        }
    }

}

/*************
* 函数名:onResetPasswordClicked
* 功能:重置密码
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void SubWidgetPHasFindpassword::onResetPasswordClicked()
{
    if(!m_pFindPassword)
    {
        return;
    }
    setEnabled(false);
    m_pFindPassword->setEnabled(false);
    do
    {
        QString verifyCode = ui->lEVerifyCode->text();
        if (verifyCode.isEmpty())
        {
            ui->labelVerifyCodeInfo->setText(tr("Verification code error"));
            break;
        }

        QString sRegisterURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/system/verifySms";
        //CString sPhoneNum = CCODE + m_UserName;    //中国地区发短信要加86;
        QString strUserName = "";
        if(m_pFindPassword)
        {
            strUserName = m_pFindPassword->GetUserName();
        }
        QString sPhoneNum = m_ZoneCode + strUserName;    //中国地区发短信要加86;
        g_crisec.Lock();
        bool bRet = m_pLoginLibMgr->VerifySmsCode(sRegisterURL, sPhoneNum, verifyCode);
        g_crisec.Unlock();

//        ////////////////////////
//        //// test huangwuxian
//        bRet = true;
//        ////////////////////////

        if(bRet)
        {
            ui->labelVerifyCodeInfo->setText(tr(""));
            //切换到处置密码窗口
            if(m_pFindPassword)
            {
                killTimer(m_nTimeoutID);
                m_pFindPassword->showPhoneContent(FindContent); //还原找回密码窗口
                m_pFindPassword->changetoResetPage(strUserName, verifyCode);
            }
        }
        else
        {
            ui->labelVerifyCodeInfo->setText(tr("Verification code error"));
            break;
        }
    }while(0);
    setEnabled(true);
    m_pFindPassword->setEnabled(true);
}

/*************
* 函数名:onResendMsgClicked
* 功能:重新发送短信验证码
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void SubWidgetPHasFindpassword::onResendMsgClicked()
{
    QString strUserName = "";
    if(!m_pFindPassword)
    {
        return;
    }
    setEnabled(false);
    m_pFindPassword->setEnabled(false);
    g_crisec.Lock();
    do
    {
        if(m_pFindPassword)
        {
            strUserName = m_pFindPassword->GetUserName();
        }
        //手机方式找回密码
        QString sRegisterURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/system/getSmsCode";
        QString sPhoneNum = m_ZoneCode + strUserName;    //中国地区发短信要加86;

        const QString strNetLanguageKeyword = CLanguageMgr::getNetLanguageKeyword(CLanguageMgr::getInstance()->getLanguageKeyword());
        bool bRet = m_pLoginLibMgr->GetSmsCode(sRegisterURL, sPhoneNum, strNetLanguageKeyword);
        if(bRet)
        {
            //计时并使按钮变灰
            m_nTimeCount = m_nTimeout;
            ui->pBResendMsg->setEnabled(false);

            m_nTimeoutID = startTimer(1000);
        }
        else
        {
            ui->labelVerifyCodeInfo->setText(tr("Message authentication code sent failure"));
        }
    }while(0);
    g_crisec.Unlock();
    setEnabled(true);
    m_pFindPassword->setEnabled(true);

}


