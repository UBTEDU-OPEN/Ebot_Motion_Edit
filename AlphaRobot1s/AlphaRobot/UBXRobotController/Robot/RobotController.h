/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：RobotController.h
* 创建时间：2015/10/18 14:39
* 文件标识：
* 文件摘要：机器人控制类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/10/18 14:39
* 版本摘要：
*/
#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H

#include <QMutex>
#include <QMutexLocker>

#include "RobotCmd.h"
//#include "SerialPort.h"
#include "DataPool.h"
#include "IMyPortInterface.h"
#include "MyHidClass.h"
#include <QWidget>

#include "Util.h"
#include "ccommandtype.h"
#include "crobotcommunicatethread.h"
#include "irobotcmd.h"
#include "robotcmdbtprotocol.h"


#ifdef Q_OS_WIN
#include "SerialPort.h"
#endif

//#pragma region 消息定义

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ACTION_DOWNLOAD_PREPARE;  //准备传输文件消息
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ACTION_DOWNLOAD_ING;      //正在传输文件消息
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ACTION_DOWNLOAD_END;      //结束传输文件消息
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_UPDATE_DOWNLOAD_PREPARE; // 准备下载更新
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_UPDATE_DOWNLOAD_ING; // 正在下载更新
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_UPDATE_DOWNLOAD_END; // 结束下载更新
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_TRANS_FILE_CANCEL; // 取消文件传输
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_PORT_OPENSTATECHANGE;     //端口打开状态改变消息
//extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ACTION_DOWNLOAD_ERROR;    //传输文件出错消息

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_READVERSION;        //舵机读版本消息
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_UPGRADE_PREPARE;    //舵机升级准备
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_UPGRADE_ING;        //舵机升级进行中
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_UPGRADE_END;        //舵机升级结束
//extern const UINT WM_MSG_MOTOR_UPGRADE_ERROR;      //舵机升级出错
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_MODID;              //舵机修改ID
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_GETID;              //舵机获取ID

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_SOFTWARE_VERSION_GET;        //获取机器人软件版本
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_HARDWARE_VERSION_GET;        //获取机器人硬件版本
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_ANGLE_GET;                   //获取舵机角度
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_ANGLE_SET;                   //设置舵机角度
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_MOVE;                   // 转动舵机

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_CONNECT;            //连接机器人
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_DISCONNECT;         //断开机器人

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_OFFSET_GET;         //舵机获取偏移值
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_MOTOR_OFFSET_SET;         //舵机修改偏移值

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_TRANSDATA;          //机器人数据转发
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_SWITCHMODE_RESULT;           //机器人转换模式结果

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_READSN_RESULT;          //机器人读序列号结果
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_WRITESN_RESULT;         //机器人写序列号结果

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_READUID_RESULT;         //机器人读UID结果

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_BATTERY_STATUS_GET;   //BATTERY STATUS结果
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_TF_CARD_STATUS;   // 机器人是否插入TF卡

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_GET_ROBOT_ACTION_LIST_RST;  // 获取动作列表
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_DELETE_ROBOT_ACTION_RST;  // 删除机器人动作

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_GET_ROBOT_WIFI_LIST_RST;  // get robot wifi list
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_CONNECT_WIFI_RST;  // robot  connect wifi

extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_MANAL_UPGRADE_RST;  ////手动升级
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_GET_AUTO_UPDATE_PROGRESS_RST;  //获取自动升级进度
extern UBXROBOTCONTROLLER_API const UINT WM_MSG_ROBOT_IS_INSTATLING_UPDATE_RST;  //是否安装升级通知


extern UBXROBOTCONTROLLER_API const UINT WM_MSG_TFCARD_NO_CAPACITY;  // 内存卡满

//#pragma endregion

