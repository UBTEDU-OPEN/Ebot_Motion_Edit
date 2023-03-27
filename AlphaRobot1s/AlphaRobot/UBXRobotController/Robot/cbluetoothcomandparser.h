
/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CBluetoothComandParser.h
* 创建时间：2016/12/06
* 文件标识：
* 文件摘要：机器人蓝牙操作命令类
*
* 当前版本：1.0.0.0
* 作    者：asu
* 版本摘要：
*/



#ifndef CBLUETOOTHCOMANDPARSER_H
#define CBLUETOOTHCOMANDPARSER_H


#include "../common/UBXPublic.h"
#include <string>


const int ROBOT_CMD_BLUE_TOOTH_START_FLAG0 = 0xFB;
const int ROBOT_CMD_BLUE_TOOTH_START_FLAG1 = 0xBF;

const int ROBOT_CMD_BLUE_TOOTH_END_FLAG = 0xED;

enum emRobotCmdBlueToothPosition
{
    eBTCmdPosStartFlag0 = 0,
    eBTCmdPosStartFlag1 = 1,
    eBTCmdPosCmdLength  = 2,
    eBTCmdPosCmd = 3,
    eBTCmdPosFirstParamStart = 4,
    eBTCmdPosParam1 = eBTCmdPosFirstParamStart,
    eBTCmdPosParam2 ,
    eBTCmdPosParam3 ,
    eBTCmdPosParam4 ,
};

enum emRobotBTCmdFlage
{
    eBTCmdInvalid = 0x00, //not define

    eBTCmdHandshake = 0x01,

    eBTCmdGetActList = 0x02, // 获取机器人动作
    eBTCmdGetActListPrepare = eBTCmdGetActList,
    eBTCmdGetActListIng = 0x80, // 正在传输动作名称
    eBTCmdGetActListEnd = 0x81, // 结束传输动作名称    

    eBTCmdGetSomeTypeResInfo = 0x90, //某种资源信息命令，公有命令, 一推一拉
    eBTCmdGetSomeTypeResInfoPrepare = eBTCmdGetSomeTypeResInfo,
    eBTCmdGetSomeTypeResInfoIng = 0x91,
    eBTCmdGetSomeTypeResInfoEnd = 0x92,


    eBTCmdPlayAct = 0x03,    
    eBTCmdGetRemoteActList = 0x04,
    eBTCmdStopPlayAct = 0x05,
    eBTCmdAudioControl = 0x06,
    eBTCmdPlayActControl = 0x07,
    eBTCmdHeartbeat = 0x08,
    eBTCmdBTNameSet = 0x09,
    eBTCmdGetDriveStatus = 0x0A,
    eBTCmdSetAudioValue = 0x0B,
    eBTCmdServosPowerOff = 0x0C,
    eBTCmdServosLedControl = 0x0D,
    eBTCmdAdjustClock = 0x0E,
    eBTCmdGetAlarmClockValue = 0x0F,
    eBTCmdSetAlarmClock = 0x10,
    eBTCmdGetRobotSofeVer = 0x11,
    eBTCmdDeleteFile = 0x12,  // 删除文件动作
    eBTCmdModifyFile = 0x13,

    eBTCmdDownloadBinPrepare = 0x14,
    eBTCmdDownloadBinIng = 0x15,
    eBTCmdDownloadBinEnd = 0x16,

    eBTCmdTransFileCancel = 0x17,
    eBTCmdGetBatteryStatus = 0x18,
    eBTCmdLowPowWerAlert = 0x19,

    eBTCmdGetBootHardWareVersion = 0x20,
    eBTCmdPlayActIsNeedReset = 0x21,
    eBTCmdControlOneServo = 0x22,
    eBTCmdControlAllServo = 0x23,
    eBTCmdGetOneServoAnglePoweroff = 0x24,
    eBTCmdGetAllServoAnglePoweroff = 0x25,
    eBTCmdSetOneServoOffset = 0x26,
    eBTCmdSetAllServoOffset = 0x27,
    eBTCmdGetOneServoOffset = 0x28,
    eBTCmdGetAllServoOffset = 0x29,
    eBTCmdGetOneServoVersion = 0x2A,
    eBTCmdGetAllServoVersion = 0x2B,

    eBTCmdPlayActFinish = 0x31,
    eBTCmdIsChargingPlaying = 0x32,
    eBTCmdReadWriteSN = 0x33,
    eBTCmdGetUDID = 0x34,

