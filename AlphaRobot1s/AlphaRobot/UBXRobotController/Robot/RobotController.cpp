/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：RobotController.cpp
* 创建时间：2015/10/18 14:39
* 文件标识：
* 文件摘要：机器人控制类
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/10/18 14:39
* 版本摘要：
*/
#include "StdAfx.h"
#include "RobotController.h"
#include <QDebug>
#include <QTimer>


#include "cbluetoothcomandparser.h"
#include "cdataoperate.h"
#include "cjsonutil.h"
#include "../UBXBase/UBXBase.h"
#include "Robot/irobotcmd.h"
#include "UBXPublic.h"

#include "ciniconfig.h"


#ifdef Q_OS_WIN
extern "C"
{
#include "setupapi.h"
#include "hidsdi.h"
}
#endif

#ifdef Q_OS_WIN
static const GUID GUID_DEVINTERFACE_LIST[] =
{
    // GUID_DEVINTERFACE_USB_DEVICE
    { 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },

    // GUID_DEVINTERFACE_DISK
    { 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },

    // GUID_DEVINTERFACE_HID,
    { 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },

    // GUID_NDIS_LAN_CLASS
    { 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }
};
#endif

//设备VID，PID定义
DEVIDTYPE     g_devidarr[] =
{
    DEVIDTYPE(ePortTypeVCP, 0x0483, 0x5740),
    DEVIDTYPE(ePortTypeHID, 0x0483, 0x5750), //old 1s
    DEVIDTYPE(ePortTypeHID, 0x0525, 0xa4ac) //arm plane
};

//#pragma region 消息定义
const UINT WM_MSG_ACTION_DOWNLOAD_PREPARE = QEvent::registerEventType();  //准备传输文件消息
const UINT WM_MSG_ACTION_DOWNLOAD_ING =     QEvent::registerEventType();      //正在传输文件消息
const UINT WM_MSG_ACTION_DOWNLOAD_END =     QEvent::registerEventType();      //结束传输文件消息
const UINT WM_MSG_UPDATE_DOWNLOAD_PREPARE = QEvent::registerEventType(); // 准备下载更新
const UINT WM_MSG_UPDATE_DOWNLOAD_ING = QEvent::registerEventType(); // 正在下载更新
const UINT WM_MSG_UPDATE_DOWNLOAD_END = QEvent::registerEventType(); // 结束下载更新
const UINT WM_MSG_TRANS_FILE_CANCEL =     QEvent::registerEventType();      //取消文件传输
const UINT WM_MSG_PORT_OPENSTATECHANGE =    QEvent::registerEventType();     //端口打开状态改变消息

const UINT WM_MSG_MOTOR_READVERSION =        QEvent::registerEventType();        //舵机读版本消息
const UINT WM_MSG_MOTOR_UPGRADE_PREPARE =   QEvent::registerEventType();    //舵机升级准备
const UINT WM_MSG_MOTOR_UPGRADE_ING =        QEvent::registerEventType();        //舵机升级进行中
const UINT WM_MSG_MOTOR_UPGRADE_END =       QEvent::registerEventType();        //舵机升级结束

const UINT WM_MSG_MOTOR_MODID =             QEvent::registerEventType();              //舵机修改ID
const UINT WM_MSG_MOTOR_GETID =              QEvent::registerEventType();              //舵机获取ID

const UINT WM_MSG_ROBOT_SOFTWARE_VERSION_GET =   QEvent::registerEventType();        //获取机器人软件版本
const UINT WM_MSG_ROBOT_HARDWARE_VERSION_GET =  QEvent::registerEventType();        //获取机器人硬件版本
const UINT WM_MSG_MOTOR_ANGLE_GET =             QEvent::registerEventType();                   //获取舵机角度
const UINT WM_MSG_MOTOR_ANGLE_SET =             QEvent::registerEventType();                   //设置舵机角度
const UINT WM_MSG_MOTOR_MOVE = QEvent::registerEventType(); // 转动舵机角度

const UINT WM_MSG_ROBOT_CONNECT =              QEvent::registerEventType();            //连接机器人
const UINT WM_MSG_ROBOT_DISCONNECT =           QEvent::registerEventType();         //断开机器人

const UINT WM_MSG_MOTOR_OFFSET_GET =           QEvent::registerEventType();         //舵机获取偏移值
const UINT WM_MSG_MOTOR_OFFSET_SET =           QEvent::registerEventType();         //舵机修改偏移值

const UINT WM_MSG_ROBOT_TRANSDATA =            QEvent::registerEventType();           //机器人数据转发
const UINT WM_MSG_ROBOT_SWITCHMODE_RESULT =  QEvent::registerEventType();           //机器人转换模式结果

const UINT WM_MSG_ROBOT_READSN_RESULT =        QEvent::registerEventType();           //机器人读序列号结果
const UINT WM_MSG_ROBOT_WRITESN_RESULT =       QEvent::registerEventType();         //机器人写序列号结果

const UINT WM_MSG_ROBOT_READUID_RESULT =       QEvent::registerEventType();         //机器人读UID结果

const UINT WM_MSG_ROBOT_BATTERY_STATUS_GET = QEvent::registerEventType();         //机器人电池结果

const UINT WM_MSG_ROBOT_TF_CARD_STATUS = QEvent::registerEventType();         // 机器人没有插入TF卡

const UINT WM_MSG_GET_ROBOT_ACTION_LIST_RST = QEvent::registerEventType(); // 获取机器人动作列表
const UINT WM_MSG_DELETE_ROBOT_ACTION_RST = QEvent::registerEventType(); // 删除机器人动作

const UINT WM_MSG_GET_ROBOT_WIFI_LIST_RST = QEvent::registerEventType(); // 获取机器人WIFI列表
const UINT WM_MSG_ROBOT_CONNECT_WIFI_RST = QEvent::registerEventType(); // 机器人CONNECT WIFI

const UINT WM_MSG_ROBOT_MANAL_UPGRADE_RST = QEvent::registerEventType();  ////手动升级
const UINT WM_MSG_ROBOT_GET_AUTO_UPDATE_PROGRESS_RST = QEvent::registerEventType();  //获取自动升级进度
const UINT WM_MSG_ROBOT_IS_INSTATLING_UPDATE_RST = QEvent::registerEventType();  //是否安装升级通知


const UINT WM_MSG_TFCARD_NO_CAPACITY = QEvent::registerEventType(); // 卡容量满


CRobotController* CRobotController::m_pControllerStatic = NULL;
BOOL     g_bFirstDeviceArrival = FALSE; //设备第一次到达

//#pragma endregion


CRobotController::CRobotController(QWidget *parent) : QWidget(parent)
{
    m_pControllerStatic = this;

    m_pPort = NULL;
    m_pCurrentTransFile = NULL;
    m_pRobotCmd = NULL;
    m_pRobotCmd = &m_objRobotCmdOfPCOld;//set default cmd obj
    //m_pRobotCmd = &m_objRobotCmdOfBTProtocol;//set default cmd obj

    m_nConnectPortTimerID = 5000;
    m_nDisconnectPortTimerID = 5001;
    m_nCheckUtf8TimerID = 5002;

    m_nTransFileFrameSize = DEFAULT_TRANS_FILE_FRAME_SIZE;
    m_nCurResendRequest = 0;
    m_nCurMotorID = -1;
    m_nCurMotorOffset = 0;
    m_nCurTime = 0;
    m_nCurRobotCmdTimerId = 0;

    m_eSwitchMode = eRobotSwitchModeUdisk;

    m_stDevIDInfo = g_devidarr[ePortTypeHID];
    //m_nCurTranDataLen = 0;
    //m_nDataTransRate = 0;

    m_bCurFileSendComplete = TRUE;
    m_bCancelTransFile = FALSE;
    m_bHasResendRequest = FALSE;
    m_bSupportUTF8 = FALSE;

    setCurrentPerformRobotCmd(eRobotCmdTypeNone);
    memset(m_szRobotSoftVersion, 0, sizeof(m_szRobotSoftVersion));

    connect(this, &CRobotController::sigOnSendCmd, this, &CRobotController::slotOnSendCmd, Qt::QueuedConnection);
    connect(this, &CRobotController::sigOnDeviceRemoved, this, &CRobotController::slotOnDeviceRemoved, Qt::QueuedConnection);

    connect(UBXBase::getInstance(), &UBXBase::sigRobotProductTypeChange,
            this, &CRobotController::slotOnRobotProductTypeChange, Qt::DirectConnection);

    RegisterDeviceNotify();
    InitProtocolAnswerLen();

    hid_init();
    LoadConfig();

    slotOnRobotProductTypeChange( eUbtProductUnknown, eUbtProductUnknown);
}

CRobotController::~CRobotController()
{
    if(GetPortOpenState())
    {
        Req_Disconnect();

        // 关闭应用程序，收不到应答，因此需要另外关闭串口
        Req_ClosePort();
    }

    hid_exit();

    m_oRobotCommunicateThread.quit();
    m_oRobotCommunicateThread.wait();
}

//#pragma region

