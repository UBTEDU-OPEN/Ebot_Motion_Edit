/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：widgetlogin.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块登录主窗体源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#include "widgetlogin.h"
#include "ui_widgetlogin.h"
#include "widgetregister.h"

#include "../zxLib/configs.h"
#include "../zxLib/ciniconfig.h"


RETRANSLATEUIFUNCIMPL(WidgetLogin)
WidgetLogin::WidgetLogin(QDialog *parent, loginLibMgr *pLoginLibMgr) :
    UBXWidget(parent),
    ui(new Ui::WidgetLogin)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    m_pLoginPage = (DialogLoginPage*)parent;
    m_pLoginLibMgr = pLoginLibMgr;
    m_sUserName = "";
    m_sPassword = "";
    m_nUserType = PHONENAME;
    m_bIsSave = false;
    m_bIsPassUser = false;
    m_bIsPassPassword = false;

    m_pSimpleCrypt = new SimpleCrypt();
    m_pSimpleCrypt->setKey(0x0c2ad4a4acb9f023);

    setUpWidget();
    initWidget();
}

WidgetLogin::~WidgetLogin()
{
    SAFE_DELETE(m_pSimpleCrypt);
    delete ui;
}

/*************
* 函数名:eventFilter
* 功能:事件过滤器
* 参数:
* 返回值:
* 时间: 2016/06/24
* 作者: hwx
*****************/
bool WidgetLogin::eventFilter(QObject *watched, QEvent *event)
{
    // 暂时不使用事件过滤，使用事件过滤后，当用户使用记住密码方式
    // 登录点击登录按钮时，会不断触发事件，导致需要点击两次才能登录
//    // 失去焦点事件
//    if (QEvent::FocusOut == event->type())
//    {
//        // 手机登录
//        if (PHONENAME == m_nUserType)
//        {
//            if (watched == ui->lEPhoneUserName)
//            {
//                OnEnKillfocusUser(PHONENAME);
//            }
//            else if (watched == ui->lEPhonePassword)
//            {
//                OnEnKillfocusPassword(PHONENAME);
//            }
//        }
//        else if (EMAILNAME == m_nUserType) // 邮箱登录
//        {
//             if (watched == ui->lEMailUserName)
//            {
//                 OnEnKillfocusUser(EMAILNAME);
//            }
//            else if (watched == ui->lEMailPassword)
//            {
//                 OnEnKillfocusPassword(EMAILNAME);
//            }
//        }
//    }

    return QWidget::eventFilter(watched, event);
}

/*************
* 函数名:setDefaultFocus
* 功能:设置默认获得焦点的控件
* 参数:
* 返回值:
* 时间: 2016/07/13
* 作者: hwx
*****************/
void WidgetLogin::setDefaultFocus()
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