    eBTCmdSetAudioSource = 0x35,
    eBTCmdGetAudioSource = 0xB5,

    eBTCmdGetWifisList = 0x36,
    eBTCmdGetWifisListPrepare = eBTCmdGetWifisList,
    eBTCmdGetWifisListIng = 0x37,
    eBTCmdGetWifisListEnd = 0x38,

    eBTCmdConnectWifi = 0x39,    


    eBTCmdWifiConnectState = 0x40,
    eBTCmdReadAutoUpGradeState = 0x41,  //读取自动升级状态
    eBTCmdSetAutoUpGradeState = 0x42,   //切换自动升级状态
    eBTCmdManalUpGrade = 0x43,          //手动升级
    eBTCmdGetAutoUpdadeProgress = 0x44, //获取自动升级进度
    eBTCmdIsInstallingUpdate = 0x45,     //是否安装升级通知
    eBTCmdHttpDownload = 0x46,          //下载动作文件
    eBTCmdInquirieActionFile = 0x47,    //查询已下载的动作文件


    eBTCmdGetObstacleDistance = 0x50,   //获取障碍物距离
    eBtCmdGetGSensorPosture = 0x52,     //陀螺仪姿态
    eBTCmdGetFallPostition = 0x53,      //获取跌倒状态

    eBTCmdPlayAudio = 0x60,             //播放音效
    eBTCmdSetEyeLed = 0x61,             //设置眼灯状态
    eBTCmdPlayFace = 0x63,              //播放表情
    eBTCmdStopPlayAudio = 0x64,         //停止音效或表情
    eBTCmdStopEyeLed = 0x65,            //停止眼灯状态
    eBTCmdAnsStartPlayAct =0x66,        //上报动作开始播放消息
    eBTCmdGetOneServoSensorType = 0x67, //sensor type such as 电位器
    eBTCmdGetAllServoSensorType = 0x68,
    eBTCmdStopPlayFace = 0x69,           //停止音效或表情


     eBTCmdDisconnect = 0x6C,


    eBTCmdGetUTF8AndServoNum = 0xB7,
};


////////////////////if the cmd is not the same to ans,
/// using the cmd because of write by cmd
///
enum emAnsCommontFlage
{
    eAnsOk = 0x00,
    eAnsErr = 0x01
};

enum emFTStat
{
    eFTNoExist = 0x00,
    eFTExist = 0x01
};

enum emCmdAudioStat
{
    ////!!!!cmd is not the same to ans
    eCmdAudioMute = 0x00,
    eCmdAudioOpen = 0x01
};

enum emAnsAudioStat
{
    ////!!!!cmd is not the same to ans
    eAnsAudioOpen = 0x00,
    eAnsAudioMute = 0x01,
};

enum emPlayActStat
{
    eAnsPlayOk = 0x00,
    eAnsErrEmptyFileName = 0x01,
    eAnsErrLowPower = 0x02,
    eAnsErrFileDetectFail = 0x03,


    //this is not defined
    eAnsErrOtherUnknown
};

enum emPlayActControlStat
{
    ePlayPause = 0x00,
    ePlayNoPause = 0x01
};

enum emServosLedStat
{
    eLedExtinguish = 0x00,
    eLedBright = 0x01
};

enum emAlarmClockStat
{
    eAlarmClockOff = 0x00,
    eAlarmClockOn = 0x01,

    eAlarmClockNoEveryDay =0x00,
    eAlarmClockEveryDay =0x01,
};

enum emDeleteFileStat
{
    eDeleteOk = 0x00,
    eDeleteFileNoExist = 0x01,
    eDeleteFail =0x02,
    eDeleteTimeout =0x03,
    eDeleteFileNameEmpty =0x04,
};

enum emDownloadBinStat
{
    eDLoadOk = 0x00,
    eDLoadDelWrongPath = 0x01,
    eDLoadDelOtherErr =0x02,
    eDLoadBusy =0x03,
    eDLoadFileNameToShortToBuild =0x04,
    eDLoadLogicErr =0x05,
    eDLoadOperateErr =0x07,
    eDLoadFileHadExist =0x08,
    eDLoadNoSpaceToSave =0x0A,
};

enum emBatteryStat
{
    eBatterNoCharging = 0x00,
    eBatterCharging = 0x01,
    eBatterNoExist =0x02,

    eBatterErrOther//nodefine
};

enum emNeedResetStat
{
    eNeedResetYes = 0x00,
    eNeedResetNo = 0x01,
};

