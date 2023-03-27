#include "ubxrobot3dcontrolwidget.h"
#include "ui_ubxrobot3dcontrolwidget.h"

#include <QDir>
#include <QTimer>


#include "stdafx.h"
#include "UBXRobotController.h"
#include "ubxrobot3dconstants.h"
#include "ubt3dcontroler.h"
#include "../zxLib/configs.h"
#include "robotDefine.h"
#include "../UBXBase/UBXBase.h"

QMap<QString, int> UBXRobot3DControlWidget::groupNameID;

RETRANSLATEUIFUNCIMPL(UBXRobot3DControlWidget)

UBXRobot3DControlWidget::UBXRobot3DControlWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::UBXRobot3DControlWidget),
    m_eReadAngleType(eReadAngleAll),
    m_bReadAngleUpdateUI(true),
    m_nConnectTimerId(0),
    m_nDisConnectTimerId(0),
    m_eTypeAngleChanged(TAC_3DModel)
{
    ui->setupUi(this);
    m_nServoCount = getMotorCount();

    ui->labelIDOperationValue->setText("");
    //将radiobutton加入组，ID对应舵机ID
    m_btnGroup.addButton(ui->btnMotor1, 1);
    m_btnGroup.addButton(ui->btnMotor2, 2);
    m_btnGroup.addButton(ui->btnMotor3, 3);
    m_btnGroup.addButton(ui->btnMotor4, 4);
    m_btnGroup.addButton(ui->btnMotor5, 5);
    m_btnGroup.addButton(ui->btnMotor6, 6);
    m_btnGroup.addButton(ui->btnMotor7, 7);
    m_btnGroup.addButton(ui->btnMotor8, 8);
    m_btnGroup.addButton(ui->btnMotor9, 9);
    m_btnGroup.addButton(ui->btnMotor10, 10);
    m_btnGroup.addButton(ui->btnMotor11, 11);
    m_btnGroup.addButton(ui->btnMotor12, 12);
    m_btnGroup.addButton(ui->btnMotor13, 13);
    m_btnGroup.addButton(ui->btnMotor14, 14);
    m_btnGroup.addButton(ui->btnMotor15, 15);
    m_btnGroup.addButton(ui->btnMotor16, 16);

    groupNameID.insert("righthand3", 1);//1号舵机组名
    groupNameID.insert("righthand2", 2);//2号舵机组名
    groupNameID.insert("righthand1", 3);//3号舵机组名
    groupNameID.insert("lefthand3", 4);//4号舵机组名
    groupNameID.insert("lefthand2", 5);//5号舵机组名
    groupNameID.insert("lefthand1", 6);//6号舵机组名
    groupNameID.insert("rightleg5", 7);//7号舵机组名
    groupNameID.insert("rightleg4", 8);//8号舵机组名
    groupNameID.insert("rightleg3", 9);//9号舵机组名
    groupNameID.insert("rightleg2", 10);//10号舵机组名
    groupNameID.insert("rightleg1", 11);//11号舵机组名
    groupNameID.insert("leftleg5", 12);//12号舵机组名
    groupNameID.insert("leftleg4", 13);//13号舵机组名
    groupNameID.insert("leftleg3", 14);//14号舵机组名
    groupNameID.insert("leftleg2", 15);//15号舵机组名
    groupNameID.insert("liftleg1", 16);//16号舵机组名

    m_mapNodeRadios.insert(1, ui->btnMotor1);
    m_mapNodeRadios.insert(2, ui->btnMotor2);
    m_mapNodeRadios.insert(3, ui->btnMotor3);
    m_mapNodeRadios.insert(4, ui->btnMotor4);
    m_mapNodeRadios.insert(5, ui->btnMotor5);
    m_mapNodeRadios.insert(6, ui->btnMotor6);
    m_mapNodeRadios.insert(7, ui->btnMotor7);
    m_mapNodeRadios.insert(8, ui->btnMotor8);
    m_mapNodeRadios.insert(9, ui->btnMotor9);
    m_mapNodeRadios.insert(10, ui->btnMotor10);
    m_mapNodeRadios.insert(11, ui->btnMotor11);
    m_mapNodeRadios.insert(12, ui->btnMotor12);
    m_mapNodeRadios.insert(13, ui->btnMotor13);
    m_mapNodeRadios.insert(14, ui->btnMotor14);
    m_mapNodeRadios.insert(15, ui->btnMotor15);
    m_mapNodeRadios.insert(16, ui->btnMotor16);

    connect(ui->btnMotor1, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor2, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor3, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor4, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor5, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor6, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor7, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor8, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor9, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor10, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor12, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor13, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor14, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor15, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);
    connect(ui->btnMotor16, &UBXPushButton::sigOnMouseDoubleClicked, this, &UBXRobot3DControlWidget::onPowerOffCurrentNode);

    connect(&m_btnGroup,static_cast<void (QButtonGroup:: *)(int, bool)>(&QButtonGroup::buttonToggled),
            this, &UBXRobot3DControlWidget::onButtonToggled);
//    connect(ui->horizontalSliderAngle, &QSlider::sliderPressed,
//            this, &UBXRobot3DControlWidget::onSliderPressed);
//    connect(ui->horizontalSliderAngle, &QSlider::sliderMoved,
//            this, &UBXRobot3DControlWidget::onSliderMoved);

    ui->horizontalSliderAngle->setMaximum(UBXBase::getInstance()->getDefaultActAngleValueMax(1));
    connect(ui->horizontalSliderAngle, &QSlider::valueChanged,
            this, &UBXRobot3DControlWidget::onSliderAnglevalueChanged);

    ui->horizontalLayout_10->setAlignment(ui->widgetAlpha, Qt::AlignCenter);
    m_pLabelSliderNum = new QLabel(ui->widgetOpContainer);
    m_pLabelSliderNum->setAlignment(Qt::AlignCenter);
    m_pLabelSliderNum->setGeometry(0,0,30,16);
    m_pLabelSliderNum->setObjectName("labelSliderNum");
    m_pLabelSliderNum->hide();


    ui->pushButtonHandReadBack->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->pushButtonReadAll->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->pushButtonLegReadback->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //读取3D模型配置文件
    ReadDefaultAngle(m_vecDefaultMotorDataList);

    if(g_pRobotController)
    {
        AddSubscribe(g_pRobotController); //添加通信消息订阅
        g_pRobotController->Req_Connect(); //连接机器人
    }

    m_pChangedAngleProcessTimer = NULL;
    m_bIsReading = false;

    EnableCtrls(false);
}

