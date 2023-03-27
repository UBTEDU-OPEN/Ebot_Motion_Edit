/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：dialogloginpage.cpp
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块主窗体源文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#include "dialogloginpage.h"
#include "ui_dialogloginpage.h"
#include "dialogusermessageedit.h"
#include <QtNetwork>
#include <QPainter>
#include <QMouseEvent>

#include "widgetlogin.h"
#include "widgetregister.h"
#include "widgetfindpassword.h"
#include "widgetresetpassword.h"

extern int g_nUserID;

/**************************************************************************
* 函数名:
* 功能:
* 参数:
* 返回值:
* 时间: 2016/01/01
* 作者: 周志平
**************************************************************************/
bool CreateLoginUI(ILogin** ppUBXLoginNotify, QWidget* pParent)
{
    DialogLoginPage* pProxy = new DialogLoginPage();
    if(pProxy)
    {
        *ppUBXLoginNotify = (ILogin*)pProxy;
        return true;
    }
    return false;
}

/**************************************************************************
* 函数名:
* 功能:
* 参数:
* 返回值:
* 时间: 2016/01/01
* 作者: 周志平
**************************************************************************/
DialogLoginPage::DialogLoginPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLoginPage)
{
    m_nLoginWay = LOGIN_ERROR;
    ui->setupUi(this);    

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_pWidgetLogin = NULL;
    m_pWidgetRegister = NULL;
    m_pWidgetFindPassword = NULL;
    m_pWidgetResetPassword = NULL;

//    if(g_sServer.isEmpty())
//    {
//        g_sServer = "http://10.10.1.12:8080";
//    }
    LoadConfig();

    initDialog();
    setDefaultStyle();

    m_dragPosition = QPoint(-1, -1);

#ifdef Q_OS_OSX
    ui->pbClose_Win->hide();
    connect(ui->pbClose, &QPushButton::clicked, this, &DialogLoginPage::reject);
#else
    ui->pbClose->hide();
    connect(ui->pbClose_Win, &QPushButton::clicked, this, &DialogLoginPage::reject);
#endif
}

/*************
* 函数名:initDialog
* 功能:初始化对话框
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogLoginPage::initDialog()
{
    m_pLoginLibMgr = new loginLibMgr(this);
    m_pWidgetLogin = new WidgetLogin(this, m_pLoginLibMgr);
    m_pWidgetRegister = new Widgetregister(this,m_pLoginLibMgr);
    m_pWidgetFindPassword = new WidgetFindPassword(this, m_pLoginLibMgr);
    m_pWidgetResetPassword = new WidgetResetPassword(this,m_pLoginLibMgr);

    ui->verticalLayoutPage->addWidget(m_pWidgetLogin);
    ui->verticalLayoutPage->addWidget(m_pWidgetRegister);
    ui->verticalLayoutPage->addWidget(m_pWidgetFindPassword);
    ui->verticalLayoutPage->addWidget(m_pWidgetResetPassword);

    connect(m_pWidgetResetPassword, &WidgetResetPassword::sigUpdateLoginWidget, m_pWidgetLogin, &WidgetLogin::slotUpdateLoginInfo);
    connect(m_pWidgetFindPassword, &WidgetFindPassword::sigUpdateLoginWidget, m_pWidgetLogin, &WidgetLogin::slotUpdateLoginInfo);
    connect(m_pWidgetRegister, &Widgetregister::sigUpdateLoginWidget, m_pWidgetLogin, &WidgetLogin::slotUpdateLoginInfo);
    connect(m_pWidgetFindPassword, &WidgetFindPassword::sigSetCurrentTab, m_pWidgetRegister, &Widgetregister::slotSetCurrentTabIndex);

    //默认是登陆窗口
    ChangeChildWidget(LOGIN_WIDGET);

#ifdef Q_OS_WIN
    ui->horizontalLayout_2->setContentsMargins(10, 6, 10, 0);
#endif
}

/*************
* 函数名:paintEvent
* 功能:重绘事件
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogLoginPage::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/*************
* 函数名:setDefaultStyle
* 功能:设置默认的皮肤风格
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogLoginPage::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/UBXLoginDefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

/**************************************************************************
* 函数名:
* 功能:析构函数
* 参数:
* 返回值:
* 时间: 2016/01/01
* 作者: 周志平
**************************************************************************/
DialogLoginPage::~DialogLoginPage()
{
    delete ui;
}