/**************************************************************************
* 函数名: RegisterDeviceNotify
* 功能: 注册设备通知
* 时间: 2015/09/08 10:08
* 作者: ZDJ
*/
void CRobotController::RegisterDeviceNotify()
{
#ifdef Q_OS_MAC

    CMyHidClass::SetArrivalCbk(OnDeviceArrival);
    CMyHidClass::SetRemovalCbk(OnDeviceRemoval);
#elif defined(Q_OS_WIN)

    HDEVNOTIFY hDevNotify = NULL;

    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    for(int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
    {
        NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
        hDevNotify = RegisterDeviceNotification((HANDLE)this->winId(), &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
        if( !hDevNotify )
        {
            //AfxMessageBox(CString(_T("Can't register device notification: ")), MB_ICONEXCLAMATION);
            return;
        }
        else
        {
            qDebug("reg notify success");
        }
    }

#endif
}

//#pragma endregion

//#pragma region 请求
/**************************************************************************
* 函数名: Req_Connect
* 功能: 请求连接指定端口的机器人
* 时间: 2015/10/18 14:54
* 作者: ZDJ
*/
void CRobotController::Req_Connect()
{
    if(Req_AutoOpenPort())
    {
         m_pRobotCmd = &m_objRobotCmdOfPCOld;
         SendCmdData(eRobotCmdTypeConnectRobot);
    }
    else
    {
        NotifyObservers(WM_MSG_ROBOT_CONNECT, NULL, FALSE, NULL);
    }
}

/**************************************************************************
* 函数名: Req_Disconnect
* 功能: 请求断开机器人的连接
* 时间: 2015/10/18 14:55
* 作者: ZDJ
*/
void CRobotController::Req_Disconnect(void)
{
    SendCmdData(eRobotCmdTypeDisconnectRobot);
    // 在断开连接完成后再close
    //Req_ClosePort();
}

/**************************************************************************
* 函数名: Req_TransFile
* 功能: 请求传输文件
* 参数:
*    @[in ] sSrcFilePath: 源文件路径
*    @[in ] sTransFilePath: 传输的文件路径
*    @[in ] sDestFilePath: 目标文件路径
*    @[in ] lpszGuid: GUID
* 返回值: 请求成功返回TRUE，失败返回FALSE
* 时间: 2015/10/18 14:55
* 作者: ZDJ
*/
BOOL CRobotController::Req_TransFile(const char *pszSrcFilePath, const char *pszTransFilePath, const char *pszDestFilePath, const char *pszGuid, emTranFileType eType)
{
    m_bCancelTransFile = FALSE;
    BOOL bRet = FALSE;

    QString sTransFilePath = AnsiToUnicode(pszTransFilePath);
    if(PathFileExists(sTransFilePath))
    {
        m_STCurSendData.Reset();

        QFile fileobj(sTransFilePath);
        if(fileobj.open(QIODevice::ReadOnly))
        {
            qint64 nFileLen = fileobj.size();

            m_STCurSendData.m_eType = eType;
            m_STCurSendData.m_sGuid = pszGuid; //test
            strcpy(m_STCurSendData.m_szSrcPath, pszSrcFilePath);
            strcpy(m_STCurSendData.m_szTransFilePath, pszTransFilePath);
            strcpy(m_STCurSendData.m_szDestPath, pszDestFilePath);

            m_STCurSendData.m_senddata.datalen = nFileLen;
            m_STCurSendData.m_senddata.databuf = new byte[nFileLen];
            fileobj.read((char*)m_STCurSendData.m_senddata.databuf, nFileLen);

            fileobj.close();

            //m_nCurTranDataLen = 0;
            //SetTimer(g_nCalcuTransRateTimerID, g_nCalcuTransRatePeriod, NULL);//计算传输速率

            SendCmdData(eRobotCmdTypeTransFilePrepare, (LPARAM)pszTransFilePath, (LPARAM)pszDestFilePath, NULL, NULL);

            //AlphaRobotLog::getInstance()->printDebug(QString("resend time: %1").arg(g_nWaitReSendTime));
            bRet = TRUE;
        }
    }

    return bRet;

}

/**************************************************************************
* 函数名: Req_TransFile
* 功能: 请求传输文件
* 参数:
*    @[in ] pTransFile: 文件数据
* 返回值: 请求成功返回TRUE，失败返回FALSE
* 时间: 2015/10/18 14:55
* 作者: ZDJ
*/
BOOL CRobotController::Req_TransFile(TRANSFILEINFO* pTransFile, emTranFileType eType)
{
    m_pCurrentTransFile = pTransFile;
    return Req_TransFile(pTransFile->sSrcFilePath, pTransFile->sTransFilePath, pTransFile->sDestFilePath, pTransFile->sGuid, eType);
}

/**************************************************************************
* 函数名: Req_CancelTransFile
* 功能: 请求取消传输文件
* 时间: 2015/10/20 9:43
* 作者: ZDJ
*/
void CRobotController::Req_CancelTransFile()
{
    // Modify by huangwuxian 2016.08.01
    // 由于连续发送多个命令的时间间隔有限制
    // 在文件还正在传输的过程中，如果立刻发送
    // 停止指令，会导致多个指令时间间隔不满足要求
    // 导致hid卡死界面无响应，因此在发送停止指令前
    // 需要先停止文件传输
    stopTransFile();
    slotOnStopTransFileTimeout();
//    QTimer::singleShot(500, this, &CRobotController::slotOnStopTransFileTimeout);
}

/**************************************************************************
* 函数名: stopTransFile
* 功能: 停止文件传输
* 时间: 2016/08/01 9:43
* 作者:hwx
*/
void CRobotController::stopTransFile()
{
    m_bCancelTransFile = TRUE;
}

/**************************************************************************
* 函数名: Req_OpenPort
* 功能: 请求打开指定端口
* 参数:
*    @[in ] nPortType: 端口类型
*    @[in ] dwVID: dwVID值
*    @[in ] dwPID: dwPID值
*    @[in ] ...: 变长参数
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/21 20:49
* 作者: ZDJ
*/
BOOL CRobotController::Req_OpenPort(int nPortType, DWORD dwVID, DWORD dwPID, ...)
{
    /*if(m_pPort && m_pPort->IsOpen())
        m_pPort->ClosePort();*/
    Req_ClosePort();

    if(nPortType == ePortTypeHID)
    {
        if(!m_pPort)
        {
            m_pPort = new CMyHidClass(this);
            m_pPort->m_pPortRecvDataFCB = CRobotController::portRecvData;
            m_pPort->m_pUser = (void*)this;
        }
        if(!m_pPort)
            return FALSE;

        if(!m_pPort->Init())
        {
            SAFE_DELETE(m_pPort);
            return FALSE;
        }

        return m_pPort->OpenPort(dwVID, dwPID);
    }
#ifdef Q_OS_WIN
    else if(nPortType == ePortTypeVCP)
    {
        if(!m_pPort)
            m_pPort = new CSerialPort(this);
        if(!m_pPort)
            return FALSE;

        if(!m_pPort->Init())
        {
            SAFE_DELETE(m_pPort);
            return FALSE;
        }

        va_list va_args;
        va_start(va_args, dwPID);

        int nBandrate = va_arg(va_args, int);
        va_end(va_args);

        m_pPort->SetParams(nBandrate);

        return m_pPort->OpenPort(dwVID, dwPID);
    }
#endif
    return FALSE;
}

/**************************************************************************
* 函数名: Req_AutoOpenPort
* 功能: 请求自动打开端口
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/21 20:49
* 作者: ZDJ
*/
BOOL CRobotController::Req_AutoOpenPort()
{
    BOOL bRet = FALSE;

    int nPortNumber = sizeof(g_devidarr)/sizeof(DEVIDTYPE);
    for(int i = 0; i < nPortNumber; i++)
    {
        //SAFE_DELETE(m_pPort);
        Req_ClosePort();

        DEVIDTYPE tempDevIDInfo = g_devidarr[i];
        if(tempDevIDInfo.nType == ePortTypeVCP)
        {
            bRet = Req_OpenPort(tempDevIDInfo.nType, tempDevIDInfo.dwVid, tempDevIDInfo.dwPid, 115200);
        }
        else if(tempDevIDInfo.nType == ePortTypeHID)
        {
            bRet = Req_OpenPort(tempDevIDInfo.nType, tempDevIDInfo.dwVid, tempDevIDInfo.dwPid);
        }

        if(bRet)
        {
            m_stDevIDInfo = tempDevIDInfo;
            break;
        }
    }


//    do
//    {
//        bRet = Req_OpenPort(tempDevIDInfo.nType, tempDevIDInfo.dwVid, tempDevIDInfo.dwPid);
//        if(bRet)
//            break;

//        else if (condition)
//        {

//        }
//        else
//        {
//            SAFE_DELETE(m_pPort);
//            tempDevIDInfo = g_devidarr[ePortTypeVCP];//HID模式打开失败后尝试以VCP模式打开
//        }

//        bRet = Req_OpenPort(tempDevIDInfo.nType, tempDevIDInfo.dwVid, tempDevIDInfo.dwPid, 115200);
//        if(bRet)
//            break;
//        else
//        {
//            SAFE_DELETE(m_pPort);
//        }

//    } while (0);

//    if(bRet)
//    {
//        m_stDevIDInfo = tempDevIDInfo;
//    }

    return bRet;
}

/**************************************************************************
* 函数名: Req_ClosePort
* 功能: 请求关闭端口
* 时间: 2015/10/19 9:23
* 作者: ZDJ
*/
BOOL CRobotController::Req_ClosePort()
{
    if(!m_pPort)
        return FALSE;

    if (m_pPort)
    {
        if(m_pPort->IsOpen())
            m_pPort->ClosePort();

        SAFE_DELETE(m_pPort);
    }

    return TRUE;
}

/**************************************************************************
* 函数名: Req_SwitchMode
* 功能: 请求切换模式
* 时间: 2015/10/19 9:23
* 作者: ZDJ
*/
BOOL CRobotController::Req_SwitchMode(emRobotSwitchMode eMode)
{
    m_eSwitchMode = eMode;
    SendCmdData(eRobotCmdTypeSwitchMode, eMode);
    return TRUE;
}

/**************************************************************************
* 函数名: Req_RobotDebug
* 功能: 机器人仿真
* 参数:
*    @[in ] pMotorData: 舵机数据缓冲区
*    @[in ] nMotorCount: 舵机数量
*    @[in ] nRunTime: 运动时间
*    @[in ] nAllTime: 总时间
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/21 18:36
* 作者: ZDJ
*/
BOOL CRobotController::Req_RobotDebug(MOTOR_DATA* pMotorData, int nMotorCount, int nRunTime, int nAllTime)
{
    SendCmdData(eRobotCmdTypeDebug, (LPARAM)pMotorData, nMotorCount, nRunTime, nAllTime);
    return TRUE;
}

/**************************************************************************
* 函数名: Req_RobotReset
* 功能: 机器人复位
* 参数:
*    @[in ] pMotorData: 舵机数据缓冲区
*    @[in ] nMotorCount: 舵机数量
*    @[in ] nRunTime: 运动时间
*    @[in ] nAllTime: 总时间
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/21 18:36
* 作者: ZDJ
*/
BOOL CRobotController::Req_RobotReset(MOTOR_DATA* pMotorData, int nMotorCount, int nRunTime, int nAllTime)
{
    SendCmdData(eRobotCmdTypeReset, (LPARAM)pMotorData, nMotorCount, nRunTime, nAllTime);
    return TRUE;
}

/**************************************************************************
* 函数名: Req_RobotReadSerialNum
* 功能: 读序列号
* 时间: 2015/11/16 11:02
* 作者: ZDJ
*/
BOOL CRobotController::Req_RobotReadSerialNum()
{
    SendCmdData(eRobotCmdTypeReadSN);
    return TRUE;
}

/**************************************************************************
* 函数名: Req_RobotWriteSerialNum
* 功能: 写序列号
* 时间: 2015/11/16 11:02
* 作者: ZDJ
*/
BOOL CRobotController::Req_RobotWriteSerialNum(char* pszSn, int nLen)
{
    SendCmdData(eRobotCmdTypeWriteSN, (LPARAM)pszSn, nLen);
    return TRUE;
}

/**************************************************************************
* 函数名: Req_RobotReadUID
* 功能: 读UID
* 时间: 2015/11/16 11:02
* 作者: ZDJ
*/
BOOL CRobotController::Req_RobotReadUID()
{
    SendCmdData(eRobotCmdTypeReadUID);
    return TRUE;
}

/**************************************************************************
* 函数名: Req_RobotGetAngle
* 功能: 读角度
* 参数:
*    @[in ] nMotorID: 舵机ID
* 时间: 2015/11/16 11:02
* 作者: ZDJ
*/
BOOL CRobotController::Req_RobotGetAngle(int nMotorID)
{
    AlphaRobotLog::getInstance()->printDebug(QString("request get robot angle of motor %1").arg(nMotorID));
    m_nCurMotorID = nMotorID;
    SendCmdData(eRobotCmdTypeGetAngle, nMotorID);
    return TRUE;
}

/**************************************************************************
* 函数名: Req_RobotSetAngle
* 功能: 设置角度
* 参数:
*    @[in ] nMotorID: 舵机ID
*    @[in ] nAngle: 角度
*    @[in ] nTime: 时间
* 时间: 2015/11/16 11:02
* 作者: ZDJ
*/
BOOL CRobotController::Req_RobotSetAngle(int nMotorID, int nAngle, int nTime)
{
    m_nCurMotorID = nMotorID;
    m_nCurMotorOffset = nAngle;
    m_nCurTime = nTime;
    SendCmdData(eRobotCmdTypeMove, nMotorID, nAngle, nTime);
    return TRUE;
}

/**************************************************************************
* 函数名: Req_RobotHardwareVersion
* 功能: 读机器人类型
* 时间: 2016/07/22 18:32
* 作者: hwx
*/
void CRobotController::Req_RobotHardwareVersion()
{
    SendCmdData(eRobotCmdTypeGetRobotHardwareVer);
}

//#pragma endregion

//#pragma region 消息响应

/**************************************************************************
* 函数名: OnReceiveChar
* 功能: 接收字符
* 参数:
*    @[in ] wParam: 接收的字符长度
*    @[in ] lParam: 接收的字符
* 返回值: 执行结果
* 时间: 2015/09/08 11:19
* 作者: ZDJ
*/
LRESULT CRobotController::OnReceiveChar(WPARAM wParam, LPARAM lParam)
{

#if 1
    int nDataLen = wParam;
    byte *pDataTemp = (byte *)lParam;
    if(!pDataTemp || nDataLen <= 0)
    {
        return false;
    }

    for(int i = 0; i < nDataLen; i++)
    {
        byte nTemp = (byte)pDataTemp[i];
        int nCmdId = m_datapool.PushData((byte)pDataTemp[i]);
        if(nCmdId > -1)
        {
            byte* databuf = NULL;
            PKGDATA* pPkg = m_datapool.GetPkg(nCmdId);
            if(pPkg)
            {
                databuf = pPkg->GetByteData();
                int nDataLen = pPkg->GetSize();
                m_datapool.ClearData(nCmdId);

                if(databuf)
                {
                    ProcessData(databuf, nDataLen);
                    SAFE_DELETE_ARRAY(databuf);
                }
            }
            else
            {
                m_datapool.ClearData(nCmdId);
            }
        }

    }

    SAFE_DELETE_ARRAY(pDataTemp);
    return TRUE;

#else
    int nCmdId = m_datapool.PushData(lParam);
    if(nCmdId > -1)
    {
        //Sleep(50);
        byte* databuf = NULL;
        PKGDATA* pPkg = m_datapool.GetPkg(nCmdId);
        if(pPkg)
        {
            databuf = pPkg->GetByteData();
            int nDataLen = pPkg->GetSize();
            m_datapool.ClearData(nCmdId);

            if(databuf)
            {
                ProcessData(databuf, nDataLen);
                SAFE_DELETE_ARRAY(databuf);
            }
        }
        else
        {
            m_datapool.ClearData(nCmdId);
        }
    }
    return TRUE;
#endif

}
//#pragma endregion

#ifdef Q_OS_WIN
/**************************************************************************
* 函数名: UpdateDevice
* 功能: 设备更新通知
* 时间: 2015/07/22 10:25
* 作者: ZDJ
*/
void CRobotController::UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam)
{
    //ASSERT(lstrlen(pDevInf->dbcc_name) > 4);
    QString szDevId = QString::fromWCharArray(pDevInf->dbcc_name+4);
    int idx = szDevId.lastIndexOf('#');
    //ASSERT( -1 != idx );
    szDevId.truncate(idx);
    szDevId.replace('#', '\\');
    szDevId.toUpper();

#ifndef QT_NO_DEBUG
    qDebug() << "szDevId:" << szDevId;
#endif

    idx = szDevId.indexOf('\\');
    //ASSERT(-1 != idx );
    QString szClass = szDevId.left(idx);

    if("ROOT" == szClass)
    {
        return;
    }

    int nTempVID = 0;
    int nTempPID = 0;
    int nTempREV = 0;

    if(szDevId.indexOf("\\VID_") != -1 && szDevId.indexOf("&PID_") != -1)
    {
        QString sTemp = szDevId.mid(szDevId.indexOf("VID_"));
        sscanf(sTemp.toStdString().c_str(), "VID_%04X&PID_%04X", &nTempVID, &nTempPID);

#ifndef QT_NO_DEBUG
        qDebug() << "sTemp:" << sTemp;
        qDebug() << "nTempVID:" << nTempVID << " nTempPID:" << nTempPID;
#endif
    }

    if((nTempVID == g_devidarr[ePortTypeHID].dwVid && nTempPID == g_devidarr[ePortTypeHID].dwPid)
        || (nTempVID == g_devidarr[ePortTypeHID + 1].dwVid && nTempPID == g_devidarr[ePortTypeHID + 1].dwPid)
        || (nTempVID == g_devidarr[ePortTypeVCP].dwVid && nTempPID == g_devidarr[ePortTypeVCP].dwPid)
        )
    {
        if(DBT_DEVICEARRIVAL == wParam )
        {
#ifndef QT_NO_DEBUG
            qDebug() << "FindMySTMDevice 1";
#endif
            if(szDevId.left(4) == "USB\\" && !FindMySTMDevice())
                return;

#ifndef QT_NO_DEBUG
            qDebug() << "FindMySTMDevice 2";
#endif

            if(m_pPort && m_pPort->IsOpen())
                return;

            //SetTimer(g_nConnectPortTimerID, TIMEDELAY_CONNECTROBOT, NULL);
            StartConncectTimer();
            qDebug("StartConncectTimer");
            //BOOL bRet = Req_AutoOpenPort();
            //NotifyObservers(WM_MSG_PORT_OPENSTATECHANGE, NULL, NULL, bRet);
        }
        else
        {
            if(!m_pPort || (m_pPort && !m_pPort->IsOpen()))
                return;

            //SetTimer(g_nDisconnectPortTimerID, TIMEDELAY_CONNECTROBOT, NULL);
            StartDisconncectTimer();

            emit sigOnDeviceRemoved();

            // USB拔出后关闭串口
            Req_ClosePort();

            qDebug("StartDisconncectTimer");
            //Req_ClosePort();
            //NotifyObservers(WM_MSG_PORT_OPENSTATECHANGE, NULL, NULL, FALSE);
        }
    }
}
#endif

/**************************************************************************
* 函数名: ProcessData
* 功能: 处理数据
* 参数:
*    @[in ] databuf: 数据缓冲区
*    @[in ] datalen: 数据长度
* 返回值: 已处理的数据长度
* 时间: 2015/07/22 10:12
* 作者: ZDJ
*/
int CRobotController::ProcessData(byte* databuf, int datalen)
{
    if(!databuf || (datalen < 0))
    {
        return -1;
    }

#ifndef QT_NO_DEBUG
//    QString sInfo = "Answ ";
//    for(int i = 0; i < datalen; i++)
//    {
//        QString sTemp;
//        sTemp.sprintf("%02X ", (byte)databuf[i]);
//        sInfo += sTemp;
//    }
//    qDebug(sInfo.toStdString().c_str());
#endif

    ///////////////////////////////////////////////Log信息////////////////////////////////////////////////
    if(g_bIsRecordLogValue)
    {
        QString sInfo = "Answ ";
        for(int i = 0; i < datalen; i++)
        {
            QString sTemp;
            sTemp.sprintf("%02X ", (byte)databuf[i]);
            sInfo += sTemp;
        }
        sInfo += '\n';
        AlphaRobotLog::getInstance()->printInfo(sInfo);
    }

    if(!m_pPort || (m_pPort && !m_pPort->IsOpen()))
    {
        return -1;
    }

    // 停止定时器
    killRobotCmdTimer();
    int nLen = GetProtocolAnswerLen(databuf[0]);
   // NotifyObservers(WM_MSG_ROBOT_TRANSDATA, NULL, datalen, (LPARAM)databuf);

   /////////////////for eRobotCmdTypeTransFileIng work fast, for do it first
//#pragma region 传输文件
    if(!m_bCancelTransFile && (
        ((m_eCurPerformRobotCmd == eRobotCmdTypeTransFilePrepare) && ((databuf[0] == 0xF8 && databuf[1] == 0x8F) || (databuf[0] == 0xFB && databuf[1] == 0xBF && databuf[3] == 0x14)))
        || ((m_eCurPerformRobotCmd == eRobotCmdTypeTransFileIng) && ((databuf[0] == 0xF7 && databuf[1] == 0x7F) || (databuf[0] == 0xFB && databuf[1] == 0xBF && databuf[3] == 0x15)))
        || ((m_eCurPerformRobotCmd == eRobotCmdTypeTransFileEnd) && ((databuf[0] == 0xF6 && databuf[1] == 0x6F) || (databuf[0] == 0xFB && databuf[1] == 0xBF && databuf[3] == 0x16)))
        ))
    {        
        processFileTransResp(databuf, datalen);
        return nLen;
    }
//#pragma endregion

/////////////////////////////////////////////////////////////////////////////////////////////////////////
    const bool bBtProtocolCmd = CBluetoothComandParser::isBTProtocol(databuf, datalen);
    int nBTCmd = eBTCmdInvalid;
    if(bBtProtocolCmd)
    {
        nBTCmd = databuf[eBTCmdPosCmd];
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//#pragma region 舵机升级
        if(((m_eCurPerformRobotCmd == eRobotCmdTypeUpgradePrepare) && (databuf[0] == 0xFC && databuf[1] == 0xCF))
            || ((m_eCurPerformRobotCmd == eRobotCmdTypeUpgradeIng) && (databuf[0] == 0xFD && databuf[1] == 0xDF))
            || ((m_eCurPerformRobotCmd == eRobotCmdTypeUpgradeEnd) && (databuf[0] == 0xFD && databuf[1] == 0xDF))
                )
        {
            processUpdateMotorResp(databuf, datalen);
            return nLen;
        }
//#pragma endregion

//#pragma region 获取动作列表
    if((m_eCurPerformRobotCmd == eRobotCmdTypeGetActionList && ROBOT_CMD_BLUE_TOOTH_START_FLAG0 == databuf[eBTCmdPosStartFlag0] &&
       ROBOT_CMD_BLUE_TOOTH_START_FLAG1 == databuf[eBTCmdPosStartFlag1] && ROBOT_CMD_BLUE_TOOTH_END_FLAG == databuf[datalen - 1])
       && (databuf[eBTCmdPosCmd] == eBTCmdGetActListIng  || databuf[eBTCmdPosCmd] == eBTCmdGetActListEnd ||
           databuf[eBTCmdPosCmd] == eBTCmdGetActList))
    {
        processRequestRobotActionList(databuf, datalen);
        return nLen;
    }
//#pragma endregion

//#pragma region 获取RES INFO
    if((ROBOT_CMD_BLUE_TOOTH_START_FLAG0 == databuf[eBTCmdPosStartFlag0] &&
        ROBOT_CMD_BLUE_TOOTH_START_FLAG1 == databuf[eBTCmdPosStartFlag1] && ROBOT_CMD_BLUE_TOOTH_END_FLAG == databuf[datalen - 1])
            && (databuf[eBTCmdPosCmd] == eBTCmdGetSomeTypeResInfo  || databuf[eBTCmdPosCmd] == eBTCmdGetSomeTypeResInfoIng ||
                databuf[eBTCmdPosCmd] == eBTCmdGetSomeTypeResInfoEnd))
    {
        processGetSomeResInfos(databuf, datalen);
        return nLen;
    }
//#pragma endregion


//#pragma region 取消文件传输
    if (m_eCurPerformRobotCmd == eRobotCmdTypeTransFileCancel && (databuf[0] == 0xFB && databuf[1] == 0xBF && databuf[eBTCmdPosCmd] == eBTCmdTransFileCancel))
    {
        processCancelFileTransResp(databuf, datalen);
    }
//#pragma endregion

#if 1
//#pragma region 读偏移和设置偏移
    if(databuf[0] == 0xFA && databuf[1] == 0xAF)
    {
        processMotorOffsetResp(databuf, datalen);
    }
//#pragma endregion

    //get angle
    if(m_eCurPerformRobotCmd == eRobotCmdTypeGetAngle)
    {
        processGetMotorAngleResp(databuf, datalen);
    }

#endif

#if 1
//#pragma region 获取ID号
    if((m_eCurPerformRobotCmd == eRobotCmdTypeGetID) && databuf[0] == 0xFA && databuf[1] == 0xAF)
    {
        processGetMotorIDResp(databuf, datalen);
    }
//#pragma endregion
#endif

//#pragma region 获取舵机版本
    if(m_eCurPerformRobotCmd == eRobotCmdTypeGetMotorVersion)
    {
        processGetMotorVersionResp(databuf, datalen);
    }
//#pragma endregion

//#pragma region 获取机器人软件版本
   // if(m_eCurPerformRobotCmd == eRobotCmdTypeGetRobotSysEmbeddedVersion)//
    if((bBtProtocolCmd && (eBTCmdGetRobotSofeVer == nBTCmd))
            || (!bBtProtocolCmd && (databuf[0] == 0xF5) && (databuf[1] == 0x5F)))
    {
        processGetRobotSoftwareVersionResp(databuf,datalen);
    }
//#pragma endregion

//#pragma region 获取机器人硬件版本
    //if(eRobotCmdTypeGetRobotHardwareVer == m_eCurPerformRobotCmd /*&&( databuf[0] == 0xF1 && databuf[1] == 0x1F)*/)
    {
        processGetRobotTypeResp(databuf, datalen);
    }
//#pragma endregion

//#pragma region
    if((eRobotCmdTypeGetBatteryStatus == m_eCurPerformRobotCmd)
            && (ROBOT_CMD_BLUE_TOOTH_START_FLAG0 == databuf[eBTCmdPosStartFlag0])
            && (ROBOT_CMD_BLUE_TOOTH_START_FLAG1 == databuf[eBTCmdPosStartFlag1])
            && (eBTCmdGetBatteryStatus == databuf[eBTCmdPosCmd]))
    {
        processGetBatteryResp(databuf, datalen);
    }
//#pragma endregion

//#pragma region 连接机器人
    if((m_eCurPerformRobotCmd == eRobotCmdTypeConnectRobot)
            ||(m_eCurPerformRobotCmd == eRobotCmdTypeDisconnectRobot))
    {
        processRobotConnectStatusResp(databuf, datalen);
    }
//#pragma endregion

//#pragma region  读取设备状态
    if ((ROBOT_CMD_BLUE_TOOTH_START_FLAG0 == databuf[eBTCmdPosStartFlag0])
            && (ROBOT_CMD_BLUE_TOOTH_START_FLAG1 == databuf[eBTCmdPosStartFlag1])
            && (eBTCmdGetDriveStatus == databuf[eBTCmdPosCmd]))
    {
        processRobotDeviceStatusResp(databuf, datalen);
    }
//#pragma endregion

//#pragma region 读写序列号
    if(databuf[0] == 0xFB && databuf[1] == 0xBF)
    {
        processReadRobotSNResp(databuf, datalen);
    }
//#pragma endregion

//#pragma region 切换模式
    if(databuf[0] == 0xF9 && databuf[1] == 0x9F)
    {
        processRobotSwitchModeResp(databuf, datalen);
    }
//#pragma endregion

//#pragma region 删除机器人动作
    if((m_eCurPerformRobotCmd == eRobotCmdTypeDeleteAction)
       && (ROBOT_CMD_BLUE_TOOTH_START_FLAG0 == databuf[eBTCmdPosStartFlag0])
       && (ROBOT_CMD_BLUE_TOOTH_START_FLAG1 == databuf[eBTCmdPosStartFlag1])
       && (eBTCmdDeleteFile == databuf[eBTCmdPosCmd]))
    {
        processDeleteRobotActionResp(databuf, datalen);
    }
//#pragma endregion

//#pragma region 设置时间
    if(m_eCurPerformRobotCmd == eRobotCmdTypeSetTime)
    {
        processSetTimeResp(databuf, datalen);
    }
//#pragma endregion


    // eRbootCmdTypeWifiScan
    if(m_eCurPerformRobotCmd == eRobotCmdTypeWifiScan)
    {
        bool bOk = processWifiScan(databuf, datalen);
        return nLen; // neet to read more info from robot
    }

    if(m_eCurPerformRobotCmd == eRobotCmdTypeConnectWifi)
    {
        bool bOk = processRobotConnectWifi(databuf, datalen);
        return nLen;// neet to read more info from robot
    }

    if(eRobotCmdTypeManalUpGrade ==  m_eCurPerformRobotCmd)
    {
        bool bOk = processRobotManalUpGrade(databuf, datalen);
    }

   // if(eRobotCmdTypeGetAutoUpdadeProgress ==  m_eCurPerformRobotCmd)
    {
        bool bOk = processRobotGetAutoUpdadeProgress(databuf, datalen);
    }

   // if(eRobotCmdTypeIsInstallingUpdate ==  m_eCurPerformRobotCmd)
    {
        bool bOk = processRobotIsInstallingUpdate(databuf, datalen);
    }

    ///////////////////////////////////////////////////////////////////////////////////
    // 除了文件传输需要等待传输最后一帧数据才
    // 认为命令结束，其他传输传完后可以认为传输完成
    setCurrentPerformRobotCmd(eRobotCmdTypeNone);

    return nLen == -1 ? datalen:nLen;//没找对对应协议的长度则处理完本次数据
}

void CRobotController::processFileTransResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    if((!IsBlueToothCmd(databuf, datalen) && databuf[2] == 0xAA)
            || (IsBlueToothCmd(databuf, datalen) && databuf[4] == 0x00))
    {
        resetData();
    }

    //static BOOL bCurFileSendComplete = TRUE;//当前文件是否发送完成

    if(IsSupportNewProtocol())
    {
        if(g_nBigFrameDelay > 0)
        {
            AlphaRobotLog::getInstance()->printDebug(QString("big frame delay: %1").arg(g_nBigFrameDelay));
            Sleep(g_nBigFrameDelay); //延时10ms
        }
    }

    if(( !IsBlueToothCmd(databuf, datalen) && databuf[2] == 0xAA)
            || (IsBlueToothCmd(databuf, datalen) && databuf[4] == 0x00))
    {
        int framesize = TRANSFILE_FRAMESIZE_OLD;
        if(IsSupportNewProtocol())
        {
            framesize = RobotCmdBTProtocol::m_nTransFilePerFrameSize;//TRANSFILE_FRAMESIZE_NEW_FOR_128
        }

        byte* curdata = new byte[framesize];
        memset(curdata, 0, framesize);

        if(m_eCurPerformRobotCmd == eRobotCmdTypeTransFileIng || m_eCurPerformRobotCmd == eRobotCmdTypeTransFilePrepare )
        {
            static int nProgressOld = 0;
            if(m_eCurPerformRobotCmd == eRobotCmdTypeTransFilePrepare)
            {
                nProgressOld = 0;
            }

            if(m_STCurSendData.m_nCurFrame * framesize < m_STCurSendData.m_senddata.datalen)
            {
                m_nTransFileFrameSize = framesize;

                //传输数据中
                memcpy(curdata, m_STCurSendData.m_senddata.databuf + (m_STCurSendData.m_nCurFrame-1)*framesize, framesize);

                int nCurFrame = m_STCurSendData.m_nCurFrame;
                if(!IsSupportNewProtocol())
                {
                    SendCmdData(eRobotCmdTypeTransFileIng, (LPARAM)curdata, (LPARAM)nCurFrame, NULL, NULL, false);
                }
                else
                {
                    g_bIsRecordLogValue = false;
                    SendCmdData(eRobotCmdTypeTransFileIng, (LPARAM)nCurFrame, (LPARAM)curdata, (LPARAM)framesize, NULL, false);
                }

                m_STCurSendData.m_nCurFrame++;
                m_STCurSendData.m_nCurSendSize += framesize;
            } // 还没发送完数据
            else
            {
                int copysize = m_STCurSendData.m_senddata.datalen - (m_STCurSendData.m_nCurFrame-1)*framesize;
                if(copysize > 0)
                {
                    //传输结束命令
                    memcpy(curdata, m_STCurSendData.m_senddata.databuf + (m_STCurSendData.m_nCurFrame-1)*framesize, copysize);

                    int nCurFrame = m_STCurSendData.m_nCurFrame;
                    if(!IsSupportNewProtocol())
                    {
                        SendCmdData(eRobotCmdTypeTransFileEnd, (LPARAM)curdata, (LPARAM)copysize, NULL, NULL, false);
                    }
                    else
                    {                                        
                        CIniConfig::getInstance().getValueIsLogEnable(g_bIsRecordLogValue);
                        SendCmdData(eRobotCmdTypeTransFileEnd, (LPARAM)nCurFrame, (LPARAM)curdata, (LPARAM)copysize, NULL, false);
                    }

                    m_bCurFileSendComplete = FALSE;

                    m_STCurSendData.m_nCurFrame++;
                    m_STCurSendData.m_nCurSendSize += copysize;
                    m_nTransFileFrameSize = copysize;  //当前发送的帧数据长度
                }
            }  // eRobotCmdTypeTransFileEnd

            // 动作下载
            if (emTranFileSynAction == m_STCurSendData.m_eType)
            {
                //for not fresh so much
                int lTotolSize = m_STCurSendData.m_senddata.datalen;   //计划发送的当前文件长度
                int lAllSendedSize = m_STCurSendData.m_nCurSendSize; //当前文件已经发送的数据长度
                int nProgressNew = lAllSendedSize * 1.0 / lTotolSize * 100;
                if((nProgressNew - nProgressOld) > 20 )
                {
                    nProgressOld = nProgressNew;
                    NotifyObservers(WM_MSG_ACTION_DOWNLOAD_ING, NULL, TRUE, (LPARAM)&m_STCurSendData);//通知下载进度
                }
            }
            else if (emTranFileUpdateSysEmbed == m_STCurSendData.m_eType) // 下载版本更新文件
            {
                NotifyObservers(WM_MSG_UPDATE_DOWNLOAD_ING, NULL, TRUE, (LPARAM)&m_STCurSendData);
            }
            m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_TRANSFILE_ING, Qt::PreciseTimer);
        }
        else if(m_eCurPerformRobotCmd == eRobotCmdTypeTransFileEnd)
        {
            CIniConfig::getInstance().getValueIsLogEnable(g_bIsRecordLogValue);


            if(!m_bCurFileSendComplete)
            {
                if(m_bHasResendRequest)
                {
                    qDebug("has resend request\n");
                    AlphaRobotLog::getInstance()->printDebug("has resend request\n");
                    m_bHasResendRequest = false;
                }

                //动作下载完成通知
                qDebug("trans file successful\n");
                AlphaRobotLog::getInstance()->printDebug("trans file successful\n");

                ///////////////////////////////////////////////////////////////////////////
                // 由于在对WM_MSG_ACTION_DOWNLOAD_END
                // 消息处理中会继续传输下一个文件，因此需要在发送
                // WM_MSG_ACTION_DOWNLOAD_END消息前设置
                m_bCurFileSendComplete = TRUE;
                //g_cursenddata.Reset();
                setCurrentPerformRobotCmd(eRobotCmdTypeNone);
                ///////////////////////////////////////////////////////////////////////////

                if (emTranFileSynAction == m_STCurSendData.m_eType)
                {
                    NotifyObservers(WM_MSG_ACTION_DOWNLOAD_END, NULL, TRUE, (LPARAM)&m_STCurSendData);//通知下载进度
                }
                else if(emTranFileUpdateSysEmbed == m_STCurSendData.m_eType)   // 更新文件下载结束
                {
                    NotifyObservers(WM_MSG_UPDATE_DOWNLOAD_END, NULL, TRUE, (LPARAM)&m_STCurSendData);
                }
            }
        }
    }

    // 错误应答
    else if((!IsBlueToothCmd(databuf, datalen))
            || (IsBlueToothCmd(databuf, datalen)))
    {
        CIniConfig::getInstance().getValueIsLogEnable(g_bIsRecordLogValue);

        AlphaRobotLog::getInstance()->printError("File trans response error");

        // 文件开始传输，如果返回0A，说明内存卡容量已满
        if (m_eCurPerformRobotCmd == eRobotCmdTypeTransFilePrepare && IsBlueToothCmd(databuf, datalen) && databuf[4] == eBTCmdRspTFCardFull)
        {
            if (emTranFileSynAction == m_STCurSendData.m_eType)
            {
                NotifyObservers(WM_MSG_TFCARD_NO_CAPACITY, NULL, TRUE, FALSE);
            }
            else if (emTranFileUpdateSysEmbed == m_STCurSendData.m_eType) // 下载版本更新文件
            {
                NotifyObservers(WM_MSG_TFCARD_NO_CAPACITY, NULL, FALSE, TRUE);
            }
        }

        if (m_eCurPerformRobotCmd == eRobotCmdTypeTransFilePrepare || m_eCurPerformRobotCmd == eRobotCmdTypeTransFileIng)
        {
            // 动作下载
            if (emTranFileSynAction == m_STCurSendData.m_eType)
            {

                 NotifyObservers(WM_MSG_ACTION_DOWNLOAD_ING, NULL, FALSE, (LPARAM)&m_STCurSendData);//通知下载进度
            }
            else if (emTranFileUpdateSysEmbed == m_STCurSendData.m_eType) // 下载版本更新文件
            {
                NotifyObservers(WM_MSG_UPDATE_DOWNLOAD_ING, NULL, FALSE, (LPARAM)&m_STCurSendData);
            }
        }
        else if (m_eCurPerformRobotCmd == eRobotCmdTypeTransFileEnd)
        {
            if (emTranFileSynAction == m_STCurSendData.m_eType)
            {
                NotifyObservers(WM_MSG_ACTION_DOWNLOAD_END, NULL, FALSE, (LPARAM)&m_STCurSendData);//通知下载进度
            }
            else if(emTranFileUpdateSysEmbed == m_STCurSendData.m_eType)   // 更新文件下载结束
            {
                NotifyObservers(WM_MSG_UPDATE_DOWNLOAD_END, NULL, FALSE, (LPARAM)&m_STCurSendData);
            }
        }
        setCurrentPerformRobotCmd(eRobotCmdTypeNone);
#if 0
        if(g_bHasResendRequest)
        {
            qDebug("has resend request\n");
        }

        g_cursenddata.Reset();
        g_bBusy = FALSE;
        qDebug("trans file failed\n");

        NotifyObservers(WM_MSG_ACTION_DOWNLOAD_END, NULL, FALSE, (LPARAM)&g_cursenddata);//通知下载结果
#endif
    }
}