UBXRobot3DControlWidget::~UBXRobot3DControlWidget()
{
    if(g_pRobotController)
    {
        if(g_pRobotController->GetPortOpenState())
        {
            g_pRobotController->Req_Disconnect();
            //g_pRobotController->Req_ClosePort();
        }

        RemoveSubscribe(g_pRobotController); //移除通信消息订阅
        //SAFE_DELETE(g_pRobotController);
    }

    CUtil::ReleaseVector(m_vecDefaultMotorDataList);

    SAFE_DELETE(m_pChangedAngleProcessTimer);

    delete ui;
}

void UBXRobot3DControlWidget::UpdateNodeDate(const char *szNodeName, float angle, float offset, TypeAngleChanged nChangeType)
{
    if(false == this->isEnabled())
        return;

    int nGroupID = -1;
    if(groupNameID.contains(szNodeName))
    {
        nGroupID = groupNameID[QString(szNodeName)];
    }

    if(nGroupID >=1 && nGroupID <= Constants::SERVO_COUNT)
    {
        m_eTypeAngleChanged = nChangeType;

        int angleAdjusted = angle - offset;
        //m_mapAngleLabels[nGroupID]->setText(QString::number(angleAdjusted));

        m_bIsReading = false;
        if(nChangeType == TAC_3DModel)
        {
            if(!m_mapNodeRadios[nGroupID]->isChecked())
            {
                m_mapNodeRadios[nGroupID]->setChecked(true);
            }
            if(m_btnGroup.checkedId() == nGroupID) //有选中才设置operation id
            { 
                QString strIDOperationValue = m_btnGroup.checkedButton()->text();
                ui->labelIDOperationValue->setText(strIDOperationValue);
                ui->horizontalSliderAngle->setMaximum(UBXBase::getInstance()->getDefaultActAngleValueMax(strIDOperationValue.toInt()));

                if(ui->horizontalSliderAngle->value() == angleAdjusted)
                {
                    return;
                }

                const int nOldNodeValue = ui->horizontalSliderAngle->value();
                int nFixValue = 0;
                bool bCanSet = Ubt3DControler::instance()->getFitedNodeValue(nGroupID, nOldNodeValue, angleAdjusted, nFixValue);
                if(!bCanSet)
                {
                    angleAdjusted = nFixValue;
                }

//                if(ui->horizontalSliderAngle->value() != angleAdjusted)
                {
                    ui->horizontalSliderAngle->setValue(angleAdjusted);
                    m_pLabelSliderNum->setText(QString::number(angleAdjusted));

                    bool bEmitToUpdate3DDisplayView = !bCanSet;
                    //changeRobotAngle(bEmitToUpdate3DDisplayView, 200);
                    startChangedAngleProcessTimer(200, bEmitToUpdate3DDisplayView);
                }
            }
        }
        else
        {
            if(m_btnGroup.checkedId() == nGroupID/* && angleAdjusted != ui->horizontalSliderAngle->value()*/) //有选中才设置operation id
            {
                if(nChangeType != TAC_ControlView)
                {
                    if(nChangeType == TAC_ReadBack)
                    {
                        m_bIsReading = true;
                    }

                    ui->horizontalSliderAngle->setValue(angleAdjusted);
                    m_pLabelSliderNum->setText(QString::number(angleAdjusted));
                }
                else
                {
                    m_pLabelSliderNum->setText(QString::number(angleAdjusted));
                }
            }
        }
    }

    if(!m_pLabelSliderNum->isVisible() && nChangeType != TAC_RESET && nChangeType != TAC_ReadBack && nChangeType !=TAC_MotionEditerManual)
    {
        m_pLabelSliderNum->show();
    }

    if(m_pLabelSliderNum->isVisible() && (m_btnGroup.checkedId() == nGroupID))
    {  
        int nSliderWidth = ui->horizontalSliderAngle->width();//这里需要比较刻度条的实际长度，不然会出现问题
        m_pLabelSliderNum->move(ui->horizontalSliderAngle->x() +
                                ui->horizontalSliderAngle->sliderPosition()*nSliderWidth/ui->horizontalSliderAngle->maximum() -
                                m_pLabelSliderNum->width() / 2,
                                ui->horizontalSliderAngle->y() + ui->horizontalSliderAngle->height());
    }
}