class UBXROBOTCONTROLLER_API CRobotController :  public QWidget, public zxObserverSubjectBase
{
        Q_OBJECT

public:	
        explicit CRobotController(QWidget* parent = NULL);
        ~CRobotController();

//#pragma region

public:	
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
        void SendCmdData(emRobotCmdType eCmdType, LPARAM lParam1 = NULL, LPARAM lParam2 = NULL,
                                            LPARAM lParam3 = NULL, LPARAM lParam4 = NULL, bool bEnqueue = true);

        /**************************************************************************
        * 函数名: sendRobotCommand
        * 功能: 发送指定命令
        * 参数:
        *    @[in ] oCmdType: 指令信息
        * 时间: 2016/11/22
        * 作者:hwx
        */
        void sendRobotCommand(CCommandType &cmdType);

        /**************************************************************************
        * 函数名: setCurrentPerformRobotCmd
        * 功能: 机器人通信线程发送指定命令的槽函数
        * 参数:
        * 时间: 2016/11/22
        * 作者:hwx
        */
        void setCurrentPerformRobotCmd(emRobotCmdType eCmd);

        /**************************************************************************
        * 函数名: getCurrentPerformRobotCmd
        * 功能: 获取机器人正在传输的命令
        * 参数:
        * 时间: 2016/11/22
        * 作者:hwx
        */
        emRobotCmdType getCurrentPerformRobotCmd();

        /**************************************************************************
        * 函数名: GetPortOpenState
        * 功能: 获取端口打开状态
        * 时间: 2015/10/20 11:09
        * 作者: ZDJ
        */
        BOOL GetPortOpenState();

        /**************************************************************************
        * 函数名: getPortInterface
        * 功能: 获取数据传输的接口
        * 时间: 2016/07/05
        * 作者: hwx
        */
        IMyPortInterface *getPortInterface() { return m_pPort; }

        /**************************************************************************
        * 函数名: GetCurMotorID
        * 功能: 获取当前操作的舵机ID
        * 时间: 2015/10/21 19:41
        * 作者: ZDJ
        */
        int GetCurMotorID();

        /**************************************************************************
        * 函数名: GetProcessRate
        * 功能: 获取处理速度，单位字节/s
        * 时间: 2015/10/28 12:26
        * 作者: ZDJ
        */
        int GetProcessRate();

        /**************************************************************************
        * 函数名: StartConncectTimer
        * 功能: 启动连接端口的时钟
        * 时间: 2015/11/01 16:45
        * 作者: ZDJ
        */
        void StartConncectTimer();

        /**************************************************************************
        * 函数名: StartDisconncectTimer
        * 功能: 启动断开端口的时钟
        * 时间: 2015/11/01 16:45
        * 作者: ZDJ
        */
        void StartDisconncectTimer();

#ifdef Q_OS_WIN
        /**************************************************************************
        * 函数名: OnDeviceChange
        * 功能: 设备更改通知
        * 时间: 2015/11/16 10:57
        * 作者: ZDJ
        */
        bool OnDeviceChange(MSG *msg, long *result);
#endif

