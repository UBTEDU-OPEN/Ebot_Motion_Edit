/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：subwidgetmhasfindpassword.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块邮箱找回密码子窗体源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#include "subwidgetmhasfindpassword.h"
#include "ui_subwidgetmhasfindpassword.h"

RETRANSLATEUIFUNCIMPL(SubWidgetMHasFindPassword)
SubWidgetMHasFindPassword::SubWidgetMHasFindPassword(QWidget *parent, loginLibMgr *pLoginLibMgr) :
    UBXWidget(parent),
    ui(new Ui::SubWidgetMHasFindPassword)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    m_strMailAccount = "";
    m_pLoginLibMgr = pLoginLibMgr;
    m_pFindPassword = (WidgetFindPassword*)parent;
    connect(ui->pBFindMailPasswordOk, &QPushButton::clicked, this, &SubWidgetMHasFindPassword::onFindMailPasswordOkClicked);
}

SubWidgetMHasFindPassword::~SubWidgetMHasFindPassword()
{
    delete ui;
}

/*************
* 函数名:setMailAccount
* 功能:设置邮箱账号
* 参数:strMailAccount 邮箱账号
* 返回值:
* 时间: 2016/06/23
* 作者: hwx
*****************/
void SubWidgetMHasFindPassword::setMailAccount(const QString &strMailAccount)
{
    m_strMailAccount = strMailAccount;
    ui->textEditSendMailMessage->clear();
    QFont fontNormal;
    fontNormal.setPixelSize(FONT_PIXEL_SIZE_14);
    ui->textEditSendMailMessage->setFont(fontNormal);
    ui->textEditSendMailMessage->setTextColor(QColor(211, 211, 211)); // #d3d3d3
    ui->textEditSendMailMessage->insertPlainText(tr("MailAccountLeft"));

    QFont fontPhoneNumber;
    fontPhoneNumber.setPixelSize(FONT_PIXEL_SIZE_16);
    ui->textEditSendMailMessage->setFont(fontPhoneNumber);
    ui->textEditSendMailMessage->setTextColor(QColor(16, 181,229)); // "#10b5e5"
    ui->textEditSendMailMessage->insertPlainText(strMailAccount);

    ui->textEditSendMailMessage->setFont(fontNormal);
    ui->textEditSendMailMessage->setTextColor(QColor(249, 78, 74)); // #f94e4a
    ui->textEditSendMailMessage->append(tr("MailAccountRight"));

    //ui->labelMailAccount->setText(strMailAccount);
}

/*************
* 函数名:onFindMailPasswordOkClicked
* 功能:通过邮箱找回密码按钮响应槽
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void SubWidgetMHasFindPassword::onFindMailPasswordOkClicked()
{
    if(m_pFindPassword)
    {
        m_pFindPassword->showMailContent(FindContent);
        m_pFindPassword->onReturnLoginClicked();
        m_pFindPassword->updateLoginWidget(m_strMailAccount, "", EMAILNAME);
    }
}
