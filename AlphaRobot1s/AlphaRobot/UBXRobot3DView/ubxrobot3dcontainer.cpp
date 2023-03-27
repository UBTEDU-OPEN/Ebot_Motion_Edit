#include "stdafx.h"
#include "ubxrobot3dcontainer.h"
#include "ui_ubxrobot3dcontrol.h"

#include "ubxrobot3dwidget.h"
#include "../Common/UBXPublic.h"
#include "ubxrobot3dconstants.h"
#include "ubt3dcontroler.h"

bool  CreateUBXRobot3DView(IUBXRobot3DView ** ppRobot3dView)
{
  UBXRobot3DContainer *pProxy = new UBXRobot3DContainer();
  if(pProxy)
  {
      *ppRobot3dView = (IUBXRobot3DView*)pProxy;
      Ubt3DControler::instance()->setRobot3DView(*ppRobot3dView);

      return true;
  }
  return false;
}

RETRANSLATEUIFUNCIMPL(UBXRobot3DContainer)

UBXRobot3DContainer::UBXRobot3DContainer(QWidget *parent, IUBXRobot3DViewEvent *pUBXRobotEvent) :
    UBXWidget(parent),
    ui(new Ui::UBXRobot3DControl)
{
    m_pUBXRobot3DWidget = NULL;
    m_pControlWidget = NULL;
    m_pEvent = NULL;

    ui->setupUi(this);
    setDefaultStyle();
    //线程通知窗体enbale时调用
    connect(this, &UBXRobot3DContainer::SigEnable, this, &UBXRobot3DContainer::setEnabled);
}

UBXRobot3DContainer::~UBXRobot3DContainer()
{
    delete ui;
}

//void UBXRobot3DControl::SetSerialPortControl(CUBXSerialPortSesstion *pConnect)
//{
//    m_pConnectPort = pConnect;
//}

void UBXRobot3DContainer::UpdateNodeDate(const char *szNodeName, float angle, float offset, TypeAngleChanged nChangeType)
{
    Q_ASSERT(szNodeName != NULL);
    if(m_pControlWidget)
    {
        m_pControlWidget->UpdateNodeDate(szNodeName, angle, offset, nChangeType);
    }
}

void UBXRobot3DContainer::Set3DViewRotationSmooth(int nID, float fRot, int nTime)
{
    //PlaySetRotation1(nDir, fRot, pNode, nTime);
}

void UBXRobot3DContainer::Set3DViewRotationManual(int nDir, float fRot, const char *pNode, int nTime)
{
    //ManualSetRotation1(nDir, fRot, pNode, nTimer);
}

void UBXRobot3DContainer::SetRotation(float x, float y, float z, float w, char *pNode)
{
    //PlaySetRotation2(x, y, z, w, pNode);源码中也已经注释
}

void UBXRobot3DContainer::OnSetRotation(int nID, int nAngle, int iTime)
{
//    if(m_pConnectPort)
//    {
//        m_pConnectPort->SetAngleData(nID, nAngle);
    //    }
}

bool UBXRobot3DContainer::IsComConnectted()
{
//    if(m_pConnectPort)
//    {
//        return m_pConnectPort->IsConnectted();
//    }
//    else
//    {
//        return false;
    //    }
    return true;
}

bool UBXRobot3DContainer::IsRobotDebuging()
{
    if(m_pEvent)
    {
        return m_pEvent->IUBXRobot3DViewEvent_IsRobotDebuging();
    }
    return false;
}

void UBXRobot3DContainer::LostPowerForRobot(int nID)
{
//    if(m_pConnectPort)
//    {
//        m_pConnectPort->ReadBackAngleOrPowerDown(nID);
    //    }
}

void UBXRobot3DContainer::OnReadRotation(int nID)
{
//    if(m_pConnectPort)
//    {
//        m_pConnectPort->ReadBackAngleOrPowerDown(nID);
//    }
}

void UBXRobot3DContainer::ResetAllAngle()
{
//    if(m_pConnectPort)
//    {
//        m_pConnectPort->ResetRobot();
    //    }
}

void UBXRobot3DContainer::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/3dviewdefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

void UBXRobot3DContainer::Connect(QWidget *parent, IUBXRobot3DViewEvent *pEvent)
{
    const int n3DWidgetStretch = 7;
    ui->verticalLayout->addWidget(ui->widgetTemp, n3DWidgetStretch);

    m_pUBXRobot3DWidget = new UBXRobot3DWidget(this, pEvent, this);
    connect(m_pUBXRobot3DWidget, &UBXRobot3DWidget::SigFirstFrameFinished, this, &UBXRobot3DContainer::OnFirstFrameFinished);    
    ui->verticalLayout->addWidget(m_pUBXRobot3DWidget, n3DWidgetStretch);

    m_pControlWidget = new UBXRobot3DControlWidget();
    ui->verticalLayout->addWidget(m_pControlWidget, 11);
    QObject::connect(m_pControlWidget, &UBXRobot3DControlWidget::angleChanged,
                     m_pUBXRobot3DWidget, &UBXRobot3DWidget::angleChanged);
    QObject::connect(m_pControlWidget, &UBXRobot3DControlWidget::clickNode,
                     m_pUBXRobot3DWidget, &UBXRobot3DWidget::clickNode);
    QObject::connect(m_pControlWidget, &UBXRobot3DControlWidget::reset,
                     m_pUBXRobot3DWidget, &UBXRobot3DWidget::reset);

    m_pUBXRobot3DWidget->initializeGLContext();
    m_pEvent = pEvent;
}