void UBXRobot3DControlWidget::onButtonToggled(int id, bool checked)
{
    if(checked)
        emit clickNode(id);
}

void UBXRobot3DControlWidget::onSliderAnglevalueChanged(int value)
{
    if (TAC_MotionEditerManual == m_eTypeAngleChanged)
    {
        return;
    }

    if(ui->horizontalSliderAngle)
    {
        const int nOldNodeValue = ui->horizontalSliderAngle->value();
        const int nCurrentMotorID = m_btnGroup.checkedId();
        int nFixValue = 0;
        bool bCanSet = Ubt3DControler::instance()->getFitedNodeValue(nCurrentMotorID, nOldNodeValue, nOldNodeValue, nFixValue);
        if(!bCanSet)
        {
            ui->horizontalSliderAngle->setValue(nFixValue);
            return;
        }

        startChangedAngleProcessTimer(50, true);
        //        QTimer::singleShot(40, this, &UBXRobot3DControlWidget::slotOnChangedRobotAngleTimeout);
        //changeRobotAngle(true, 50);
    }
}

void UBXRobot3DControlWidget::on_pushButtonMinus_clicked()
{
    int nNewAngle = ui->horizontalSliderAngle->value()-1;
    if(nNewAngle >= 0)
    {
        const int nCurrentMotorID = m_btnGroup.checkedId();
        const bool bCanSet = Ubt3DControler::instance()->testNodeValue(nCurrentMotorID, ui->horizontalSliderAngle->value(), nNewAngle);
        if(!bCanSet)
        {
            return;
        }

        ui->horizontalSliderAngle->setValue(nNewAngle);

        QString sTemp;
        sTemp.sprintf("%d", nNewAngle);
        m_pLabelSliderNum->setText(sTemp);
        
        changeRobotAngle(true, 50);        
    }
}