/*************
* 函数名:Connect
* 功能:连接到对话框
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
bool DialogLoginPage::Connect(QWidget *pParent, ILoginEvent *pNotify)
{
    m_nEvent = pNotify;
    return true;
}

/*************
* 函数名:Disconnect
* 功能:删除连接并销毁对话框
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogLoginPage::Disconnect()
{
    delete this;
}

/*************
* 函数名:getDefaultNationPairList
* 功能:获取国家码列表信息
* 参数:
* 返回值:
* 时间: 2016/07/02
* 作者: hwx
*****************/
QList<QPair<QString, QVariant> > DialogLoginPage::getDefaultNationPairList()
{
    QList<QPair<QString, QVariant> > slstNationCode;
    slstNationCode.clear();

   // slstNationCode.append(qMakePair(tr("UnKnown(+0)"), QString("UnKnown")));

    slstNationCode.append(qMakePair(tr("Angola(+244)"), QString("Angola")));
    slstNationCode.append(qMakePair(tr("Afghanistan(+93)"), QString("Afghanistan")));
    slstNationCode.append(qMakePair(tr("Albania(+355)"), QString("Albania")));
    slstNationCode.append(qMakePair(tr("Algeria(+213)"), QString("Algeria")));
    slstNationCode.append(qMakePair(tr("Andorra(+376)"), QString("Andorra")));

    slstNationCode.append(qMakePair(tr("Anguilla(+1264)"), QString("Anguilla")));
    slstNationCode.append(qMakePair(tr("Antigua and Barbuda(+1268)"), QString("AntiguaAndBarbuda")));
    slstNationCode.append(qMakePair(tr("Argentina(+54)"), QString("Argentina")));
    slstNationCode.append(qMakePair(tr("Armenia(+374)"), QString("Armenia")));
    slstNationCode.append(qMakePair(tr("Ascension(+247)"), QString("Ascension")));

    slstNationCode.append(qMakePair(tr("Australia(+61)"), QString("Australia")));
    slstNationCode.append(qMakePair(tr("Austria(+43)"), QString("Austria")));
    slstNationCode.append(qMakePair(tr("Azerbaijan(+994)"), QString("Azerbaijan")));
    slstNationCode.append(qMakePair(tr("Bahamas(+1242)"), QString("Bahamas")));
    slstNationCode.append(qMakePair(tr("Bahrain(+973)"), QString("Bahrain")));

    slstNationCode.append(qMakePair(tr("Bangladesh(+880)"), QString("Bangladesh")));
    slstNationCode.append(qMakePair(tr("Barbados(+1246)"), QString("Barbados")));
    slstNationCode.append(qMakePair(tr("Belarus(+375)"), QString("Belarus")));
    slstNationCode.append(qMakePair(tr("Belgium(+32)"), QString("Belgium")));
    slstNationCode.append(qMakePair(tr("Belize(+501)"), QString("Belize")));

    slstNationCode.append(qMakePair(tr("Benin(+229)"), QString("Benin")));
    slstNationCode.append(qMakePair(tr("BermudaIs.(+1441)"), QString("BermudaIs")));
    slstNationCode.append(qMakePair(tr("Bolivia(+591)"), QString("Bolivia")));
    slstNationCode.append(qMakePair(tr("Botswana(+267)"), QString("Botswana")));
    slstNationCode.append(qMakePair(tr("Brazil(+55)"), QString("Brazil")));

    slstNationCode.append(qMakePair(tr("Brunei(+673)"), QString("Brunei")));
    slstNationCode.append(qMakePair(tr("Bulgaria(+359)"), QString("Bulgaria")));
    slstNationCode.append(qMakePair(tr("Burkina-faso(+226)"), QString("BurkinaFaso")));
    slstNationCode.append(qMakePair(tr("Burma(+95)"), QString("Burma")));
    slstNationCode.append(qMakePair(tr("Burundi(+257)"), QString("Burundi")));

    slstNationCode.append(qMakePair(tr("Cameroon(+237)"), QString("Cameroon")));
    slstNationCode.append(qMakePair(tr("Canada(+1)"), QString("Canada")));
    slstNationCode.append(qMakePair(tr("Cayman Is.(+1345)"), QString("CaymanIslands")));
    slstNationCode.append(qMakePair(tr("Central African Republic(+236)"), QString("CentralAfricanRepublic")));
    slstNationCode.append(qMakePair(tr("Chad(+235)"), QString("Chad")));

    slstNationCode.append(qMakePair(tr("Chile(+56)"), QString("Chile")));
    slstNationCode.append(qMakePair(tr("China(+86)"), QString("China")));
    slstNationCode.append(qMakePair(tr("Colombia(+57)"), QString("Colombia")));
    slstNationCode.append(qMakePair(tr("Congo(+242)"), QString("Congo")));
    slstNationCode.append(qMakePair(tr("Cook Is.(+682)"), QString("CookIslands")));

    slstNationCode.append(qMakePair(tr("Costa Rica(+506)"), QString("CostaRica")));
    slstNationCode.append(qMakePair(tr("Cuba(+53)"), QString("Cuba")));
    slstNationCode.append(qMakePair(tr("Cyprus(+357)"), QString("Cyprus")));
    slstNationCode.append(qMakePair(tr("Czech Republic(+420)"), QString("CzechRepublic")));
    slstNationCode.append(qMakePair(tr("Denmark(+45)"), QString("Denmark")));

    slstNationCode.append(qMakePair(tr("Djibouti(+253)"), QString("Djibouti")));
    slstNationCode.append(qMakePair(tr("Dominica Rep.(+1890)"), QString("DominicanRepublic")));
    slstNationCode.append(qMakePair(tr("Ecuador(+593)"), QString("Ecuador")));
    slstNationCode.append(qMakePair(tr("Egypt(+20)"), QString("Egypt")));
    slstNationCode.append(qMakePair(tr("EISalvador(+503)"), QString("EISalvador")));

    slstNationCode.append(qMakePair(tr("Estonia(+372)"), QString("Estonia")));
    slstNationCode.append(qMakePair(tr("Ethiopia(+251)"), QString("Ethiopia")));
    slstNationCode.append(qMakePair(tr("Fiji(+679)"), QString("Fiji")));
    slstNationCode.append(qMakePair(tr("Finland(+358)"), QString("Finland")));
    slstNationCode.append(qMakePair(tr("France(+33)"), QString("France")));

    slstNationCode.append(qMakePair(tr("French Guiana(+594)"), QString("FrenchGuiana")));
    slstNationCode.append(qMakePair(tr("Gabon(+241)"), QString("Gabon")));
    slstNationCode.append(qMakePair(tr("Gambia(+220)"), QString("Gambia")));
    slstNationCode.append(qMakePair(tr("Georgia(+995)"), QString("Georgia")));
    slstNationCode.append(qMakePair(tr("Germany(+49)"), QString("Germany")));

    slstNationCode.append(qMakePair(tr("Ghana(+233)"), QString("Ghana")));
    slstNationCode.append(qMakePair(tr("Gibraltar(+350)"), QString("Gibraltar")));
    slstNationCode.append(qMakePair(tr("Greece(+30)"), QString("Greece")));
    slstNationCode.append(qMakePair(tr("Grenada(+1809)"), QString("Grenada")));
    slstNationCode.append(qMakePair(tr("Guam(+1671)"), QString("Guam")));

    slstNationCode.append(qMakePair(tr("Guatemala(+502)"), QString("Guatemala")));
    slstNationCode.append(qMakePair(tr("Guinea(+224)"), QString("Guinea")));
    slstNationCode.append(qMakePair(tr("Guyana(+592)"), QString("Guyana")));
    slstNationCode.append(qMakePair(tr("Haiti(+509)"), QString("Haiti")));
    slstNationCode.append(qMakePair(tr("Honduras(+504)"), QString("Honduras")));

    slstNationCode.append(qMakePair(tr("HongKong(+852)"), QString("HongKong")));
    slstNationCode.append(qMakePair(tr("Hungary(+36)"), QString("Hungary")));
    slstNationCode.append(qMakePair(tr("Iceland(+354)"), QString("Iceland")));
    slstNationCode.append(qMakePair(tr("India(+91)"), QString("India")));
    slstNationCode.append(qMakePair(tr("Indonesia(+62)"), QString("Indonesia")));

    slstNationCode.append(qMakePair(tr("Iran(+98)"), QString("Iran")));
    slstNationCode.append(qMakePair(tr("Iraq(+964)"), QString("Iraq")));
    slstNationCode.append(qMakePair(tr("Ireland(+353)"), QString("Ireland")));
    slstNationCode.append(qMakePair(tr("Israel(+972)"), QString("Israel")));
    slstNationCode.append(qMakePair(tr("Italy(+39)"), QString("Italy")));

    slstNationCode.append(qMakePair(tr("IvoryCoast(+225)"), QString("IvoryCoast")));
    slstNationCode.append(qMakePair(tr("Jamaica(+1876)"), QString("Jamaica")));
    slstNationCode.append(qMakePair(tr("Japan(+81)"), QString("Japan")));
    slstNationCode.append(qMakePair(tr("Jordan(+962)"), QString("Jordan")));
    slstNationCode.append(qMakePair(tr("Kampuchea(+855)"), QString("Kampuchea")));

    slstNationCode.append(qMakePair(tr("Kazakstan(+327)"), QString("Kazakstan")));
    slstNationCode.append(qMakePair(tr("Kenya(+254)"), QString("Kenya")));
    slstNationCode.append(qMakePair(tr("Korea(+82)"), QString("Korea")));
    slstNationCode.append(qMakePair(tr("Kuwait(+965)"), QString("Kuwait")));
    slstNationCode.append(qMakePair(tr("Kyrgyzstan(+331)"), QString("Kyrgyzstan")));

    slstNationCode.append(qMakePair(tr("Laos(+856)"), QString("Laos")));
    slstNationCode.append(qMakePair(tr("Latvia(+371)"), QString("Latvia")));
    slstNationCode.append(qMakePair(tr("Lebanon(+961)"), QString("Lebanon")));
    slstNationCode.append(qMakePair(tr("Lesotho(+266)"), QString("Lesotho")));
    slstNationCode.append(qMakePair(tr("Liberia(+231)"), QString("Liberia")));
    slstNationCode.append(qMakePair(tr("Libya(+218)"), QString("Libya")));

    slstNationCode.append(qMakePair(tr("Liechtenstein(+423)"), QString("Liechtenstein")));
    slstNationCode.append(qMakePair(tr("Lithuania(+370)"), QString("Lithuania")));
    slstNationCode.append(qMakePair(tr("Luxembourg(+352)"), QString("Luxembourg")));
    slstNationCode.append(qMakePair(tr("Macao(+853)"), QString("Macao")));
    slstNationCode.append(qMakePair(tr("Madagascar(+261)"), QString("Madagascar")));

    slstNationCode.append(qMakePair(tr("Malawi(+265)"), QString("Malawi")));
    slstNationCode.append(qMakePair(tr("Malaysia(+60)"), QString("Malaysia")));
    slstNationCode.append(qMakePair(tr("Maldives(+960)"), QString("Maldives")));
    slstNationCode.append(qMakePair(tr("Mali(+223)"), QString("Mali")));
    slstNationCode.append(qMakePair(tr("Malta(+356)"), QString("Malta")));

    slstNationCode.append(qMakePair(tr("Mariana Is.(+1670)"), QString("MarshallIslands")));
    slstNationCode.append(qMakePair(tr("Martinique(+596)"), QString("Martinique")));
    slstNationCode.append(qMakePair(tr("Mauritius(+230)"), QString("Mauritius")));
    slstNationCode.append(qMakePair(tr("Mexico(+52)"), QString("Mexico")));
    slstNationCode.append(qMakePair(tr("Moldova(+373)"), QString("Moldova")));

    slstNationCode.append(qMakePair(tr("Monaco(+377)"), QString("Monaco")));
    slstNationCode.append(qMakePair(tr("Mongolia(+976)"), QString("Mongolia")));
    slstNationCode.append(qMakePair(tr("Montserrat Is(+1664)"), QString("Montserrat")));
    slstNationCode.append(qMakePair(tr("Morocco(+212)"), QString("Morocco")));
    slstNationCode.append(qMakePair(tr("Mozambique(+258)"), QString("Mozambique")));

    slstNationCode.append(qMakePair(tr("Namibia(+264)"), QString("Namibia")));
    slstNationCode.append(qMakePair(tr("Nauru(+674)"), QString("Nauru")));
    slstNationCode.append(qMakePair(tr("Nepal(+977)"), QString("Nepal")));
    slstNationCode.append(qMakePair(tr("Netheriands Antilles(+599)"), QString("Netherlands")));
    slstNationCode.append(qMakePair(tr("Netherlands(+31)"), QString("Netherlands")));

    slstNationCode.append(qMakePair(tr("NewZealand(+64)"), QString("NewZealand")));
    slstNationCode.append(qMakePair(tr("Nicaragua(+505)"), QString("Nicaragua")));
    slstNationCode.append(qMakePair(tr("Niger(+227)"), QString("Niger")));
    slstNationCode.append(qMakePair(tr("Nigeria(+234)"), QString("Nigeria")));
    slstNationCode.append(qMakePair(tr("North Korea(+850)"), QString("NorthKorea")));

    slstNationCode.append(qMakePair(tr("Kazakstan(+327)"), QString("Kazakstan")));
    slstNationCode.append(qMakePair(tr("Kenya(+254)"), QString("Kenya")));
    slstNationCode.append(qMakePair(tr("Korea(+82)"), QString("Korea")));
    slstNationCode.append(qMakePair(tr("Kuwait(+965)"), QString("Kuwait")));
    slstNationCode.append(qMakePair(tr("Kyrgyzstan(+331)"), QString("Kyrgyzstan")));

    slstNationCode.append(qMakePair(tr("Laos(+856)"), QString("Laos")));
    slstNationCode.append(qMakePair(tr("Latvia(+371)"), QString("Latvia")));
    slstNationCode.append(qMakePair(tr("Lebanon(+961)"), QString("Lebanon")));
    slstNationCode.append(qMakePair(tr("Liberia(+231)"), QString("Liberia")));
    slstNationCode.append(qMakePair(tr("Libya(+218)"), QString("Libya")));

    slstNationCode.append(qMakePair(tr("Norway(+47)"), QString("Norway")));
    slstNationCode.append(qMakePair(tr("Oman(+968)"), QString("Oman")));
    slstNationCode.append(qMakePair(tr("Pakistan(+92)"), QString("Pakistan")));
    slstNationCode.append(qMakePair(tr("Panama(+507)"), QString("Panama")));
    slstNationCode.append(qMakePair(tr("Papua New Cuinea(+675)"), QString("PapuaNewGuinea")));
    slstNationCode.append(qMakePair(tr("Paraguay(+595)"), QString("Paraguay")));

    slstNationCode.append(qMakePair(tr("Peru(+51)"), QString("Peru")));
    slstNationCode.append(qMakePair(tr("Philippines(+63)"), QString("Philippines")));
    slstNationCode.append(qMakePair(tr("Poland(+48)"), QString("Poland")));
    slstNationCode.append(qMakePair(tr("French Polynesia(+689)"), QString("FrenchPolynesia")));
    slstNationCode.append(qMakePair(tr("Portugal(+351)"), QString("Portugal")));
    slstNationCode.append(qMakePair(tr("PuertoRico(+1787)"), QString("PuertoRico")));

    slstNationCode.append(qMakePair(tr("Qatar(+974)"), QString("Qatar")));
    slstNationCode.append(qMakePair(tr("Reunion(+262)"), QString("Reunion")));
    slstNationCode.append(qMakePair(tr("Romania(+40)"), QString("Romania")));
    slstNationCode.append(qMakePair(tr("Russia(+7)"), QString("Russia")));
    slstNationCode.append(qMakePair(tr("Saint Lueia(+1758)"), QString("SaintLucia")));
    slstNationCode.append(qMakePair(tr("Saint Vincent(+1784)"), QString("SaintVincentAndTheGrenadines")));

    slstNationCode.append(qMakePair(tr("Samoa Eastern(+684)"), QString("Samoa")));
    slstNationCode.append(qMakePair(tr("Samoa Western(+685)"), QString("Samoa")));
    slstNationCode.append(qMakePair(tr("San Marino(+378)"), QString("SanMarino")));
    slstNationCode.append(qMakePair(tr("Sao Tome and Principe(+239)"), QString("SaoTomeAndPrincipe")));
    slstNationCode.append(qMakePair(tr("Saudi Arabia(+966)"), QString("SaudiArabia")));
    slstNationCode.append(qMakePair(tr("Senegal(+221)"), QString("Senegal")));

    slstNationCode.append(qMakePair(tr("Seychelles(+248)"), QString("Seychelles")));
    slstNationCode.append(qMakePair(tr("Sierra Leone(+232)"), QString("SierraLeone")));
    slstNationCode.append(qMakePair(tr("Singapore(+65)"), QString("Singapore")));
    slstNationCode.append(qMakePair(tr("Slovakia(+421)"), QString("Slovakia")));
    slstNationCode.append(qMakePair(tr("Slovenia(+386)"), QString("Slovenia")));
    slstNationCode.append(qMakePair(tr("Solomon Is.(+677)"), QString("SolomonIslands")));

    slstNationCode.append(qMakePair(tr("Somali(+252)"), QString("Somali")));
    slstNationCode.append(qMakePair(tr("South Africa(+27)"), QString("SouthAfrica")));
    slstNationCode.append(qMakePair(tr("Spain(+34)"), QString("Spain")));
    slstNationCode.append(qMakePair(tr("Sri Lanka(+94)"), QString("SriLanka")));
//    slstNationCode.append(qMakePair(tr("St.Lucia(+1758)"), QString("SaintLucia")));
//    slstNationCode.append(qMakePair(tr("St.Vincent(+1784)"), QString("SaintVincentAndTheGrenadines")));

    slstNationCode.append(qMakePair(tr("Sudan(+249)"), QString("Sudan")));
    slstNationCode.append(qMakePair(tr("Suriname(+597)"), QString("Suriname")));
    slstNationCode.append(qMakePair(tr("Swaziland(+268)"), QString("Swaziland")));
    slstNationCode.append(qMakePair(tr("Sweden(+46)"), QString("Sweden")));
    slstNationCode.append(qMakePair(tr("Switzerland(+41)"), QString("Switzerland")));
    slstNationCode.append(qMakePair(tr("Syria(+963)"), QString("Syria")));

    slstNationCode.append(qMakePair(tr("Taiwan(+886)"), QString("Taiwan")));
    slstNationCode.append(qMakePair(tr("Tajikstan(+992)"), QString("Tajikstan")));
    slstNationCode.append(qMakePair(tr("Tanzania(+255)"), QString("Tanzania")));
    slstNationCode.append(qMakePair(tr("Thailand(+66)"), QString("Thailand")));
    slstNationCode.append(qMakePair(tr("Togo(+228)"), QString("Togo")));
    slstNationCode.append(qMakePair(tr("Tonga(+676)"), QString("Tonga")));

    slstNationCode.append(qMakePair(tr("Trinidad and Tobago(+1809)"), QString("TrinidadAndTobago")));
    slstNationCode.append(qMakePair(tr("Tunisia(+216)"), QString("Tunisia")));
    slstNationCode.append(qMakePair(tr("Turkey(+90)"), QString("Turkey")));
    slstNationCode.append(qMakePair(tr("Turkmenistan(+993)"), QString("Turkmenistan")));
    slstNationCode.append(qMakePair(tr("Uganda(+256)"), QString("Uganda")));
    slstNationCode.append(qMakePair(tr("Ukraine(+380)"), QString("Ukraine")));

    slstNationCode.append(qMakePair(tr("United Arab Emirates(+971)"), QString("UnitedArabEmirates")));
    slstNationCode.append(qMakePair(tr("United Kiongdom(+44)"), QString("UnitedKingdom")));
    slstNationCode.append(qMakePair(tr("United States of America(+1)"), QString("UnitedStates")));
    slstNationCode.append(qMakePair(tr("Uruguay(+598)"), QString("Uruguay")));
    slstNationCode.append(qMakePair(tr("Uzbekistan(+233)"), QString("Uzbekistan")));
    slstNationCode.append(qMakePair(tr("Venezuela(+58)"), QString("Venezuela")));

    slstNationCode.append(qMakePair(tr("Vietnam(+84)"), QString("Vietnam")));
    slstNationCode.append(qMakePair(tr("Yemen(+967)"), QString("Yemen")));
    slstNationCode.append(qMakePair(tr("Yugoslavia(+381)"), QString("Yugoslavia")));
    slstNationCode.append(qMakePair(tr("Zimbabwe(+263)"), QString("Zimbabwe")));
    slstNationCode.append(qMakePair(tr("Zaire(+243)"), QString("Zaire")));
    slstNationCode.append(qMakePair(tr("Zambia(+260)"), QString("Zambia")));

    return slstNationCode;
}