void UBXRobot3DContainer::Disconnect()
{
    if(m_pControlWidget)
        delete m_pControlWidget;
    if(m_pUBXRobot3DWidget)
        delete m_pUBXRobot3DWidget;
    delete this;
}

bool UBXRobot3DContainer::getMotorInfo(QMap<int, int> &motorInfos)
{
    motorInfos.clear();

    if(m_pUBXRobot3DWidget)
    {
        return m_pUBXRobot3DWidget->getMotorInfo(motorInfos);
    }
    return false;
}

bool UBXRobot3DContainer::GetMotorDatas(char **pData, int &nLen)
{
    if(m_pUBXRobot3DWidget)
    {
        return m_pUBXRobot3DWidget->Control_GetMotorDatas(pData, nLen);
    }
    return false;
}

void UBXRobot3DContainer::SetMotorDatas(char *pData, int nLen, int runTime)
{
    if(m_pUBXRobot3DWidget)
    {
        m_pUBXRobot3DWidget->Control_SetAllMotorPowerState(POWERON);
        m_pUBXRobot3DWidget->Control_SetMotorDatas(pData, nLen, runTime);
    }
}

void UBXRobot3DContainer::SetMotorDatasManual(char *pData, int nLen, int runTime)
{
    if(m_pUBXRobot3DWidget)
    {
        m_pUBXRobot3DWidget->Control_SetViewMotorDatasManual(pData, nLen, runTime);
    }
}

void UBXRobot3DContainer::SetMotorAngle(int nID, int nAngle)
{
    if(m_pUBXRobot3DWidget)
    {
        m_pUBXRobot3DWidget->Control_SetMotorAngle(nID, nAngle);
    }
}

void UBXRobot3DContainer::Set3DModelPlayState(bool bState)
{
    if(m_pUBXRobot3DWidget)
    {
        emit m_pUBXRobot3DWidget->setPlayState(bState);
    }
}

void UBXRobot3DContainer::SetMotorPowerState(int nMotorID, int nState)
{
    if(m_pUBXRobot3DWidget)
    {
        m_pUBXRobot3DWidget->Control_SetMotorPowerState(nMotorID, nState);
    }
}

void UBXRobot3DContainer::SetAllMotorAngle(char *pData, int nLen, int nRunTime, int nAllTime)
{
    if(pData == NULL || nLen <= 4)
    {
        return;
    }
    int AngleArr[18] = {0};
    MOTOR_DATA* pMotor = (MOTOR_DATA*)(pData + sizeof(nLen));
    int nCount = (nLen - sizeof(nLen)) / sizeof(MOTOR_DATA);
    for(int i = 0; i < /*nCount*/16; i++) //源码中此处用16，暂时不知道为什么不用ncount，调试时可以看看
    {
        AngleArr[i] = pMotor[i].nAngle;
    }
    AngleArr[16] = nRunTime;
    AngleArr[17] = nAllTime;

//    if(m_pConnectPort)
//    {
//        m_pConnectPort->DebugRobot(AngleArr);
    //    }
}

bool UBXRobot3DContainer::GetComConnectState()
{
//    if(m_pConnectPort)
//    {
//        return true;
//    }
//    else
//    {
//        return false;
    //    }
    return false;
}

void UBXRobot3DContainer::EnableViewCtrl(bool bEnable)
{
    if(m_pControlWidget)
    {
        //线程通知窗体enbale时调用
        emit SigEnable(bEnable);
        if(bEnable)
        {
            emit m_pUBXRobot3DWidget->updateData();
        }
    }
}

int UBXRobot3DContainer::GetMotorCount()
{
    if(m_pUBXRobot3DWidget)
    {
        return m_pUBXRobot3DWidget->GetMotorCount();
    }
    return 0;
}

QWidget *UBXRobot3DContainer::GetWidget()
{
    return this;
}

void UBXRobot3DContainer::showEvent(QShowEvent *event)
{
    if(isVisible())
    {
        int ratio = windowHandle()->devicePixelRatio();
        QSize sizeRe = size();
        sizeRe.setWidth(sizeRe.width() / ratio);
        resize(sizeRe);
    }
}

void UBXRobot3DContainer::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void UBXRobot3DContainer::OnFirstFrameFinished()
{
    ui->verticalLayout->removeWidget(ui->widgetTemp);
    ui->widgetTemp->deleteLater();

    m_pUBXRobot3DWidget->show();
}

UBXRobot3DControlWidget* UBXRobot3DContainer::GetCtrlWidget()
{
    return m_pControlWidget;
}

UBXRobot3DWidget *UBXRobot3DContainer::get3DWidget()
{
    return m_pUBXRobot3DWidget;
}