void UBXRobot3DControlWidget::on_pushButtonPlus_clicked()
{
    const int nNewAngle = ui->horizontalSliderAngle->value()+1;
    const int nCurrentMotorID = m_btnGroup.checkedId();
    if(nNewAngle <= UBXBase::getInstance()->getDefaultActAngleValueMax(nCurrentMotorID))
    {        
        const bool bCanSet = Ubt3DControler::instance()->testNodeValue(nCurrentMotorID, ui->horizontalSliderAngle->value(), nNewAngle);
        if(!bCanSet)
        {
            return;
        }
        
        ui->horizontalSliderAngle->setValue(nNewAngle);

        QString sTemp;
        sTemp.sprintf("%d", nNewAngle);
        m_pLabelSliderNum->setText(sTemp);

        changeRobotAngle(true, 50);
    }
}

//void UBXRobot3DControlWidget::onSliderMoved(int value)
//{
//    changeRobotAngle();
//}

//void UBXRobot3DControlWidget::onSliderPressed()
//{
//    changeRobotAngle();
//}

void UBXRobot3DControlWidget::on_pushButtonReset_clicked()
{
    onResetGesture();
}

void UBXRobot3DControlWidget::onResetGesture()
{
    emit reset();

//    if(m_pConnectPort)
//    {
//        m_pConnectPort->ResetRobot();
//    }
    ResetRobot();
}

/**************************************************************************
* 函数名: slotOnRobotPowerOff
* 功能: 机器人掉电
* 参数:
* 返回值: void
* 时间: 2017/02/08
* 作者: hwx
*/
void UBXRobot3DControlWidget::slotOnRobotPowerOff(bool bUpdateUI)
{
    slotOnReadAll(bUpdateUI);
}

void UBXRobot3DControlWidget::on_pushButtonLegReadback_clicked()
{
    m_eReadAngleType = eReadAngleLeg;
    if(g_pRobotController && g_pRobotController->GetPortOpenState())
    {
        g_pRobotController->Req_RobotGetAngle(7); //腿部舵机ID从7开始
    }
}

void UBXRobot3DControlWidget::on_pushButtonHandReadBack_clicked()
{
    m_eReadAngleType = eReadAngleHand;
    if(g_pRobotController && g_pRobotController->GetPortOpenState())
    {
        g_pRobotController->Req_RobotGetAngle(1);
    }
}

void UBXRobot3DControlWidget::on_pushButtonReadAll_clicked()
{
    slotOnReadAll(true);
}

void UBXRobot3DControlWidget::slotOnReadAll(bool bUpdateUI)
{
    m_eReadAngleType = eReadAngleAll;
    m_bReadAngleUpdateUI = bUpdateUI;
    if(g_pRobotController && g_pRobotController->GetPortOpenState())
    {
        g_pRobotController->Req_RobotGetAngle(1);
    }
}

/**************************************************************************
* 函数名: OnNotify
* 功能: 通知消息
* 参数:
*    @[in ] pSubject: 被观察者
*    @[in ] nMsgID: 消息ID
*    @[in ] dwContext: 消息上下文
*    @[in ] wParam: 参数1
*    @[in ] lParam: 参数2
* 返回值: void
* 时间: 2015/08/22 19:09
* 作者: zdj
*/
void UBXRobot3DControlWidget::OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    zxEvent* pNewEvent = new zxEvent(nMsgID);
    pNewEvent->eEventType = zxEventTypeWindow;
    pNewEvent->lReceiver = (long)this;
    pNewEvent->dwMsgId = nMsgID;
    pNewEvent->wParam = wParam;
    pNewEvent->lParam = lParam;
    QCoreApplication::postEvent(this, pNewEvent);
}

