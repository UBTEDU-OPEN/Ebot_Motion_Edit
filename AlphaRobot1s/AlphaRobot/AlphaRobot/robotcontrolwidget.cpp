/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：robotcontrolwidget.cpp
* 创建时间：2016/08/05
* 文件标识：
* 文件摘要：robot label widget ui
*
* 当前版本：1.0.0.0
* 作    者：asu
* 版本摘要：
*/

#include "robotcontrolwidget.h"
#include "ui_robotcontrolwidget.h"

#include <QMouseEvent>
#include <QTimer>
#include <QPainter>
#include <QDebug>

#include "UBXRobotController.h"
#include "../zxLib/widget/ubxwidget.h"
#include "../zxLib/widget/batteryicon.h"
#include "alpharobotconst.h"
#include "ubxupdateadministrator.h"
#include "../UBXRobotController/Robot/cbluetoothcomandparser.h"
#include "widget/messagebox.h"
#include "../zxLib/widget/cdialogcontainer.h"
#include "setting/cwifiinfowidget.h"
#include "UBXBase.h"

const int DEFAULT_TIME_GET_BATTERY_STATUS  = 3;

CRobotControlWidget::CRobotControlWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::CRobotControlWidget),
    m_mousePressedPos(INT_INVALID_VALUE, INT_INVALID_VALUE),
    m_pParentWidget(parent),
    m_pBtnRecoverGesture(NULL),
    m_pBtnDisconnectRobot(NULL),
    m_pBtnConnectRobot(NULL),
    m_pBtnPowerOff(NULL),
    m_pBtnBattery(NULL),
    m_pBtnRobotHead(NULL),
    m_pBtnRobotWifi(NULL),
    m_pMyAlpha(NULL)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_pBtnRecoverGesture = ui->pbResetRobot;
    m_pBtnDisconnectRobot = ui->pbDisconnectRobot;
    m_pBtnConnectRobot = ui->pbConnectRobot;
    m_pBtnPowerOff = ui->pbPowerOff;
    m_pBtnBattery = ui->pbBattery;
    m_pBtnRobotHead = ui->pbRobotHead;
    m_pBtnRobotWifi = ui->pbRobotWifi;

    connect(m_pBtnRecoverGesture, &QPushButton::clicked, this, &CRobotControlWidget::onResetRobotBodyButtonClicked);
    connect(m_pBtnDisconnectRobot, &QPushButton::clicked, this, &CRobotControlWidget::onDisconnectRobotButtonClicked);
    connect(m_pBtnConnectRobot, &QPushButton::clicked, this, &CRobotControlWidget::onConnectRobotButtonClicked);
    connect(m_pBtnPowerOff, &QPushButton::clicked, this, &CRobotControlWidget::onPowerOffButtonClicked);
    connect(m_pBtnBattery, &QPushButton::clicked, this, &CRobotControlWidget::onBatteryButtonClicked);
    connect(m_pBtnRobotHead, &QPushButton::clicked, this, &CRobotControlWidget::onRobotHeadButtonClicked);
    connect(m_pBtnRobotWifi, &QPushButton::clicked, this, &CRobotControlWidget::onRobotWifiButtonClicked);
    connect(UBXBase::getInstance(), &UBXBase::sigRobotProductTypeChange, this, &CRobotControlWidget::onRobotProductTypeChange);

    setTitleBarHeight(this->height());
    setEnableMove(true);

    ui->pbRobotLabelFrame->setMouseTracking(true);

    ui->pbRobotLabelFrame->installEventFilter(this);
    ui->pbConnectRobot->installEventFilter(this);
    ui->pbDisconnectRobot->installEventFilter(this);
    ui->pbResetRobot->installEventFilter(this);
    ui->pbPowerOff->installEventFilter(this);
    ui->pbRobotHead->installEventFilter(this);
    ui->pbBattery->installEventFilter(this);
    ui->pbRobotWifi->installEventFilter(this);


    ui->pbRobotWifi->setEnabled(false);
    // 目前固件3001读取机器人动作有问题，暂时屏蔽
    ui->pbRobotHead->setEnabled(false);

    // 设置默认样式
    slotOnPortOpenChanged(FALSE);

    this->hide();

    if(g_pRobotController)
    {
        AddSubscribe(g_pRobotController); //添加通信消息订阅
    }

    QTimer::singleShot(DEFAULT_TIME_GET_BATTERY_STATUS, this, &CRobotControlWidget::onGetBatteryStatus);


    m_pBtnRecoverGesture->setPressInterval(1000);
    m_pBtnDisconnectRobot->setPressInterval(2000);
    m_pBtnConnectRobot->setPressInterval(4000);
    m_pBtnPowerOff->setPressInterval(1000);
    m_pBtnRobotHead->setPressInterval(1000);
    m_pBtnRobotWifi->setPressInterval(1000);

    m_pBtnRecoverGesture->enablePressInterval(true);
    m_pBtnDisconnectRobot->enablePressInterval(true);
    m_pBtnConnectRobot->enablePressInterval(true);
    m_pBtnPowerOff->enablePressInterval(true);
    m_pBtnRobotHead->enablePressInterval(true);
    m_pBtnRobotWifi->enablePressInterval(true);

    retranslateUi();
}