/*************
* 函数名:keyPressEvent
* 功能:键盘按键事件
* 参数:
* 返回值:
* 时间: 2016/04/01
* 作者: 周志平
*****************/
void WidgetLogin::keyPressEvent(QKeyEvent *event)
{

    if(event->key() == Qt::Key_Return ||event->key() == Qt::Key_Enter)
    {
        if(m_nUserType == PHONENAME)
        {
            onPhoneLoginClicked();
        }
        else if(m_nUserType == EMAILNAME)
        {
            onMaiLoginClicked();
        }
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

/*************
* 函数名:setUpWidget
* 功能:初始化窗体
* 参数:
* 返回值:
* 时间: 2016/04/01
* 作者: 周志平
*****************/
void WidgetLogin::setUpWidget()
{
    // 设置背景提示
    ui->lEPhoneUserName->setPlaceholderText(tr("Please enter your phone number"));
    ui->lEPhonePassword->setPlaceholderText(tr("Please enter the password"));
    ui->lEMailUserName->setPlaceholderText(tr("Please enter your Emai account"));
    ui->lEMailPassword->setPlaceholderText(tr("Please enter the password"));

    //connect信号槽
    connect(ui->pBPhoneLogin,&QPushButton::clicked,this,&WidgetLogin::onPhoneLoginClicked);
    connect(ui->pBRegisterByPhone,&QPushButton::clicked,this,&WidgetLogin::onBPhoneResgisterClicked);
    connect(ui->pBLostPhonePassword,&QPushButton::clicked,this, &WidgetLogin::onLostPhonePasswordClicked);

    connect(ui->pBMaiLogin,&QPushButton::clicked,this,&WidgetLogin::onMaiLoginClicked);
    connect(ui->pBRegisterByMail,&QPushButton::clicked,this,&WidgetLogin::onMailRegisterClicked);
    connect(ui->pBLostMailPassWord,&QPushButton::clicked,this,&WidgetLogin::onLostMailPassWordClicked);

    connect(ui->tabWidgetLogin,&QTabWidget::currentChanged,this,&WidgetLogin::onLoginCurrentChanged);

    // 为了使Country右对齐，QComboBox没有设置右对齐的方式
    QLineEdit *letNationComboBox  = new QLineEdit(ui->cbxCountryCode);
    const int nWidthLineEdit = ui->cbxCountryCode->width() - ui->cbxCountryCode->iconSize().width();
    letNationComboBox->setMaximumWidth(nWidthLineEdit);
    ui->cbxCountryCode->setLineEdit(letNationComboBox);
    letNationComboBox->setAlignment(Qt::AlignRight);
    letNationComboBox->setEnabled(false);
    letNationComboBox->setFocusPolicy(Qt::NoFocus);
    connect(ui->cbxCountryCode, &QComboBox::editTextChanged, this, &WidgetLogin::slotOnCurrentNationCodeChanged);

    // 注册键盘响应筛选器
    ui->lEPhoneUserName->installEventFilter(this);
    ui->lEPhonePassword->installEventFilter(this);
    ui->lEMailUserName->installEventFilter(this);
    ui->lEMailPassword->installEventFilter(this);

    // 初始化国家码
    ui->cbxCountryCode->addItems(m_pLoginPage->getDefaultNationPairList());

    //新建注册窗口
}

/*************
* 函数名:initWidget
* 功能:初始化窗体
* 参数:
* 返回值:
* 时间: 2016/06/27
* 作者: hwx
*****************/
void WidgetLogin::initWidget()
{
    if (PHONENAME == m_nUserType)
    {
        initPhoneLoginWidget();
    }
    else if (EMAILNAME == m_nUserType)
    {
        initMailLoginWidget();
    }
}

/*************
* 函数名:initPhoneLoginWidget
* 功能:初始化手机登录窗体
* 参数:
* 返回值:
* 时间: 2016/06/27
* 作者: hwx
*****************/
void WidgetLogin::initPhoneLoginWidget()
{
    //读取配置文件用户名和用户类型
    bool bOKUserType = true;
    bOKUserType = CIniConfig::getInstance().getValuePLUserType(m_nUserType);
    CIniConfig::getInstance().getValuePLIsSave(m_bIsSave);    

    if((m_nUserType != PHONENAME && m_nUserType != EMAILNAME) || (false == bOKUserType))
    {
        m_nUserType = PHONENAME;      //如果读取配置信息有误，则默认是手机登陆
    }

    // 国家码
    QString strCountryCode = "";
    CIniConfig::getInstance().getValuePLCountryCode(strCountryCode);
    if (strCountryCode.isEmpty())
    {
        strCountryCode = m_pLoginPage->getDefaultNationText();
    }

    CIniConfig::getInstance().getValuePLUserName(m_sUserName);

    m_sPassword = ""; // 用户密码需要复位，否则在切换tab时会错误使用另一个tab密码
    if (m_bIsSave)
    {
        QString strEncodePassword;
        CIniConfig::getInstance().getValuePLPassword(strEncodePassword);
        m_sPassword = m_pSimpleCrypt->decryptToString(strEncodePassword);
    }

    ui->tabWidgetLogin->setCurrentIndex(0);

    //初始化界面
    ui->cbxCountryCode->setCurrentText(strCountryCode);
    ui->cBSavePPassword->setChecked(m_bIsSave);
    ui->lEPhoneUserName->setText(m_sUserName);
    ui->lEPhonePassword->setText(m_sPassword);
    m_pLoginPage->setLoginWay(PHONE_LOGION);
    //ui->lEISO->setText(tr("+86"));
    //    ui->lEPhonePassword->setText(tr("654321"));
    //    ui->lEMailUserName->setText(tr("993318516@qq.com"));
    //    ui->lEMailPassword->setText(tr("654321"));

//     ui->lEPhoneUserName->setFocus();
}

/*************
* 函数名:initMailLoginWidget
* 功能:初始化邮箱登录窗体
* 参数:
* 返回值:
* 时间: 2016/06/27
* 作者: hwx
*****************/
void WidgetLogin::initMailLoginWidget()
{
    //读取配置文件用户名和用户类型
    bool bOKUserType = true;
    bOKUserType = CIniConfig::getInstance().getValueMLUserType(m_nUserType);

    if((m_nUserType != PHONENAME && m_nUserType != EMAILNAME) || (false == bOKUserType))
    {
        m_nUserType = EMAILNAME;      //如果读取配置信息有误，则默认是邮箱登陆
    }

    CIniConfig::getInstance().getValueMLUserName(m_sUserName);
    CIniConfig::getInstance().getValueMLIsSave(m_bIsSave);

    m_sPassword = ""; // 用户密码需要复位，否则在切换tab时会错误使用另一个tab密码
    if (m_bIsSave)
    {
        QString strEncodePassword;
        CIniConfig::getInstance().getValueMLPassword(strEncodePassword);
        m_sPassword = m_pSimpleCrypt->decryptToString(strEncodePassword);
    }

    ui->tabWidgetLogin->setCurrentIndex(1);

    //初始化界面
    ui->cBSaveMPassword->setChecked(m_bIsSave);
    ui->lEMailUserName->setText(m_sUserName);
    ui->lEMailPassword->setText(m_sPassword);
    ui->lEMailUserName->setFocus();
    m_pLoginPage->setLoginWay(MAIL_LOGIN);

//#ifndef QT_NO_DEBUG
//        ui->lEMailUserName->setText("1@2.com");
//        ui->lEMailPassword->setText("123456");
    //#endif
}

/*************
* 函数名:slotUpdateLoginInfo
* 功能:设置用户信息
* 参数:strUserName：用户名，可能是邮箱或手机号码
* 参数：strPassword：用户登录密码
* 返回值:
* 时间: 2016/06/27
* 作者: hwx
*****************/
void WidgetLogin::slotUpdateLoginInfo(const QString &strUserName, const QString &strPassword, emAccountType eAccountType)
{
    onLoginCurrentChanged(eAccountType);
    if (PHONENAME == m_nUserType)
    {
        QString strCountry = m_pLoginPage->getNationPair(m_ZoneCode).second.toString();
        ui->cbxCountryCode->setCurrentText(strCountry);
        ui->lEPhoneUserName->setText(strUserName) ;
        ui->lEPhonePassword->setText(strPassword);
    }
    else if (EMAILNAME == m_nUserType)
    {
        ui->lEMailUserName->setText(strUserName);
        ui->lEMailPassword->setText(strPassword);
    }
}

/**************************************************************************
* 函数名:UpdateCfgContent
* 功能:更新配置文件
* 参数:int nType 用户类型
* 返回值:无
* 时间:2016/03/15 15:40
* 作者: 周志平
* 改动日志
**************************************************************************/
void WidgetLogin::UpdateCfgContent(int nType)
{
    //读取配置文件用户名和用户类型
    int bRemember = false;
    do
    {
        if(nType == PHONENAME)
        {
            // 国家码始终记住
            QString strCountry = m_pLoginPage->getNationPair(m_ZoneCode).second.toString();
            CIniConfig::getInstance().setValuePLCountryCode(strCountry);

            // 用户名始终记住            
            CIniConfig::getInstance().setValuePLUserName(ui->lEPhoneUserName->text().trimmed());

            if(ui->cBSavePPassword->checkState() == Qt::Checked)
            {
                QString strOriginalPassword = ui->lEPhonePassword->text();
                QString strEncodePassword = m_pSimpleCrypt->encryptToString(strOriginalPassword);

                CIniConfig::getInstance().setValuePLPassword(strEncodePassword);
                CIniConfig::getInstance().setValuePLUserType(PHONENAME);
                CIniConfig::getInstance().setValuePLIsSave(true);

                bRemember = true;
                break;
            }
        }
        else if(nType == EMAILNAME)
        {
            // 用户名始终记住
            CIniConfig::getInstance().setValueMLUserName(ui->lEMailUserName->text().trimmed());
            if(ui->cBSaveMPassword->checkState() == Qt::Checked)
            {
                QString strOriginalPassword = ui->lEMailPassword->text();
                QString strEncodePassword = m_pSimpleCrypt->encryptToString(strOriginalPassword);

                CIniConfig::getInstance().setValueMLPassword(strEncodePassword);
                CIniConfig::getInstance().setValueMLUserType(EMAILNAME);
                CIniConfig::getInstance().setValueMLIsSave(true);

                bRemember = true;
                break;
            }
        }

    }while(0);

    if(!bRemember)
    {
        //pIniSet->setValue(sUserNameKeyPath, "");  // 用户名始终需要记住

        if(PHONENAME == nType)
        {
            CIniConfig::getInstance().setValuePLPassword("");
            CIniConfig::getInstance().setValuePLUserType(0);
            CIniConfig::getInstance().setValuePLIsSave(false);
        }
        else if (EMAILNAME == nType)
        {
            CIniConfig::getInstance().setValueMLPassword("");
            CIniConfig::getInstance().setValueMLUserType(0);
            CIniConfig::getInstance().setValueMLIsSave(false);
        }
    }
}

/*************
* 函数名:on_UpdateMessage
* 功能:更新信息
* 参数:strName 用户名
* 参数:strPassword 密码
* 返回值:
* 时间: 2016/04/01
* 作者: 周志平
*****************/
void WidgetLogin::on_UpdateMessage(QString strName, QString strPassword)
{
    if(m_pLoginPage->getLoginWay() == PHONE_LOGION)
    {
        ui->lEPhoneUserName->setText(strName);
        ui->lEPhonePassword->setText(strPassword);

    }
    else if(m_pLoginPage->getLoginWay() == MAIL_LOGIN)
    {
        ui->lEMailUserName->setText(strName);
        ui->lEMailPassword->setText(strPassword);
    }
}

/*************
* 函数名:onPhoneLoginClicked
* 功能:手机登录
* 参数:
* 返回值:
* 时间: 2016/04/01
* 作者: 周志平
*****************/
void WidgetLogin::onPhoneLoginClicked()
{
    bool bRet = false;

    // 虽然在控件失去焦点时已经做过判断，但是在这里
    // 再次判断还是有必要的
    // 1、用户可能使用了记住密码方式，此时如果m_bIsPassUser/m_bIsPassPassword都是false
    // 2、考虑用户第一次登陆，但是账号密码什么都不填写直接点击登录得情况
    OnEnKillfocusUser(PHONENAME);
    OnEnKillfocusPassword(PHONENAME);
    if(!m_pLoginPage)
    {
        return;
    }
    if(!m_bIsPassUser)
    {
        return;
    }
    if(!m_bIsPassPassword)
    {
        return;
    }
    setEnabled(false);
    m_pLoginPage->setEnabled(false);
    g_crisec.Lock();
    do
    {
        QString strLoginURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/login";
        int userID = 0;
        int nLoginStatus = 0;
        QString strPhoneNum = ui->lEPhoneUserName->text();
        QString strPassword = ui->lEPhonePassword->text();
        //手机号码验证
        if(strPhoneNum.isEmpty())
        {
            break;
        }
        QString strISOCode = ui->lEISO->text().remove(QChar('+'));
        QString strUserName = strISOCode + strPhoneNum;
        //密码验证
        if(strPassword.isEmpty())
        {
            break;
        }

        nLoginStatus = m_pLoginLibMgr->LoginP(strLoginURL, strISOCode, strUserName, strPassword, m_userGender, userID);
        if(nLoginStatus == REQUEST_SUCCESS)
        {
            bRet = true;
            AlphaRobotLog::getInstance()->printDebug(tr("login success!"));
            //ui->labelPLoginInfo->setText(tr("login success!"));
        }
        else if(nLoginStatus == REQUEST_CONNECT_FAIL)
        {
            ui->labelPLoginInfo->setText(tr("Request connection failure!"));
        }
        else if(nLoginStatus == REQUEST_NO_RESPONSES)
        {
            ui->labelPLoginInfo->setText(tr("The request didn't respond!"));
        }
        else if(nLoginStatus == USERNAMNE_OR_PASSWORD_ERROR)
        {
            ui->labelPLoginInfo->setText(tr("Password error!"));//用户名是否正确在前面判断过了
        }
        else
        {
            ui->labelPLoginInfo->setText(tr("Unknown Error!"));
        }

    }while(0);
    g_crisec.Unlock();
    setEnabled(true);
    m_pLoginPage->setEnabled(true);
    if(bRet)
    {
        m_pLoginPage->setLoginStatus(true);
        UpdateCfgContent(PHONENAME);
    }
}

/**************************************************************************
* 函数名:onMaiLoginClicked
* 功能:邮箱登录
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void WidgetLogin::onMaiLoginClicked()
{
    bool bRet = false;

    // 虽然在控件失去焦点时已经做过判断，但是在这里
    // 再次判断还是有必要的
    // 1、用户可能使用了记住密码方式，此时如果m_bIsPassUser/m_bIsPassPassword都是false
    // 2、考虑用户第一次登陆，但是账号密码什么都不填写直接点击登录得情况
    OnEnKillfocusUser(EMAILNAME);
    OnEnKillfocusPassword(EMAILNAME);
    if(!m_pLoginPage)
    {
        return;
    }
    setEnabled(false);
    m_pLoginPage->setEnabled(false);
    g_crisec.Lock();
    do
    {
        QString strLoginURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/login";
        int userID = 0;
        int nLoginStatus = 0;
        QString strMailName = ui->lEMailUserName->text();
        QString strMailPassword = ui->lEMailPassword->text();
        //邮箱验证
        if(strMailName.isEmpty())
        {
            break;
        }
        //密码验证
        if(strMailPassword.isEmpty())
        {
            break;
        }

        nLoginStatus = m_pLoginLibMgr->LoginMail(strLoginURL, strMailName, strMailPassword, m_userGender, userID);

        if(nLoginStatus == REQUEST_SUCCESS)
        {
            bRet = true;
            ui->labelMLoginInfo->setText(tr("login success!"));
        }
        else if(nLoginStatus == REQUEST_CONNECT_FAIL)
        {
            ui->labelMLoginInfo->setText(tr("Request connection failure!"));
        }
        else if(nLoginStatus == REQUEST_NO_RESPONSES)
        {
            ui->labelMLoginInfo->setText(tr("The request didn't respond!"));
        }
        else if(nLoginStatus == USERNAMNE_OR_PASSWORD_ERROR)
        {
            ui->labelMLoginInfo->setText(tr("Password error!"));//用户名是否正确在前面判断过了
        }
        else
        {
            ui->labelPLoginInfo->setText(tr("Unknown Error!"));
        }
    }while(0);

    g_crisec.Unlock();
    setEnabled(true);
    m_pLoginPage->setEnabled(true);

    if(bRet)
    {
        m_pLoginPage->setLoginStatus(true);
        UpdateCfgContent(EMAILNAME);
    }
}

/**************************************************************************
* 函数名:onLoginCurrentChanged
* 功能:用户登录方式切换
* 参数:index  0、手机登录 1、邮箱登录
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void WidgetLogin::onLoginCurrentChanged(int index)
{
    if(m_pLoginPage)
    {
        switch (index)
        {
        case PHONENAME:
            m_nUserType = PHONENAME;
            m_pLoginPage->setLoginWay(PHONE_LOGION);
            initPhoneLoginWidget();
            break;
        case EMAILNAME:
            m_nUserType = EMAILNAME;
            m_pLoginPage->setLoginWay(MAIL_LOGIN);
            initMailLoginWidget();
            break;
        default:
            m_nUserType = USERNAME;
            initPhoneLoginWidget();
            break;
        }
    }
}

/**************************************************************************
* 函数名:onBPhoneResgisterClicked
* 功能:手机用户注册
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void WidgetLogin::onBPhoneResgisterClicked()
{
    if(m_pLoginPage)
    {
        m_pLoginPage->ChangeChildWidget(REGISTER_WIDGET);
        m_pLoginPage->getRegisterWidget()->setCurrentRegisterTabIndex(PHONENAME);
    }
}

/**************************************************************************
* 函数名:onMailRegisterClicked
* 功能:邮箱用户注册
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void WidgetLogin::onMailRegisterClicked()
{
    if(m_pLoginPage)
    {
        m_pLoginPage->ChangeChildWidget(REGISTER_WIDGET);
        m_pLoginPage->getRegisterWidget()->setCurrentRegisterTabIndex(EMAILNAME);
    }
}

/**************************************************************************
* 函数名:onLostMailPassWordClicked
* 功能:邮箱用户找回密码
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void WidgetLogin::onLostMailPassWordClicked()
{
    if(m_pLoginPage)
    {
        m_pLoginPage->ChangeChildWidget(FINDPASSWORD_WIDGET);
    }
    //    DialogForgetPassword passwordDig(NULL, m_pLoginLibMgr ,EMAILNAME);
    //    if(passwordDig.exec() == QDialog::Accepted)
    //    {
    //        ui->lEMailUserName->setText(g_userinfo.m_sUserAccount);
    //        ui->lEMailPassword->setText(g_userinfo.m_sUserPwd);
    //    }
}

/**************************************************************************
* 函数名:onLostPhonePasswordClicked
* 功能:手机用户找回密码
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void WidgetLogin::onLostPhonePasswordClicked()
{
    m_ZoneCode = ui->lEISO->text().remove(QChar('+'));
    if(m_pLoginPage)
    {
        m_pLoginPage->ChangeChildWidget(FINDPASSWORD_WIDGET);
    }
}

/**************************************************************************
* 函数名:slotOnCurrentNationCodeChanged
* 功能:国家码变化的槽函数
* 参数:
* 返回值:
* 时间:2016/07/04
* 作者: hwx
* 改动日志
**************************************************************************/
void WidgetLogin::slotOnCurrentNationCodeChanged(const QString &strNation)
{
    QString selectCountry = strNation;
    if (!selectCountry.isEmpty())
    {
        ui->lEISO->setEnabled(true);        
        const int nNationID = DialogLoginPage::getNationID(selectCountry);
        ui->lEISO->setText("+" + QString::number(nNationID));
        ui->lEISO->setEnabled(false);
    }
}

/**************************************************************************
* 函数名: OnEnKillfocusUser
* 功能:验证用户名是否合法
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void WidgetLogin::OnEnKillfocusUser(int nType)
{
    m_bIsPassUser = false;
    if(!m_pLoginPage)
    {
        return;
    }
    QString sQueryURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/check";

    if(nType == PHONENAME)
    {
        QString sPhoneNum = ui->lEPhoneUserName->text();
        QString sZoneCode = ui->lEISO->text();
        if(sZoneCode.isEmpty())
        {
            ui->labelPUserInfo->setText(tr("Country Code cant't be empty"));
            return;
        }
//        if(sZoneCode.length()>5)
//        {
//            ui->labelPUserInfo->setText(tr("Country Code is wrong"));
//            return;
//        }
//        暂时不判断国家码如何正确，没有加列表
        QString sZoneNum = sZoneCode.remove(QChar('+'));
        QString sPhoneName = sZoneNum + sPhoneNum;

        if (sPhoneNum.isEmpty())
        {
            ui->labelPUserInfo->setText(tr("Mobile phone cant't be empty!"));
            return;
        }

//        if (sPhoneNum.length() != 11)
//        {
//            ui->labelPUserInfo->setText(tr("Mobile phone num is wrong!"));
//            return;
//        }

        int nQueryStatus =m_pLoginLibMgr->QueryUser(sQueryURL, sPhoneName,PHONENAME);

        if (nQueryStatus == REQUEST_SUCCESS)
        {
            ui->labelPUserInfo->setText(tr("Mobile phone number isn't be registered!"));
        }
        else if(nQueryStatus == QUERY_USER_IS_EXIST)
        {
           // ui->labelPUserInfo->setText(tr("Mobile phone number available!"));
            ui->labelPUserInfo->setText(""); // 如果填写没问题，不提示任何信息（如果原来提示了错误，需要清空）
            m_bIsPassUser = true;
        }
        else if(nQueryStatus == REQUEST_CONNECT_FAIL)
        {
            ui->labelPUserInfo->setText(tr("Request connection failure!"));
        }
        else if(nQueryStatus == REQUEST_NO_RESPONSES)
        {
            ui->labelPUserInfo->setText(tr("The request didn't respond!"));
        }
        else
        {
            ui->labelPUserInfo->setText(tr("Unknown Error!"));
        }
    }
    else if(nType == EMAILNAME)
    {
        QString sMailNum = ui->lEMailUserName->text();

        if (sMailNum.isEmpty())
        {
            ui->labelMUserInfo->setText(tr("Email cannot be empty!"));
            return;
        }
        int nIndex = sMailNum.indexOf('@');
        if (nIndex <= 0 || nIndex >= sMailNum.length() - 1)
        {
            ui->labelMUserInfo->setText(tr("Email is wrong!"));
            return;
        }
//        int nIndex2 = sMailNum.indexOf('.');
//        if (nIndex2 <= nIndex + 1 || nIndex2 >= sMailNum.length() - 1)
//        {
//            ui->labelMUserInfo->setText(tr("Email is wrong!"));
//            return;
//        }
        int nQueryStatus =m_pLoginLibMgr->QueryUser(sQueryURL, sMailNum,EMAILNAME);

        if (nQueryStatus == REQUEST_SUCCESS)
        {
            ui->labelMUserInfo->setText(tr("Email isn't be registered!"));
        }
        else if(nQueryStatus == QUERY_USER_IS_EXIST)
        {
            //ui->labelMUserInfo->setText(tr("Email available!"));
            ui->labelMUserInfo->setText(""); // 如果填写没问题，不提示任何信息（如果原来提示了错误，需要清空）
            m_bIsPassUser = true;
        }
        else if(nQueryStatus == REQUEST_CONNECT_FAIL)
        {
            ui->labelMUserInfo->setText(tr("Request connection failure!"));
        }
        else if(nQueryStatus == REQUEST_NO_RESPONSES)
        {
            ui->labelMUserInfo->setText(tr("The request didn't respond!"));
        }
        else
        {
            ui->labelMUserInfo->setText(tr("Unknown Error!"));
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
void WidgetLogin::OnEnKillfocusPassword(int nType)
{
    m_bIsPassPassword = false;
    QString  sUserPassword = "";
    if(nType == PHONENAME)
    {
        sUserPassword = ui->lEPhonePassword->text();
        if (sUserPassword.isEmpty())
        {
            ui->labelPPassWordInfo->setText(tr("Incorrect password!"));
            return;
        }

        int results = VerifyPasswordCounter(sUserPassword);

        if (results == 1)
        {
            ui->labelPPassWordInfo->setText(tr("Please enter 15 to 6 bit password!"));
        }
        else if (results == 2)
        {
            ui->labelPPassWordInfo->setText(tr("Password may not contain characters other than letters or Numbers!"));
        }
        else if (results == 3)
        {
            //ui->labelPPassWordInfo->setText(tr("The password can be used!"));
             ui->labelPPassWordInfo->setText(""); // 如果填写没问题，不提示任何信息（如果原来提示了错误，需要清空）
            m_bIsPassPassword = true;
        }
    }
    else if(nType == EMAILNAME)
    {
        sUserPassword = ui->lEMailPassword->text();
        if (sUserPassword.isEmpty())
        {
            ui->labelMPasswordInfo->setText(tr("Incorrect password!"));
            return;
        }

        int results = VerifyPasswordCounter(sUserPassword);

        if (results == 1)
        {
            ui->labelMPasswordInfo->setText(tr("Please enter 15 to 6 bit password!"));
        }
        else if (results == 2)
        {
            ui->labelMPasswordInfo->setText(tr("Password may not contain characters other than letters or Numbers!"));
        }
        else if (results == 3)
        {
            //ui->labelMPasswordInfo->setText(tr("The password can be used!"));
            ui->labelMPasswordInfo->setText(""); // 如果填写没问题，不提示任何信息（如果原来提示了错误，需要清空）
            m_bIsPassPassword = true;
        }
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
int WidgetLogin::VerifyPasswordCounter(QString strPassword)
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
* 函数名:setZoneNum
* 功能:设置国家码
* 参数:
* 返回值:
* 时间:2016/04/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void WidgetLogin::setZoneNum()
{
    QString sZoneNum = ui->lEISO->text().remove(QChar('+'));
    if (sZoneNum.isEmpty())
    {
        sZoneNum = "86";
    }
    m_ZoneCode = sZoneNum;
}
