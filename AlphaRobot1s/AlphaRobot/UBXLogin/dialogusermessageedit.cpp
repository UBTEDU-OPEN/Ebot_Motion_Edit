/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：dialogusermessageedit.cpp
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块用户信息修改窗体源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#include "dialogusermessageedit.h"
#include "ui_dialogusermessageedit.h"
#include "loginlibmgr.h"
#include <QMouseEvent>

#include "../zxLib/widget/messagebox.h"
#include "dialogloginpage.h"

DialogUserMessageEdit::DialogUserMessageEdit(QWidget *parent, loginLibMgr *pLoginLibMgr) :
    QDialog(parent),
    ui(new Ui::DialogUserMessageEdit)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    initDialog();
    setDefaultStyle();
    m_bIsNeedUpdateData = false;
    m_nGenderType = NOTKNOW;
    connect(ui->pBEditUserInfo, &QPushButton::clicked, this, &DialogUserMessageEdit::onEditUserInfoClicked);
    //connect(ui->pbCloseMessage, &QPushButton::clicked, this, &DialogUserMessageEdit::reject);
    connect(ui->pBOk, &QPushButton::clicked, this, &DialogUserMessageEdit::onEditUserInfoOk);
    connect(ui->pBCancel, &QPushButton::clicked, this, &DialogUserMessageEdit::reject);
    m_pLoginLibMgr = pLoginLibMgr;
    m_dragPosition = QPoint(-1, -1);

#ifdef Q_OS_OSX
    ui->pbClose_Win->hide();
    connect(ui->pbCloseMessage, &QPushButton::clicked, this, &DialogUserMessageEdit::reject);
#else
    ui->pbCloseMessage->hide();
    connect(ui->pbClose_Win, &QPushButton::clicked, this, &DialogUserMessageEdit::reject);
#endif

#ifdef Q_OS_WIN
    ui->horizontalLayout_8->setContentsMargins(10, -10, 10, 0);
#endif


}

DialogUserMessageEdit::~DialogUserMessageEdit()
{
    delete ui;
}