void CRobotController::processRequestRobotActionList(byte *databuf, int nDataLen)
{
    // 正在传输动作名称
    if (databuf[eBTCmdPosCmd] == eBTCmdGetActListIng)
    {
        byte *byteActionName = new byte[nDataLen - 5];
        memset(byteActionName, 0, nDataLen - 5);
        memcpy(byteActionName, databuf + 4, nDataLen - 6);

        // 转为UTF8格式
        char *pUTF8ActionName = NULL;
        int nLen = AnsiToUtf8((char *)byteActionName, pUTF8ActionName);

        SAFE_DELETE(byteActionName);

        if (nLen <= 0 || !pUTF8ActionName)
        {
            return;
        }

        AlphaRobotLog::getInstance()->printDebug(QString("Robot controller get action %1").arg(QString(pUTF8ActionName)));

//            //////////////////////////////////////////////Log/////////////////////////////////////
//            for (int n = 0; n < nDataLen - 6; n++)
//            {
//            QString  sReceive;
//            sReceive.sprintf("%02X", (byte) byteActionName[n]);
//            QString strLog = QString("get byte: %1").arg(sReceive);
//            AlphaRobotLog::getInstance()->printError(strLog);
//            }
//            ///////////////////////////////////////////////////////////////////////////////////////

        NotifyObservers(WM_MSG_GET_ROBOT_ACTION_LIST_RST, NULL, TRUE, (LPARAM)pUTF8ActionName);
    }

    // 传输动作名称结束
    else if (databuf[eBTCmdPosCmd] == eBTCmdGetActListEnd)
    {
        setCurrentPerformRobotCmd(eRobotCmdTypeNone);
        NotifyObservers(WM_MSG_GET_ROBOT_ACTION_LIST_RST, NULL, TRUE, NULL);
    }
}

bool CRobotController::processGetSomeResInfos(byte *databuf, int nDataLen)
{
    if( !CBluetoothComandParser::isBTProtocol(databuf, nDataLen))
    {
        return false;
    }

    if ( (databuf[eBTCmdPosCmd] != eBTCmdGetSomeTypeResInfo)
         && (databuf[eBTCmdPosCmd] != eBTCmdGetSomeTypeResInfoIng)
         && (databuf[eBTCmdPosCmd] != eBTCmdGetSomeTypeResInfoEnd))
    {
           return false;
    }

    // 不重发
    resetData();

    const int nResInfoType = databuf[eBTCmdPosParam1];

    // 传输动作名称
    if(nResInfoType == eResInfoTypeActList)
    {
        if (databuf[eBTCmdPosCmd] == eBTCmdGetSomeTypeResInfoIng)
        {            
            const int nInfoLen = databuf[eBTCmdPosCmdLength] - 6;
            char* pInfo = new char[nInfoLen + 1];
            memset(pInfo, 0, nInfoLen + 1);
            memcpy(pInfo, (char*)(&databuf[eBTCmdPosParam2]), nInfoLen);

            QString strJson(pInfo);
            QString strName = CJsonUtil::getValue(strJson, "path");

            SAFE_DELETE(pInfo);

            //// 转为UTF8格式 not neet to change

            if(!strName.isEmpty())
            {
                int nLenStr = strName.toStdString().length() + 1;
                char* pInfoSend = new char[nLenStr];
                memset(pInfoSend, 0, nLenStr);
                memcpy(pInfoSend, (char*)strName.toStdString().c_str(), strName.toStdString().length());

                AlphaRobotLog::getInstance()->printDebug(QString("Robot controller get action %1").arg(strName));
                NotifyObservers(WM_MSG_GET_ROBOT_ACTION_LIST_RST, NULL, TRUE, (LPARAM)pInfoSend);
            }

            setCurrentPerformRobotCmd(eRobotCmdTypeNone);
            Req_RobotResInfos(eResInfoTypeActList, eResInfoCmdFlagContinueGet);
        }
        else if(databuf[eBTCmdPosCmd] == eBTCmdGetSomeTypeResInfoEnd)
        {
            setCurrentPerformRobotCmd(eRobotCmdTypeNone);
            NotifyObservers(WM_MSG_GET_ROBOT_ACTION_LIST_RST, NULL, TRUE, NULL);
        }
    }
}

void CRobotController::processCancelFileTransResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    if (databuf[4] == 0x00)  // 取消文件传输成功
    {
        NotifyObservers(WM_MSG_TRANS_FILE_CANCEL, NULL, true, NULL);
    }
    else
    {
        NotifyObservers(WM_MSG_TRANS_FILE_CANCEL, NULL, false, NULL);
    }
}

void CRobotController::processUpdateMotorResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    if(databuf[3] == 0xAA)
    {
        resetData();
    }
    static BOOL bCurFileSendComplete = TRUE;//当前文件是否发送完成

    if(databuf[3] == 0xAA)
    {
        const int framesize = DEFAULT_UPDATE_FRAME_SIZE;
        byte curdata[framesize] = {0};

        if(m_eCurPerformRobotCmd == eRobotCmdTypeUpgradePrepare || m_eCurPerformRobotCmd == eRobotCmdTypeUpgradeIng)
        {
            if(m_STCurSendData.m_nCurFrame * framesize - m_STCurSendData.m_senddata.datalen < framesize)
            {
                setCurrentPerformRobotCmd(eRobotCmdTypeUpgradeIng);

                //传输数据中
                memcpy(curdata, m_STCurSendData.m_senddata.databuf + (m_STCurSendData.m_nCurFrame-1)*framesize, framesize);

                int sendsize = 0;
                byte* sendbuf = m_pRobotCmd->GetCmd_MotorUpgradeIng(m_nCurMotorID, m_STCurSendData.m_nCurFrame-1, curdata, framesize, sendsize);

                m_STCurSendData.m_nCurFrame++;
                m_STCurSendData.m_nCurSendSize += framesize;

                if(m_pPort && m_pPort->IsOpen())
                    m_pPort->SendData(sendbuf, sendsize);
            }
            else
            {
                //传输结束命令
                setCurrentPerformRobotCmd(eRobotCmdTypeUpgradeEnd);

                int sendsize = 0;
                bCurFileSendComplete = FALSE;

                byte* sendbuf = m_pRobotCmd->GetCmd_MotorUpgradeEnd(m_nCurMotorID, m_STCurSendData.m_nCurFrame-1, m_STCurSendData.m_senddata.databuf, m_STCurSendData.m_senddata.datalen,sendsize);

                m_STCurSendData.m_nCurFrame++;
                m_STCurSendData.m_nCurSendSize += framesize;

                if(m_pPort && m_pPort->IsOpen())
                    m_pPort->SendData(sendbuf, sendsize);
            }

            NotifyObservers(WM_MSG_MOTOR_UPGRADE_ING, NULL, NULL, (LPARAM)&m_STCurSendData);//通知下载进度
            m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_UPDATE_ING, Qt::PreciseTimer);
        }
        else if(m_eCurPerformRobotCmd == eRobotCmdTypeUpgradeEnd)
        {
            if(!bCurFileSendComplete)
            {
                if(m_bHasResendRequest)
                {
                    qDebug("has resend request\n");
                    m_bHasResendRequest = false;
                }

                bCurFileSendComplete = TRUE;
                setCurrentPerformRobotCmd(eRobotCmdTypeNone);
                m_STCurSendData.Reset();

                //升级完成

                qDebug("upgrade motor successful\n");
                NotifyObservers(WM_MSG_MOTOR_UPGRADE_END, NULL, TRUE, (LPARAM)&m_STCurSendData);//通知升级结果

            }
        }
    }
    else if(databuf[3] == 0xEE)
    {
#if 0
        if(g_bHasResendRequest)
        {
            qDebug("has resend request\n");
        }

        g_cursenddata.Reset();
        g_bBusy = FALSE;

        qDebug("upgrade failed\n");

        NotifyObservers(WM_MSG_MOTOR_UPGRADE_END, NULL, FALSE, (LPARAM)&g_cursenddata);//通知升级结果
#endif
    }
}