enum emGetServoAngleStat
{
    eGetServoAngleOk = 0x00,
    eGetServoAngleErr = 0x01,
    eGetServoAngleIDErr = 0xFE,
    eGetServoAngleNoAns = 0xFF,
};

enum emSetServoOffsetStat
{
    eSetServoAngleOk = 0x00,
    eSetServoAngleErr = 0x01,
    eSetServoAngleNoAns = 0x02,
    eSetServoAngleIDErr = 0xFE,
};

enum emServoControlStat
{
    eServoControlOk = 0x00,
    eServoControlIDErr = 0x01,
    eServoControlAngleExceed = 0x02,
    eServoControlNoAns = 0x03,

    eServoControlOtherErr //nodefine
};

enum emServoLedsStat
{
    eServoLedGlitter =  0x00,
    eServoLedPoweroff = 0x01,

    eServoLedBrightKeeping,//nodefine

    eServoLedStatUnknown,//nodefine
};

enum emGetServoOffsetStat
{
    eSetServoOffsetNoAns = 0X8888,//0X88xx舵机没应答
};

enum emGetServoVersionStat
{
    eGetServoVerNoAns = 0X88888888,//0x88xxxxxx为舵机没应答
};

enum emChargingPlayingStat
{
    eChargingPlayingEnable = 0X00,
    eChargingPlayingUnenable = 0X01,
};

enum emReadWriteSNStat
{
    eSNCmdRead = 0X00,
    eSNCmdWrite = 0X01,

    eSNWriteOk = 0X00,
    eSNWriteErr = 0X01,
};


enum emAudioSourceStat
{
    eAudioSourceTF = 0X00,
    eAudioSourceAppBT = 0X01,


    eAudioSourceErr, //nodefine
};

enum emGetSomeTypeResInfoResType
{
    eResInfoTypeInvalid = 0,
    eResInfoTypeActList = 1,
};

enum emGetSomeTypeResInfoCmdFlag
{
    eResInfoCmdFlagRestartGetInfo = 0,
    eResInfoCmdFlagContinueGet = 1,
    eResInfoCmdFlagAbort = 2,
};

enum emWifiInfosStat
{
    eWifiInfosWifiListRestartGet = 0x00,
    eWifiInfosWifiListContinueGet = 0x01,
    eWifiInfosWifiListAbortGet = 0x02,

};

enum emAnsWifiSettingStat
{
    eAnsDisconnected = 0x00,
    eAnsConnecting,
    eAnsConectSuccessed,
    eAnsConnectFailed
};

enum emGetAutoUpdadeProgressStatus
{
     eUpdadeStatusFail = 0x00,
    eUpdadeStatusDowning = 0x01,
    eUpdadeStatusOk = 0x02,
};

enum emIsInstallingUpdateStatus
{
     eIsInstallingUpdateStatusPowerLessNoInstall= 0x00,
    eIsInstallingUpdateStatusPowerFullToInstall = 0x01,
};



//bt cmd params struct
typedef struct __AdjustClockData
{
   uchar y;
   uchar m;
   uchar d;
   uchar h;
   uchar minute;
   uchar s;
}ADJUST_CLOCK_DATA;

typedef struct __SetAlarmClockData
{
    bool bOpen;
    bool bAlarmEveryDay;
    uchar h;
    uchar m;
    uchar s;
    string strActName;
}SET_ALARM_CLOCK_DATA;

typedef struct __ModifyFileData
{
    string strNameOld;
    string strNameNew;
}MODIFY_FILE_DATA;

typedef struct __ControlOneServoData
{
    MOTOR_DATA motoData;
    int nRunTime;
    int nRunTimeAll;
}CONTROL_ONE_SERVO_DATA;

typedef struct __ControlAllServoData
{
    int nRunTime;
    int nRunTimeAll;
    int nLenMotoData;
    MOTOR_DATA motoData[ROBOT_TOTAL_MOTOR_NUMBER];
}CONTROL_ALL_SERVO_DATA;

typedef struct __SetOneServoOffset
{
    MOTOR_DATA motoData;
}SET_ONE_SERVO_OFF_SET_DATA;

typedef struct __SetAllServoOffset
{
    MOTOR_DATA motoData[ROBOT_TOTAL_MOTOR_NUMBER];
}SET_ALL_SERVO_OFF_SET_DATA;