void UBXRobot3DControlWidget::customEvent(QEvent* event)
{
    zxEvent* pEvent = (zxEvent*)event;
    DWORD nMsgID = pEvent->dwMsgId;
    WPARAM wParam = pEvent->wParam;
    LPARAM lParam = pEvent->lParam;

    if(nMsgID == WM_MSG_ROBOT_CONNECT)
    {
        if(lParam)
        {
            //连接机器人
            m_nConnectTimerId = startTimer(TIMEDELAY_CONNECTROBOT);//延时连接
        }
        else
        {
            if(wParam)
            {
                //连接成功
                //ui->pushButtonOpenPort->setEnabled(true);
                //ui->pushButtonOpenPort->setText(tr("Disconnect"));

                EnableCtrls(true);

                //asu, bug14606, 为了机器人重启后显示角度一致
                on_pushButtonReset_clicked();
            }
            else
            {
                //连接失败
                //ui->pushButtonOpenPort->setEnabled(true);
            }
        }
    }
    else if(nMsgID == WM_MSG_ROBOT_DISCONNECT)
    {
        // 拨出USB时断开连接
        if(lParam)
        {
//            if(g_pRobotController/* && g_pRobotController->GetPortOpenState()*/)
//            {
//                g_pRobotController->Req_Disconnect();
//                g_pRobotController->Req_ClosePort();
//            }
            //断开连接机器人
            m_nDisConnectTimerId = startTimer(TIMEDELAY_DISCONNECTROBOT);//延时断开连接

        }
        else
        {
            if(wParam) // 点击断开连接
            {
                //断开连接
                //ui->pushButtonOpenPort->setEnabled(true);
                //ui->pushButtonOpenPort->setText(tr("Connect To Robot"));

                EnableCtrls(false);
            }
            else
            {

            }
        }
    }
    else if(nMsgID == WM_MSG_ROBOT_TRANSDATA)
    {
        const char* pStr = (const char*)lParam;

    }
    else if(nMsgID == WM_MSG_ACTION_DOWNLOAD_PREPARE)
    {

    }
    else if(nMsgID == WM_MSG_ACTION_DOWNLOAD_ING)
    {
        //OnDownloadProgress(wParam, lParam);
    }
    else if(nMsgID == WM_MSG_ACTION_DOWNLOAD_END)
    {
        //OnDownloadEnd(wParam, lParam);
    }
    else if(nMsgID == WM_MSG_ROBOT_SWITCHMODE_RESULT)
    {

    }
    else if(nMsgID == WM_MSG_MOTOR_ANGLE_GET)
    {
        if(g_pRobotController && g_pRobotController->GetPortOpenState())
        {
            int nMotorID = g_pRobotController->GetCurMotorID(); //当前舵机ID
            if(wParam)
            {
                //当前舵机角度读取成功
                short nAngle = lParam; //舵机角度

                if (m_bReadAngleUpdateUI)
                {
                    emit angleChanged(nMotorID, nAngle, 1000, TAC_ReadBack);
                }
            }
            else
            {
                /////////////////////////////////Log////////////////////
                AlphaRobotLog::getInstance()->printDebug("Get motor angle error");
                /////////////////////////////////Log////////////////////
                //当前舵机角度读取失败
            }

            // 读取下一个舵机
            readNextMotor();
        }
    }
}

/**************************************************************************
* 函数名: EnableCtrls
* 功能: 设置控件启用状态
* 参数:
*    @[in ] bEnable: 启用标识
* 返回值: void
* 时间: 2016/01/13 14:40
* 作者: ZDJ
*/
void UBXRobot3DControlWidget::EnableCtrls(bool bEnable)
{
    ui->pushButtonPowerOff->setEnabled(bEnable);
    ui->pushButtonHandReadBack->setEnabled(bEnable);
    ui->pushButtonLegReadback->setEnabled(bEnable);
    ui->pushButtonReadAll->setEnabled(bEnable);

#if 0
    //ui->pushButton_open->setEnabled(bEnable);
    ui->pushButton_addact->setEnabled(bEnable);
    ui->pushButton_removeact->setEnabled(bEnable);
    ui->pushButton_downloadact->setEnabled(bEnable);
    ui->pushButton_udiskmode->setEnabled(bEnable);
    if(eUbtProduct1E == UBXBase::getInstance()->getCurrentRobotProductType())
    {
        ui->pushButton_udiskmode->setEnabled(false);
    }
    ui->tableWidget_localact->setEnabled(bEnable);
    ui->checkBox_selall->setEnabled(bEnable);
#endif
}