void CRobotController::processMotorOffsetResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    if(m_eCurPerformRobotCmd == eRobotCmdTypeGetOffset)
    {
        if(databuf[3] == 0xAA)
        {
            resetData();

            //获取偏移值
            QString sTemp2;

            short tempoffset = 0;
            memcpy(&tempoffset, &databuf[7], 1);
            memcpy((byte*)&tempoffset + 1, &databuf[6], 1);

            //g_nCurOffset = tempoffset;

            sTemp2.sprintf("%d", tempoffset);
            //m_editOffset.SetWindowText(sTemp2);
            NotifyObservers(WM_MSG_MOTOR_OFFSET_GET, NULL, TRUE, tempoffset);
        }
        else if(databuf[3] == 0xEE)
        {
            //m_editOffset.SetWindowText(_T(""));
            //AfxMessageBox(LANGUAGE_LOAD(MOTORINFO_OFFSET_GET_FAILED_TEXT));
            NotifyObservers(WM_MSG_MOTOR_OFFSET_GET, NULL, FALSE, NULL);
        }
    }
    else if(m_eCurPerformRobotCmd == eRobotCmdTypeSetOffset)
    {
        if(databuf[3] == 0xAA)
        {
            resetData();
            //g_nCurOffset = g_nTempSetOffset;
            //Sleep(100);
            //SendCmdData(eRobotCmdTypeGetOffset, m_nCurMotorID);
            //AfxMessageBox(LANGUAGE_LOAD(MOTORINFO_OFFSET_SET_SUCCESS_TEXT), MB_OK|MB_ICONINFORMATION);
            NotifyObservers(WM_MSG_MOTOR_OFFSET_SET, NULL, TRUE, NULL);
        }
        else if(databuf[3] == 0xEE)
        {
            //AfxMessageBox(LANGUAGE_LOAD(MOTORINFO_OFFSET_SET_FAILED_TEXT));
            NotifyObservers(WM_MSG_MOTOR_OFFSET_SET, NULL, FALSE, NULL);
        }

        //CString sTemp;
        //sTemp.Format(_T("%d"), g_nCurOffset);
        //m_editOffset.SetWindowText(sTemp);
    }
    else if(m_eCurPerformRobotCmd == eRobotCmdTypeMove)
    {
        if(databuf[4] == 0xAA)
        {
            resetData();
            //SendCmdData(eRobotCmdTypeGetOffset, m_nCurMotorID);
//				CString sCurAngle;
//				m_editCurAngle.GetWindowText(sCurAngle);

//				int nTempOffset = (_tstoi(sCurAngle) - g_nBaseAngle) * 3 + g_nCurOffset;

//				CString sCurOffset;
//				sCurOffset.Format(_T("%d"), nTempOffset);
//				m_editOffset.SetWindowText(sCurOffset);
        }
        else if(databuf[4] == 0xEE)
        {
            //OutputDebugString(LANGUAGE_LOAD(MOTORINFO_MOTORMOVE_ERROR_TEXT) + _T("\n"));
        }
    }

    //#pragma region 设置ID号
    else if(m_eCurPerformRobotCmd == eRobotCmdTypeSetID)
    {
        if(databuf[3] == 0xAA)
        {
            resetData();
            m_nCurMotorID = databuf[2];

            //设置ID成功
            NotifyObservers(WM_MSG_MOTOR_MODID, NULL, TRUE, m_nCurMotorID);
        }
        else if(databuf[3] == 0xEE)
        {
            //设置ID失败
            NotifyObservers(WM_MSG_MOTOR_MODID, NULL, FALSE, NULL);
        }
//                }
    }
}

bool CRobotController::processGetMotorAngleResp(byte *databuf, int datalen)
{    //获取舵机角度
    if (!databuf || datalen <= 0)
    {
        return false;
    }

    if(databuf[0] == 0xFA && databuf[1] == 0xAF)
    {
        if(databuf[3] == 0xAA)
        {
            resetData();

            //获取角度值
            short tempangle = 0;
            memcpy(&tempangle, &databuf[7], 1);
            memcpy((byte*)&tempangle + 1, &databuf[6], 1);//符号位

            NotifyObservers(WM_MSG_MOTOR_ANGLE_GET, NULL, TRUE, tempangle);//获取舵机角度成功
        }
        else
        {
            NotifyObservers(WM_MSG_MOTOR_ANGLE_GET, NULL, FALSE, NULL);  //获取舵机角度失败
        }

        return true;
    }

    if(CBluetoothComandParser::isBTProtocol(databuf, datalen))
    {
        int nAngle = -1;
        CDataOperate::getValue(&databuf[eBTCmdPosFirstParamStart + 1], 1, nAngle, 1);
        if(nAngle >=0 && nAngle < 0xfe)
        {
            resetData();
            NotifyObservers(WM_MSG_MOTOR_ANGLE_GET, NULL, TRUE, nAngle);//获取舵机角度成功
        }
        else
        {
            NotifyObservers(WM_MSG_MOTOR_ANGLE_GET, NULL, FALSE, NULL);  //获取舵机角度失败
        }
        return true;
    }

    return false;
}

void CRobotController::processGetMotorIDResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    if(databuf[3] == 0xAA)
    {
        resetData();
        m_nCurMotorID = databuf[2];//保存舵机号

        NotifyObservers(WM_MSG_MOTOR_GETID, NULL, TRUE, m_nCurMotorID);
    }
    else if(databuf[3] == 0xEE)
    {
        NotifyObservers(WM_MSG_MOTOR_GETID, NULL, FALSE, NULL);
    }
}

void CRobotController::processGetMotorVersionResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    if(databuf[0] == 0xFC && databuf[1] == 0xCF)
    {
        if(databuf[3] == 0xAA)
        {
            resetData();
            NotifyObservers(WM_MSG_MOTOR_READVERSION, NULL, TRUE, (LPARAM)&databuf[4]);//读版本成功
        }
        else if(databuf[3] == 0xEE)
        {
            NotifyObservers(WM_MSG_MOTOR_READVERSION, NULL, FALSE, NULL);//读版本出错
        }
    }
}

void CRobotController::processGetRobotSoftwareVersionResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    const bool bBTProtocol = CBluetoothComandParser::isBTProtocol(databuf, datalen);
    if(!bBTProtocol)
    {
        if((databuf[0] == 0xF5) && (databuf[1] == 0x5F))
        {
            if(databuf[2] == 0xAA)
            {
                char szVer[11] = {0};
                memcpy(szVer, databuf + 3, 10);

                strcpy(m_szRobotSoftVersion, szVer);

        //            if(IsSupportNewProtocol())
        //                g_nWaitReSendTime = 2000;
                resetData();
                NotifyObservers(WM_MSG_ROBOT_SOFTWARE_VERSION_GET, NULL, sizeof(szVer), (LPARAM)&m_szRobotSoftVersion);//读版本成功
            }
            else
            {
                NotifyObservers(WM_MSG_ROBOT_SOFTWARE_VERSION_GET, NULL, 0, NULL);//读版本失败
            }
        }
    }

    if(bBTProtocol)
    {
        const int nBTCmd = databuf[eBTCmdPosCmd];
        if(nBTCmd == eBTCmdGetRobotSofeVer)
        {
            byte* pRamamsHead = CBluetoothComandParser::getParamsPoidHead(databuf, datalen);
            int nLenParams = CBluetoothComandParser::getParamsLen(databuf, datalen);
            if(pRamamsHead && (nLenParams > 0))
            {
                memcpy(m_szRobotSoftVersion, pRamamsHead, nLenParams);

                resetData();
                NotifyObservers(WM_MSG_ROBOT_SOFTWARE_VERSION_GET, NULL, nLenParams, (LPARAM)&m_szRobotSoftVersion);//读版本成功
            }
            else
            {
                NotifyObservers(WM_MSG_ROBOT_SOFTWARE_VERSION_GET, NULL, 0, NULL);//读版本失败
            }
        }
    }
}

void CRobotController::processGetRobotTypeResp(byte *databuf, int datalen)
{
    //g_bHasHardwareVerinfo = TRUE;//获取到硬件版本

    if (!databuf || datalen <= 0)
    {
        return;
    }    

    const bool bBTProtocol = CBluetoothComandParser::isBTProtocol(databuf, datalen);
    if(!bBTProtocol)
    {
        if((databuf[0] == 0xF1) && (databuf[1] == 0x1F))
        {
            byte strsize = databuf[2];
            char* pstr = new char[strsize + 1];
            memset(pstr, 0, strsize+1);
            memcpy(pstr, databuf+3, strsize);

        //		int nRobotType = 0;  //机器人产品类型

        //        string stemp = pstr;
        //        std::transform(stemp.begin(), stemp.end(), stemp.begin(), ::tolower);

        //        if(stemp.find("alpha1s_v") != -1)
        //            nRobotType = CheckUpdateTypeNewRobot1S;//新版1S
        //        else if(stemp.find("alpha1_v2.0") != -1)
        //            nRobotType = CheckUpdateTypeNewRobot1SV2;//新版1SV2
        //        else if(stemp.find("alpha1p_v") != -1)
        //            nRobotType = CheckUpdateTypeRobot1P;//1P
        //        else
        //            nRobotType = CheckUpdateTypeOldRobot1S;//旧版1S

        //		SAFE_DELETE_ARRAY(pstr);

            resetData();
            NotifyObservers(WM_MSG_ROBOT_HARDWARE_VERSION_GET, NULL, true, (LPARAM)pstr);//读版本成功
        }
    }

    if(bBTProtocol)
    {
        const int nBTCmd = databuf[eBTCmdPosCmd];
        if(nBTCmd == eBTCmdGetBootHardWareVersion)
        {
            byte* pRamamsHead = CBluetoothComandParser::getParamsPoidHead(databuf, datalen);
            int nLenParams = CBluetoothComandParser::getParamsLen(databuf, datalen);
            if(pRamamsHead && (nLenParams > 0))
            {
                char* pstr = new char[nLenParams];
                memset(pstr, 0, nLenParams);
                memcpy(pstr, pRamamsHead, nLenParams);

                resetData();
                NotifyObservers(WM_MSG_ROBOT_HARDWARE_VERSION_GET, NULL, true, (LPARAM)pstr);//读版本成功
            }
        }
    }
}

void CRobotController::processGetBatteryResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    byte strsize = databuf[eBTCmdPosCmdLength];
    int nSize = sizeof(m_szRobotBatteryStatus);
    memset(m_szRobotBatteryStatus, 0, nSize);
    memcpy(m_szRobotBatteryStatus, databuf, strsize);
    resetData();
    NotifyObservers(WM_MSG_ROBOT_BATTERY_STATUS_GET, NULL, TRUE, (LPARAM)&m_szRobotBatteryStatus);
}

void CRobotController::processRobotConnectStatusResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    if(databuf[0] == 0xF4 && databuf[1] == 0x4F)
    {
        if(m_eCurPerformRobotCmd == eRobotCmdTypeConnectRobot)
        {
    //        if(databuf[2] == 0xAA)
    //        {
    //            NotifyObservers(WM_MSG_ROBOT_CONNECT, NULL, TRUE, NULL);
    //            resetData();
    //            //SendCmdData(eRobotCmdTypeCheckUtf8);//请求检查是否支持UTF8
    ////                Req_RobotSysEmbeddedVersion();
    //        }
    //        else
    //        {
    //            NotifyObservers(WM_MSG_ROBOT_CONNECT, NULL, FALSE, NULL);
    //        }

            if(databuf[2] == 0xAA)//if(databuf[3] == 0x01)///
            {
                NotifyObservers(WM_MSG_ROBOT_CONNECT, NULL, TRUE, NULL);
                resetData();
                //SendCmdData(eRobotCmdTypeCheckUtf8);//请求检查是否支持UTF8
                //                Req_RobotSysEmbeddedVersion();

// 2016103001固件版本即之前的版本都存在问题，暂时不设置时间，设置时间后
// 通信会存在问题，连接断开等操作会无应答
#if 0
                // 连接上机器后进行时间校准
                QDateTime *pCurDateTime = new QDateTime(QDateTime::currentDateTime());
                Req_SetTime(pCurDateTime);
                SAFE_DELETE(pCurDateTime);
#endif
            }
            else
            {
                NotifyObservers(WM_MSG_ROBOT_CONNECT, NULL, FALSE, NULL);
            }
        }

        else if(m_eCurPerformRobotCmd == eRobotCmdTypeDisconnectRobot)
        {
            if(databuf[2] == 0xAA)
            {
                NotifyObservers(WM_MSG_ROBOT_DISCONNECT, NULL, TRUE, NULL);//
                resetData();
            }
            else
            {
                NotifyObservers(WM_MSG_ROBOT_DISCONNECT, NULL, FALSE, NULL);
            }

            // 收到断开连接应答后再关闭串口
            Req_ClosePort();
        }
    }

    if(CBluetoothComandParser::isBTProtocol(databuf, datalen))
    {
        int nBTCmd = databuf[eBTCmdPosCmd];
        if(m_eCurPerformRobotCmd == eRobotCmdTypeConnectRobot)
        {
            if(nBTCmd == eBTCmdHandshake)
            {
                NotifyObservers(WM_MSG_ROBOT_CONNECT, NULL, TRUE, NULL);
                resetData();
            }
//            else
//            {
//                NotifyObservers(WM_MSG_ROBOT_CONNECT, NULL, FALSE, NULL);
//            }
        }
        else if(m_eCurPerformRobotCmd == eRobotCmdTypeDisconnectRobot)
        {
            NotifyObservers(WM_MSG_ROBOT_DISCONNECT, NULL, FALSE, NULL);
            // 收到断开连接应答后再关闭串口
            Req_ClosePort();
        }
    }
}

void CRobotController::processRobotDeviceStatusResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    if (databuf[4] == 0x04)  // TF卡状态
    {
        if (databuf[5] == 0x01)  // TF卡插入状态
        {
            Req_RobotSysEmbeddedVersion();
            resetData();

            // 由于全局函数和类成员函数同名，这里需要调用的是全局函数
            ::NotifyObservers(WM_MSG_ROBOT_TF_CARD_STATUS, NULL, true, NULL);
        }
        else  // TF卡拨出状态
        {
            ::NotifyObservers(WM_MSG_ROBOT_TF_CARD_STATUS, NULL, false, NULL);
        }
    }
}

void CRobotController::processReadRobotSNResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    if(IsBlueToothCmd((byte*)databuf, datalen))
    {
        if(m_eCurPerformRobotCmd == eRobotCmdTypeReadSN && databuf[eBTCmdPosCmd] == eBTCmdReadWriteSN)
        {
            int nSnLen = (databuf[2] + 1) - 7;
            byte* pSnBuf = (byte*)databuf + 5;
            resetData();
            NotifyObservers(WM_MSG_ROBOT_READSN_RESULT, NULL, nSnLen, (LPARAM)pSnBuf);
        }
        else if(m_eCurPerformRobotCmd == eRobotCmdTypeWriteSN && databuf[eBTCmdPosCmd] == eBTCmdReadWriteSN)
        {
            int nSnLen = (databuf[2] + 1) - 7;
            byte* pSnBuf = (byte*)databuf + 5;
            resetData();
            NotifyObservers(WM_MSG_ROBOT_WRITESN_RESULT, NULL, nSnLen, (LPARAM)pSnBuf);
        }
        else if(m_eCurPerformRobotCmd == eRobotCmdTypeReadUID && databuf[eBTCmdPosCmd] == eBTCmdGetUDID)
        {
            int nSnLen = (databuf[2] + 1) - 6;
            byte* pSnBuf = (byte*)databuf + 4;
            resetData();
            NotifyObservers(WM_MSG_ROBOT_READUID_RESULT, NULL, nSnLen, (LPARAM)pSnBuf);
        }
        else if(m_eCurPerformRobotCmd == eRobotCmdTypeCheckUtf8 && databuf[eBTCmdPosCmd] == 0x35)
        {
            if(databuf[4] == 1)
            {
                m_bSupportUTF8 = TRUE;
            }else {
                m_bSupportUTF8 = false;
            }
        }
    }
}

void CRobotController::processRobotSwitchModeResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    if(databuf[2] == 0xAA)
    {
        resetData();
        NotifyObservers(WM_MSG_ROBOT_SWITCHMODE_RESULT, NULL, TRUE, NULL);
    }
    else if(databuf[2] == 0xEE)
    {
        NotifyObservers(WM_MSG_ROBOT_SWITCHMODE_RESULT, NULL, FALSE, NULL);
    }
}