/*************
* 函数名:getNationID
* 功能:从带有国家码ID的信息文本中获取国家码ID
* 参数:strNationTextWithID （as: tr("Angola(+244)"）
* 返回值:
* 时间: 20161008
* 作者: asu
*****************/
int DialogLoginPage::getNationID(const QString &strNationTextWithID)
{
    QString strNationID = strNationTextWithID;
    if(!strNationID.isEmpty())
    {
        strNationID = strNationID.right(strNationID.length()-strNationID.indexOf("(")-1);
        strNationID = strNationID.left(strNationID.length()-1);
        strNationID = strNationID.remove(QChar('+'));
    }

    return strNationID.toInt();
}

/*************
* 函数名:getNationIDByNation
* 功能:获取国家码ID
* 参数:strNationWithOutID （as: "Angola"）
* 返回值:
* 时间: 20161008
* 作者: asu
*****************/
int DialogLoginPage::getNationIDByNation(const QString &strNationWithOutID)
{
    int nNationID = -1;
    QList<QPair<QString, QVariant> > listNation = DialogLoginPage::getDefaultNationPairList();
    for(int i = 0; i < listNation.count(); ++i)
    {
        if(listNation.at(i).second.toString() == strNationWithOutID)
        {
            nNationID = DialogLoginPage::getNationID(listNation.at(i).first);
        }
    }

    return nNationID;
}

