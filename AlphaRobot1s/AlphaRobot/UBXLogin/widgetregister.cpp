/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：widgetregister.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块注册主窗体源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#include "widgetregister.h"
#include "ui_widgetregister.h"
#include <QTimerEvent>
#include <QListView>

#include "../zxLib/widget/messagebox.h"
#include "dialogloginpage.h"
#include "clanguagemgr.h"

RETRANSLATEUIFUNCIMPL(Widgetregister)

Widgetregister::Widgetregister(QWidget *parent, loginLibMgr * pLoginLibMgr) :
    UBXWidget(parent),
    ui(new Ui::Widgetregister)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    m_pLoginLibMgr = pLoginLibMgr;
    m_pLoginPage = (DialogLoginPage*)parent;

    connect(ui->pBReturnLogin,&QPushButton::clicked,this,&Widgetregister::onReturnLogin);
    connect(ui->pBPhoneRegister,&QPushButton::clicked,this, &Widgetregister::onPhoneRegisterClicked);
    connect(ui->pBMailRegister, &QPushButton::clicked,this, &Widgetregister::onMailRegisterClicked);
    connect(ui->pBGetMessageCode, &QPushButton::clicked,this, &Widgetregister::onGetMessageCodeClicked);
    //connect(this,&Widgetregister::SigRegisterResult, this, &Widgetregister::OnUserResgisterResult);
    connect(ui->tabWidgetRegister, &QTabWidget::currentChanged, this, &Widgetregister::onTabWidgetRegisterCurrentChanged);
    connect(ui->cBNation, &QComboBox::editTextChanged, this, &Widgetregister::slotOnCurrentNationCodeChanged);
    ui->tabWidgetRegister->setCurrentIndex(0);
    InitNationCode();

    ui->cBNation->setView(new QListView); // 为了使QComboBox下拉QSS设置生效
    ui->cBNation->setMaxVisibleItems(20); //设置最大显示下列项 超过要使用滚动条拖拉

    // 为了使Country右对齐，QComboBox没有设置右对齐的方式
    QLineEdit *letNationComboBox  = new QLineEdit(ui->cBNation);
    ui->cBNation->setLineEdit(letNationComboBox);
    letNationComboBox->setAlignment(Qt::AlignRight);
    letNationComboBox->setEnabled(false);
    letNationComboBox->setFocusPolicy(Qt::NoFocus);


    // 设置背景提示
    ui->lEPhoneUserName->setPlaceholderText(tr("Please input the phone number"));
    ui->lEPhonePassword->setPlaceholderText(tr("Please input 6~15 char password"));
    ui->lEPhonePasswordConfirm->setPlaceholderText(tr("Please input the password again"));
    ui->lEMessageCode->setPlaceholderText(tr("Please input the verify code"));
    ui->lEMailUserName->setPlaceholderText(tr("Please input your Email"));
    ui->lEMailPassword->setPlaceholderText(tr("Please input 6~15 char password"));
    ui->lEMailPasswordConfirm->setPlaceholderText(tr("Please input the password again"));

    m_bIsPassVerify = false;
    m_bIsPassPassword = false;
    m_bIsPassPwdAgain = false;
    m_bIsPassUser = false;

    ui->cBNation->setCurrentText(m_pLoginPage->getDefaultNationText());

    //ui->lEISO->setText(tr("+86"));
    m_nTimeoutID = 1;//超时时钟ID
    m_nTimeout = 60000;//超时时间60s
    m_nTimeCount = 0;
    m_nUserType = USERNAME;

    setDefaultFocus();
}

Widgetregister::~Widgetregister()
{
    delete ui;
}