void CRobotController::processDeleteRobotActionResp(byte *databuf, int datalen)
{
    if (!databuf || datalen <= 0)
    {
        return;
    }

    // 不重发
    resetData();
    if (databuf[4] == 0x00)     // 文件删除成功
    {
        NotifyObservers(WM_MSG_DELETE_ROBOT_ACTION_RST, NULL, (int)eDeleteRobotActionRspOk, (LPARAM)m_baDeleteActionName.data());
    }
    else if (databuf[4] == 0x01) // 文件不存在
    {
        NotifyObservers(WM_MSG_DELETE_ROBOT_ACTION_RST, NULL, (int)eDeleteRobotActionRspFileNotExist, (LPARAM)m_baDeleteActionName.data());
    }
    else if (databuf[4] == 0x02) // 正在播放该文件
    {
        NotifyObservers(WM_MSG_DELETE_ROBOT_ACTION_RST, NULL, (int)eDeleteRobotActionRspActionIsPlaying, (LPARAM)m_baDeleteActionName.data());
    }
    else if (databuf[4] == 0x03) // 空文件名
    {
        NotifyObservers(WM_MSG_DELETE_ROBOT_ACTION_RST, NULL, (int)eDeleteRobotActionRspNullFile, (LPARAM)m_baDeleteActionName.data());
    }
}

bool CRobotController::processWifiScan(byte *databuf, int datalen)
{
    if( !CBluetoothComandParser::isBTProtocol(databuf, datalen))
    {
        return false;
    }

    // 不重发
    resetData();

    static int nWifi =0;
    static int nReWifi = 0;
    if (databuf[eBTCmdPosCmd] == eBTCmdGetWifisList)
    {
        nWifi = databuf[eBTCmdPosFirstParamStart];
        nReWifi = 0;
    }


    // 正在传输
    if (databuf[eBTCmdPosCmd] == eBTCmdGetWifisListIng)
    {
        //            char* pInfo = (char*)(&databuf[eBTCmdPosCmd + 1]);
        const int nInfoLen = databuf[eBTCmdPosCmdLength] - 5;
        char* pInfo = new char[nInfoLen + 1];
        memset(pInfo, 0, nInfoLen + 1);
        memcpy(pInfo, (char*)(&databuf[eBTCmdPosCmd + 1]), nInfoLen);
        NotifyObservers(WM_MSG_GET_ROBOT_WIFI_LIST_RST, NULL, TRUE, (LPARAM)pInfo);

        nReWifi++;

        //for get next info
        setCurrentPerformRobotCmd(eRobotCmdTypeNone);
        Req_RobotWifiScan(eWifiInfosWifiListContinueGet);//eBTCmdGetWifisList or eBTCmdGetWifisListIng is ok
    }
    // 传输结束
    else if (databuf[eBTCmdPosCmd] == eBTCmdGetWifisListEnd)
    {
        setCurrentPerformRobotCmd(eRobotCmdTypeNone);
        NotifyObservers(WM_MSG_GET_ROBOT_WIFI_LIST_RST, NULL, TRUE, NULL);
    }

    return true;
}

bool CRobotController::processRobotConnectWifi(byte *databuf, int datalen)
{
    if( !CBluetoothComandParser::isBTProtocol(databuf, datalen))
    {
        return false;
    }
    // 不重发
    resetData();

    //
    if (databuf[eBTCmdPosCmd] == eBTCmdConnectWifi)
    {
        char param = databuf[eBTCmdPosCmd + 1];
        emAnsWifiSettingStat  eWifiSetting = (emAnsWifiSettingStat)param;
        if(eAnsConnecting != eWifiSetting)
        {
            setCurrentPerformRobotCmd(eRobotCmdTypeNone);
        }

        NotifyObservers(WM_MSG_ROBOT_CONNECT_WIFI_RST, NULL, (emAnsCommontFlage)eWifiSetting, NULL);
        return true;
    }

    return false;
}

bool CRobotController::processRobotManalUpGrade(byte *databuf, int datalen)
{
    // eRobotCmdTypeManalUpGrade

    if( !CBluetoothComandParser::isBTProtocol(databuf, datalen))
    {
        return false;
    }
    // 不重发
    resetData();

    //
    if (databuf[eBTCmdPosCmd] == eBTCmdManalUpGrade)
    {
        char param = databuf[eBTCmdPosCmd + 1];

        bool bOk = false;
        if(eAnsOk == (emAnsCommontFlage)param)
        {
            bOk = true;
        }

        NotifyObservers(WM_MSG_ROBOT_MANAL_UPGRADE_RST, NULL, bOk, NULL);
        return true;
    }

    return false;
}

bool CRobotController::processRobotGetAutoUpdadeProgress(byte *databuf, int datalen)
{
    if( !CBluetoothComandParser::isBTProtocol(databuf, datalen))
    {
        return false;
    }
    // 不重发
    resetData();

    //
    if (databuf[eBTCmdPosCmd] == eBTCmdGetAutoUpdadeProgress)
    {
        byte* pParams = CBluetoothComandParser::getParamsPoidHead(databuf, datalen);
        int nLenParams = CBluetoothComandParser::getParamsLen(databuf, datalen);

        NotifyObservers(WM_MSG_ROBOT_GET_AUTO_UPDATE_PROGRESS_RST, NULL, nLenParams, (LPARAM)pParams);
        return true;
    }

    return false;
}

bool CRobotController::processRobotIsInstallingUpdate(byte *databuf, int datalen)
{
    if( !CBluetoothComandParser::isBTProtocol(databuf, datalen))
    {
        return false;
    }
    // 不重发
    resetData();

    //
    if (databuf[eBTCmdPosCmd] == eBTCmdIsInstallingUpdate)
    {
        char param = databuf[eBTCmdPosCmd + 1];
        NotifyObservers(WM_MSG_ROBOT_IS_INSTATLING_UPDATE_RST, NULL, param, NULL);
        return true;
    }

    return false;
}

void CRobotController::processSetTimeResp(byte *databuf, int datalen)
{
    if (!databuf || datalen < BLUE_CMD_RSP_COMMON_LEN)
    {
        return;
    }

    if ((ROBOT_CMD_BLUE_TOOTH_START_FLAG0 == databuf[eBTCmdPosStartFlag0])
            && (ROBOT_CMD_BLUE_TOOTH_START_FLAG1 == databuf[eBTCmdPosStartFlag1])
            && (eBTCmdSetTime == databuf[eBTCmdPosCmd]))
    {
        if (databuf[4] == eBTCmdRspStateOK)
        {
            AlphaRobotLog::getInstance()->printDebug("Set time Successfully");
        }
        else
        {
            AlphaRobotLog::getInstance()->printDebug("Set time Failed");
        }
    }
}

/**************************************************************************
* 函数名: SendCmdData
* 功能: 发送舵机指令
* 参数:
*    @[in ] eCmdType: 指令类型
*    @[in ] lParam1: 参数1
*    @[in ] lParam2: 参数2
*    @[in ] lParam3: 参数3
*    @[in ] lParam4: 参数4
*    @[in ] bEnqueue: 是否需要将指令数据入队缓存，文件数据只有文件起始传输人指令才需要入队缓存
* 返回值: void
* 时间: 2015/09/08 13:25
* 作者: ZDJ
*/
void CRobotController::SendCmdData(emRobotCmdType eCmdType, LPARAM lParam1, LPARAM lParam2,
                                   LPARAM lParam3, LPARAM lParam4, bool bEnqueue)
{
    if(!m_pPort || (m_pPort && !m_pPort->IsOpen()))
    {
        return;
    }

    // 文件传输的优先级低，因此可以被其他操作中断
    if (bEnqueue)
    {
        interruptActionSync(eCmdType, lParam1, lParam2, lParam3, lParam4);
    }

    int nDatalen = 0;
    byte* pData = NULL;

    switch(eCmdType)
    {
    case eRobotCmdTypeTransFilePrepare:
        {
            if(!IsSupportNewProtocol())
                pData = m_pRobotCmd->GetCmd_DownloadBinPrepare((const char*)lParam1, (const char*)lParam2, m_bSupportUTF8, nDatalen);
            else
                pData = m_pRobotCmd->GetCmd_DownloadBinPrepare2((const char*)lParam1, (const char*)lParam2, m_bSupportUTF8, nDatalen);
        }
        break;
    case eRobotCmdTypeTransFileIng:
        {
            if(!IsSupportNewProtocol())
            {
                pData = m_pRobotCmd->GetCmd_DownloadBinIng((byte*)lParam1, lParam2, nDatalen);
            }
            else
            {
                pData = m_pRobotCmd->GetCmd_DownloadBinIng2(lParam1, (byte*)lParam2, lParam3, nDatalen,
                                                                UBXBase::getInstance()->getCurrentRobotProductType());
            }
        }
        break;
    case eRobotCmdTypeTransFileEnd:
        {
            if(!IsSupportNewProtocol())
            {
                pData = m_pRobotCmd->GetCmd_DownloadBinEnd((byte*)lParam1, lParam2, nDatalen);
            }
            else
            {
                pData = m_pRobotCmd->GetCmd_DownloadBinEnd2(lParam1, (byte*)lParam2, lParam3, nDatalen,
                                                            UBXBase::getInstance()->getCurrentRobotProductType());
            }
        }
        break;
    case eRobotCmdTypeTransFileCancel:
        {
            if (!IsSupportNewProtocol())
            {
                // 旧版本不支持取消传输
            }
            else
            {
                pData = m_pRobotCmd->GetCmd_TransFileCancel(nDatalen);
            }
            break;
        }
    case eRobotCmdTypeGetOffset:
        {
            pData = m_pRobotCmd->GetCmd_MotorGetOffset(lParam1, nDatalen);
        }
        break;
    case eRobotCmdTypeSetOffset:
        {
            pData = m_pRobotCmd->GetCmd_MotorSetOffset(lParam1, lParam2, nDatalen);
        }
        break;
    case eRobotCmdTypeGetMotorVersion:
        {
            pData = m_pRobotCmd->GetCmd_MotorVersion(lParam1, nDatalen);
        }
        break;
    case eRobotCmdTypeGetID:
        {
            pData = m_pRobotCmd->GetCmd_MotorGetID(nDatalen);
        }
        break;
    case eRobotCmdTypeSetID:
        {
            pData = m_pRobotCmd->GetCmd_MotorSetID(lParam1, lParam2, nDatalen);
        }
        break;
    case eRobotCmdTypeUpgradePrepare:
        {
            pData = m_pRobotCmd->GetCmd_MotorUpgradePrepare(lParam1, nDatalen);
        }
        break;
    case eRobotCmdTypeUpgradeIng:
        {
            pData = m_pRobotCmd->GetCmd_MotorUpgradeIng(
                lParam1,           //舵机ID
                lParam2,           //当前帧
                (byte*)lParam3,    //用户数据
                lParam4,           //用户数据长度
                nDatalen);
        }
        break;
    case eRobotCmdTypeUpgradeEnd:
        {
            pData = m_pRobotCmd->GetCmd_MotorUpgradeEnd(lParam1,         //舵机ID
                                                         lParam2,         //当前帧
                                                         (byte*)lParam3,  //用户数据
                                                         lParam4,         //用户数据长度
                                                         nDatalen);
        }
        break;
    case eRobotCmdTypeMove:
        {
            pData = m_pRobotCmd->GetCmd_MotorMove(lParam1, lParam2, lParam3, nDatalen);
        }
        break;
    case eRobotCmdTypeConnectRobot:
        {
            pData = m_pRobotCmd->GetCmd_ConnectRobot(nDatalen);
        }
        break;
    case eRobotCmdTypeDisconnectRobot:
        {
            pData = m_pRobotCmd->GetCmd_DisconnectRobot(nDatalen);
        }
        break;
    case eRobotCmdTypeDebug:
        {
            pData = m_pRobotCmd->GetCmd_RobotDebug((MOTOR_DATA*)lParam1, lParam2, lParam3, lParam4, nDatalen);
        }
        break;
    case eRobotCmdTypeSwitchMode:
        {
            pData = m_pRobotCmd->GetCmd_SwitchMode(lParam1, nDatalen);
        }
        break;
    case eRobotCmdTypeReset:
        {
            pData = m_pRobotCmd->GetCmd_RobotReset((MOTOR_DATA*)lParam1, lParam2, lParam3, lParam4, nDatalen);
        }
        break;
    case eRobotCmdTypeReadSN:
        {
            pData = m_pRobotCmd->GetCmd_RobotReadSN(nDatalen);
        }
        break;
    case eRobotCmdTypeWriteSN:
        {
            pData = m_pRobotCmd->GetCmd_RobotWriteSN((char*)lParam1, lParam2, nDatalen);
        }
        break;
    case eRobotCmdTypeReadUID:
        {
            pData = m_pRobotCmd->GetCmd_RobotReadUID(nDatalen);
        }
        break;
    case eRobotCmdTypeCheckUtf8:
        {
            m_bSupportUTF8 = false;//asu
            pData = m_pRobotCmd->GetCmd_RobotCheckUTF8(nDatalen);
        }
        break;
    case eRobotCmdTypeGetAngle:
        {
            pData = m_pRobotCmd->GetCmd_MotorGetAngle(lParam1, nDatalen);
        }
        break;
    case eRobotCmdTypeGetRobotHardwareVer:
        {
            pData = m_pRobotCmd->GetCmd_ReadHardwareVer(nDatalen);
        }
        break;
    case eRobotCmdTypeGetRobotSysEmbeddedVersion:
        {
            pData = m_pRobotCmd->GetCmd_ReadSoftwareVer(nDatalen);
        }
        break;

    case eRobotCmdTypeGetBatteryStatus:
        {
            pData = m_pRobotCmd->GetCmd_GetBatteryStatus(nDatalen);
        }
        break;

    case eRobotCmdTypeGetDeviceStatus:
        {
            pData = m_pRobotCmd->GetCmd_GetDeviceStatus(nDatalen);
        }
        break;

    case eRobotCmdTypeGetSomeResInfos:
        {
            pData = m_pRobotCmd->GetCmd_GetResInfos(lParam1, lParam2, (char*)lParam3, lParam4, nDatalen);
        }
        break;

    case eRobotCmdTypeGetActionList:
        {
            pData = m_pRobotCmd->GetCmd_GetActionList((char*)lParam1, lParam2, nDatalen);
        }
        break;

    case eRobotCmdTypeDeleteAction:
    {
        //if(IsSupportUTF8())

        pData = m_pRobotCmd->GetCmd_DeleteAction((char *) lParam1, lParam2,  nDatalen);
        break;
    }

    case eRobotCmdTypeWifiScan:
    {
        pData = m_pRobotCmd->GetCmd_WifiScan((char) lParam1, nDatalen);
        break;
    }

    case eRobotCmdTypeConnectWifi:
    {
        pData = m_pRobotCmd->GetCmd_ConnectWifi((char *) lParam1, (char) lParam2,
                                                (char *)lParam3, (char) lParam4,
                                                nDatalen);
        break;
    }

    case eRobotCmdTypePlayAct:
    {
        pData = m_pRobotCmd->GetCmd_PlayAction((char *) lParam1, lParam2,  nDatalen);
        break;
    }

    case eRobotCmdTypeSetTime:
    {
        pData = m_pRobotCmd->GetCmd_SetTime((QDateTime *) lParam1, nDatalen);
        break;
    }

    case eRobotCmdTypeManalUpGrade:
    {
        pData = m_pRobotCmd->GetCmd_ManalUpGrade(nDatalen);
        break;
    }


    default:
       {
        pData = NULL;
        nDatalen = 0;
       }
        break;
    }

    if (pData && (nDatalen > 0))
    {
        if (bEnqueue)
        {
            m_oRobotCommunicateThread.pushData(eCmdType, pData, nDatalen);
            m_oRobotCommunicateThread.runThread();
        }
        else
        {
            if(m_pPort && m_pPort->IsOpen())
            {
                setCurrentPerformRobotCmd(eCmdType);
                if((eCmdType == eRobotCmdTypeTransFileIng || eCmdType == eRobotCmdTypeTransFileEnd) && IsSupportNewProtocol())
                {
                    for(int i = 0; i < nDatalen; i += RobotCmdBTProtocol::m_nTransFilePackageSize)
                    {
                        m_pPort->SendData(pData + i, RobotCmdBTProtocol::m_nTransFilePackageSize); //每次发送64字节
                        if(g_nSmallFrameDelay > 0)
                            Sleep(g_nSmallFrameDelay);
                    }
                }
                else
                {
                    m_pPort->SendData(pData, nDatalen);
                }
            }
        }
    }

//    if(pData && (nDatalen > 0))
//    {
//        if(m_pPort && m_pPort->IsOpen())
//        {
//            if((eCmdType == eRobotCmdTypeTransFileIng || eCmdType == eRobotCmdTypeTransFileEnd) && IsSupportNewProtocol())
//            {
//                for(int i = 0; i < nDatalen; i += TRANSFILE_PKGSIZE)
//                {
//                    m_pPort->SendData(pData + i, TRANSFILE_PKGSIZE); //每次发送64字节
//                    if(g_nSmallFrameDelay > 0)
//                        Sleep(g_nSmallFrameDelay);
//                }
//            }
//            else
//            {
//                m_pPort->SendData(pData, nDatalen);
//            }
//        }
//    }

    if(eCmdType == eRobotCmdTypeTransFilePrepare
        || eCmdType == eRobotCmdTypeTransFileIng
        || eCmdType == eRobotCmdTypeTransFileEnd)
    {
        SAFE_DELETE_ARRAY(pData);
    }
}

/**************************************************************************
* 函数名: sendRobotCommand
* 功能: 发送指定命令
* 参数:
*    @[in ] oCmdType: 指令信息
* 时间: 2016/11/22
* 作者:hwx
*/
void CRobotController::sendRobotCommand(CCommandType &oCmdType)
{
    setCurrentPerformRobotCmd(oCmdType.getCommand());
    emit sigOnSendCmd(oCmdType);
}