/*************
* 函数名:getNationByNationID
* 功能：利用国家码ID获取国家码
* 参数:nNationID
* 返回值:
* 时间: 20161008
* 作者: asu
*****************/
QString DialogLoginPage::getNationByNationID(int nNationID)
{
    QString strNation;
    strNation.clear();

    QList<QPair<QString, QVariant> > listNation = DialogLoginPage::getDefaultNationPairList();
    for(int i = 0; i < listNation.count(); ++i)
    {
        const int nTempNationID = DialogLoginPage::getNationID(listNation.at(i).first);
        if(nTempNationID == nNationID)
        {
            strNation = listNation.at(i).second.toString();
        }
    }

    return strNation;
}

/*************
* 函数名:getDefaultNationText
* 功能:获取默认国家文本，如China
* 参数:
* 返回值:
* 时间: 2016/07/04
* 作者: hwx
*****************/
QString DialogLoginPage::getDefaultNationText()
{
    QLocale locale;
    const QString strCountry = locale.countryToString(locale.country());
    return strCountry;

    //    return QString("China(+86)");
}

/*************
* 函数名:getNationPair
* 功能: 根据国家码获取NationPair
* 参数: strNationCode：国家码
* 返回值:
* 时间: 2016/07/06
* 作者: hwx
*****************/
QPair<QString, QVariant> DialogLoginPage::getNationPair(const QString &strNationCode)
{
    QRegExp o_code_exp("\\s*\\w+\\(\\s*\\+\\s*(.*)\\)\\s*");
    QList<QPair<QString, QVariant> > lstNationPair = getDefaultNationPairList();
    for (int id = 0; id < lstNationPair.size(); id++)
    {
        QPair<QString, QVariant> nationPair = lstNationPair[id];
        QString strNationWithCode = nationPair.first;
        if (-1 == o_code_exp.indexIn(strNationWithCode))
        {
            continue;
        }

        QString strCode = o_code_exp.cap(1).trimmed();
        if (0 == strCode.compare(strNationCode))
        {
            return nationPair;
        }
    }

    return qMakePair(QString(), QString());
}