/**************************************************************************
* 函数名: ResetRobot
* 功能: 复位机器人
* 时间: 2016/01/25 17:49
* 作者: ZDJ
*/
void UBXRobot3DControlWidget::ResetRobot()
{
    int nMotorCount = m_vecDefaultMotorDataList.size();

#ifdef _DEBUG0
    print_msg("reset angle:\n");
#endif
    MOTOR_DATA* pMotorData = new MOTOR_DATA[nMotorCount];
    for(size_t i = 0; i < nMotorCount; i++)
    {
        MOTOR_DATA* pTempData = m_vecDefaultMotorDataList[i];
        memcpy(pMotorData + i, pTempData, sizeof(MOTOR_DATA));

#ifdef _DEBUG0
        print_msg("%d:%02X ", pMotorData[i].nID, pMotorData[i].nAngle);
#endif
    }
#ifdef _DEBUG0
    print_msg("\n");
#endif

    if(g_pRobotController)
    {
        g_pRobotController->Req_RobotReset(pMotorData, nMotorCount, 50, 50);
    }

    SAFE_DELETE_ARRAY(pMotorData);
}

/**************************************************************************
* 函数名: ReadDefaultAngle
* 功能: 读取舵机默认角度
* 参数:
*    @[out] motorlist: 舵机数据列表
* 返回值: void
* 时间: 2016/01/26 10:15
* 作者: ZDJ
*/
void UBXRobot3DControlWidget::ReadDefaultAngle(vector<MOTOR_DATA*>& motorlist)
{    
    QString sPath = CConfigs::getLocalRobotPath(MODEL_XML_NAME);
    QFile file(sPath);

    if(file.open(QIODevice::ReadOnly))
    {
        QDomDocument doc;
        doc.setContent(&file);
        ReadModelXml(doc.documentElement(), motorlist);

        file.close();
    }
}


/**************************************************************************
* 函数名: startChangedAngleProcessTimer
* 功能: 启动定时器处理机器人角度变化
    *  @[in ] 参数nTime: 定时时间
    * @ [in ] 参数bSendSignal：是否发送信号
* 返回值: void
* 时间: 2016/07/15 10:15
* 作者:hwx
*/
void UBXRobot3DControlWidget::startChangedAngleProcessTimer(int nTime, bool bSendSignal)
{
    if (NULL == m_pChangedAngleProcessTimer)
    {
        m_pChangedAngleProcessTimer = new QTimer();
        connect(m_pChangedAngleProcessTimer, &QTimer::timeout, this, &UBXRobot3DControlWidget::slotOnChangedRobotAngleTimeout);
    }

    // 该判断可以保证如果定时器还在运行，下一个数据不会发送
    // 从而保证硬件不会卡死
    if (m_pChangedAngleProcessTimer->isActive())
    {
        return;
    }

    m_pChangedAngleProcessTimer->setSingleShot(true);
    m_pChangedAngleProcessTimer->setProperty("TimeProperty", nTime);
    m_pChangedAngleProcessTimer->setProperty("SendSignalProperty", bSendSignal);

    m_pChangedAngleProcessTimer->start(40);
}

/**************************************************************************
* 函数名: setReadBackButtonStatus
* 功能: 设置回读按钮状态，防止用户联系多次点击
* 参数:
*    @[in ] bEnable 是否使能
* 返回值: void
* 时间: 2016/07/20 10:15
* 作者:hwx
*/
void UBXRobot3DControlWidget::setReadBackButtonStatus(bool bEnable)
{
    ui->pushButtonHandReadBack->setEnabled(bEnable);
    ui->pushButtonLegReadback->setEnabled(bEnable);
    ui->pushButtonReadAll->setEnabled(bEnable);
}