        /**************************************************************************
        * 函数名: IsSupportNewProtocol
        * 功能: 检测机器人是否支持新协议
        * 返回值: 是否支持新协议
        * 时间: 2016/05/27 15:11
        * 作者: ZDJ
        */
        bool IsSupportNewProtocol();

private:
#if 0
        /**************************************************************************
        * 函数名: OpenComPort
        * 功能: 打开串口
        * 参数:
        *    @[in ] sComName: 串口名称，如COM1
        *    @[in ] nBandrate: 打开端口使用的波特率
        *    @[out] lpszErrStr: 错误信息
        * 返回值: 成功返回TRUE，失败返回FALSE
        * 时间: 2015/07/16 17:09
        * 作者: ZDJ
        */
        BOOL OpenComPort(const QString& sComName, int nBandrate, char* lpszErrStr);
#endif
        /**************************************************************************
        * 函数名: RegisterDeviceNotify
        * 功能: 注册设备通知
        * 时间: 2015/09/08 10:08
        * 作者: ZDJ
        */
        void RegisterDeviceNotify();

#ifdef Q_OS_WIN
        /**************************************************************************
        * 函数名: UpdateDevice
        * 功能: 设备更新通知
        * 时间: 2015/07/22 10:25
        * 作者: ZDJ
        */
        void UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam);
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
        int ProcessData(byte* databuf, int datalen);

        // 处理文件传输响应
        void processFileTransResp(byte* databuf, int datalen);

        // 处理请求机器人动作列表
        void processRequestRobotActionList(byte *databuf, int nDataLen);
        bool processGetSomeResInfos(byte *databuf, int nDataLen);

        // 处理取消文件传输响应
        void processCancelFileTransResp(byte* databuf, int datalen);

        // 处理舵机升级响应
        void processUpdateMotorResp(byte* databuf, int datalen);

        // 处理读取舵机angle应答
        bool processGetMotorAngleResp(byte* databuf, int datalen);

        // 处理读取和设置舵机偏移应答
        void processMotorOffsetResp(byte* databuf, int datalen);

        // 处理获取舵机ID
        void processGetMotorIDResp(byte* databuf, int datalen);

        // 处理获取舵机版本应答
        void processGetMotorVersionResp(byte* databuf, int datalen);

        // 处理获取机器人嵌入式版本应答
        void processGetRobotSoftwareVersionResp(byte* databuf, int datalen);

        // 处理获取机器人型号应答
        void processGetRobotTypeResp(byte* databuf, int datalen);

        // 处理获取机器人电量信息应答
        void processGetBatteryResp(byte* databuf, int datalen);

        // 处理机器人连接状态应答
        void processRobotConnectStatusResp(byte* databuf, int datalen);

        // 处理机器人设备状态，如TF卡等
        void processRobotDeviceStatusResp(byte* databuf, int datalen);

        // 处理读取机器人序列号应答
        void processReadRobotSNResp(byte* databuf, int datalen);

        // 处理机器人模式切换应答
        void processRobotSwitchModeResp(byte* databuf, int datalen);

        // 处理删除机器人动作应答
        void processDeleteRobotActionResp(byte* databuf, int datalen);

        // 处理设置机器人时间应当
        void processSetTimeResp(byte* databuf, int datalen);

        // handle the wifi scan ans
        bool processWifiScan(byte* databuf, int datalen);

        //handle robot connect to the wifi ans
        bool processRobotConnectWifi(byte* databuf, int datalen);

        //handle robot manal update ans
        bool processRobotManalUpGrade(byte* databuf, int datalen);
        ////获取自动升级进度
        bool processRobotGetAutoUpdadeProgress(byte* databuf, int datalen);
        //是否进入安装升级
        bool processRobotIsInstallingUpdate(byte* databuf, int datalen);




#ifdef Q_OS_WIN
        /**************************************************************************
        * 函数名: FindMySTMDevice
        * 功能: 查找STM设备
        * 时间: 2015/11/01 16:30
        * 作者: ZDJ
        */
        BOOL FindMySTMDevice();