/*************
* 函数名:GetDialog
* 功能:获取对话框指针
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
QDialog *DialogLoginPage::GetDialog()
{

    return this;
}

/**************************************************************************
* 函数名: UserLoginOut
* 功能: 用户注销登录
* 时间: 2015/10/30 16:37
* 作者: ZDJ
*/
bool DialogLoginPage::UserLoginOut()
{
    bool bLoginOut = false;
    QString strLoginOutURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/user/loginOut";
    bLoginOut = m_pLoginLibMgr->LoginOut(strLoginOutURL);
    return bLoginOut;
}

/**************************************************************************
* 函数名: UserMessageEdit
* 功能: 用户修改个人信息
* 时间: 2015/10/30 16:37
* 作者: ZDJ
*/
bool DialogLoginPage::UserMessageEdit()
{
    bool bRet = false;
    do
    {
        DialogUserMessageEdit messageDlg(NULL, m_pLoginLibMgr);
        connect(&messageDlg, &DialogUserMessageEdit::sigNickNameChanged, this, &DialogLoginPage::slotNickNameChanged);
        if(messageDlg.exec() == QDialog::Accepted)
        {
            emit SigUpdataMessage(g_userinfo.m_sUserAccount, g_userinfo.m_sUserPwd);
            bRet = true;
        }

    }while(0);
    return true;
}