#if 0
/**************************************************************************
* 函数名: OnTimer
* 功能: 计时器响应
* 时间: 2015/09/12 9:02
* 作者: ZDJ
*/
void CRobotController::OnTimer(UINT_PTR nIDEvent)
{
//#pragma region 超时处理
    if(nIDEvent == g_nWaitReSendTimerId)
    {
        if(g_bRecvTimeout)
        {
            KillTimer(nIDEvent);

            if(!g_bCancelTransFile && (g_eRobotCmdType == eRobotCmdTypeTransFilePrepare
                || g_eRobotCmdType == eRobotCmdTypeTransFileIng
                || g_eRobotCmdType == eRobotCmdTypeTransFileEnd)
                )
            {
                if(!g_bResendFailed)
                {
                    if(g_nResendRequest > g_nTimeoutReSendCount)
                    {
                        g_nResendRequest = 0;
                        g_bResendFailed = TRUE;

                        g_bBusy = FALSE;
                        g_cursenddata.Reset();
                        g_eRobotCmdType = eRobotCmdTypeNone;

                        NotifyObservers(WM_MSG_ACTION_DOWNLOAD_END, NULL, FALSE, (LPARAM)&g_cursenddata);//通知下载结果

                    }
                    else
                    {
#if 1
                        if(g_eRobotCmdType == eRobotCmdTypeTransFilePrepare)
                        {
                            qDebug("resend transfer prepare request...\n");

                            SendCmdData(eRobotCmdTypeTransFilePrepare, (LPARAM)g_cursenddata.m_szTransFilePath, (LPARAM)g_cursenddata.m_szDestPath, NULL, NULL);
                        }
                        else if(g_eRobotCmdType == eRobotCmdTypeTransFileIng)
                        {
                            qDebug("resend transfering request...\n");

                            int nResendFrame = g_cursenddata.m_nCurFrame - 1; //待重发的帧号，从1开始
                            int nSendedSize = g_cursenddata.m_nCurSendSize - g_nTransFileFrameSize;//已经发送成功的数据长度
                            if(nResendFrame < 1)
                                nResendFrame = 1;
                            if(nSendedSize < 0)
                                nSendedSize = 0;

                            SendCmdData(
                                eRobotCmdTypeTransFileIng,
                                (LPARAM)(g_cursenddata.m_senddata.databuf + nSendedSize),
                                nResendFrame/* - 1*/, //从1开始的帧索引
                                NULL, NULL
                                );
                        }
                        else if(g_eRobotCmdType == eRobotCmdTypeTransFileEnd)
                        {
                            qDebug("resend transfer end request...\n");
                            int nSendedSize = g_cursenddata.m_nCurSendSize - g_nTransFileFrameSize;//已经发送成功的数据长度
                            if(nSendedSize < 0)
                                nSendedSize = 0;
                                SendCmdData(eRobotCmdTypeTransFileEnd, (LPARAM)(g_cursenddata.m_senddata.databuf + nSendedSize), g_nTransFileFrameSize, NULL, NULL);
                        }

                        g_bHasResendRequest = TRUE;
                        g_nResendRequest++;

                        SetTimer(g_nWaitReSendTimerId, g_nWaitReSendTime, NULL);
#endif
                    }
                }
            }

            else if(g_eRobotCmdType == eRobotCmdTypeUpgradePrepare
                || g_eRobotCmdType == eRobotCmdTypeUpgradeIng
                || g_eRobotCmdType == eRobotCmdTypeUpgradeEnd
                )
            {
                if(!g_bResendFailed)
                {
                    if(g_nResendRequest > g_nTimeoutReSendCount)
                    {
                        g_nResendRequest = 0;
                        g_bResendFailed = TRUE;

                        g_bBusy = FALSE;
                        g_cursenddata.Reset();
                        g_eRobotCmdType = eRobotCmdTypeNone;

                        NotifyObservers(WM_MSG_MOTOR_UPGRADE_END, NULL, FALSE, (LPARAM)&g_cursenddata);  //升级失败
                    }
                    else
                    {
#if 1
                        if(g_eRobotCmdType == eRobotCmdTypeUpgradePrepare)
                        {
                            qDebug("resend upgrade prepare request...\n");

                            SendCmdData(eRobotCmdTypeUpgradePrepare, m_nCurMotorID);
                        }
                        else if(g_eRobotCmdType == eRobotCmdTypeUpgradeIng)
                        {
                            qDebug("resend upgrading request...\n");

                            int nResendFrame = g_cursenddata.m_nCurFrame - 1; //待重发的帧号，从0开始
                            int nSendedSize = g_cursenddata.m_nCurSendSize - g_nUpgradeFrameSize;//已经发送成功的数据长度
                            if(nResendFrame < 1)
                                nResendFrame = 1;
                            if(nSendedSize < 0)
                                nSendedSize = 0;

                            SendCmdData(
                                eRobotCmdTypeUpgradeIng,
                                m_nCurMotorID,
                                nResendFrame - 1, //从0开始的帧索引
                                (LPARAM)(g_cursenddata.m_senddata.databuf + nSendedSize),
                                g_nUpgradeFrameSize
                                );
                        }
                        else if(g_eRobotCmdType == eRobotCmdTypeUpgradeEnd)
                        {
                            qDebug("resend upgrade end request...\n");

                            SendCmdData(eRobotCmdTypeUpgradeEnd, m_nCurMotorID);
                        }

                        g_bHasResendRequest = TRUE;
                        g_nResendRequest++;

                        SetTimer(g_nWaitReSendTimerId, g_nWaitReSendTime, NULL);
#endif
                    }
                }
            }
            else if(g_eRobotCmdType == eRobotCmdTypeGetVersion)
            {
                if(!g_bResendFailed)
                {
                    if(g_nResendRequest > g_nTimeoutReSendCount)
                    {
                        g_nResendRequest = 0;
                        g_bResendFailed = TRUE;

                        g_bBusy = FALSE;
                        g_cursenddata.Reset();
                        g_eRobotCmdType = eRobotCmdTypeNone;

                        NotifyObservers(WM_MSG_MOTOR_READVERSION, NULL, FALSE, NULL);  //读版本失败
                    }
                    else
                    {
#if 1
                        if(g_eRobotCmdType == eRobotCmdTypeUpgradePrepare)
                        {
                            qDebug("resend readversion request...\n");

                            SendCmdData(eRobotCmdTypeGetVersion, m_nCurMotorID);
                        }

                        g_bHasResendRequest = TRUE;
                        g_nResendRequest++;

                        SetTimer(g_nWaitReSendTimerId, g_nWaitReSendTime, NULL);
#endif
                    }
                }
            }
            else if(g_eRobotCmdType == eRobotCmdTypeSetID)
            {
                if(!g_bResendFailed)
                {
                    if(g_nResendRequest > g_nTimeoutReSendCount)
                    {
                        g_nResendRequest = 0;
                        g_bResendFailed = TRUE;

                        g_bBusy = FALSE;
                        g_cursenddata.Reset();
                        g_eRobotCmdType = eRobotCmdTypeNone;

                        NotifyObservers(WM_MSG_MOTOR_MODID, NULL, FALSE, NULL);  //修改ID失败
                    }
                    else
                    {
#if 1
                        if(g_eRobotCmdType == eRobotCmdTypeUpgradePrepare)
                        {
                            qDebug("resend setid request...\n");

                            SendCmdData(eRobotCmdTypeSetID, m_nCurMotorID);
                        }

                        g_bHasResendRequest = TRUE;
                        g_nResendRequest++;

                        SetTimer(g_nWaitReSendTimerId, g_nWaitReSendTime, NULL);
#endif
                    }
                }
            }
        }
    }
//#pragma endregion
//#pragma region 连接断开
    else if(nIDEvent == g_nConnectPortTimerID)
    {
        KillTimer(nIDEvent);

        BOOL bRet = Req_AutoOpenPort();
        NotifyObservers(WM_MSG_PORT_OPENSTATECHANGE, NULL, NULL, TRUE);
    }
    else if(nIDEvent == g_nDisconnectPortTimerID)
    {
        KillTimer(nIDEvent);

        Req_ClosePort();
        NotifyObservers(WM_MSG_PORT_OPENSTATECHANGE, NULL, NULL, FALSE);
    }
//#pragma endregion

// 	else if(nIDEvent == g_nCalcuTransRateTimerID)
// 	{
// 		m_nDataTransRate = m_nCurTranDataLen * 1000 / g_nCalcuTransRatePeriod;  //单位秒
// 		m_nCurTranDataLen = 0;
// 	}

    CWnd::OnTimer(nIDEvent);
}
#else
void CRobotController::timerEvent(QTimerEvent *event)
{
    int nIDEvent = event->timerId();
    killTimer(nIDEvent);
    if(nIDEvent == m_nCurRobotCmdTimerId)
    {
        m_nCurRobotCmdTimerId = 0;
        handleRobotCmdTimeout();
    }
    else if(nIDEvent == m_nConnectPortTimerID)
    {
        m_nConnectPortTimerID = 0;
        NotifyObservers(WM_MSG_ROBOT_CONNECT, NULL, NULL, TRUE);
    }
    else if(nIDEvent == m_nDisconnectPortTimerID)
    {
        m_nDisconnectPortTimerID = 0;
        NotifyObservers(WM_MSG_ROBOT_DISCONNECT, NULL, NULL, TRUE);
    }
}

void CRobotController::customEvent(QEvent *event)
{
    zxEvent* pEvent = (zxEvent*)event;
    if(event->type() == WM_MSG_RXCHAR)
        OnReceiveChar(pEvent->wParam, pEvent->lParam);
}

#endif
/**************************************************************************
* 函数名: GetPortOpenState
* 功能: 获取端口打开状态
* 时间: 2015/10/20 11:09
* 作者: ZDJ
*/
BOOL CRobotController::GetPortOpenState()
{
    if(m_pPort)
        return m_pPort->IsOpen();
    return FALSE;
}

/**************************************************************************
* 函数名: Req_MotorUpgrade
* 功能: 升级舵机
* 参数:
*    @[in ] nMotorID: 舵机ID
*    @[in ] lpszFilePath: 升级文件路径
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/21 18:32
* 作者: ZDJ
*/
BOOL CRobotController::Req_MotorUpgrade(int nMotorID, const char *pszFilePath)
{
    QString sFilePath = AnsiToUnicode(pszFilePath);
    if(PathFileExists(sFilePath))
    {
        m_STCurSendData.Reset();

        QFile fileobj(sFilePath);
        if(fileobj.open(QIODevice::ReadOnly))
        {
            qint64 nFileLen = fileobj.size();
            m_STCurSendData.m_senddata.datalen = nFileLen;
            m_STCurSendData.m_senddata.databuf = new byte[nFileLen];
            fileobj.read((char*)m_STCurSendData.m_senddata.databuf, nFileLen);

            fileobj.close();

            m_nCurMotorID = nMotorID; //记录当前舵机ID
            SendCmdData(eRobotCmdTypeUpgradePrepare, nMotorID);
            return TRUE;
        }
    }

    return FALSE;
}

/**************************************************************************
* 函数名: Req_MotorReadVersion
* 功能: 读版本
* 参数:
*    @[in ] nMotorID: 舵机ID
* 返回值: void
* 时间: 2015/10/21 18:32
* 作者: ZDJ
*/
void CRobotController::Req_MotorReadVersion(int nMotorID)
{
    m_STCurSendData.Reset();

    m_nCurMotorID = nMotorID;
    SendCmdData(eRobotCmdTypeGetMotorVersion, nMotorID);
}

/**************************************************************************
* 函数名: Req_MotorWriteID
* 功能: 修改舵机ID
* 参数:
*    @[in ] nMotorOldID: 旧ID
*    @[in ] nMotorNewID: 新ID
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/21 18:36
* 作者: ZDJ
*/
BOOL CRobotController::Req_MotorWriteID(int nMotorOldID, int nMotorNewID)
{
    if(nMotorNewID >= MIN_MOTOR_ID && nMotorNewID <= MAX_MOTOR_ID)
    {
        m_STCurSendData.Reset();

        m_nCurMotorID = nMotorOldID; //记录当前舵机ID
        SendCmdData(eRobotCmdTypeSetID, nMotorOldID, nMotorNewID);
        return TRUE;
    }
    return FALSE;
}

/**************************************************************************
* 函数名: GetCurMotorID
* 功能: 获取当前操作的舵机ID
* 时间: 2015/10/21 19:41
* 作者: ZDJ
*/
int CRobotController::GetCurMotorID()
{
    return m_nCurMotorID;
}

///**************************************************************************
//* 函数名: GetProcessRate
//* 功能: 获取处理速度，单位字节/s
//* 时间: 2015/10/28 12:26
//* 作者: ZDJ
//*/
//int CRobotController::GetProcessRate()
//{
//	return m_nDataTransRate <= 0 ? 1 : m_nDataTransRate; //test
//}