#endif        

        /**************************************************************************
        * 函数名: IsSupportUTF8
        * 功能: 获取是否支持UTF8
        * 时间: 2015/11/25 11:26
        * 作者: ZDJ
        */
        BOOL IsSupportUTF8();

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
        int GetRobotSoftVersion(char* pszVersion, int nSize);

        /**************************************************************************
        * 函数名: killRobotCmdTimer
        * 功能: 停止机器人指定定时器
        * 参数:
        * 返回值:
        * 时间: 2016/11/22
        * 作者:hwx
        */
        void killRobotCmdTimer();

        // 机器人操作指令超时处理
        void handleRobotCmdTimeout();

        // 同步动作文件指令超时处理
        void handleTransFileCmdTimeout();

        // 取消文件传输指令超时处理
        void handleTransFileCancelCmdTimeout();

        // 获取舵机偏移值指令超时处理
        void handleGetOffsetCmdTimeout();

        // 设置舵机偏移指令超时处理
        void handleSetOffsetCmdTimeout();

        // 获取舵机角度指令超时处理
        void handleGetAngleCmdTimeout();

        // 设置舵机转到指定角度指令超时
        void handleMotorMoveCmdTimeout();

        // 获取ID指令超时
        void handleGetIDCmdTimeout();

        // 处理设置ID指令超时
        void handleSetIDCmdTimeout();

        // 获取舵机版本指令超时处理
        void handleGetMotorVersionCmdTimeout();

        // 舵机升级指令超时处理
        void handleMotorUpdateCmdTimeout();

        // 连接机器人指令超时处理
        void handleConnectRobotCmdTimeout();

        // 端口机器人连接超时处理
        void handleDisconnectRobotCmdTimeout();

        // 仿真机器人指令超时
        void handleRobotDebugCmdTimeout();

        // 切换模式指令超时处理
        void handleSwitchModeCmdTimeout();

        // 复位机器人指令超时处理
        void handleResetRobotCmdTimeout();

        // 读序列号指令超时处理
        void handleReadSNCmdTimeout();

        void handleWriteSNCmdTimeout();

        // 读机器人UID超时处理
        void handleReadUidCmdTimeout();

        void handleCheckUtf8CmdTimeout();

        // 获取嵌入式系统版本超时处理
        void handleGetRobotSysEmbedVerCmdTimeout();

        // 机器人硬件版本超时处理
        void handleGetRobotHardwareVersionCmdTimeout();

        // 获取机器人电池信息超时处理
        void handleGetBatteryStatusCmdTimeout();

        // 获取机器人设备状态超时处理
        void handleGetDeviceStatusCmdTimeout();

        // 读取机器人动作列表超时处理
        void handleGetRobotActionListCmdTimeout();
        void handleGetSomeResInfosCmdTimeout();

        // 删除机器人动作超时处理
        void handleDeleteRobotActionTimeout();
        //robot wifi scan time out
        void handleRobotWifiScanCmdTimeout();
        //robot wifi connect time out
        void handleRobotConnectWifiCmdTimeout();
        void handleRobotManalUpGradeCmdTimeout();


        // 复位数据
        void resetData();

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
        void interruptActionSync(emRobotCmdType eCmdType, LPARAM lParam1 = NULL, LPARAM lParam2 = NULL, LPARAM lParam3 = NULL, LPARAM lParam4 = NULL);

public slots:
        /**************************************************************************
        * 函数名: setRobotSoftVersion
        * 功能: 更新版本信息
        * 参数:
        *    @[in ] sNewVersion: 新版本信息
        * 时间: 2016/07/25 15:00
        * 作者:hwx
        */
        void setRobotSoftVersion(const QString &sNewVersion);

private slots:
        /**************************************************************************
        * 函数名: slotOnStopTransFileTimeout
        * 功能: 停止文件传输定时时间槽函数
        * 参数:
        * 时间: 2016/08/01 15:00
        * 作者:hwx
        */
        void slotOnStopTransFileTimeout();

        /**************************************************************************
        * 函数名: slotOnSendCmd
        * 功能: 发送机器人指令的槽函数
        * 参数:
        *    @[in ] oCmdType: 指令信息
        * 返回值: void
        * 时间: 2016/11/22
        * 作者: hwx
        */
        void slotOnSendCmd(CCommandType oCmdType);

        /**************************************************************************
        * 函数名: slotOnDeviceRemoved
        * 功能: 拔出USB的槽函数
        * 参数:
        * 返回值: void
        * 时间: 2016/11/22
        * 作者: hwx
        */
        void slotOnDeviceRemoved();
        void slotOnRobotProductTypeChange(int eUbtProductTypeOld, int eUbtProductTypeNew);

//#pragma endregion