class CBluetoothComandParser
{
public:
    CBluetoothComandParser();
    ~CBluetoothComandParser();

public:
    /**************************************************************************
    * 函数名: isBTProtocol
    * 功能:
    * 参数:
    *    @[in ] pBufBTCmdAllData: is the all data of cmd , or the all data of ans
    *    @[in ] nLenBuf:
    * 返回值:
    * 时间: 20170117
    * 作者: asu
    */
    static bool isBTProtocol(const byte* pBufBTCmdAllData, const int nLenBuf);
    static byte* getParamsPoidHead(const byte* pBufBTCmdAllData, const int nLenBuf);
    static int getParamsLen(const byte* pBufBTCmdAllData, const int nLenBuf);


public:
    /**************************************************************************
    * 函数名: motorDataArrToVect
    * 功能: MOTOR_DATA Arr To vecMotorData
    * 参数:
    *    @[in ] MOTOR_DATA: 数据
    *    @[in ] nMotorNum:
    *    @[in ] vecData:
    * 返回值:
    * 时间: 20170117
    * 作者: asu
    */
    static void motorDataArrToVect(const MOTOR_DATA motoData[ROBOT_TOTAL_MOTOR_NUMBER], vecMotorData& vecData, int nMotorNum = ROBOT_TOTAL_MOTOR_NUMBER);


    /**************************************************************************
    * 函数名: vecMotorDataToArr
    * 功能: vecMotorData To MOTOR_DATA arr
    * 参数:
    *    @[in ] MOTOR_DATA: 数据
    *    @[in ] nMotorNum:
    *    @[in ] vecData:
    * 返回值:
    * 时间: 20170117
    * 作者: asu
    */
    static void vecMotorDataToArr(const vecMotorData& vecData, MOTOR_DATA motoData[ROBOT_TOTAL_MOTOR_NUMBER], int nMotorNum = ROBOT_TOTAL_MOTOR_NUMBER);


public:
    /**************************************************************************
     * 函数名: GetCmd_handshake
     * 功能:
     * 参数:
     *    @[out]
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_handshake();
    static vecByteLst getCmd_disconnect();

    /**************************************************************************
     * 函数名: getCmd_getActList
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_getActList(char *pActionDir, int nActionDirLen);
    static vecByteLst getCmd_getSomeTypeResInfo(emGetSomeTypeResInfoResType eResInfoType, emGetSomeTypeResInfoCmdFlag eCmdFlag,
                                                const char* const pParam = NULL, int nParamLen = 0);

    /**************************************************************************
     * 函数名: getCmd_playAct
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_playAct(const byte* pName, int nLenName);

    /**************************************************************************
     * 函数名: getCmd_getRemoteActList
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_getRemoteActList();

    /**************************************************************************
     * 函数名: getCmd_stopPlayAct
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_stopPlayAct();

    /**************************************************************************
     * 函数名: getCmd_audioControl
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_audioControl(bool bMute);

    /**************************************************************************
     * 函数名: getCmd_playActControl
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_playActControl(bool bPause);

    /**************************************************************************
     * 函数名: getCmd_heartbeat
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_heartbeat();

    /**************************************************************************
     * 函数名: getCmd_BTNameSet
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_BTNameSet(const byte* pName, int nLenName);

    /**************************************************************************
     * 函数名: getCmd_getDriveStatus
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_getDriveStatus();

    /**************************************************************************
     * 函数名: getCmd_setAudioValue
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_setAudioValue(int nValue);

    /**************************************************************************
     * 函数名: getCmd_servosPowerOff
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_servosPowerOff();

    /**************************************************************************
     * 函数名: getCmd_servosLedControl
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_servosLedControl(bool bOpen);

    /**************************************************************************
     * 函数名: getCmd_adjustClock
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_adjustClock(uchar y, uchar m, uchar d, uchar h, uchar minute, uchar s);

    /**************************************************************************
     * 函数名: getCmd_setAlarmClock
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_setAlarmClock(bool bOpen, bool bAlarmEveryDay,
                                           uchar h, uchar m, uchar s,
                                           const byte* pActName, int nLenName);

    /**************************************************************************
     * 函数名: getCmd_getAlarmClockValue
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161208
     * 作者: asu
     */
    static vecByteLst getCmd_getAlarmClockValue();