#ifdef Q_OS_WIN
/**************************************************************************
* 函数名: FindMySTMDevice
* 功能: 查找STM设备
* 时间: 2015/11/01 16:30
* 作者: ZDJ
*/
BOOL CRobotController::FindMySTMDevice()
{
    DWORD dwFlag = DIGCF_ALLCLASSES | DIGCF_PRESENT;
    HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, L"USB", NULL,dwFlag);
    if( INVALID_HANDLE_VALUE == hDevInfo )
    {
        return FALSE;
    }

    SP_DEVINFO_DATA spDevInfoData;
    spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for(int i=0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++)
    {
        DWORD nSize = 0 ;
        WCHAR buf[MAX_PATH] = {0};

        DWORD DataT ;
        if(SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_FRIENDLYNAME, &DataT, (PBYTE)buf, sizeof(buf), &nSize))
        {
            QString  str = QString::fromWCharArray(buf);

#ifndef QT_NO_DEBUG
            qDebug() << "SetupDiGetDeviceRegistryProperty:str:" << str;
#endif

            QString strGetName1 = str.left(str.indexOf('('));

            if ((strGetName1.indexOf(STM_DEVICE_NAME_NAME1) != -1)
                    || (strGetName1.indexOf(STM_DEVICE_NAME_NAME2) != -1))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
#endif

/**************************************************************************
* 函数名: StartConncectTimer
* 功能: 启动连接端口的时钟
* 时间: 2015/11/01 16:45
* 作者: ZDJ
*/
void CRobotController::StartConncectTimer()
{
    m_nConnectPortTimerID = startTimer(TIMEDELAY_CONNECTROBOT, Qt::PreciseTimer);
}

/**************************************************************************
* 函数名: StartDisconncectTimer
* 功能: 启动断开端口的时钟
* 时间: 2015/11/01 16:45
* 作者: ZDJ
*/
void CRobotController::StartDisconncectTimer()
{
    m_nDisconnectPortTimerID = startTimer(TIMEDELAY_DISCONNECTROBOT, Qt::PreciseTimer);
}

/**************************************************************************
* 函数名: Req_MotorGetOffset
* 功能: 获取舵机偏移值
* 参数:
*    @[in ] nMotorID: 舵机ID
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/21 18:36
* 作者: ZDJ
*/
BOOL CRobotController::Req_MotorGetOffset(int nMotorID)
{
    m_nCurMotorID = nMotorID;
    SendCmdData(eRobotCmdTypeGetOffset, nMotorID);
    return TRUE;
}

/**************************************************************************
* 函数名: Req_MotorSetOffset
* 功能: 设置舵机偏移值
* 参数:
*    @[in ] nMotorID: 舵机ID
*    @[in ] nOffset: 舵机偏移值
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/10/21 18:36
* 作者: ZDJ
*/
BOOL CRobotController::Req_MotorSetOffset(int nMotorID, int nOffset)
{
    m_nCurMotorID = nMotorID;
    m_nCurMotorOffset = nOffset;
    SendCmdData(eRobotCmdTypeSetOffset, nMotorID, nOffset);
    return TRUE;
}

/**************************************************************************
* 函数名: Req_RobotReadVersion
* 功能: 读机器人嵌入式软件版本
* 时间: 2015/10/21 18:32
* 作者: ZDJ
*/
void CRobotController::Req_RobotSysEmbeddedVersion()
{
    SendCmdData(eRobotCmdTypeGetRobotSysEmbeddedVersion);
}

/**************************************************************************
* 函数名: Req_RobotBatteryStatus
* 功能: 读机器人battery status
* 时间: 20160824
* 作者: asu
*/
void CRobotController::Req_RobotBatteryStatus()
{
    SendCmdData(eRobotCmdTypeGetBatteryStatus);
}

/**************************************************************************
* 函数名: Req_RobotDeviceStatus
* 功能: 请求机器人设备状态
* 时间: 2016/10/25
* 作者:  hwx
*/
void CRobotController::Req_RobotDeviceStatus()
{
    SendCmdData(eRobotCmdTypeGetDeviceStatus);
}

/**************************************************************************
* 函数名: Req_RobotActionList
* 功能: 请求机器人动作列表
* 参数:
*    @[in ] pActionDir: 动作路径
*    @[in ] nActionDirLen: 动作路径长度
* 时间: 2016/12/01
* 作者:  hwx
*/
void CRobotController::Req_RobotActionList(const char *pActionDir, int nActionDirLen)
{
    AlphaRobotLog::getInstance()->printDebug("Request robot action list");
    SendCmdData(eRobotCmdTypeGetActionList, (LPARAM)pActionDir, nActionDirLen);
}

void CRobotController::Req_RobotResInfos(emGetSomeTypeResInfoResType eResInfosType, emGetSomeTypeResInfoCmdFlag eResInfoCmdFlag,
                                         char *pParam, int nLenParam)
{
    QString strName(pParam);
    int nLen = nLenParam;
    AlphaRobotLog::getInstance()->printDebug(QString("Req_RobotResInfos type=%1 flag=%2").arg(eResInfosType).arg(eResInfoCmdFlag));
    SendCmdData(eRobotCmdTypeGetSomeResInfos,  eResInfosType, eResInfoCmdFlag, (LPARAM)pParam, nLenParam);
}

/**************************************************************************
* 函数名: Req_DeleteRobotAction
* 功能: 请求删除机器人动作
* 参数:
*    @[in ] pActionName: 动作名称，发送前需要转为Ascii码
*    @[in ] nActionNameLen: 动作名称长度
* 时间: 2016/12/01
* 作者:  hwx
*/
void CRobotController::Req_DeleteRobotAction(const char *pActionName, int nActionNameLen)
{
    AlphaRobotLog::getInstance()->printDebug(QString("Request delete robot action: %1").arg(QString(pActionName)));

    m_baDeleteActionName.clear();
    m_baDeleteActionName.append(pActionName);
    SendCmdData(eRobotCmdTypeDeleteAction, (LPARAM)pActionName, nActionNameLen);
}


/**************************************************************************
* 函数名: Req_RobotWifiScan
* 功能: 请求机器人WIFI列表
* 参数:
*    @[in ] nWifiStat:
* 时间: 2017/02/27
* 作者:  asu
*/
void CRobotController::Req_RobotWifiScan(int nWifiStat)
{
    SendCmdData(eRobotCmdTypeWifiScan, nWifiStat);

//    QString strAct = "act0";
//    if(nWifiStat <= 0)
//    {
//        strAct = "act0";
//    }
//    if(nWifiStat > 0)
//    {
//        strAct = "act1";
//    }

//    string strTemp = strAct.toStdString().c_str();
    //    SendCmdData(eRbootCmdTypePlayAct, (LPARAM)strTemp.c_str(), strTemp.length());
}

/**************************************************************************
* 函数名: Req_RobotConnectWifi
* 功能: 请求机器人connect WIFI
* 参数:
*    @[in ] strESSID:
*    @[in ] strPassKey:
* 时间: 2017/03/08
* 作者:  asu
*/
void CRobotController::Req_RobotConnectWifi(QString strESSID, QString strPassKey)
{
    SendCmdData(eRobotCmdTypeConnectWifi,
                (LPARAM)strESSID.toStdString().c_str(), strESSID.toStdString().length(),
                (LPARAM)strPassKey.toStdString().c_str(), strPassKey.toStdString().length());
}

/**************************************************************************
* 函数名: Req_SetTime
* 功能: 设置机器人时间
* 参数:
*    @[in ] pDateTime:时间信息
* 时间: 2017/03/14
* 作者:  hwx
*/
void CRobotController::Req_SetTime(const QDateTime *pDateTime)
{
    SendCmdData(eRobotCmdTypeSetTime, (LPARAM)pDateTime);
}

void CRobotController::Req_ManalUpGrade()
{
    SendCmdData(eRobotCmdTypeManalUpGrade);
}

/**************************************************************************
* 函数名: IsSupportUTF8
* 功能: 获取是否支持UTF8
* 时间: 2015/11/25 11:26
* 作者: ZDJ
*/
BOOL CRobotController::IsSupportUTF8()
{
    return m_bSupportUTF8;
}

#ifdef Q_OS_MAC

/**************************************************************************
* 函数名: OnDeviceRemoval
* 功能: 设备移除通知
* 时间: 2015/11/16 10:57
* 作者: ZDJ
*/
void CRobotController::OnDeviceRemoval(void *                  context,
                                       IOReturn                result,
                                       void *                  sender,
                                       IOHIDDeviceRef          device)
{
    int nVID = get_vendor_id(device);
    int nPID = get_product_id(device);
    if((nVID == g_devidarr[ePortTypeHID].dwVid && nPID == g_devidarr[ePortTypeHID].dwPid && g_bFirstDeviceArrival)
            || (nVID == g_devidarr[ePortTypeHID + 1].dwVid && nPID == g_devidarr[ePortTypeHID + 1].dwPid && g_bFirstDeviceArrival)
            || (nVID == g_devidarr[ePortTypeVCP].dwVid && nPID == g_devidarr[ePortTypeVCP].dwPid && g_bFirstDeviceArrival))
    {
        g_bFirstDeviceArrival = FALSE;
        qDebug("device removal");
        m_pControllerStatic->NotifyObservers(WM_MSG_ROBOT_DISCONNECT, NULL, TRUE, TRUE);// （是否断开， usb 拨出)

        emit m_pControllerStatic->sigOnDeviceRemoved();

        // USB拔出后关闭串口
        //Req_ClosePort();
    }
}

/**************************************************************************
* 函数名: OnDeviceArrival
* 功能: 设备接入通知
* 时间: 2015/11/16 10:57
* 作者: ZDJ
*/
void CRobotController::OnDeviceArrival(void *                  context,
                                       IOReturn                result,
                                       void *                  sender,
                                       IOHIDDeviceRef          device)
{
    int nVID = get_vendor_id(device);
    int nPID = get_product_id(device);
    if((nVID == g_devidarr[ePortTypeHID].dwVid && nPID == g_devidarr[ePortTypeHID].dwPid && !g_bFirstDeviceArrival)
            || (nVID == g_devidarr[ePortTypeHID + 1].dwVid && nPID == g_devidarr[ePortTypeHID + 1].dwPid && !g_bFirstDeviceArrival)
            || (nVID == g_devidarr[ePortTypeVCP].dwVid && nPID == g_devidarr[ePortTypeVCP].dwPid && !g_bFirstDeviceArrival))
    {
        g_bFirstDeviceArrival = TRUE;
        qDebug("device arrival");
        m_pControllerStatic->NotifyObservers(WM_MSG_ROBOT_CONNECT, NULL, TRUE, TRUE);
    }
}
#endif

int CRobotController::portRecvData(void *pUser, byte *pData, int nDataLen)
{
    if(pUser && pData && (nDataLen > 0))
    {
        CRobotController* pController = (CRobotController*)pUser;
        pController->OnReceiveChar(nDataLen,  (LPARAM)pData);
    }

    return nDataLen;
}

#ifdef Q_OS_WIN

/**************************************************************************
* 函数名: OnDeviceChange
* 功能: 设备更改通知
* 时间: 2015/11/16 10:57
* 作者: ZDJ
*/
bool CRobotController::OnDeviceChange(MSG *msg,long *result)
{
    int msgType = msg->message;
    if(msgType == WM_DEVICECHANGE)
    {
        //qDebug() << "receive ondevicechange event " ;
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
        if(lpdb && lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
        {
            switch(msg->wParam)
            {
            case DBT_DEVICETYPESPECIFIC:
#ifndef QT_NO_DEBUG
                qDebug() << "DBT_DEVICETYPESPECIFIC " ;
#endif
                break;
            case DBT_DEVICEARRIVAL:
            case DBT_DEVICEREMOVECOMPLETE:
                UpdateDevice((PDEV_BROADCAST_DEVICEINTERFACE)lpdb, msg->wParam);
                break;
            default:
#ifndef QT_NO_DEBUG
                qDebug() << "DBT OTHER" ;
#endif
                break;
            }
        }
    }
    return false;
}

#endif

/**************************************************************************
* 函数名: GetRobotSoftVersion
* 功能: 获取机器人软件版本号
* 参数:
*    @[in ] pszVersion: 数据接收缓冲区
*    @[in ] nSize: 接收缓冲区长度
* 返回值: 实际数据长度
* 时间: 2016/05/27 15:00
* 作者: ZDJ
*/
int CRobotController::GetRobotSoftVersion(char* pszVersion, int nSize)
{
    if(pszVersion)
    {
        int nTempSize = sizeof(m_szRobotSoftVersion);
        int nCopySize = qMin(nTempSize, nSize);
        strncpy(pszVersion, m_szRobotSoftVersion, nCopySize);
        return nCopySize;
    }
    return -1;
}

/**************************************************************************
* 函数名: killRobotCmdTimer
* 功能: 停止机器人指定定时器
* 参数:
* 返回值:
* 时间: 2016/11/22
* 作者:hwx
*/
void CRobotController::killRobotCmdTimer()
{
    if (m_nCurRobotCmdTimerId > 0)
    {
        killTimer(m_nCurRobotCmdTimerId);
        m_nCurRobotCmdTimerId = 0;
    }
}

void CRobotController::handleRobotCmdTimeout()
{
    if(!m_bCancelTransFile && (m_eCurPerformRobotCmd == eRobotCmdTypeTransFilePrepare
        || m_eCurPerformRobotCmd == eRobotCmdTypeTransFileIng
        || m_eCurPerformRobotCmd == eRobotCmdTypeTransFileEnd)
        )
    {
        handleTransFileCmdTimeout();
        return;  // 返回，如果不返回正常文件传输也会设置eRobotCmdTypeNone，导致串口冲突
    }
    else if(m_eCurPerformRobotCmd == eRobotCmdTypeUpgradePrepare
        || m_eCurPerformRobotCmd == eRobotCmdTypeUpgradeIng
        || m_eCurPerformRobotCmd == eRobotCmdTypeUpgradeEnd
        )
    {
        handleMotorUpdateCmdTimeout();
        return;
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeTransFileCancel)
    {
        handleTransFileCancelCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeGetOffset)
    {
        handleGetOffsetCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeSetOffset)
    {
        handleSetOffsetCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeGetAngle)
    {
        handleGetAngleCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeMove)
    {
        handleMotorMoveCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeGetID)
    {
        handleGetIDCmdTimeout();
    }
    else if(m_eCurPerformRobotCmd == eRobotCmdTypeSetID)
    {
        handleSetIDCmdTimeout();
    }
    else if(m_eCurPerformRobotCmd == eRobotCmdTypeGetMotorVersion)
    {
        handleGetMotorVersionCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeConnectRobot)
    {
        handleConnectRobotCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeDisconnectRobot)
    {
        handleDisconnectRobotCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeDebug)
    {
        handleRobotDebugCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeSwitchMode)
    {
        handleSwitchModeCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeReset)
    {
        handleResetRobotCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeReadSN)
    {
        handleReadSNCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeWriteSN)
    {
        handleWriteSNCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeReadUID)
    {
        handleReadUidCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeCheckUtf8)
    {
        handleCheckUtf8CmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeGetRobotSysEmbeddedVersion)
    {
        handleGetRobotSysEmbedVerCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeGetRobotHardwareVer)
    {
        handleGetRobotHardwareVersionCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeGetBatteryStatus)
    {
        handleGetBatteryStatusCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeGetDeviceStatus)
    {
        handleGetDeviceStatusCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeGetSomeResInfos)
    {
        handleGetSomeResInfosCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeGetActionList)
    {
        handleGetRobotActionListCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeDeleteAction)
    {
        handleDeleteRobotActionTimeout();
    }    
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeWifiScan)
    {
        handleRobotWifiScanCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeConnectWifi)
    {
        handleRobotConnectWifiCmdTimeout();
    }
    else if (m_eCurPerformRobotCmd == eRobotCmdTypeManalUpGrade)
    {
        handleRobotManalUpGradeCmdTimeout();
    }


    // 除了文件传输需要等待传输最后一帧，其他传输都要设置eRobotCmdTypeNone
    // 否则重发不出去
    setCurrentPerformRobotCmd(eRobotCmdTypeNone);
}

void CRobotController::handleTransFileCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();
        setCurrentPerformRobotCmd(eRobotCmdTypeNone);

        AlphaRobotLog::getInstance()->printDebug(QString("CRobotController::handleTransFileCmdTimeout, sync action file failed"));
        NotifyObservers(WM_MSG_ACTION_DOWNLOAD_END, NULL, FALSE, (LPARAM)&m_STCurSendData);//通知下载结果
    }
    else
    {
#if 1
        if(m_eCurPerformRobotCmd == eRobotCmdTypeTransFilePrepare)
        {
            qDebug("resend transfer prepare request...\n");
            AlphaRobotLog::getInstance()->printDebug(QString("resend transfer prepare request of file %1...\n").arg(QString(m_STCurSendData.m_szSrcPath)));
            setCurrentPerformRobotCmd(eRobotCmdTypeNone); // 如果不设置，队列会因为当前的指令不是eRobotCmdTypeNone而无法pop数据
            SendCmdData(eRobotCmdTypeTransFilePrepare, (LPARAM)m_STCurSendData.m_szTransFilePath, (LPARAM)m_STCurSendData.m_szDestPath, NULL, NULL);
        }
        else if(m_eCurPerformRobotCmd == eRobotCmdTypeTransFileIng)
        {
            qDebug("resend transfering request...\n");
            AlphaRobotLog::getInstance()->printDebug(QString("resend transfering request of file %1...\n").arg(QString(m_STCurSendData.m_szSrcPath)));

            int nResendFrame = m_STCurSendData.m_nCurFrame - 1; //待重发的帧号，从1开始
            int nSendedSize = m_STCurSendData.m_nCurSendSize - m_nTransFileFrameSize;//已经发送成功的数据长度
            if(nResendFrame < 1)
                nResendFrame = 1;
            if(nSendedSize < 0)
                nSendedSize = 0;

            if(!IsSupportNewProtocol())
                SendCmdData(
                    eRobotCmdTypeTransFileIng,
                    (LPARAM)(m_STCurSendData.m_senddata.databuf + nSendedSize),
                    nResendFrame/* - 1*/, //从1开始的帧索引
                    NULL, NULL, false
                    );
            else
                SendCmdData(
                    eRobotCmdTypeTransFileIng,
                    nResendFrame,
                    (LPARAM)(m_STCurSendData.m_senddata.databuf + nSendedSize),
                    m_nTransFileFrameSize,
                    NULL, false
                    );
        }
        else if(m_eCurPerformRobotCmd == eRobotCmdTypeTransFileEnd)
        {
            qDebug("resend transfer end request...\n");
            AlphaRobotLog::getInstance()->printDebug(QString("resend transfer end request of file %1...\n").arg(QString(m_STCurSendData.m_szSrcPath)));

            int nResendFrame = m_STCurSendData.m_nCurFrame - 1; //待重发的帧号，从1开始
            int nSendedSize = m_STCurSendData.m_nCurSendSize - m_nTransFileFrameSize;//已经发送成功的数据长度
            if(nResendFrame < 1)
                nResendFrame = 1;
            if(nSendedSize < 0)
                nSendedSize = 0;
            if(!IsSupportNewProtocol())
                SendCmdData(eRobotCmdTypeTransFileEnd, (LPARAM)(m_STCurSendData.m_senddata.databuf + nSendedSize), m_nTransFileFrameSize, NULL, NULL, false);
            else
                SendCmdData(eRobotCmdTypeTransFileEnd, nResendFrame, (LPARAM)(m_STCurSendData.m_senddata.databuf + nSendedSize), m_nTransFileFrameSize, NULL, false);
        }

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_TRANSFILE_ING, Qt::PreciseTimer);
#endif
    }
}

void CRobotController::handleTransFileCancelCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_TRANS_FILE_CANCEL, NULL, FALSE, NULL);  // 文件传输失败
        AlphaRobotLog::getInstance()->printError(QString("Cancel action sync timeout, repeat: %1").arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug(tr("Resend sync cancel command"));
       SendCmdData(eRobotCmdTypeTransFileCancel);
        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;
        m_nCurRobotCmdTimerId = startTimer(TIMIEOUT_CMD_TRANSFILE_CANCEL, Qt::PreciseTimer);
    }
}

void CRobotController::handleGetOffsetCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_MOTOR_OFFSET_GET, NULL, FALSE, NULL);
        AlphaRobotLog::getInstance()->printError(QString("Get offset of motor %1 timeout, repeat: %2").arg(m_nCurMotorID).arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug(QString("Resend get offset command of motor %1").arg(m_nCurMotorID));

       SendCmdData(eRobotCmdTypeGetOffset, m_nCurMotorID);
        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;
        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_GET_OFFSET, Qt::PreciseTimer);
    }
}

void CRobotController::handleSetOffsetCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_MOTOR_OFFSET_SET, NULL, FALSE, NULL);
        AlphaRobotLog::getInstance()->printError(QString("Set offset of motor %1  timeout, repeat: %2").arg(m_nCurMotorID).arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug(QString("Resend set offset command of motor %1").arg(m_nCurMotorID));

       SendCmdData(eRobotCmdTypeSetOffset, m_nCurMotorID, m_nCurMotorOffset);
        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;
        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_SET_OFFSET, Qt::PreciseTimer);
    }
}

void CRobotController::handleGetAngleCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_MOTOR_ANGLE_GET, NULL, FALSE, NULL);
        AlphaRobotLog::getInstance()->printDebug(QString("Get angle of motor %1 timeout, repeat: %2").arg(m_nCurMotorID).arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug(QString("Resend get angle command of motnor %1").arg(m_nCurMotorID));

       SendCmdData(eRobotCmdTypeGetAngle, m_nCurMotorID);
        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;
        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_GET_ANGLE, Qt::PreciseTimer);
    }
}

void CRobotController::handleMotorMoveCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_MOTOR_MOVE, NULL, FALSE, NULL);
        AlphaRobotLog::getInstance()->printDebug(QString("Move motor  %1 timeout, repeat: %2").arg(m_nCurMotorID).arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug(QString("Resend motor %1 move command").arg(m_nCurMotorID));

       SendCmdData(eRobotCmdTypeMove, m_nCurMotorID, m_nCurMotorOffset, m_nCurTime);
        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;
        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_MOVE, Qt::PreciseTimer);
        }
}

void CRobotController::handleGetIDCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_MOTOR_GETID, NULL, FALSE, NULL);
        AlphaRobotLog::getInstance()->printDebug(QString("Get motor ID %1 timeout, repeat: %2").arg(m_nCurMotorID).arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug("resend get id request...\n");
        SendCmdData(eRobotCmdTypeGetID, m_nCurMotorID);
        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_GET_ID, Qt::PreciseTimer);
    }
}

void CRobotController::handleMotorUpdateCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();
        setCurrentPerformRobotCmd(eRobotCmdTypeNone);

        NotifyObservers(WM_MSG_MOTOR_UPGRADE_END, NULL, FALSE, (LPARAM)&m_STCurSendData);  //升级失败
        AlphaRobotLog::getInstance()->printDebug(QString("Motor %1 update timeout, repeat: %2").arg(m_nCurMotorID).arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
#if 1
        if(m_eCurPerformRobotCmd == eRobotCmdTypeUpgradePrepare)
        {
            AlphaRobotLog::getInstance()->printDebug("resend upgrade prepare request...\n");
            setCurrentPerformRobotCmd(eRobotCmdTypeNone); // 如果不设置，无法从队列pop数据
            SendCmdData(eRobotCmdTypeUpgradePrepare, m_nCurMotorID);
        }
        else if(m_eCurPerformRobotCmd == eRobotCmdTypeUpgradeIng)
        {
            AlphaRobotLog::getInstance()->printDebug("resend upgrading request...\n");

            int nResendFrame = m_STCurSendData.m_nCurFrame - 1; //待重发的帧号，从0开始
            int nSendedSize = m_STCurSendData.m_nCurSendSize - DEFAULT_UPDATE_FRAME_SIZE;//已经发送成功的数据长度
            if(nResendFrame < 1)
                nResendFrame = 1;
            if(nSendedSize < 0)
                nSendedSize = 0;

            SendCmdData(
                eRobotCmdTypeUpgradeIng,
                m_nCurMotorID,
                nResendFrame - 1, //从0开始的帧索引
                (LPARAM)(m_STCurSendData.m_senddata.databuf + nSendedSize),
                DEFAULT_UPDATE_FRAME_SIZE, false
                );
        }
        else if(m_eCurPerformRobotCmd == eRobotCmdTypeUpgradeEnd)
        {
            AlphaRobotLog::getInstance()->printDebug("resend upgrade end request...\n");

            int nResendFrame = m_STCurSendData.m_nCurFrame - 1; //待重发的帧号，从1开始
            if(nResendFrame < 1)
                nResendFrame = 1;

            SendCmdData(eRobotCmdTypeUpgradeEnd, m_nCurMotorID, nResendFrame - 1, (LPARAM)m_STCurSendData.m_senddata.databuf,
                        m_STCurSendData.m_senddata.datalen, false);

        }

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_UPDATE_ING, Qt::PreciseTimer);
#endif
    }
}