/*************
* 函数名:initDialog
* 功能:初始化对话框
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogUserMessageEdit::initDialog()
{
    ui->lENickName->setText(m_userNickName);
    InitNationCode();
    m_sCountryCode = m_ZoneCode;
    if(m_userGender.compare("0", Qt::CaseInsensitive) == 0)
    {
        ui->rBMale->setChecked(true);
        ui->rBFemale->setChecked(false);
        m_nGenderType = NOTKNOW;    ///不明白为啥Window这样写，难道有三种性别？？？？
    }
    else if(m_userGender.compare("1", Qt::CaseInsensitive) == 0)
    {
        ui->rBMale->setChecked(true);
        ui->rBFemale->setChecked(false);
        m_nGenderType = MALE;
    }
    else if(m_userGender.compare("2", Qt::CaseInsensitive) == 0)
    {
        ui->rBMale->setChecked(false);
        ui->rBFemale->setChecked(true);
        m_nGenderType = FEMALE;
    }

    if(-1 == m_userNameInfo.indexOf("@"))
    {
        ui->labelPhoneName->show();
        ui->labelMailName->hide();
        ui->lEPhoneName->show();
        ui->lEMailName->hide();
        ui->lEPhoneName->setText(m_userNameInfo);
        m_UserType = PHONENAME;
    }
    else
    {
        ui->labelPhoneName->hide();
        ui->labelMailName->show();
        ui->lEPhoneName->hide();
        ui->lEMailName->show();
        ui->lEMailName->setText(m_userNameInfo);
        m_UserType = EMAILNAME;
    }
    SetDialogEnalbe(false);
    ui->lEPhoneName->setEnabled(false);
    ui->lEMailName->setEnabled(false);

    QString strText;
    for(int i = 0; i<ui->cBNation->count(); i++)
    {
        strText = ui->cBNation->itemText(i);
        if (!strText.isEmpty())
        {
            QString CountryCode = QString::number(DialogLoginPage::getNationID(strText));
            if(QString::compare(m_ZoneCode, CountryCode, Qt::CaseInsensitive) == 0)
            {
                ui->cBNation->setCurrentIndex(i);
                break;
            }
        }
    }

}

/*************
* 函数名:InitNationCode
* 功能:初始化国家码
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogUserMessageEdit::InitNationCode()
{    
    QList<QPair<QString, QVariant> > lstNationCode = DialogLoginPage::getDefaultNationPairList();

    for(int i = 0; i < lstNationCode.count(); ++i)
    {
        ui->cBNation->addItem(lstNationCode.at(i).first);
    }
}

/*************
* 函数名:setDefaultStyle
* 功能:设置默认皮肤风格
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogUserMessageEdit::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/UBXUserMessageDefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

/*************
* 函数名:SetDialogEnalbe
* 功能:设置对话框是否可用
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogUserMessageEdit::SetDialogEnalbe(bool bRet)
{
    ui->lENickName->setEnabled(bRet);
    ui->rBMale->setEnabled(bRet);
    ui->rBFemale->setEnabled(bRet);
    if(m_UserType == PHONENAME)
    {
        ui->cBNation->setEnabled(false);
    }
    else
    {
        ui->cBNation->setEnabled(bRet);
    }

}

/*************
* 函数名:onEditUserInfoClicked
* 功能:编辑用户信息按钮响应
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogUserMessageEdit::onEditUserInfoClicked()
{
    SetDialogEnalbe(true);
    m_bIsNeedUpdateData = true;
}

/*************
* 函数名:onEditUserInfoOk
* 功能:修改信息确认按钮响应槽
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogUserMessageEdit::onEditUserInfoOk()
{
    if(m_bIsNeedUpdateData)
    {
        setEnabled(false);
        g_crisec.Lock();
        do
        {
            QString strNickName =ui->lENickName->text();
            m_userNickName = strNickName;
            int nLoginStatus = 0;

            if(ui->rBMale->isChecked()&&!ui->rBFemale->isChecked())
            {
                m_userGender = "1";
            }
            else if(ui->rBFemale->isChecked()&&!ui->rBMale->isChecked())
            {
                m_userGender = "2";
            }
            else
            {
                m_userGender = "0";
            }
            m_ZoneCode = m_sCountryCode;

            QString sLoginURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/edit";
            if(m_pLoginLibMgr)
            {

                if(m_UserType == PHONENAME)
                {
                    nLoginStatus = m_pLoginLibMgr->SetUserInfoP(sLoginURL, m_userNameInfo, strNickName, m_userGender);
                }
                else if(m_UserType == EMAILNAME)
                {
                    nLoginStatus = nLoginStatus = m_pLoginLibMgr->SetUserInfo(sLoginURL, m_userNameInfo, m_userNickName, m_userGender, m_ZoneCode);
                }
                if (nLoginStatus ==0)
                {
                    emit sigNickNameChanged(strNickName);
                    CMessageBox::warning(NULL, "", tr("Successfully changed, please login again!"));
                }
                else
                {
                    CMessageBox::warning(NULL, "", tr("Failed changed!"));
                }
            }
        }while(0);
        g_crisec.Unlock();

        setEnabled(true);

    }
    accept();
}

/*************
* 函数名:mouseReleaseEvent
* 功能:鼠标离开事件
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogUserMessageEdit::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = QPoint(-1, -1);
        event->accept();
    }
}

/*************
* 函数名:mousePressEvent
* 功能:鼠标按下事件
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogUserMessageEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

/*************
* 函数名:mouseMoveEvent
* 功能:鼠标移动事件
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogUserMessageEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() &Qt::LeftButton)
    {
        if (m_dragPosition != QPoint(-1, -1))
            move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

/*************
* 函数名:on_cBNation_currentIndexChanged
* 功能:当前国家码选择修改
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogUserMessageEdit::on_cBNation_currentIndexChanged(int index)
{
    QString selectCountry = ui->cBNation->itemText(index);
    if (!selectCountry.isEmpty())
    {
        m_sCountryCode = QString::number(DialogLoginPage::getNationID(selectCountry));
    }
}