/**************************************************************************
* 函数名: GetUserName
* 功能: 获取用户名
* 时间: 2015/10/30 16:37
* 作者: ZDJ
*/
QString DialogLoginPage::GetUserName()
{
    return m_userNickName;
}

/**************************************************************************
* 函数名: GetUserPwd
* 功能: 获取用户密码
* 时间: 2015/10/30 16:37
* 作者: ZDJ
*/
QString DialogLoginPage::GetUserPwd()
{
    return m_strPassword;
}

/*************
  * 函数名:SetResetWidgetInfo
  * 功能:重置密码窗口的信息设置
  * 参数:
  * 返回值:
  * 时间: 2016/06/01
  * 作者: 周志平
  *****************/
void DialogLoginPage::SetResetWidgetInfo(QString strName, QString strVervifyCode)
{
    if(m_pWidgetResetPassword)
    {
        m_pWidgetResetPassword->SetUserName(strName);
        m_pWidgetResetPassword->SetVerifyCode(strVervifyCode);
    }
}

/************************************
* 名称: GetUserID
* 功能: 获取用户ID
* 返回:   void
* 时间:   2016/04/13
* 作者:   hels
************************************/
int DialogLoginPage::GetUserID()
{
    return g_nUserID;
}

/************************************
* 名称: getLoginWidget
* 功能: 获取登录窗口
* 返回:   void
* 时间:   2016/07/02
* 作者:   hwx
************************************/
WidgetLogin *DialogLoginPage::getLoginWidget()
{
    return m_pWidgetLogin;
}