    /**************************************************************************
     * 函数名: getCmd_getRobotSofeVer
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getRobotSofeVer();

    /**************************************************************************
     * 函数名: getCmd_deleteFile
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_deleteFile(const byte* pName, int nLenName);

    /**************************************************************************
     * 函数名: getCmd_modifyFile
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_modifyFile(const byte* pNameOld, int nLenNameOld,
                                        const byte* pNameNew, int nLenNameNew);

    /**************************************************************************
     * 函数名: getCmd_transFileCancel
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_transFileCancel();

    /**************************************************************************
     * 函数名: getCmd_getBatteryStatus
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getBatteryStatus();

    /**************************************************************************
     * 函数名: getCmd_getBootHardWareVersion
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getBootHardWareVersion();

    /**************************************************************************
     * 函数名: getCmd_playActIsNeedReset
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_playActIsNeedReset(bool bNeedReset);

    /**************************************************************************
     * 函数名: getCmd_controlOneServo
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_controlOneServo(byte ID, byte angle, byte runTime, int runTimeAll);

    /**************************************************************************
     * 函数名: getCmd_controlAllServo
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_controlAllServo(const vecMotorData& vecData, int nRunTime, int nRunTimeAll);

    /**************************************************************************
     * 函数名: getCmd_getOneServoAnglePoweroff
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getOneServoAnglePoweroff(byte servoID);

    /**************************************************************************
     * 函数名: getCmd_getAllServoAnglePoweroff
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getAllServoAnglePoweroff();

    /**************************************************************************
     * 函数名: getCmd_setOneServoOffset
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_setOneServoOffset(byte servoID, int nOffset);

    /**************************************************************************
     * 函数名: getCmd_setAllServoOffset
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_setAllServoOffset(const vecMotorData& vecData);

    /**************************************************************************
     * 函数名: getCmd_getOneServoOffset
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getOneServoOffset(byte servoID);

    /**************************************************************************
     * 函数名: getCmd_getAllServoOffset
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getAllServoOffset();

    /**************************************************************************
     * 函数名: getCmd_getOneServoVersion
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getOneServoVersion(byte servoID);

    /**************************************************************************
     * 函数名: getCmd_getAllServoVersion
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getAllServoVersion();

    /**************************************************************************
     * 函数名: getCmd_IsChargingPlaying
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_IsChargingPlaying(bool bIsChargingPlaying);

    /**************************************************************************
     * 函数名: getCmd_readSN
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_readSN();

    /**************************************************************************
     * 函数名: getCmd_writeSN
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_writeSN(const byte* pName, int nLenName);

    /**************************************************************************
     * 函数名: getCmd_getUDID
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getUDID();

    static vecByteLst getCmd_getUTF8AndServoNum();


    /**************************************************************************
     * 函数名: getCmd_setAudioSource
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_setAudioSource(emAudioSourceStat eAudioSet);

    /**************************************************************************
     * 函数名: getCmd_getAudioSource
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_getAudioSource();

    /**************************************************************************
     * 函数名: getCmd_GetWifisList
     * 功能:
     * 参数:
     *    @[int]
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_GetWifisList(emWifiInfosStat eWifiInfo);

    /**************************************************************************
     * 函数名: getCmd_ConnectWifis
     * 功能:
     * 参数:
     *    @[int]pData: json format
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_ConnectWifis(const byte* pData, int nLenData);

    //manal update robot
    static vecByteLst getCmd_ManalUpGrade();



///////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /**************************************************************************
    * 函数名: getAns_commonCmd
    * 功能: 获取应答数据
    * 参数:
    *    @[int] cmdOfBluetooth: cmd of the bluetooth
    *    @[int] pParams
    *    @[int] nLenParams
    * 返回值: 应答数据
    * 时间: 20170227
    * 作者:asu
    */
    static vecByteLst getAns_commonCmd(byte cmdOfBluetooth, const byte* pParams, int nLenParams);


    /**************************************************************************
    * 函数名: getAns_getBTName
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161208
    * 作者:asu
    */
    static vecByteLst getAns_getBTName(const byte* pBTName, int nLenName);

    /**************************************************************************
    * 函数名: getAns_getDriveStatAudioStat
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161208
    * 作者:asu
    */
    static vecByteLst getAns_getDriveStatAudioStat(bool bMute);

    /**************************************************************************
    * 函数名: getAns_getDriveStatPlayActStat
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161208
    * 作者:asu
    */
    static vecByteLst getAns_getDriveStatPlayActStat(bool bPause);

    /**************************************************************************
    * 函数名: getAns_getDriveStatAudioValue
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161208
    * 作者:asu
    */
    static vecByteLst getAns_getDriveStatAudioValue(int nValue);