void CRobotController::handleConnectRobotCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        if(m_pRobotCmd != &m_objRobotCmdOfBTProtocol)
        {
            m_pRobotCmd = &m_objRobotCmdOfBTProtocol;
            SendCmdData(eRobotCmdTypeConnectRobot);
            return; //for connect againt
        }

        NotifyObservers(WM_MSG_ROBOT_CONNECT, NULL, FALSE, NULL);
        AlphaRobotLog::getInstance()->printDebug(QString("Connect robot timeout, repeat: %1").arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug("resend connect robot request...\n");
        SendCmdData(eRobotCmdTypeConnectRobot);

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_CONNECT_ROBOT, Qt::PreciseTimer);
    }
}

void CRobotController::handleDisconnectRobotCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_ROBOT_DISCONNECT, NULL, FALSE, NULL);

        // 拔出机器人时会出现超时，此时也要关闭串口
        Req_ClosePort();

        AlphaRobotLog::getInstance()->printDebug(QString("Disonnect robot timeout, repeat: %1").arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug("resend disconnect robot request...\n");
        SendCmdData(eRobotCmdTypeDisconnectRobot);

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_DISCONNECT_ROBOT, Qt::PreciseTimer);
    }
}

void CRobotController::handleRobotDebugCmdTimeout()
{
//    if(m_nCurResendRequest > ROBOT_CMD_RESEND_CNT)
//    {
//        m_STCurSendData.Reset();
//        setCurrentPerformRobotCmd(eRobotCmdTypeNone);

//        NotifyObservers(WM_MSG_ROBOT_DISCONNECT, NULL, FALSE, NULL);
//    }
//    else
//    {
//        qDebug("resend debug robot request...\n");
//        SendCmdData(eRobotCmdTypeDebug);

//        m_bHasResendRequest = TRUE;
//        m_nCurResendRequest++;

//        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_DEBUG, Qt::PreciseTimer);
//    }
}

void CRobotController::handleSwitchModeCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_ROBOT_SWITCHMODE_RESULT, NULL, FALSE, NULL);
        AlphaRobotLog::getInstance()->printDebug(QString("Switch mode timeout, repeat: %1").arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug("resend switch mode request...\n");
        SendCmdData(eRobotCmdTypeSwitchMode, m_eSwitchMode);

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_SWITCH_MODE, Qt::PreciseTimer);
    }
}

void CRobotController::handleResetRobotCmdTimeout()
{
//    if(m_nCurResendRequest > ROBOT_CMD_RESEND_CNT)
//    {
//        m_STCurSendData.Reset();
//        setCurrentPerformRobotCmd(eRobotCmdTypeNone);

//        NotifyObservers(, NULL, FALSE, NULL);
//    }
//    else
//    {
//        qDebug("resend switch mode request...\n");
//        SendCmdData(eRobotCmdTypeReset);

//        m_bHasResendRequest = TRUE;
//        m_nCurResendRequest++;

//        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_RESET, Qt::PreciseTimer);
//    }
}

void CRobotController::handleReadSNCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_ROBOT_READSN_RESULT, NULL, 0, NULL);
        AlphaRobotLog::getInstance()->printDebug(QString("Read SN timeout, repeat: %1").arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        qDebug("resend read SN request...\n");
        SendCmdData(eRobotCmdTypeReadSN);

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_READ_SN, Qt::PreciseTimer);
    }
}

void CRobotController::handleWriteSNCmdTimeout()
{
//    if(m_nCurResendRequest > ROBOT_CMD_RESEND_CNT)
//    {
//        m_STCurSendData.Reset();
//        setCurrentPerformRobotCmd(eRobotCmdTypeNone);

//        NotifyObservers(WM_MSG_ROBOT_WRITESN_RESULT, NULL, 0, NULL);
//    }
//    else
//    {
//        qDebug("resend write SN request...\n");
//        SendCmdData(eRobotCmdTypeWriteSN);

//        m_bHasResendRequest = TRUE;
//        m_nCurResendRequest++;

//        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_WRITE_SN, Qt::PreciseTimer);
//    }
}

void CRobotController::handleReadUidCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_ROBOT_READUID_RESULT, NULL, 0, NULL);
        AlphaRobotLog::getInstance()->printDebug(QString("Read UID timeout, repeat: %1").arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug("resend read UID request...\n");
        SendCmdData(eRobotCmdTypeReadUID);

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_READ_UID, Qt::PreciseTimer);
    }
}

void CRobotController::handleCheckUtf8CmdTimeout()
{

}

void CRobotController::handleGetRobotSysEmbedVerCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();
        setCurrentPerformRobotCmd(eRobotCmdTypeNone);

        NotifyObservers(WM_MSG_ROBOT_SOFTWARE_VERSION_GET, NULL, 0, NULL);//读版本失败
        AlphaRobotLog::getInstance()->printError(QString("Read robot system embeded version timeout, repeat: %1").arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug("resend get system embeded version request...\n");

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_GET_SYS_EMBEDED_VERSION, Qt::PreciseTimer);
    }
}

void CRobotController::handleGetRobotHardwareVersionCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_ROBOT_HARDWARE_VERSION_GET, NULL, false, NULL);//读版本失败
        AlphaRobotLog::getInstance()->printError(QString("Read robot hardware version timeout, repeat: %1").arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug("resend get robot hardware version request...\n");
        SendCmdData(eRobotCmdTypeGetRobotHardwareVer);

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_GET_HARDWARE_VERSION, Qt::PreciseTimer);
    }
}

void CRobotController::handleGetBatteryStatusCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_ROBOT_BATTERY_STATUS_GET, NULL, false, NULL);
        AlphaRobotLog::getInstance()->printError(QString("Get battery status timeout, repeat: %1").arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug("resend get robot  battery request...\n");
        SendCmdData(eRobotCmdTypeGetBatteryStatus);

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_GET_BATTERY_STATUS, Qt::PreciseTimer);
    }
}

void CRobotController::handleGetDeviceStatusCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();

        NotifyObservers(WM_MSG_ROBOT_TF_CARD_STATUS, NULL, false, NULL);
        AlphaRobotLog::getInstance()->printError(QString("Get robot device timeout, repeat: %1").arg(ROBOT_CMD_RESEND_CNT));
    }
    else
    {
        AlphaRobotLog::getInstance()->printDebug("resend get robot device status request...\n");
        SendCmdData(eRobotCmdTypeGetDeviceStatus);

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_GET_DEVICE_STATUS, Qt::PreciseTimer);
    }
}

void CRobotController::handleGetRobotActionListCmdTimeout()
{
    // 请求动作列表不要重发，以免出现接收了一部分动作名后因为超时
    // 重发重复接收的问题
     AlphaRobotLog::getInstance()->printDebug("Get robot action list timeout\n");

     resetData();
     m_STCurSendData.Reset();

     NotifyObservers(WM_MSG_GET_ROBOT_ACTION_LIST_RST, NULL, false, NULL);
}

void CRobotController::handleGetSomeResInfosCmdTimeout()
{
    // 请求动作列表不要重发，以免出现接收了一部分动作名后因为超时
    // 重发重复接收的问题
     AlphaRobotLog::getInstance()->printDebug("GetSomeResInfosCmdTimeout\n");

     resetData();
     m_STCurSendData.Reset();
}

void CRobotController::handleDeleteRobotActionTimeout()
{
    // 请求动作列表不要重发，以免出现接收了一部分动作名后因为超时
    // 重发重复接收的问题
     AlphaRobotLog::getInstance()->printDebug("Delete robot action timeout\n");

     resetData();
     m_STCurSendData.Reset();

     NotifyObservers(WM_MSG_DELETE_ROBOT_ACTION_RST, NULL, (int)eDeleteRobotActionRspTimeout, (LPARAM)m_baDeleteActionName.data());
}

void CRobotController::handleRobotWifiScanCmdTimeout()
{
    AlphaRobotLog::getInstance()->printDebug("RobotWifiScan timeout\n");

    resetData();
    m_STCurSendData.Reset();

    NotifyObservers(WM_MSG_GET_ROBOT_WIFI_LIST_RST, NULL, false, NULL);
}

void CRobotController::handleRobotConnectWifiCmdTimeout()
{
    AlphaRobotLog::getInstance()->printDebug("RobotConnectWifi timeout\n");

    resetData();
    m_STCurSendData.Reset();

    NotifyObservers(WM_MSG_ROBOT_CONNECT_WIFI_RST, NULL, false, NULL);
}

void CRobotController::handleRobotManalUpGradeCmdTimeout()
{
    AlphaRobotLog::getInstance()->printDebug("ManalUpGrade timeout\n");

    resetData();
    m_STCurSendData.Reset();
    
   NotifyObservers(WM_MSG_ROBOT_MANAL_UPGRADE_RST, NULL, false, NULL);
}

void CRobotController::resetData()
{
    m_nCurResendRequest = 0;
    //m_bHasResendRequest = FALSE;
}

void CRobotController::handleGetMotorVersionCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();
        setCurrentPerformRobotCmd(eRobotCmdTypeNone);

        NotifyObservers(WM_MSG_MOTOR_READVERSION, NULL, FALSE, NULL);  //读版本失败
    }
    else
    {
#if 1
//                        if(m_eCurPerformRobotCmd == eRobotCmdTypeUpgradePrepare)
//                        {
            qDebug("resend read motor version request...\n");

            SendCmdData(eRobotCmdTypeGetMotorVersion, m_nCurMotorID);
//                        }

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_GET_MOTOR_VERSION, Qt::PreciseTimer);
#endif
    }
}

void CRobotController::handleSetIDCmdTimeout()
{
    if(m_nCurResendRequest >= ROBOT_CMD_RESEND_CNT)
    {
        resetData();
        m_STCurSendData.Reset();
        setCurrentPerformRobotCmd(eRobotCmdTypeNone);

        NotifyObservers(WM_MSG_MOTOR_MODID, NULL, FALSE, NULL);  //修改ID失败
    }
    else
    {
#if 1
//                        if(m_eCurPerformRobotCmd == eRobotCmdTypeUpgradePrepare)
//                        {
            qDebug("resend set id request...\n");

            SendCmdData(eRobotCmdTypeSetID, m_nCurMotorID);
//                        }

        m_bHasResendRequest = TRUE;
        m_nCurResendRequest++;

        m_nCurRobotCmdTimerId = startTimer(TIMEOUT_CMD_SET_ID, Qt::PreciseTimer);
#endif
    }
}

/**************************************************************************
* 函数名: interruptActionSync
* 功能: 中断动作文件同步，待执行完其他操作后再恢复动作同步
* 参数:
*    @[in ] eCmdType: 指令类型
*    @[in ] lParam1: 参数1
*    @[in ] lParam2: 参数2
*    @[in ] lParam3: 参数3
*    @[in ] lParam4: 参数4
* 返回值: void
* 时间: 2015/11/23
* 作者: hwx
*/
void CRobotController::interruptActionSync(emRobotCmdType eCmdType, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4)
{
    // 只针对开始传输和正在传输的指令，如果已经传到文件最后一帧了，就不要取消传输了
    if ((m_eCurPerformRobotCmd == eRobotCmdTypeTransFilePrepare || m_eCurPerformRobotCmd == eRobotCmdTypeTransFileIng) &&
         (eCmdType != eRobotCmdTypeTransFilePrepare && eCmdType != eRobotCmdTypeTransFileIng))
    {
         // 不能设置为eRobotCmdTypeNone，否则没有等待机器人应答就会发数据，导致串口挂死
        // 也不能不设置，否则会一直进入当前分支，因此增加一个unknown状态
        setCurrentPerformRobotCmd(eRobotCmdTypeUnkonwn);

        // 如果当前的操作不是取消同步文件时，先取消文件同步；如果已经是取消同步
        // 则不必再发取消指令
        if (eCmdType != eRobotCmdTypeTransFileCancel)
        {
            Req_CancelTransFile();
        }

        // 如果是升级，就不要接着传文件了，因为升级过程中机器人就断开连接了
        // 如果是动作文件，切不是文件传输，则处理完传输后需要恢复文件同步
        if (m_STCurSendData.m_eType != emTranFileUpdateSysEmbed && eCmdType != eRobotCmdTypeTransFileCancel)
        {
            Req_TransFile(m_pCurrentTransFile, emTranFileSynAction);
        }
    }
}

/**************************************************************************
* 函数名: setRobotSoftVersion
* 功能: 更新版本信息
* 参数:
*    @[in ] sNewVersion: 新版本信息
* 时间: 2016/07/25 15:00
* 作者:hwx
*/
void CRobotController::setRobotSoftVersion(const QString &sNewVersion)
{
    std::string strNewVersion = sNewVersion.toStdString();
    const char *pNewVersion = strNewVersion.c_str();

    if (pNewVersion && (strNewVersion.length() > 0))
    {
        int nTempSize = sizeof(m_szRobotSoftVersion);
        int nSize = strNewVersion.length() + 1;
        int nCopySize = qMin(nTempSize, nSize);
        strncpy(m_szRobotSoftVersion, pNewVersion, nCopySize);
    }
}

/**************************************************************************
* 函数名: slotOnStopTransFileTimeout
* 功能: 停止文件传输定时时间槽函数
* 参数:
* 时间: 2016/08/01 15:00
* 作者:hwx
*/
void CRobotController::slotOnStopTransFileTimeout()
{
    SendCmdData(eRobotCmdTypeTransFileCancel);
}

/**************************************************************************
* 函数名: slotOnSendCmd
* 功能: 发送机器人指令的槽函数
* 参数:
*    @[in ] oCmdType: 指令信息
* 返回值: void
* 时间: 2016/11/22
* 作者: hwx
*/
void CRobotController::slotOnSendCmd(CCommandType oCmdType)
{
     m_nCurRobotCmdTimerId = startTimer(oCmdType.getTimeoutValue(), Qt::PreciseTimer);
     AlphaRobotLog::getInstance()->printDebug(QString("Start timer: %1, timeout: %2, timerid: %3").arg(oCmdType.getCmdDisplayName())
                                              .arg(oCmdType.getTimeoutValue()).arg(m_nCurRobotCmdTimerId));
}

/**************************************************************************
* 函数名: slotOnDeviceRemoved
* 功能: 拔出USB的槽函数
* 参数:
* 返回值: void
* 时间: 2016/11/22
* 作者: hwx
*/
void CRobotController::slotOnDeviceRemoved()
{
    m_oRobotCommunicateThread.exit();
    m_oRobotCommunicateThread.wait(1000);

    m_oRobotCommunicateThread.abortAllData();

    AlphaRobotLog::getInstance()->printDebug("Abort robot send data");
}

void CRobotController::slotOnRobotProductTypeChange(int eUbtProductTypeOld, int eUbtProductTypeNew)
{
    if(eUbtProduct1E == UBXBase::getInstance()->getCurrentRobotProductType())
    {
       // RobotCmdBTProtocol::m_nTransFilePackageSize = TRANSFILE_PKGSIZE_128;
        RobotCmdBTProtocol::m_nTransFilePackageSize = TRANSFILE_PKGSIZE_1024;
        RobotCmdBTProtocol::m_nTransFilePerFrameSize = TRANSFILE_FRAMESIZE_NEW_FOR_1024;

        m_bSupportUTF8 = true;//1e is utf8
    }
    else
    {
        RobotCmdBTProtocol::m_nTransFilePackageSize = TRANSFILE_PKGSIZE_64;
        RobotCmdBTProtocol::m_nTransFilePerFrameSize = TRANSFILE_FRAMESIZE_NEW_FOR_128;

        m_bSupportUTF8 = false;//1s 1p not utf8
    }
}

/**************************************************************************
* 函数名: setCurrentPerformRobotCmd
* 功能: 机器人通信线程发送指定命令的槽函数
* 参数:
* 时间: 2016/11/22
* 作者:hwx
*/
void CRobotController::setCurrentPerformRobotCmd(emRobotCmdType eCmd)
{
    if (eCmd == eRobotCmdTypeNone)
    {
        AlphaRobotLog::getInstance()->printDebug("Set current perform robot cmd eRobotCmdTypeNone");

    }
    QMutexLocker locker(&m_mutex);

    if(g_bIsRecordLogValue)
    {
        QString temLog = QString("RobotCmd %1 > %2").arg(m_eCurPerformRobotCmd).arg(eCmd);
        AlphaRobotLog::getInstance()->printDebug(temLog);
        //qDebug() << temLog << endl;
    }

    m_eCurPerformRobotCmd = eCmd;
}

/**************************************************************************
* 函数名: getCurrentPerformRobotCmd
* 功能: 获取机器人正在传输的命令
* 参数:
* 时间: 2016/11/22
* 作者:hwx
*/
emRobotCmdType CRobotController::getCurrentPerformRobotCmd()
{
    QMutexLocker locker(&m_mutex);
    return m_eCurPerformRobotCmd;
}

/**************************************************************************
* 函数名: IsSupportNewProtocol
* 功能: 检测机器人是否支持新协议
* 返回值: 是否支持新协议
* 时间: 2016/05/27 15:11
* 作者: ZDJ
*/
bool CRobotController::IsSupportNewProtocol()
{
#if 0
    QString sInfo = "m_szRobotSoftVersion:(";
    for(int i = 0; i < sizeof(m_szRobotSoftVersion); i++)
    {
        QString sTemp;
        sTemp.sprintf("%02X ", m_szRobotSoftVersion[i]);
        sInfo += sTemp;
    }
    sInfo += ")";
    qDebug() << sInfo << endl;
#endif

#if 1
//    if(strcmp(m_szRobotSoftVersion, "2016041313") > 0)
//        return true;

        return true;
#endif
    return false;
}