/**************************************************************************
* 函数名: readNextMotor
* 功能: 读取下一个舵机角度
* 返回值: void
* 时间: 2016/08/04 10:15
* 作者:hwx
*/
void UBXRobot3DControlWidget::readNextMotor()
{
    if (!g_pRobotController || !g_pRobotController->GetPortOpenState())
    {
        return;
    }

    // 读取单个舵机时，不处理
    if (m_eReadAngleType == eReadAngleSingle)
    {
        return;
    }

    int nCurrentMotor = g_pRobotController->GetCurMotorID(); //当前舵机ID

    //读取下一个舵机
    int nIdMax = m_nServoCount;
    if(m_eReadAngleType == eReadAngleHand)
    {
        nIdMax = 6;
    }

    if(nCurrentMotor < nIdMax)
    {
        // 通过使能按钮来实现防止用户多次点击回读问题，
        // 设置按钮状态，当次回读数据返回前，不允许用户点击回读
        setReadBackButtonStatus(false);
        g_pRobotController->Req_RobotGetAngle(nCurrentMotor + 1);
    }
    else
    {
        setReadBackButtonStatus(true);
    }
}

#if 0
/**************************************************************************
* 函数名: ReadModelXml
* 功能: 读取3D模型的xml配置文件
* 参数:
*    @[in ] element: dom元素
*    @[out] motorlist: 舵机数据列表
* 返回值: void
* 时间: 2016/01/26 10:15
* 作者: ZDJ
*/
void UBXRobot3DControlWidget::ReadModelXml(const QDomElement& element, vector<MOTOR_DATA*>& motorlist)
{
    QDomNode node = element.firstChild();
    static MOTOR_DATA* pMotorData = NULL;

    while(!node.isNull())
    {
        QDomElement nodeelem = node.toElement();
        if(nodeelem.tagName() == "Motor")
        {
            pMotorData = new MOTOR_DATA;
            motorlist.push_back(pMotorData);
            ReadModelXml(nodeelem, motorlist);
        }
        else if(nodeelem.tagName() == "ID")
        {
            QDomNode childnode = node.firstChild();
            if(childnode.nodeType() == QDomNode::TextNode)
            {
                if(pMotorData)
                    pMotorData->nID = childnode.toText().data().toInt();
            }
        }
        else if(nodeelem.tagName() == "InitAngle")
        {
            QDomNode childnode = node.firstChild();
            if(childnode.nodeType() == QDomNode::TextNode)
            {
                if(pMotorData)
                    pMotorData->nAngle = childnode.toText().data().toInt();
            }
        }

        node = node.nextSibling();
    }
}
#endif

/**************************************************************************
* 函数名: on_pushButtonPowerOff_clicked
* 功能: 掉电消息响应
* 时间: 2016/01/28 17:39
* 作者: ZDJ
*/
void UBXRobot3DControlWidget::on_pushButtonPowerOff_clicked()
{
    onPowerOffCurrentNode();
}

void UBXRobot3DControlWidget::onPowerOffCurrentNode()
{
    m_eReadAngleType = eReadAngleSingle;
    if(g_pRobotController && g_pRobotController->GetPortOpenState())
    {
        int nMotorID = m_btnGroup.checkedId();
        if(nMotorID != -1)
            g_pRobotController->Req_RobotGetAngle(nMotorID);
    }
}

/**************************************************************************
* 函数名: OnPortOpenChanged
* 功能: 端口改变消息响应
* 时间: 2016/01/28 17:39
* 作者: ZDJ
*/
void UBXRobot3DControlWidget::OnPortOpenChanged(BOOL bOpen)
{
    if(bOpen)
    {
        //端口已连接
        //ui->pushButtonOpenPort->setEnabled(true);
        //ui->pushButtonOpenPort->setText(tr("Disconnect"));
        EnableCtrls(true);
    }
    else
    {
        //端口已关闭
        //ui->pushButtonOpenPort->setEnabled(true);
        //ui->pushButtonOpenPort->setText(tr("Connect To Robot"));
        EnableCtrls(false);
    }
}