    /**************************************************************************
    * 函数名: getAns_getDriveStatServoLedStat
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161208
    * 作者:asu
    */
    static vecByteLst getAns_getDriveStatServoLedStat(bool bBright);

    /**************************************************************************
    * 函数名: getAns_getDriveStatTFStat
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161208
    * 作者:asu
    */
    static vecByteLst getAns_getDriveStatTFStat(bool bExistTF);

    /**************************************************************************
    * 函数名: getAns_getActListPrepare
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161208
    * 作者:asu
    */
    static vecByteLst getAns_getActListPrepare();

    /**************************************************************************
    * 函数名: getAns_getActListSending
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161208
    * 作者:asu
    */
    static vecByteLst getAns_getActListSending(const byte *pName, int nLenName);

    /**************************************************************************
    * 函数名: getAns_getActListSendEnd
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161208
    * 作者:asu
    */
    static vecByteLst getAns_getActListSendEnd(bool bOk);

    /**************************************************************************
    * 函数名: getAns_playAct
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161208
    * 作者:asu
    */
    static vecByteLst getAns_playAct(emPlayActStat eStat);

    /**************************************************************************
    * 函数名: getAns_stopPlayAct
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_stopPlayAct();

    /**************************************************************************
    * 函数名: getAns_audioControl
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_audioControl();

    /**************************************************************************
    * 函数名: getAns_playActControl
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_playActControl();

    /**************************************************************************
    * 函数名: getAns_heartbeat
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_heartbeat();

    /**************************************************************************
    * 函数名: getAns_BTNameSet
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_BTNameSet();

    /**************************************************************************
    * 函数名: getAns_setAudioValue
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_setAudioValue();

    /**************************************************************************
    * 函数名: getAns_servosPowerOff
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_servosPowerOff();

    /**************************************************************************
    * 函数名: getAns_servosLedControl
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_servosLedControl();

    /**************************************************************************
    * 函数名: getAns_adjustClock
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_adjustClock(bool bOk);

    /**************************************************************************
    * 函数名: getAns_getAlarmClockValue
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getAlarmClockValue(bool bOpen, bool bAlarmEveryDay,
                                                uchar h, uchar m, uchar s,
                                                const byte *pActName, int nLenName);

    /**************************************************************************
    * 函数名: getAns_setAlarmClock
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_setAlarmClock();

    /**************************************************************************
    * 函数名: getAns_getRobotSofeVer
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getRobotSofeVer(const byte *pVersion, int nLen);

    /**************************************************************************
    * 函数名: getAns_deleteFile
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_deleteFile(emDeleteFileStat eDeleteStat);

    /**************************************************************************
    * 函数名: getAns_modifyFile
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_modifyFile(bool bOk);

    /**************************************************************************
    * 函数名: getAns_downloadBinEnd
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_downloadBinEnd(emDownloadBinStat eStat);

    /**************************************************************************
    * 函数名: getAns_getBatteryStatus
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getBatteryStatus(emBatteryStat eStat, int mV, int nPercent);

    /**************************************************************************
    * 函数名: getAns_lowPowWerAlert
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_lowPowWerAlert();

    /**************************************************************************
    * 函数名: getAns_getBootHardWareVersion
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getBootHardWareVersion(const uchar* pParams, int nLenParams);

    /**************************************************************************
    * 函数名: getAns_controlOneServo
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_controlOneServo(int ID, emServoControlStat eStat);

    /**************************************************************************
    * 函数名: getAns_controlAllServo
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_controlAllServo(const vecMotorData& vecData);

    /**************************************************************************
    * 函数名: getAns_getOneServoAnglePoweroff
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getOneServoAnglePoweroff(int ID, int nAngleOrGetServoAngleStat);

    /**************************************************************************
    * 函数名: getAns_getAllServoAnglePoweroff
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getAllServoAnglePoweroff(const vecMotorData& vecData);

    /**************************************************************************
    * 函数名: getAns_setOneServoOffset
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_setOneServoOffset(int ID, emSetServoOffsetStat eStat);

    /**************************************************************************
    * 函数名: getAns_setAllServoOffset
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_setAllServoOffset(const vecMotorData& vecData);

    /**************************************************************************
    * 函数名: getAns_getOneServoOffset
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getOneServoOffset(int ID, int nOffsetOrGetServoOffsetStat);

    /**************************************************************************
    * 函数名: getAns_getAllServoOffset
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getAllServoOffset(const vecMotorData& vecData);

    /**************************************************************************
    * 函数名: getAns_getOneServoVersion
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getOneServoVersion(int ID, int nVer);

    /**************************************************************************
    * 函数名: getAns_getAllServoVersion
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getAllServoVersion(const vecMotorData& vecData);

    /**************************************************************************
    * 函数名: getAns_playActFinish
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_playActFinish(const uchar* pName, int nLenName);

    /**************************************************************************
    * 函数名: getAns_IsChargingPlaying
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_IsChargingPlaying(emChargingPlayingStat eStat);

    /**************************************************************************
    * 函数名: getAns_readSN
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_readSN(const uchar* pSN, int nLenSN);

    /**************************************************************************
    * 函数名: getAns_writeSN
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_writeSN(bool bOk);

    /**************************************************************************
    * 函数名: getAns_getUDID
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getUDID(const uchar* pUDID, int nLenUDID);

    /**************************************************************************
    * 函数名: getAns_setAudioSource
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_setAudioSource(emAudioSourceStat eStat);

    /**************************************************************************
    * 函数名: getAns_getAudioSource
    * 功能: 获取应答数据
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getAudioSource(emAudioSourceStat eStat);

    /**************************************************************************
    * 函数名: getAns_getAllServoID
    * 功能: 获取应答数据, such as for ans to cmd of getUTF8AndServoNum
    * 参数:
    *    @[int]
    * 返回值: 应答数据
    * 时间: 20161209
    * 作者:asu
    */
    static vecByteLst getAns_getAllServoID();


public:
    /**************************************************************************
    * 函数名: getCmdParams_playAct
    * 功能: get value of cmd params on params
    * 参数:
    *    @[int]pParamsOfCmd: point to params postion of cmd
    *    @[int]nLenParams: length of params
    * 返回值: params is valid return true, or return false
    * 时间: 20170112
    * 作者:asu
    */
    static bool getCmdParams_playAct(const uchar* pParamsOfCmd, int nLenParams, string& strName);