signals:
        /**************************************************************************
        * 函数名: sigOnSendCmd
        * 功能: 发送机器人指令的信号
        * 参数:
        *    @[in ] oCmdType: 指令信息
        * 返回值: void
        * 时间: 2016/11/22
        * 作者: hwx
        */
        void sigOnSendCmd(CCommandType oCmdType);

        // 拔出USB
        void sigOnDeviceRemoved();

//#pragma region 请求

public:	
        /**************************************************************************
        * 函数名: Req_Connect
        * 功能: 请求连接指定端口的机器人
        * 时间: 2015/10/18 14:54
        * 作者: ZDJ
        */
        void Req_Connect();

        /**************************************************************************
        * 函数名: Req_Disconnect
        * 功能: 请求断开机器人的连接
        * 时间: 2015/10/18 14:55
        * 作者: ZDJ
        */
        void Req_Disconnect(void);

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
        BOOL Req_TransFile(const char* pszSrcFilePath, const char* pszTransFilePath, const char* pszDestFilePath, const char* pszGuid, emTranFileType eType);

        /**************************************************************************
        * 函数名: Req_TransFile
        * 功能: 请求传输文件
        * 参数:
        *    @[in ] pTransFile: 文件数据
        * 返回值: 请求成功返回TRUE，失败返回FALSE
        * 时间: 2015/10/18 14:55
        * 作者: ZDJ
        */
        BOOL Req_TransFile(TRANSFILEINFO* pTransFile, emTranFileType eType);

        /**************************************************************************
        * 函数名: Req_CancelTransFile
        * 功能: 请求取消传输文件
        * 时间: 2015/10/20 9:43
        * 作者: ZDJ
        */
        void Req_CancelTransFile();

        /**************************************************************************
        * 函数名: stopTransFile
        * 功能: 停止文件传输
        * 时间: 2016/08/01 9:43
        * 作者:hwx
        */
        void stopTransFile();

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
        BOOL Req_OpenPort(int nPortType, DWORD dwVID, DWORD dwPID, ...);

        /**************************************************************************
        * 函数名: Req_AutoOpenPort
        * 功能: 请求自动打开端口
        * 返回值: 成功返回TRUE，失败返回FALSE
        * 时间: 2015/10/21 20:49
        * 作者: ZDJ
        */
        BOOL Req_AutoOpenPort();

        /**************************************************************************
        * 函数名: Req_ClosePort
        * 功能: 请求关闭端口
        * 时间: 2015/10/19 9:23
        * 作者: ZDJ
        */
        BOOL Req_ClosePort();

        /**************************************************************************
        * 函数名: Req_SwitchMode
        * 功能: 请求切换模式
        * 时间: 2015/10/19 9:23
        * 作者: ZDJ
        */
        BOOL Req_SwitchMode(emRobotSwitchMode eMode);

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
        BOOL Req_MotorUpgrade(int nMotorID, const char* pszFilePath);

        /**************************************************************************
        * 函数名: Req_MotorReadVersion
        * 功能: 读版本
        * 参数:
        *    @[in ] nMotorID: 舵机ID
        * 返回值: void
        * 时间: 2015/10/21 18:32
        * 作者: ZDJ
        */
        void Req_MotorReadVersion(int nMotorID);

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
        BOOL Req_MotorWriteID(int nMotorOldID, int nMotorNewID);

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
        BOOL Req_RobotDebug(MOTOR_DATA* pMotorData, int nMotorCount, int nRunTime, int nAllTime);

        /**************************************************************************
        * 函数名: Req_MotorGetOffset
        * 功能: 获取舵机偏移值
        * 参数:
        *    @[in ] nMotorID: 舵机ID
        * 返回值: 成功返回TRUE，失败返回FALSE
        * 时间: 2015/10/21 18:36
        * 作者: ZDJ
        */
        BOOL Req_MotorGetOffset(int nMotorID);

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
        BOOL Req_MotorSetOffset(int nMotorID, int nOffset);

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
        BOOL Req_RobotReset(MOTOR_DATA* pMotorData, int nMotorCount, int nRunTime, int nAllTime);

        /**************************************************************************
        * 函数名: Req_RobotReadSerialNum
        * 功能: 读序列号
        * 时间: 2015/11/16 11:02
        * 作者: ZDJ
        */
        BOOL Req_RobotReadSerialNum();

        /**************************************************************************
        * 函数名: Req_RobotWriteSerialNum
        * 功能: 写序列号
        * 时间: 2015/11/16 11:02
        * 作者: ZDJ
        */
        BOOL Req_RobotWriteSerialNum(char* pszSn, int nLen);

        /**************************************************************************
        * 函数名: Req_RobotReadUID
        * 功能: 读UID
        * 时间: 2015/11/16 11:02
        * 作者: ZDJ
        */
        BOOL Req_RobotReadUID();

        /**************************************************************************
        * 函数名: Req_RobotGetAngle
        * 功能: 读角度
        * 参数:
        *    @[in ] nMotorID: 舵机ID
        * 时间: 2015/11/16 11:02
        * 作者: ZDJ
        */
        BOOL Req_RobotGetAngle(int nMotorID);

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
        BOOL Req_RobotSetAngle(int nMotorID, int nAngle, int nTime);

        /**************************************************************************
        * 函数名: Req_RobotHardwareVersion
        * 功能: 读机器人类型
        * 时间: 2016/07/22 18:32
        * 作者: hwx
        */
        void Req_RobotHardwareVersion();

        /**************************************************************************
        * 函数名: Req_RobotReadVersion
        * 功能: 读机器人嵌入式软件版本
        * 时间: 2015/10/21 18:32
        * 作者: ZDJ
        */
        void Req_RobotSysEmbeddedVersion();

        /**************************************************************************
        * 函数名: Req_RobotBatteryStatus
        * 功能: 读机器人battery status
        * 时间: 20160824
        * 作者: asu
        */
        void Req_RobotBatteryStatus();

        /**************************************************************************
        * 函数名: Req_RobotDeviceStatus
        * 功能: 请求机器人设备状态
        * 时间: 2016/10/25
        * 作者:  hwx
        */
        void Req_RobotDeviceStatus();

        /**************************************************************************
        * 函数名: Req_RobotActionList
        * 功能: 请求机器人动作列表
        * 参数:
        *    @[in ] pActionDir: 动作路径
        *    @[in ] nActionDirLen: 动作路径长度
        * 时间: 2016/12/01
        * 作者:  hwx
        */
        void Req_RobotActionList(const char *pActionDir, int nActionDirLen);
        void Req_RobotResInfos(emGetSomeTypeResInfoResType eResInfosType, emGetSomeTypeResInfoCmdFlag eResInfoCmdFlag,
                               char *pParam = NULL, int nLenParam = 0);

        /**************************************************************************
        * 函数名: Req_DeleteRobotAction
        * 功能: 请求删除机器人动作
        * 参数:
        *    @[in ] pActionName: 动作名称，需要转为Ascii码
        *    @[in ] nActionNameLen: 动作名称长度
        * 时间: 2016/12/01
        * 作者:  hwx
        */
        void Req_DeleteRobotAction(const char *pActionName, int nActionNameLen);

        /**************************************************************************
        * 函数名: Req_RobotWifiScan
        * 功能: 请求机器人WIFI列表
        * 参数:
        *    @[in ] nWifiStat:
        * 时间: 2017/02/27
        * 作者:  asu
        */
        void Req_RobotWifiScan(int nWifiStat = 0);

        /**************************************************************************
        * 函数名: Req_RobotConnectWifi
        * 功能: 请求机器人connect WIFI
        * 参数:
        *    @[in ] strESSID:
        *    @[in ] strPassKey:
        * 时间: 2017/03/08
        * 作者:  asu
        */
        void Req_RobotConnectWifi(QString strESSID, QString strPassKey);


        /**************************************************************************
        * 函数名: Req_SetTime
        * 功能: 设置机器人时间
        * 参数:
        *    @[in ] pDateTime:时间信息
        * 时间: 2017/03/14
        * 作者:  hwx
        */
        void Req_SetTime(const QDateTime *pDateTime);

        /**************************************************************************
        * 函数名: Req_ManalUpGrade
        * 功能: 请求机器人Manal UpGrade
        * 参数:
        * 时间: 2017/03/08
        * 作者:  asu
        */
        void Req_ManalUpGrade();