/************************************
* 名称: getRegisterWidget
* 功能: 获取注册窗口
* 返回:   void
* 时间:   2016/07/02
* 作者:   hwx
************************************/
Widgetregister *DialogLoginPage::getRegisterWidget()
{
    return m_pWidgetRegister;
}

/************************************
* 名称: getFindPasswordWidget
* 功能: 获取找回密码窗口
* 返回:   void
* 时间:   2016/07/02
* 作者:   hwx
************************************/
WidgetFindPassword *DialogLoginPage::getFindPasswordWidget()
{
    return m_pWidgetFindPassword;
}

/************************************
* 名称: getResetPasswordWidget
* 功能: 获取重置密码窗口
* 返回:   void
* 时间:   2016/07/02
* 作者:   hwx
************************************/
WidgetResetPassword *DialogLoginPage::getResetPasswordWidget()
{
    return m_pWidgetResetPassword;
}

/*************
* 函数名:setLoginStatus
* 功能:设置登录状态
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogLoginPage::setLoginStatus(bool bRet)
{
    if(bRet)
    {
        accept();
    }
    else
    {
        reject();
    }
}

/*************
* 函数名:setLoginWay
* 功能:设置登录方式
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogLoginPage::setLoginWay(int nWay)
{
    m_nLoginWay = nWay;
}

/*************
* 函数名:getLoginWay
* 功能:获取登录方式
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
int DialogLoginPage::getLoginWay()
{
    return m_nLoginWay;
}

/*************
* 函数名:ChangeChildWidget
* 功能:根据子窗体的类型显示响应的子窗体
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogLoginPage::ChangeChildWidget(int nType)
{
    switch (nType)
    {
    case LOGIN_WIDGET:
        m_pWidgetRegister->hide();
        m_pWidgetLogin->show();
        m_pWidgetLogin->setDefaultFocus();
        m_pWidgetFindPassword->hide();
        m_pWidgetResetPassword->hide();
        setFixedWidth(366);
        setFixedHeight(388);
        break;
    case REGISTER_WIDGET:
        m_pWidgetLogin->hide();
        m_pWidgetRegister->show();
        m_pWidgetRegister->setDefaultFocus();
        m_pWidgetFindPassword->hide();
        m_pWidgetResetPassword->hide();
        setFixedWidth(400);
        setFixedHeight(490);
//        setFixedWidth(374);
//        setFixedHeight(490);
        break;
    case FINDPASSWORD_WIDGET:
        m_pWidgetFindPassword->SetFindPassWordType(m_nLoginWay);
        m_pWidgetLogin->hide();
        m_pWidgetRegister->hide();
        m_pWidgetFindPassword->show();
        m_pWidgetResetPassword->hide();
        setFixedWidth(366);
        setFixedHeight(388);
        break;
    case RESETPASSWORD_WIDGET:
        m_pWidgetLogin->hide();
        m_pWidgetRegister->hide();
        m_pWidgetFindPassword->hide();
        m_pWidgetResetPassword->show();
        setFixedWidth(366);
        setFixedHeight(388);
        break;
    default:
        break;
    }
}

/*************
* 函数名:slotNickNameChanged
* 功能: 昵称发生变化的槽函数
* 参数:
* 返回值:
* 时间: 2016/06/22
* 作者: hwx
*****************/
void DialogLoginPage::slotNickNameChanged(const QString &strNickName)
{
    if (m_nEvent)
    {
        m_nEvent->setUserNickName(strNickName);
    }
}

/*************
* 函数名:mouseReleaseEvent
* 功能:鼠标离开事件
* 参数:
* 返回值:
* 时间: 2016/06/01
* 作者: 周志平
*****************/
void DialogLoginPage::mouseReleaseEvent(QMouseEvent *event)
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
void DialogLoginPage::mousePressEvent(QMouseEvent *event)
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
void DialogLoginPage::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() &Qt::LeftButton)
    {
        if (m_dragPosition != QPoint(-1, -1))
            move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}