    /**************************************************************************
    * 函数名: getCmdParams_audioControl
    * 功能: get value of cmd params on params
    * 参数:
    *    @[int]pParamsOfCmd: point to params postion of cmd
    *    @[int]nLenParams: length of params
    * 返回值: params is valid return true, or return false
    * 时间: 20170112
    * 作者:asu
    */
    static bool getCmdParams_audioControl(const uchar* pParamsOfCmd, int nLenParams, bool& bMute);
    static bool getCmdParams_playActControl(const uchar* pParamsOfCmd, int nLenParams, bool& bPause);
    static bool getCmdParams_BTNameSet(const uchar* pParamsOfCmd, int nLenParams, string& strName);
    static bool getCmdParams_setAudioValue(const uchar* pParamsOfCmd, int nLenParams, int& nValue);
    static bool getCmdParams_servosLedControl(const uchar* pParamsOfCmd, int nLenParams, bool& bOpen);
    static bool getCmdParams_adjustClock(const uchar* pParamsOfCmd, int nLenParams, ADJUST_CLOCK_DATA& clockData);
    static bool getCmdParams_setAlarmClock(const uchar* pParamsOfCmd, int nLenParams,
                                           SET_ALARM_CLOCK_DATA& data);

    static bool getCmdParams_deleteFile(const uchar* pParamsOfCmd, int nLenParams, string& strName);
    static bool getCmdParams_modifyFile(const uchar* pParamsOfCmd, int nLenParams,
                                        string& strNameOld, string& strNameNew);

    static bool getCmdParams_playActIsNeedReset(const uchar* pParamsOfCmd, int nLenParams, bool& bNeedReset);
    static bool getCmdParams_controlOneServo(const uchar* pParamsOfCmd, int nLenParams, CONTROL_ONE_SERVO_DATA& servoData);

    static bool getCmdParams_controlAllServo(const uchar* pParamsOfCmd, int nLenParams,
                                             vecMotorData& vecData, int& nRunTime, int& nRunTimeAll);
    static bool getCmdParams_getOneServoAnglePoweroff(const uchar* pParamsOfCmd, int nLenParams, byte& servoID);
    static bool getCmdParams_setOneServoOffset(const uchar* pParamsOfCmd, int nLenParams,
                                               SET_ONE_SERVO_OFF_SET_DATA& data);
    static bool getCmdParams_setAllServoOffset(const uchar* pParamsOfCmd, int nLenParams,
                                               SET_ALL_SERVO_OFF_SET_DATA& data);