/**************************************************************************
* 函数名: slotOnChangedRobotAngleTimeout
* 功能: 改变机器人定时时间到达的槽函数
* 时间: 2016/07/12
* 作者: hwx
*/
void UBXRobot3DControlWidget::slotOnChangedRobotAngleTimeout()
{
    if (NULL == m_pChangedAngleProcessTimer)
    {
        return;
    }
    bool bOk = true;
    int nTime = m_pChangedAngleProcessTimer->property("TimeProperty").toInt(&bOk);
    if (!bOk)
    {
        return;
    }

    bool bSendSignal = m_pChangedAngleProcessTimer->property("SendSignalProperty").toBool();
    changeRobotAngle(bSendSignal, nTime);
    m_pChangedAngleProcessTimer->stop();
}


//void UBXRobot3DControlWidget::on_pushButtonOpenPort_clicked()
//{
//    if(g_pRobotController)
//    {
//        if(!g_pRobotController->GetPortOpenState())
//        {
//            //ui->pushButtonOpenPort->setEnabled(false);
//            g_pRobotController->Req_Connect();
//        }
//        else
//        {
//            g_pRobotController->Req_Disconnect();
//            g_pRobotController->Req_ClosePort();

//            //ui->pushButtonOpenPort->setEnabled(true);
//            //ui->pushButtonOpenPort->setText(tr("Connect To Robot"));

//            EnableCtrls(false);

//            emit PortOpenChanged(FALSE);
//        }
//    }
//}

void UBXRobot3DControlWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    int nSliderWidth = ui->horizontalSliderAngle->width();//这里需要比较刻度条的实际长度，不然会出现问题
    m_pLabelSliderNum->move(ui->horizontalSliderAngle->x() + ui->horizontalSliderAngle->sliderPosition()*nSliderWidth/ui->horizontalSliderAngle->maximum() - m_pLabelSliderNum->width() / 2,
                             ui->horizontalSliderAngle->y() + ui->horizontalSliderAngle->height());
}

bool UBXRobot3DControlWidget::changeRobotAngle(bool bEmitSignal, int nTime)
{
    bool bHadSet = false;

    if(m_btnGroup.checkedId() != -1 && ui->horizontalSliderAngle)
    {
        const int nNewAngle = ui->horizontalSliderAngle->value();

        const int nCurrentMotorID = m_btnGroup.checkedId();
//        const bool bCanSet = Ubt3DControler::instance()->testNodeValue(nCurrentMotorID, ui->horizontalSliderAngle->value(), nNewAngle);
//        if(!bCanSet)
//        {
//            bHadSet = false;

//            //recorve the slider value
//            const int nCurNodeAngle = Ubt3DControler::instance()->getMotorAngle(nCurrentMotorID);
//            ui->horizontalSliderAngle->setValue(nCurNodeAngle);

//            return bHadSet;
//        }

        if(!m_bIsReading && g_pRobotController && g_pRobotController->GetPortOpenState())
        {
            g_pRobotController->Req_RobotSetAngle(nCurrentMotorID, nNewAngle, 10);
        }
        if(bEmitSignal)
        {
            emit angleChanged(nCurrentMotorID, nNewAngle, nTime, TAC_ControlView);
        }

        bHadSet = true;
    }

    return bHadSet;
}

void UBXRobot3DControlWidget::timerEvent(QTimerEvent* event)
{
    int nTimerId = event->timerId();
    if(nTimerId == m_nConnectTimerId)
    {
        killTimer(nTimerId);
        m_nConnectTimerId = 0;
        if(g_pRobotController && !g_pRobotController->GetPortOpenState())
            g_pRobotController->Req_Connect();
    }
    else if(nTimerId == m_nDisConnectTimerId)
    {
        killTimer(nTimerId);
        m_nDisConnectTimerId = 0;

        if(g_pRobotController && g_pRobotController->GetPortOpenState())
        {
            g_pRobotController->Req_Disconnect();
            //g_pRobotController->Req_ClosePort();
        }

        //断开连接
        //ui->pushButtonOpenPort->setEnabled(true);
        //ui->pushButtonOpenPort->setText(tr("Connect To Robot"));

        EnableCtrls(false);

        emit PortOpenChanged(FALSE);
    }
}