//#pragma endregion

//#pragma region 窗口消息响应        

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
        LRESULT OnReceiveChar(WPARAM wParam, LPARAM lParam);

        virtual void timerEvent(QTimerEvent *event);
        virtual void customEvent(QEvent *event);

#ifdef Q_OS_MAC

        /**************************************************************************
        * 函数名: OnDeviceArrival
        * 功能: 设备接入通知
        * 时间: 2015/11/16 10:57
        * 作者: ZDJ
        */
        static void OnDeviceArrival(void *context, IOReturn result, void *sender, IOHIDDeviceRef device);

        /**************************************************************************
        * 函数名: OnDeviceRemoval
        * 功能: 设备移除通知
        * 时间: 2015/11/16 10:57
        * 作者: ZDJ
        */
        static void OnDeviceRemoval(void *context, IOReturn result, void *sender, IOHIDDeviceRef device);
#endif
//#pragma endregion

public:
    static int portRecvData(void* pUser, byte* pData, int nDataLen);

private:
        IRobotCmd* m_pRobotCmd; //机器人命令类
        CRobotCmd  m_objRobotCmdOfPCOld;   //机器人命令类
        RobotCmdBTProtocol m_objRobotCmdOfBTProtocol;


        IMyPortInterface*  m_pPort;         //端口实例
        CDataPool          m_datapool;      //数据缓冲池
        DEVIDTYPE          m_stDevIDInfo;   //当前端口id数据

        //const UINT    g_nCalcuTransRateTimerID = 5002;  //计算传输速率时钟id
        //const int     g_nCalcuTransRatePeriod = 100;    //计算传输速率时钟周期，单位毫秒
        UINT          m_nCheckUtf8TimerID;   //检查支持UTF8时钟id

        UINT          m_nConnectPortTimerID;     //连接端口时钟id
        UINT          m_nDisconnectPortTimerID;  //断开端口时钟id

        int                      m_nTransFileFrameSize;  //传输数据帧长度
        int                      m_nCurResendRequest;   //当前正在重发请求次数
        int                      m_nCurRobotCmdTimerId;     // 机器人指定定时器ID

        int                      m_nCurMotorID;   //当前操作舵机ID
        int                      m_nCurMotorOffset;  // 舵机偏移、舵机角度
        int                      m_nCurTime;  // 舵机转到指定角度需要的时间

        QByteArray      m_baDeleteActionName;  // 记录当前正在删除的动作名称

        BOOL               m_bCurFileSendComplete;    //当前文件是否发送完成
        BOOL               m_bCancelTransFile;  //取消传输文件标识
        BOOL               m_bHasResendRequest; //是否重发过数据
        BOOL               m_bSupportUTF8;  //是否支持UTF8

        emRobotSwitchMode m_eSwitchMode; // 切换模式

        static CRobotController* m_pControllerStatic;

        char               m_szRobotSoftVersion[64]; //机器人软件版本号
        char               m_szRobotBatteryStatus[16];//电池回复结果

        CRobotCommunicateThread m_oRobotCommunicateThread; // 和机器人通信的线程类

        emRobotCmdType m_eCurPerformRobotCmd; //当前正在发送指令类型
        QMutex m_mutex; // 串口状态锁

        SENDDATAOBJ   m_STCurSendData;               //当前发送的文件数据
        TRANSFILEINFO *m_pCurrentTransFile; // 记录当前正在传输的文件信息，当因为其他操作打断文件传输时，恢复重传
};

#endif