    static bool getCmdParams_getOneServoOffset(const uchar* pParamsOfCmd, int nLenParams, byte& servoID);
    static bool getCmdParams_getOneServoVersion(const uchar* pParamsOfCmd, int nLenParams, byte& servoID);
    static bool getCmdParams_IsChargingPlaying(const uchar* pParamsOfCmd, int nLenParams, bool& bIsChargingPlaying);
    static bool getCmdParams_ISWriteSN(const uchar* pParamsOfCmd, int nLenParams, bool& bWriteSN);
    static bool getCmdParams_setAudioSource(const uchar* pParamsOfCmd, int nLenParams, emAudioSourceStat& eAudioSet);

public:
    //获取头部字节长度
    static int getHeadSize();
    static int getLenFlagSize();
    static int getCmdIDFlagSize();
    //获取校验码字节字节长度
    static int getChksumSize();
    //获取尾部字节长度
    static int getTailSize();

private:
    //长度:命令头+长度+命令+参数+checksum的长度.
    static int getCmdLenByParamsLen(int nLenCmdAndParams);
    static int getCmdTotalLenByParamsLen(int nLenCmdAndParams);


    /**************************************************************************
     * 函数名: packageBTCmd
     * 功能: must "DEFAULT_MAX_LEN_CMD_ARRAY > (nLenParams + 1)"
     * 参数:
     *    @[int] pBufCmd: cmd and the params datas(命令(1B) +<参数1[参数2][参数3](nB) >)
     *    @[out]
     * 返回值: 命令数据vecByteLst
     * 时间: 20161207
     * 作者: asu
     */
    static vecByteLst packageBTCmd(const uchar* pBufCmdToPackaged, int nLenCmd);
    static vecByteLst packageBTCmd(uchar cmd, const uchar* pParams, int nLenParams);
    static vecByteLst packageBTCmd(uchar cmd, uchar subCmd, const uchar* pParams, int nLenParams);
    static vecByteLst packageBTCmd(uchar cmd, uchar subCmd,  uchar subCmd2, const uchar* pParams, int nLenParams);

    /**************************************************************************
     * 函数名: packageBTCmd
     * 功能:
     * 参数:
     *    @[int] param1
     * 返回值: 命令数据vecByteLst
     * 时间: 20161207
     * 作者: asu
     */
    static vecByteLst packageBTCmd(uchar pos1Param);

    /**************************************************************************
     * 函数名: packageBTCmd
     * 功能:
     * 参数:
     *    @[int]
     * 返回值: 命令数据vecByteLst
     * 时间: 20161207
     * 作者: asu
     */
    static vecByteLst packageBTCmd(uchar pos1Param, uchar pos2Param);

    /**************************************************************************
     * 函数名: packageBTCmd
     * 功能:
     * 参数:
     *    @[int]
     * 返回值: 命令数据vecByteLst
     * 时间: 20161207
     * 作者: asu
     */
    static vecByteLst packageBTCmd(uchar pos1Param, uchar pos2Param, uchar pos3Param);

    /**************************************************************************
     * 函数名: packageBTCmd
     * 功能:
     * 参数:
     *    @[int]
     * 返回值: 命令数据vecByteLst
     * 时间: 20161207
     * 作者: asu
     */
    static vecByteLst packageBTCmd(uchar pos1Param, uchar pos2Param, uchar pos3Param, uchar pos4Param);

    /**************************************************************************
     * 函数名: packageBTCmd
     * 功能:
     * 参数:
     *    @[int]
     * 返回值: 命令数据vecByteLst
     * 时间: 20161207
     * 作者: asu
     */
    static vecByteLst packageBTCmd(uchar pos1Param, uchar pos2Param, uchar pos3Param,
                                   uchar pos4Param, uchar pos5Param);

    /**************************************************************************
     * 函数名: packageBTCmd
     * 功能:
     * 参数:
     *    @[int]
     * 返回值: 命令数据vecByteLst
     * 时间: 20161207
     * 作者: asu
     */
    static vecByteLst packageBTCmd(uchar pos1Param, uchar pos2Param, uchar pos3Param,
                                   uchar pos4Param, uchar pos5Param, uchar pos6Param);


private:
    static int m_nHeadSize;
    static int m_nLenFlagSize;
    static int m_nCmdIDFlagSize;
    static int m_nChksumSize;
    static int m_nTailSize;
};

#endif // CBLUETOOTHCOMANDPARSER_H