CRobotControlWidget::~CRobotControlWidget()
{
    SAFE_DELETE(m_pMyAlpha);
    delete ui;
}

void CRobotControlWidget::retranslateUi()
{
    ui->retranslateUi(this);

    //    m_pBtnRobotHead->setToolTip(tr("My Alpha"));
        m_pBtnRecoverGesture->setToolTip(tr("Reset Robot"));
        m_pBtnDisconnectRobot->setToolTip(tr("Disconnect Robot"));
        m_pBtnConnectRobot->setToolTip(tr("Connect Robot"));
        m_pBtnPowerOff->setToolTip(tr("Power Off"));
    //    m_pBtnBattery->setToolTip(tr("Robot battery"));
        m_pBtnRobotWifi->setToolTip(tr("Robot WIFI"));
}

bool CRobotControlWidget::getBatteryStatus(const char *const pBatteryStatus, int &nVoltageMil, int& nBatteryStatus, int &nQuantity)
{
    const int nPosBatteryCmd = 3;
    const int nPosBatteryVoltage = 4;
    const int nPosBatteryStatus = 6;
    const int nPostBatteryQuantity = 7;

    bool bOk = false;

    if(pBatteryStatus && (eBTCmdGetBatteryStatus == pBatteryStatus[nPosBatteryCmd]))
    {
        int nVolH = pBatteryStatus[nPosBatteryVoltage];
        int nVolL = pBatteryStatus[nPosBatteryVoltage + 1];
        nVoltageMil = nVolH*16 + nVolL;
        nBatteryStatus = pBatteryStatus[nPosBatteryStatus];
        nQuantity = pBatteryStatus[nPostBatteryQuantity];

        bOk = true;
    }

    return bOk;
}

void CRobotControlWidget::onResetRobotBodyButtonClicked()
{
    emit sigResetRobot();
}

void CRobotControlWidget::onDisconnectRobotButtonClicked()
{
    if(g_pRobotController && g_pRobotController->GetPortOpenState())
    {
        emit sigDisconnectRobot();
    }
}

void CRobotControlWidget::onConnectRobotButtonClicked()
{
    if(g_pRobotController /*&& !g_pRobotController->GetPortOpenState()*/)
    {
        emit sigConnectRobot();
    }

//    g_pRobotController->Req_RobotBatteryStatus();
}

void CRobotControlWidget::onPowerOffButtonClicked()
{
    if(QMessageBox::Yes != CMessageBox::question(NULL, tr(""), tr("Alpha will loosen up after power off. Please hold it carefully!"),
                                                 QMessageBox::Cancel|QMessageBox::Yes,
                                                 QMessageBox::Yes))
    {
        return;
    }

    emit sigRobotPowerOff(false);
}

void CRobotControlWidget::onBatteryButtonClicked()
{
//todo
}

void CRobotControlWidget::onRobotHeadButtonClicked()
{
#if 1 // 目前3001固件版本读取机器人列表还有问题，暂时禁用该功能
    if (NULL == m_pMyAlpha)
    {
        m_pMyAlpha = new CMyAlpha(NULL);
        connect(UBXUpdateAdministrator::getInstance(), &UBXUpdateAdministrator::sigOnGetSystemEmbedVersionInfo, m_pMyAlpha, &CMyAlpha::slotOnGetSystemEmbedVersionInfo);
        connect(this, &CRobotControlWidget::sigOnPortOpenChanged, m_pMyAlpha, &CMyAlpha::slotOnPortOpenChanged);
    }
    CMyAlpha::execByAppModal(m_pMyAlpha);
    m_pMyAlpha->resetData();
    m_pMyAlpha->checkSystemEmbedUpdate();
    m_pMyAlpha->requestRobotActionList();
#endif
}