/**************************************************************************
* 函数名:keyPressEvent
* 功能:键盘按键事件
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::keyPressEvent(QKeyEvent *event)
{

    if(event->key() == Qt::Key_Return ||event->key() == Qt::Key_Enter)
    {
        if(m_nUserType == PHONENAME)
        {
            onPhoneRegisterClicked();
        }
        else if(m_nUserType == EMAILNAME)
        {
            onMailRegisterClicked();
        }
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

/*************
* 函数名:slotSetCurrentTabIndex
* 功能: 设置当前tab
* 参数:
* 返回值:
* 时间: 2016/07/011
* 作者: hwx
*****************/
void Widgetregister::slotSetCurrentTabIndex(int nIndex)
{
    ui->tabWidgetRegister->setCurrentIndex(nIndex);
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
void Widgetregister::timerEvent(QTimerEvent *event)
{

    int nIDEvent = event->timerId();
    if(nIDEvent == m_nTimeoutID)
    {
        if(m_nTimeCount > 0)
        {
            m_nTimeCount -= 1000;
            QString sTip;
            sTip.sprintf("%ds", m_nTimeCount / 1000);
            ui->pBGetMessageCode->setText(sTip);
        }
        else
        {
            killTimer(nIDEvent);
            m_nTimeCount = m_nTimeout;
            ui->pBGetMessageCode->setEnabled(true);
            ui->pBGetMessageCode->setText(tr("Get Verify Code"));
        }
    }

}

/**************************************************************************
* 函数名:setDefaultFocus
* 功能:设置默认取得焦点的控件
* 参数:
* 返回值:
* 时间:2016/06/18
* 作者: hwx
* 改动日志
**************************************************************************/
void Widgetregister::setDefaultFocus()
{
    if (PHONENAME == m_nUserType)
    {
        ui->lEPhoneUserName->setFocus();
    }
    else if (EMAILNAME == m_nUserType)
    {
        ui->lEMailUserName->setFocus();
    }
}

/**************************************************************************
* 函数名:setCurrentRegisterTabIndex
* 功能:设置当前注册方式
* 参数:eAccountType 账号类型
* 返回值:
* 时间:2016/07/02
* 作者: hwx
* 改动日志
**************************************************************************/
void Widgetregister::setCurrentRegisterTabIndex(emAccountType eAccountType)
{
    ui->tabWidgetRegister->setCurrentIndex(eAccountType);
}

/**************************************************************************
* 函数名:OnUserResgisterResult
* 功能:注册结果事件
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::OnUserResgisterResult(const QString &strUserName, const QString &strPassword, emAccountType eAccountType,  bool bRslt)
{
    if(bRslt)
    {
        //注册成功
        if(m_pLoginPage)
        {
            if(QMessageBox::Yes == CMessageBox::question(this, "", tr("Register Success! Go To Login?"),
                                                         QMessageBox::No|QMessageBox::Yes,
                                                         QMessageBox::Yes))
            {
                m_pLoginPage->ChangeChildWidget(LOGIN_WIDGET);
                emit sigUpdateLoginWidget(strUserName, "", eAccountType);
            }
            else
            {
                return;
            }
        }
    }
    else
    {
        CMessageBox::information(NULL, "", tr("Register Failed!"));
    }
    return ;
}

/**************************************************************************
* 函数名:onReturnLogin
* 功能:返回登录
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::onReturnLogin()
{
    if(m_pLoginPage)
    {
        m_pLoginPage->ChangeChildWidget(LOGIN_WIDGET);
        emit sigUpdateLoginWidget(m_strUserName, "", m_nUserType);
    }
}

/**************************************************************************
* 函数名:onGetMessageCodeClicked
* 功能:获取手机验证码
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::onGetMessageCodeClicked()
{
    //分别判断用户名、密码、两次密码是否一致
    OnEnKillfocusUser(PHONENAME);
    OnEnKillfocusPassword(PHONENAME);
    OnPasswordAgain(PHONENAME);
    if(!m_pLoginPage)
    {
        return;
    }
    setEnabled(false);
    m_pLoginPage->setEnabled(false);
    g_crisec.Lock();
    do
    {
        //判断用户等是否有效
        if (!m_bIsPassUser)
        {
            //            ui->labelPhoneNumStatus->setText(tr("User is invalid"));
            //            CMessageBox::warning(NULL,"", tr("User is invalid"));
            break;
        }

        if (!m_bIsPassPassword)
        {
            //            ui->labelPPasswordStatus->setText(tr("Password is invalid"));
            //            CMessageBox::warning(NULL,"", tr("Password is invalid"));
            break;
        }

        if (!m_bIsPassPwdAgain)
        {
            //            ui->labelPConfirmPasswordStatus->setText(tr("Password do not match"));
            //            CMessageBox::warning(NULL,"", tr("Password do not match"));
            break;
        }

        QString sGetSmsCodeURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/system/getSmsCode";
        QString strPhoneNum = ui->lEPhoneUserName->text();
        setZoneNum();
        QString sPhoneNum = m_ZoneCode + strPhoneNum;    //中国地区发短信要加86;
        bool bRegisterStatus = false;

        const QString strNetLanguageKeyword = CLanguageMgr::getNetLanguageKeyword(CLanguageMgr::getInstance()->getLanguageKeyword());
        bRegisterStatus = m_pLoginLibMgr->GetSmsCode(sGetSmsCodeURL, sPhoneNum, strNetLanguageKeyword);

        if (bRegisterStatus)
        {
            //计时并使按钮变灰
            ui->labelGetMsgCodeStatus->setText(tr("message is sending to your phone"));
            //            CMessageBox::information(NULL, "", tr("message is sending to your phone"));

            m_nTimeCount = m_nTimeout;
            ui->pBGetMessageCode->setEnabled(false);

            m_nTimeoutID = startTimer(1000);
        }
        else
        {

            //            m_textVerifyCodeTip.SetWindowText(LANGUAGE_LOAD(ID_TEXT_SMSSENDFAILED));
        }

    } while (0);

    g_crisec.Unlock();

    setEnabled(true);
    m_pLoginPage->setEnabled(true);
}

/**************************************************************************
* 函数名:onPhoneRegisterClicked
* 功能:手机注册
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::onPhoneRegisterClicked()
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
        OnEnKillfocusUser(PHONENAME);
        OnEnKillfocusPassword(PHONENAME);
        OnPasswordAgain(PHONENAME);
        OnEnKillfocusVerifyMsg();

//        /////////////////////////
//        // test huangwuxian
//        m_bIsPassUser = true;
//        ///////////////////////////

        if (!m_bIsPassUser)
        {
            break;
        }

        if (!m_bIsPassPassword)
        {
            break;
        }

        if (!m_bIsPassPwdAgain)
        {
            break;
        }

//        /////////////////////////
//        // test huangwuxian
//        m_bIsPassVerify = true;
//        ///////////////////////////

        if (!m_bIsPassVerify)
        {
            break;
        }

        QString sRegisterURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/register";
        m_strUserName = ui->lEPhoneUserName->text();
        QString sUserPassword = ui->lEPhonePassword->text();
        setZoneNum();
        QString sPhoneNum = m_ZoneCode+m_strUserName;

        int nRegisterStatus = m_pLoginLibMgr->RegisterP(sRegisterURL, m_ZoneCode, sPhoneNum, sUserPassword, PHONENAME);

//        /////////////////////////
//        // test huangwuxian
//        nRegisterStatus = REQUEST_SUCCESS;
//        ///////////////////////////

        if (nRegisterStatus == REQUEST_SUCCESS)
        {
            g_userinfo = USERINFO(0, sPhoneNum.toUtf8().data(), sUserPassword.toUtf8().data(), false);
            ui->labelRegisgerPhoneInfo->setText(tr("Register successful"));
            QTimer::singleShot(2000, this, &Widgetregister::onReturnLogin);
            //emit SigRegisterResult(sUserName, "", PHONENAME, true);
        }
        else if (nRegisterStatus == USERNAME_ALREADY_EXIST)
        {

            CMessageBox::warning(NULL, "", tr("Mobile phone number has been in existence") );
            //            ui->labelPhoneNumStatus->setText(tr("Mobile phone number has been in existence"));
        }
        else if(nRegisterStatus == REQUEST_CONNECT_FAIL)
        {
            CMessageBox::warning(NULL, "", tr("Request connection failure") );
            //            ui->labelPhoneNumStatus->setText(tr("Request connection failure"));
        }
        else if(nRegisterStatus == REQUEST_NO_RESPONSES)
        {
            CMessageBox::warning(NULL, "", tr("The request didn't respond") );
            //            ui->labelPhoneNumStatus->setText(tr("The request didn't respond"));
        }
        else
        {
            CMessageBox::warning(NULL, "", tr("Unknown Error"));
            //            ui->labelPhoneNumStatus->setText(tr("Unknown Error"));
        }

    } while (0);

    g_crisec.Unlock();

    setEnabled(true);
    m_pLoginPage->setEnabled(true);
}

/**************************************************************************
* 函数名:onMailRegisterClicked
* 功能:邮箱注册
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::onMailRegisterClicked()
{
    if(!m_pLoginPage)
    {
        return;
    }
    setEnabled(false);
    m_pLoginPage->setEnabled(false);
    do
    {
        OnEnKillfocusUser(EMAILNAME);
        OnEnKillfocusPassword(EMAILNAME);
        OnPasswordAgain(EMAILNAME);
//        /////////////////////////
//        // test huangwuxian
//        m_bIsPassUser = true;
//        ///////////////////////////
        if (!m_bIsPassUser)
        {
            break;
        }

        if (!m_bIsPassPassword)
        {
            break;
        }

        if (!m_bIsPassPwdAgain)
        {
            break;
        }
        QString sRegisterURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/register";
        m_strUserName = ui->lEMailUserName->text();
        QString sUserPassword = ui->lEMailPassword->text();
        g_crisec.Lock();
        int nRegisterRslt = m_pLoginLibMgr->RegisterMail(sRegisterURL, m_strUserName, sUserPassword, EMAILNAME);
        g_crisec.Unlock();

//        /////////////////////////
//        // test huangwuxian
//        nRegisterRslt = REQUEST_SUCCESS;
//        ///////////////////////////

        if (nRegisterRslt == REQUEST_SUCCESS)
        {
            g_userinfo = USERINFO(0, m_strUserName.toUtf8().data(), sUserPassword.toUtf8().data(), false);
            ui->labelRegisterMailInfo->setText(tr("Register successful"));
            QTimer::singleShot(2000, this, &Widgetregister::onReturnLogin);
            //emit SigRegisterResult(sUserName, "", EMAILNAME, true);
        }
        else if (nRegisterRslt == USERNAME_ALREADY_EXIST)
        {
            CMessageBox::warning(NULL, "", tr("Mail Account has been in existence") );
        }
        else if(nRegisterRslt == REQUEST_CONNECT_FAIL)
        {
            CMessageBox::warning(NULL, "", tr("Request connection failure") );
        }
        else if(nRegisterRslt == REQUEST_NO_RESPONSES)
        {
            CMessageBox::warning(NULL, "", tr("The request didn't respond") );
        }
        else
        {
            CMessageBox::warning(NULL, "", tr("Unknown Error"));
        }

    } while (0);

    setEnabled(true);
    m_pLoginPage->setEnabled(true);
}

/**************************************************************************
* 函数名:setZoneNum
* 功能:设置国家码
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::setZoneNum()
{
    QString sZoneNum = ui->lEISO->text().remove(QChar('+'));
    if (sZoneNum.isEmpty())
    {
        sZoneNum = "86";
    }
    m_ZoneCode = sZoneNum;
}

/**************************************************************************
* 函数名: OnEnKillfocusUser
* 功能:验证用户名
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::OnEnKillfocusUser(int nType)
{
    m_bIsPassUser = false;
    QString sQueryURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/check";

    if(nType == PHONENAME)
    {
        QString sPhoneNum = ui->lEPhoneUserName->text();

        setZoneNum();
        QString sPhoneName = m_ZoneCode + sPhoneNum;

        if (sPhoneNum.isEmpty())
        {
            ui->labelPhoneNumStatus->setText(tr("Mobile phone cant't be empty!"));
            return;
        }

//        if (sPhoneNum.length() != 11)
//        {
//            ui->labelPhoneNumStatus->setText(tr("Mobile phone num is wrong!"));
//            return;
//        }

        int nQueryStatus =m_pLoginLibMgr->QueryUser(sQueryURL, sPhoneName,PHONENAME);

        if (nQueryStatus == REQUEST_SUCCESS)
        {
            //ui->labelPhoneNumStatus->setText(tr("Mobile phone number available!"));
            ui->labelPhoneNumStatus->setText("");  // 填写正确后需要清空原来信息
            m_bIsPassUser = true;
        }
        else if(nQueryStatus == QUERY_USER_IS_EXIST)
        {
            ui->labelPhoneNumStatus->setText(tr("Mobile phone number has been in existence!"));
        }
        else if(nQueryStatus == REQUEST_CONNECT_FAIL)
        {
            ui->labelPhoneNumStatus->setText(tr("Request connection failure!"));
        }
        else if(nQueryStatus == REQUEST_NO_RESPONSES)
        {
            ui->labelPhoneNumStatus->setText(tr("The request didn't respond!"));
        }
        else
        {
            ui->labelPhoneNumStatus->setText(tr("Unknown Error!"));
        }
    }
    else if(nType == EMAILNAME)
    {
        QString sMailNum = ui->lEMailUserName->text();

        if (sMailNum.isEmpty())
        {
            ui->labelMailNumStatus->setText(tr("Email cannot be empty!"));
            return;
        }
        int nIndex = sMailNum.indexOf('@');
        if (nIndex <= 0 || nIndex >= sMailNum.length() - 1)
        {
            ui->labelMailNumStatus->setText(tr("Email is wrong!"));
            return;
        }
//        int nIndex2 = sMailNum.indexOf('.', nIndex);
//        if (nIndex2 <= nIndex + 1 || nIndex2 >= sMailNum.length() - 1)
//        {
//            ui->labelMailNumStatus->setText(tr("Email is wrong!"));
//            return;
//        }
        int nQueryStatus =m_pLoginLibMgr->QueryUser(sQueryURL, sMailNum,EMAILNAME);

        if (nQueryStatus == REQUEST_SUCCESS)
        {
            ui->labelMailNumStatus->setText(tr(""));
            m_bIsPassUser = true;
        }
        else if(nQueryStatus == QUERY_USER_IS_EXIST)
        {
            ui->labelMailNumStatus->setText(tr("Email has been in existence!"));
        }
        else if(nQueryStatus == REQUEST_CONNECT_FAIL)
        {
            ui->labelMailNumStatus->setText(tr("Request connection failure!"));
        }
        else if(nQueryStatus == REQUEST_NO_RESPONSES)
        {
            ui->labelMailNumStatus->setText(tr("The request didn't respond!"));
        }
        else
        {
            ui->labelMailNumStatus->setText(tr("Unknown Error!"));
        }

    }

}

/**************************************************************************
* 函数名: OnEnKillfocusPassword
* 功能: 验证密码是否有效
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::OnEnKillfocusPassword(int nType)
{
    m_bIsPassPassword = false;

    if(nType == PHONENAME)
    {
        m_userPassword = ui->lEPhonePassword->text();
        if (m_userPassword.isEmpty())
        {
            ui->labelPPasswordStatus->setText(tr("Incorrect password!"));
            return;
        }

        int results = VerifyPasswordCounter(m_userPassword);

        if (results == 1)
        {
            ui->labelPPasswordStatus->setText(tr("Please enter 15 to 6 bit password!"));
        }
        else if (results == 2)
        {
            ui->labelPPasswordStatus->setText(tr("Password may not contain characters other than letters or Numbers!"));
        }
        else if (results == 3)
        {
            ui->labelPPasswordStatus->setText(tr(""));
            m_bIsPassPassword = true;
        }
    }
    else if(nType == EMAILNAME)
    {
        m_userPassword = ui->lEMailPassword->text();
        if (m_userPassword.isEmpty())
        {
            ui->labelMailPasswordStatus->setText(tr("Incorrect password!"));
            return;
        }

        int results = VerifyPasswordCounter(m_userPassword);

        if (results == 1)
        {
            ui->labelMailPasswordStatus->setText(tr("Please enter 15 to 6 bit password!"));
        }
        else if (results == 2)
        {
            ui->labelMailPasswordStatus->setText(tr("Password may not contain characters other than letters or Numbers!"));
        }
        else if (results == 3)
        {
            ui->labelMailPasswordStatus->setText(tr(""));
            m_bIsPassPassword = true;
        }
    }
}

/**************************************************************************
* 函数名:OnPasswordAgain
* 功能:再次输入密码
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::OnPasswordAgain(int nType)
{
    if(!m_bIsPassPassword)
        return;
    if(nType == PHONENAME)
    {
        m_bIsPassPwdAgain = false;
        m_userPasswordAgain = ui->lEPhonePasswordConfirm->text();
        if (m_userPassword.compare(m_userPasswordAgain) == 0)
        {
            ui->labelPConfirmPasswordStatus->setText(tr(""));
            m_bIsPassPwdAgain = true;
        }
        else
        {
            ui->labelPConfirmPasswordStatus->setText(tr("The two passwords are not identical!"));
        }
    }
    else
    {
        m_bIsPassPwdAgain = false;
        m_userPasswordAgain = ui->lEMailPasswordConfirm->text();
        if (m_userPassword.compare(m_userPasswordAgain) == 0)
        {
            ui->labelMailPasswordAgainStatus->setText(tr(""));
            m_bIsPassPwdAgain = true;
        }
        else
        {
            ui->labelMailPasswordAgainStatus->setText(tr("The two passwords are not identical!"));
        }
    }

}

/**************************************************************************
* 函数名:OnEnKillfocusVerifyMsg
* 功能:短信校验
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::OnEnKillfocusVerifyMsg()
{
    m_bIsPassVerify = false;
    do
    {
        QString sRegisterURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/system/verifySms";
        QString UserName = "";
        //m_editPhoneNum.GetWindowText(UserName);
        //QString sPhoneNum = CCODE + UserName;    //中国地区发短信要加86;
        QString sPhoneNum = ui->lEPhoneUserName->text();
        setZoneNum();
        UserName = m_ZoneCode + sPhoneNum;

        QString strCode = ui->lEMessageCode->text();
        if (strCode.isEmpty())
        {
            ui->labelGetMsgCodeStatus->setText(tr("Please Enter Verification code error"));
            break;
        }

        bool bRegisterStatus = false;
        bRegisterStatus = m_pLoginLibMgr->VerifySmsCode(sRegisterURL, UserName, strCode);

        if (bRegisterStatus)
        {
            m_bIsPassVerify = true;
            ui->labelGetMsgCodeStatus->setText(tr(""));
        }
        else
        {
            m_bIsPassVerify = false;
            ui->labelGetMsgCodeStatus->setText(tr("Verification code error"));
        }
    }while(0);
}

/**************************************************************************
* 函数名:InitNationCode
* 功能:初始化国家码
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::InitNationCode()
{
    ui->cBNation->addItems(m_pLoginPage->getDefaultNationPairList());
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
int Widgetregister::VerifyPasswordCounter(QString strPassword)
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
* 函数名:slotOnCurrentNationCodeChanged
* 功能:获取当前国家码
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::slotOnCurrentNationCodeChanged(const QString &strNation)
{
    QString selectCountry = strNation;
    if (!selectCountry.isEmpty())
    {
        ui->lEISO->setEnabled(true);
        const int nNationID = DialogLoginPage::getNationID(selectCountry);
        ui->lEISO->setText("+" +QString::number(nNationID));
        ui->lEISO->setEnabled(false);
        ui->lEPhoneUserName->setFocus();
    }
}

/**************************************************************************
* 函数名:onTabWidgetRegisterCurrentChanged
* 功能:注册方式切换
* 参数:[@int ] index 0、手机注册 1、邮箱注册
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void Widgetregister::onTabWidgetRegisterCurrentChanged(int index)
{
        m_nUserType = (emAccountType) index;
}