void CRobotControlWidget::onRobotWifiButtonClicked()
{
    CDialogContainer dlgContainer;
    CWifiInfoWidget wifiWdg;
    dlgContainer.exec(&wifiWdg);
}

void CRobotControlWidget::setRobotHeadTip(const QString strTip)
{
    m_pBtnRobotHead->setToolTip(strTip);
}

void CRobotControlWidget::setBattery(int eBatteryStatus, int nBatteryValue)
{
    CBatteryIcon *pBatteryIcon = dynamic_cast<CBatteryIcon *>(m_pBtnBattery);
    if(pBatteryIcon)
    {
        pBatteryIcon->setBattery(eBatteryStatus, nBatteryValue);
    }
}

/**************************************************************************
* 函数名: setButtonState
* 功能: 设置按钮状态
*     * 参数:
*    @[in ] bConnected: 是否连接上机器人标志
*
* 时间: 2017/02/08
* 作者: hwx
*/
void CRobotControlWidget::setButtonState(bool bConnected)
{
    ui->pbRobotHead->setEnabled(bConnected);
    ui->pbConnectRobot->setEnabled(!bConnected);
    ui->pbDisconnectRobot->setEnabled(bConnected);
    ui->pbResetRobot->setEnabled(bConnected);
    ui->pbPowerOff->setEnabled(bConnected);

    setButtonStateWifi(bConnected);
    setButtonStateHead(bConnected);
}

void CRobotControlWidget::setButtonStateWifi(bool bConnected)
{
    emUbtProductType eProductType = UBXBase::getInstance()->getCurrentRobotProductType();
    if(bConnected && g_pRobotController && g_pRobotController->GetPortOpenState() && (eProductType == eUbtProduct1E))
    {
        //g_pRobotController->GetPortOpenState() is not the real time status, maybe is not right
        ui->pbRobotWifi->setEnabled(true);
    }
    else
    {
        ui->pbRobotWifi->setEnabled(false);
    }
}

void CRobotControlWidget::setButtonStateHead(bool bConnected)
{
    emUbtProductType eProductType = UBXBase::getInstance()->getCurrentRobotProductType();
    if(bConnected && g_pRobotController && g_pRobotController->GetPortOpenState() && (eProductType == eUbtProduct1E))
    {
        //g_pRobotController->GetPortOpenState() is not the real time status, maybe is not right
        ui->pbRobotHead->setEnabled(true);
    }
    else
    {
        ui->pbRobotHead->setEnabled(false);
    }
}

void CRobotControlWidget::onRobotProductTypeChange(int eUbtProductTypeOld, int eUbtProductTypeNew)
{
    setButtonStateWifi(true);
    setButtonStateHead(true);
}

void CRobotControlWidget::onGetBatteryStatus()
{
//    g_pRobotController->Req_RobotBatteryStatus();

    //    QTimer::singleShot(DEFAULT_TIME_GET_BATTERY_STATUS, this, &CRobotLabelUI::onGetBatteryStatus);
}

void CRobotControlWidget::OnNotify(const zxObserverSubjectBase *pSubject, int nMsgID,
                             DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    zxObserverBase::OnNotify(this, pSubject, zxEventTypeWindow, nMsgID, dwContext, wParam, lParam);
}

/**************************************************************************
* 函数名: slotOnPortOpenChanged
* 功能: 断开连接机器人状态变化槽函数
*     * 参数:
*    @[in ] bOpen: 是否连接上机器人标志
*
* 时间: 2017/02/08
* 作者: hwx
*/
void CRobotControlWidget::slotOnPortOpenChanged(BOOL bOpen)
{
#if 0
    if (bOpen)
    {
        ui->pbRobotHead->setStyleSheet("QPushButton#pbRobotHead{border-image: url(:/res/images/alpha_connect.png);}");
        ui->pbConnectRobot->setStyleSheet("QPushButton#pbConnectRobot{border-image: url(:/res/images/icon_connect_disable.png);}");
        ui->pbDisconnectRobot->setStyleSheet("QPushButton#pbDisconnectRobot{border-image: url(:/res/images/icon_disconnect_enable.png);}");
        ui->pbResetRobot->setStyleSheet("QPushButton#pbResetRobot{border-image: url(:/res/images/icon_reset_body_enable.png);}");
        ui->pbPowerOff->setStyleSheet("QPushButton#pbPowerOff{border-image: url(:/res/images/icon_poweroff_enable.png);}");
    }
    else
    {
        ui->pbRobotHead->setStyleSheet("QPushButton#pbRobotHead{border-image: url(:/res/images/alpha_disconnect.png);}");
        ui->pbConnectRobot->setStyleSheet("QPushButton#pbConnectRobot{border-image: url(:/res/images/icon_connect_enable.png);}");
        ui->pbDisconnectRobot->setStyleSheet("QPushButton#pbDisconnectRobot{border-image: url(:/res/images/icon_disconnect_disable.png);}");
        ui->pbResetRobot->setStyleSheet("QPushButton#pbResetRobot{border-image: url(:/res/images/reset_body_disable.png);}");
        ui->pbPowerOff->setStyleSheet("QPushButton#pbPowerOff{border-image: url(:/res/images/icon_poweroff_disable.png);}");
    }
#endif
    setButtonState(bOpen);
    emit sigOnPortOpenChanged(bOpen);
}

bool CRobotControlWidget::eventFilter(QObject *obj, QEvent *event)
{
    // 即使点击的是按钮，也能拖动
    if(obj && (ui->pbRobotLabelFrame == obj || ui->pbConnectRobot == obj || ui->pbDisconnectRobot == obj || ui->pbResetRobot == obj
               || ui->pbPowerOff == obj || ui->pbRobotHead == obj || ui->pbBattery == obj  || ui->pbRobotWifi == obj))
    {
        QMouseEvent *eventMouse = dynamic_cast<QMouseEvent *> (event);
        if(eventMouse)
        {
            // 判断是否是拖动全局控制按钮事件
            QPoint mouseReleasedPos = eventMouse->globalPos();
            switch (eventMouse->type()) {
            case QEvent::MouseButtonPress:
                UBXWidget::mousePressEvent(eventMouse);
                m_mousePressedPos = eventMouse->globalPos();
                break;
            case QEvent::MouseButtonRelease:
                UBXWidget::mouseReleaseEvent(eventMouse);

                // 如果超过指定距离，则认为进行的操作是拖动全局控制按钮
                // 此时不要响应按钮事件（由于QPushButton的MouseMove无效!!!!）
                if ((qAbs(mouseReleasedPos.x() - m_mousePressedPos.x()) > MOUSE_MOVE_MIN_OFFSET)
                        || (qAbs(mouseReleasedPos.y() - m_mousePressedPos.y()) > MOUSE_MOVE_MIN_OFFSET))
                {
                    return true;
                }
                break;
            case QEvent::MouseMove:  // !!!!!!!!!!!!!!!!QPushButton的mouseMove事件一直没有触发!!!!!!!
                UBXWidget::mouseMoveEvent(eventMouse);
                break;
            default:
                break;
            }
        }
    }
    return UBXWidget::eventFilter(obj, event);
}

void CRobotControlWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CRobotControlWidget::customEvent(QEvent *event)
{
    zxEvent* pEvent = (zxEvent*)event;
    DWORD nMsgID = pEvent->dwMsgId;
    WPARAM wParam = pEvent->wParam;
    LPARAM lParam = pEvent->lParam;

    //qDebug()<< "WM_MSG_ROBOT_BATTERY_STATUS_GET:"<< WM_MSG_ROBOT_BATTERY_STATUS_GET<<"\n";
    if(WM_MSG_ROBOT_BATTERY_STATUS_GET == nMsgID)
    {
        int nVoltageMil = 0;
        int nQuantity = 0;
        int nStatus = false;

        QString strRobotType((char *) lParam);
        bool bGetOk = CRobotControlWidget::getBatteryStatus((char*)lParam, nVoltageMil, nStatus, nQuantity);

        return;
    }
}

void CRobotControlWidget::mouseMoveEvent(QMouseEvent *event)
{
    UBXWidget::mouseMoveEvent(event);
}
