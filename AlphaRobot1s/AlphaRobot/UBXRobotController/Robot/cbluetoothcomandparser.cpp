#include "cbluetoothcomandparser.h"

#include "cdataoperate.h"


const uchar BT_CMD_HEAD = 0XFB;
const uchar BT_CMD_END = 0XED;


const int DEFAULT_MAX_LEN_CMD_ARRAY = 1024;

const int DEFAULT_MAX_LEN_SN = 16;
const int DEFAULT_MAX_LEN_UDID = 16;




//命令头(2B) +长度(1B) +命令(1B) +<参数1[参数2][参数3](nB) >+ checksum(1B)  + 结束符(1B)
int CBluetoothComandParser::m_nHeadSize = 2;
int CBluetoothComandParser::m_nLenFlagSize = 1;
int CBluetoothComandParser::m_nCmdIDFlagSize = 1;
int CBluetoothComandParser::m_nChksumSize = 1;
int CBluetoothComandParser::m_nTailSize = 1;


CBluetoothComandParser::CBluetoothComandParser()
{

}

CBluetoothComandParser::~CBluetoothComandParser()
{

}

/**************************************************************************
* 函数名: isBTProtocol
* 功能:
* 参数:
*    @[in ] pBuf: is the all data of cmd , or the all data of ans
*    @[in ] nLenBuf:
* 返回值:
* 时间: 20170117
* 作者: asu
*/
bool CBluetoothComandParser::isBTProtocol(const byte *pBuf, const int nLenBuf)
{
    const int nBufLenLess = CBluetoothComandParser::m_nHeadSize +
            CBluetoothComandParser::m_nLenFlagSize +
            CBluetoothComandParser::m_nCmdIDFlagSize +
            CBluetoothComandParser::m_nChksumSize +
            CBluetoothComandParser::m_nTailSize;

    if(!pBuf || (nLenBuf < nBufLenLess))
    {
        return false;
    }

   //check the flage
    if((ROBOT_CMD_BLUE_TOOTH_START_FLAG0 != pBuf[eBTCmdPosStartFlag0])
            || ( ROBOT_CMD_BLUE_TOOTH_START_FLAG1 != pBuf[eBTCmdPosStartFlag1])
            || ( ROBOT_CMD_BLUE_TOOTH_END_FLAG != pBuf[nLenBuf - 1]))
    {
        return false;
    }

    //check the length
    const int cmdLen = nLenBuf - CBluetoothComandParser::m_nTailSize;
    const int temCmdLen = pBuf[eBTCmdPosCmdLength];
    if(temCmdLen != cmdLen)
    {
        return false;
    }

    //check sum
    uchar checkSum = (uchar)CDataOperate::checkSum(pBuf + eBTCmdPosCmdLength,
                                                   nLenBuf - (CBluetoothComandParser::m_nHeadSize +
                                                              CBluetoothComandParser::m_nChksumSize +
                                                                CBluetoothComandParser::m_nTailSize));
    uchar tempCheckSum = pBuf[nLenBuf - 1 - CBluetoothComandParser::m_nTailSize];
    if(checkSum != tempCheckSum)
    {
        return false;
    }

    return true;
}

byte *CBluetoothComandParser::getParamsPoidHead(const byte *pBufBTCmdAllData, const int nLenBuf)
{
    byte* pParamsHead = NULL;
    if((nLenBuf > eBTCmdPosFirstParamStart) && pBufBTCmdAllData)
    {
        pParamsHead = (byte *)(pBufBTCmdAllData + eBTCmdPosFirstParamStart);
    }
    return pParamsHead;
}

int CBluetoothComandParser::getParamsLen(const byte *pBufBTCmdAllData, const int nLenBuf)
{
    int nCmdParamBufLen = 0;
    if(pBufBTCmdAllData)
    {
        nCmdParamBufLen = nLenBuf - CBluetoothComandParser::getLenFlagSize()
                - CBluetoothComandParser::getCmdIDFlagSize()
                - CBluetoothComandParser::getHeadSize()
                - CBluetoothComandParser::getChksumSize()
                - CBluetoothComandParser::getTailSize();

        if(nCmdParamBufLen < 0)
        {
            nCmdParamBufLen = 0;
        }
    }
    return nCmdParamBufLen;
}

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
void CBluetoothComandParser::motorDataArrToVect(const MOTOR_DATA motoData[], vecMotorData &vecData, int nMotorNum)
{
    vecData.clear();
    for(int i = 0; i < nMotorNum; ++i)
    {
        vecData.push_back(motoData[i]);
    }
}

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
void CBluetoothComandParser::vecMotorDataToArr(const vecMotorData &vecData, MOTOR_DATA motoData[], int nMotorNum)
{
    const int nVecDataNum = vecData.size();
    for(int i = 0; (i < nVecDataNum) && (i < nMotorNum); ++i)
    {
        motoData[i].nID = vecData.at(i).nID;
        motoData[i].nAngle = vecData.at(i).nAngle;
    }
}

/**************************************************************************
 * 函数名: GetCmd_handshake
 * 功能:
 * 参数:
 *    @[out]
 * 返回值: 命令数据
 * 时间: 20161208
 * 作者: asu
 */
vecByteLst CBluetoothComandParser::getCmd_handshake()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdHandshake, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_disconnect()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdDisconnect, 0x02, 0X00);
}

/**************************************************************************
 * 函数名: getCmd_getActList
 * 功能:
 * 参数:
 * 返回值: 命令数据
 * 时间: 20161208
 * 作者: asu
 */
vecByteLst CBluetoothComandParser::getCmd_getActList(char *pActionDir, int nActionDirLen)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetActList, (char)nActionDirLen, (const uchar*)pActionDir, nActionDirLen);
}

vecByteLst CBluetoothComandParser::getCmd_getSomeTypeResInfo(emGetSomeTypeResInfoResType eResInfoType, emGetSomeTypeResInfoCmdFlag eCmdFlag,
                                                             const char * const pParam, int nParamLen)
{
     if(pParam && (nParamLen > 0))
     {
          return CBluetoothComandParser::packageBTCmd((char)eBTCmdGetSomeTypeResInfo, (char)eResInfoType, (char)eCmdFlag, (const uchar*)pParam, nParamLen);
     }
     else
     {
         return CBluetoothComandParser::packageBTCmd((char)eBTCmdGetSomeTypeResInfo, (char)eResInfoType, (char)eCmdFlag);
     }
}

vecByteLst CBluetoothComandParser::getCmd_playAct(const byte *pName, int nLenName)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdPlayAct, pName, nLenName);
}

bool CBluetoothComandParser::getCmdParams_playAct(const uchar* pParamsOfCmd, int nLenParams, string& strName)
{    
    strName = (char*)pParamsOfCmd;
    return true;
}

vecByteLst CBluetoothComandParser::getCmd_getRemoteActList()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetRemoteActList, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_stopPlayAct()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdStopPlayAct, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_audioControl(bool bMute)
{
    uchar param = 0;
    if(!bMute)
    {
        param = eCmdAudioOpen;
    }
    else
    {
        param = eCmdAudioMute;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdAudioControl, param);
}

bool CBluetoothComandParser::getCmdParams_audioControl(const uchar* pParamsOfCmd, int nLenParams, bool& bMute)
{
    int param;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams, param, 1);
    if(!bOk)
    {
        return false;
    }

    bool bValidParams = true;
    switch (param)
    {
    case eCmdAudioOpen:
        bMute = false;
        break;

    case eCmdAudioMute:
        bMute = true;
        break;

    default:
        bValidParams = false;
        break;
    }
    return bValidParams;
}


vecByteLst CBluetoothComandParser::getCmd_playActControl(bool bPause)
{
    uchar param = 0;
    if(!bPause)
    {
        param = ePlayNoPause;
    }
    else
    {
        param = ePlayPause;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdPlayActControl, param);
}

bool CBluetoothComandParser::getCmdParams_playActControl(const uchar* pParamsOfCmd, int nLenParams, bool& bPause)
{
    int param;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams, param, 1);
    if(!bOk)
    {
        return false;
    }

    bool bValidParams = true;
    switch (param)
    {
    case ePlayNoPause:
        bPause = false;
        break;

    case ePlayPause:
        bPause = true;
        break;

    default:
        bValidParams = false;
        break;
    }
    return bValidParams;
}

vecByteLst CBluetoothComandParser::getCmd_heartbeat()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdHeartbeat, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_BTNameSet(const byte *pName, int nLenName)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdBTNameSet, pName, nLenName);
}

bool CBluetoothComandParser::getCmdParams_BTNameSet(const uchar* pParamsOfCmd, int nLenParams, string& strName)
{    
    strName = (char*)pParamsOfCmd;
    return true;
}

vecByteLst CBluetoothComandParser::getCmd_getDriveStatus()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetDriveStatus, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_setAudioValue(int nValue)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetDriveStatus, nValue);
}

bool CBluetoothComandParser::getCmdParams_setAudioValue(const uchar* pParamsOfCmd, int nLenParams, int& nValue)
{    
    return CDataOperate::getValue(pParamsOfCmd, nLenParams, nValue, 1);
}

vecByteLst CBluetoothComandParser::getCmd_servosPowerOff()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdServosPowerOff, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_servosLedControl(bool bOpen)
{
    uchar param = 0;
    if(bOpen)
    {
        param = eLedBright;
    }
    else
    {
        param = eLedExtinguish;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdServosLedControl, param);
}

bool CBluetoothComandParser::getCmdParams_servosLedControl(const uchar* pParamsOfCmd, int nLenParams, bool& bOpen)
{
    int param;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams, param, 1);
    if(!bOk)
    {
        return false;
    }

    bool bValidParams = true;
    switch (param)
    {
    case eLedExtinguish:
        bOpen = false;
        break;

    case eLedBright:
        bOpen = true;
        break;

    default:
        bValidParams = false;
        break;
    }
    return bValidParams;
}

vecByteLst CBluetoothComandParser::getCmd_adjustClock(uchar y, uchar m, uchar d,
                                                      uchar h, uchar minute, uchar s)
{
    byte dataCmdTemp[] = {eBTCmdAdjustClock, y, m, d, h, minute, s};
    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

bool CBluetoothComandParser::getCmdParams_adjustClock(const uchar* pParamsOfCmd, int nLenParams, ADJUST_CLOCK_DATA& clockData)
{
    int y;
    int m;
    int d;
    int h;
    int minute;
    int s;

    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams,
                                  y, 1,
                                  m, 1,
                                  d, 1,
                                  h, 1,
                                  minute, 1,
                                  s, 1);
    clockData.y = y;
    clockData.m = m;
    clockData.d = d;
    clockData.h = h;
    clockData.minute = minute;
    clockData.s = s;

    return bOk;
}

vecByteLst CBluetoothComandParser::getCmd_setAlarmClock(bool bOpen, bool bAlarmEveryDay,
                                                        uchar h, uchar m, uchar s,
                                                        const byte *pActName, int nLenName)
{
    byte dataCmdTemp[DEFAULT_MAX_LEN_CMD_ARRAY] = {0x00};
    if(bOpen)
    {
        dataCmdTemp[0] = eAlarmClockOn;
    }
    else
    {
        dataCmdTemp[0] = eAlarmClockOff;
    }

    if(bAlarmEveryDay)
    {
        dataCmdTemp[1] = eAlarmClockEveryDay;
    }
    else
    {
        dataCmdTemp[1] = eAlarmClockNoEveryDay;
    }

    dataCmdTemp[2] = h;
    dataCmdTemp[3] = m;
    dataCmdTemp[4] = s;


    dataCmdTemp[5] = 0;
    if(pActName)
    {
        dataCmdTemp[5] = nLenName;
        memcpy((dataCmdTemp + 6), pActName, nLenName);
    }

    const int nLenCmd = 6 + nLenName;
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nLenCmd);
}

bool CBluetoothComandParser::getCmdParams_setAlarmClock(const uchar* pParamsOfCmd, int nLenParams,
                                                        SET_ALARM_CLOCK_DATA& data)
{
    int nBopen;
    int nBAlarmEveryDay;
    int h;
    int m;
    int s;
    int nLenName;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams,
                                      nBopen, 1,
                                      nBAlarmEveryDay, 1,
                                      h, 1,
                                      m, 1,
                                      s, 1,
                                      nLenName, 1);

    string strActName;
//    UBTString::getString((char*)pParamsOfCmd + 6, nLenName, strActName);
    strActName = (char*)pParamsOfCmd + 6;

    bool bValidParam = true;
    bool bOpen = false;
    bool bAlarmEveryDay = false;

    switch (nBopen)
    {
    case eAlarmClockOn:
        bOpen = true;
        break;

    case eAlarmClockOff:
        bOpen = false;
        break;

    default:
        bValidParam = false;
        break;
    }

    switch (nBAlarmEveryDay)
    {
    case eAlarmClockEveryDay:
        bAlarmEveryDay = true;
        break;

    case eAlarmClockNoEveryDay:
        bAlarmEveryDay = false;
        break;

    default:
        bValidParam = false;
        break;
    }

    if(bOk)
    {
        data.bOpen = bOpen;
        data.bAlarmEveryDay = bAlarmEveryDay;
        data.h = h;
        data.m = m;
        data.s = s;
        data.strActName = strActName;
    }

    return (bValidParam && bOk);
}


vecByteLst CBluetoothComandParser::getCmd_getAlarmClockValue()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetAlarmClockValue, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_getRobotSofeVer()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetRobotSofeVer, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_deleteFile(const byte *pName, int nLenName)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdDeleteFile, pName, nLenName);
}

bool CBluetoothComandParser::getCmdParams_deleteFile(const uchar* pParamsOfCmd, int nLenParams, string& strName)
{
    if(!pParamsOfCmd)
    {
        return false;
    }
    strName = (char*)pParamsOfCmd;
    return true;
}

vecByteLst CBluetoothComandParser::getCmd_modifyFile(const byte *pNameOld, int nLenNameOld,
                                                     const byte *pNameNew, int nLenNameNew)
{
    if(!pNameNew
            || !pNameOld
            || (nLenNameNew <= 0)
            || (nLenNameOld <= 0))
    {
        return vecByteLst();
    }

    byte data[DEFAULT_MAX_LEN_CMD_ARRAY] = {0x00};
    data[0] = eBTCmdModifyFile;
    data[1] = nLenNameOld;
    data[2] = nLenNameNew;

    const int nLenAll = nLenNameOld + nLenNameNew;
    memcpy((data + 3), pNameOld, nLenNameOld);
    memcpy(data + 3 + nLenNameOld, pNameNew, nLenNameNew);

    return CBluetoothComandParser::packageBTCmd(eBTCmdModifyFile, data, nLenAll);
}

bool CBluetoothComandParser::getCmdParams_modifyFile(const uchar* pParamsOfCmd, int nLenParams,
                                                     string& strNameOld, string& strNameNew)
{
    int nLenNameOld;
    int nLenNameNew;
    CDataOperate::getValue(pParamsOfCmd, nLenParams, nLenNameOld, 1,
                           nLenNameNew, 1);

//    UBTString::getString((const char*)(pParamsOfCmd + 2), nLenNameOld, strNameOld);
//    UBTString::getString((const char*)(pParamsOfCmd + 2 + nLenNameOld), nLenNameNew, strNameNew);

    strNameOld = (char*)(pParamsOfCmd + 2);
    strNameNew = (char*)(pParamsOfCmd + 2 + nLenNameOld);
    return true;
}

vecByteLst CBluetoothComandParser::getCmd_transFileCancel()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdTransFileCancel);
}

vecByteLst CBluetoothComandParser::getCmd_getBatteryStatus()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetBatteryStatus, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_getBootHardWareVersion()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetBootHardWareVersion, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_playActIsNeedReset(bool bNeedReset)
{
    byte data = eNeedResetNo ;
    if(bNeedReset)
    {
        data = eNeedResetYes;
    }

    return CBluetoothComandParser::packageBTCmd(eBTCmdPlayActIsNeedReset, data);
}

bool CBluetoothComandParser::getCmdParams_playActIsNeedReset(const uchar* pParamsOfCmd, int nLenParams, bool& bNeedReset)
{
    int param;
    bool bValidParams = CDataOperate::getValue(pParamsOfCmd, 1, param, 1);
    switch (param)
    {
    case eNeedResetNo:
        bNeedReset = false;
        break;

    case eNeedResetYes:
        bNeedReset = true;
        break;

    default:
        bValidParams = false;
        break;
    }
    return bValidParams;
}

vecByteLst CBluetoothComandParser::getCmd_controlOneServo(byte ID, byte angle, byte runTime, int runTimeAll)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdControlOneServo, ID, angle, runTime,
                                                CDataOperate::getValue(runTimeAll, 0),
                                                CDataOperate::getValue(runTimeAll, 1));
}

bool CBluetoothComandParser::getCmdParams_controlOneServo(const uchar* pParamsOfCmd, int nLenParams,
                                                          CONTROL_ONE_SERVO_DATA& servoData)
{
    int ID;
    int nAngle;
    int nRunTime;
    int nRunTimeAll;

    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams,
                                      ID, 1,
                                      nAngle, 1,
                                      nRunTime, 1,
                                      nRunTimeAll, 2);
    if(bOk)
    {
        servoData.motoData.nID = ID;
        servoData.motoData.nAngle = nAngle;
        servoData.nRunTime = nRunTime;
        servoData.nRunTimeAll = nRunTimeAll;
    }

    return bOk;
}

vecByteLst CBluetoothComandParser::getCmd_controlAllServo(const vecMotorData& vecData, int nRunTime, int nRunTimeAll)
{
    byte dataCmdTemp[20] = {0x00};
    dataCmdTemp[0] = eBTCmdControlAllServo;

    int nSizeVecData = vecData.size();
    if(nSizeVecData > ROBOT_TOTAL_MOTOR_NUMBER)
    {
        nSizeVecData = ROBOT_TOTAL_MOTOR_NUMBER;
    }

    for(int i = 0; i < nSizeVecData; ++i)
    {
        for(int j = 0; j < nSizeVecData; ++j)
        {
            if( (i + ROBOT_ID_DEFAULT_START_NUMBER) == vecData.at(j).nID)
            {
                dataCmdTemp[1 + i] = (byte)vecData.at(j).nAngle;
            }
        }
    }

    const int nMod = 20;
    int nTempRunTime = nRunTime/nMod;
    dataCmdTemp[17] = CDataOperate::getValue(nTempRunTime, 0);
    const int nRunTimeAllTemp = nRunTimeAll/nMod;
    dataCmdTemp[18] = (byte)CDataOperate::getValue(nRunTimeAllTemp, 0);
    dataCmdTemp[19] = (byte)CDataOperate::getValue(nRunTimeAllTemp, 1);

    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

bool CBluetoothComandParser::getCmdParams_controlAllServo(const uchar* pParamsOfCmd, int nLenParams,
                                                          vecMotorData& vecData, int& nRunTime, int& nRunTimeAll)
{
    if(!pParamsOfCmd)
    {
        return false;
    }

    vecData.clear();
    for(int i = 0; i < ROBOT_TOTAL_MOTOR_NUMBER; i++)
    {
        int nAngle = CDataOperate::getValueInt(pParamsOfCmd + i, 1, false);
        vecData.push_back(MOTOR_DATA(ROBOT_ID_DEFAULT_START_NUMBER + i, nAngle));
    }

    CDataOperate::getValue(pParamsOfCmd + ROBOT_TOTAL_MOTOR_NUMBER, 3,
                           nRunTime, 1,
                           nRunTimeAll, 2);
    return true;
}

vecByteLst CBluetoothComandParser::getCmd_getOneServoAnglePoweroff(byte servoID)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetOneServoAnglePoweroff, servoID);
}

bool CBluetoothComandParser::getCmdParams_getOneServoAnglePoweroff(const uchar* pParamsOfCmd, int nLenParams,
                                                                   byte& servoID)
{
    int nServoID;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams, nServoID, 1);
    servoID = nServoID;
    return bOk;
}

vecByteLst CBluetoothComandParser::getCmd_getAllServoAnglePoweroff()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetAllServoAnglePoweroff, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_setOneServoOffset(byte servoID, int nOffset)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdSetOneServoOffset,
                                                servoID,
                                                CDataOperate::getValue(nOffset, 1),//!!!big endian
                                                CDataOperate::getValue(nOffset, 0));//!!!big endian
}

bool CBluetoothComandParser::getCmdParams_setOneServoOffset(const uchar* pParamsOfCmd, int nLenParams,
                                                                   SET_ONE_SERVO_OFF_SET_DATA& data)
{
    int nMotoData = 0;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams,
                                      data.motoData.nID, 1,
                                      nMotoData, 2, true);////big endian
    data.motoData.nAngle = (signed short)nMotoData;//!!!offset maybe is negative
    return bOk;
}

vecByteLst CBluetoothComandParser::getCmd_setAllServoOffset(const vecMotorData& vecData)
{
    int nVecDataSize = vecData.size();
    if(nVecDataSize != ROBOT_TOTAL_MOTOR_NUMBER)
    {
        return vecByteLst();
    }

    uchar* params = new uchar[nVecDataSize];
    memset(params, 0, nVecDataSize);

    for(int i = 0; i < nVecDataSize; ++i)
    {
        for(int j = 0; j < nVecDataSize; ++j)
        {
            if(vecData.at(j).nID == (i + ROBOT_ID_DEFAULT_START_NUMBER))
            {
                params[i*2] = CDataOperate::getValue(vecData.at(j).nAngle, 1);//!!!big endian
                params[i*2 + 1] = CDataOperate::getValue(vecData.at(j).nAngle, 0);
            }
        }
    }


    vecByteLst vecRet = CBluetoothComandParser::packageBTCmd(eBTCmdSetAllServoOffset,
                                                params,
                                                nVecDataSize);
    SAFE_DELETE_ARRAY(params);
    return vecRet;
}

bool CBluetoothComandParser::getCmdParams_setAllServoOffset(const uchar *pParamsOfCmd, int nLenParams, SET_ALL_SERVO_OFF_SET_DATA &data)
{
    if(!pParamsOfCmd)
    {
        return false;
    }

    for(int i = 0; i < ROBOT_TOTAL_MOTOR_NUMBER; i++)
    {
        int nAngle = CDataOperate::getValueInt(pParamsOfCmd + i*2, 2, true); //!!!big endian
        data.motoData[i].nID = i + ROBOT_ID_DEFAULT_START_NUMBER;
//        data.motoData[i].nAngle = nAngle;
        data.motoData[i].nAngle = (signed short)nAngle;//!!!offset maybe is negative
    }
    return true;
}

vecByteLst CBluetoothComandParser::getCmd_getOneServoOffset(byte servoID)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetOneServoOffset, servoID);
}

bool CBluetoothComandParser::getCmdParams_getOneServoOffset(const uchar* pParamsOfCmd, int nLenParams, byte& servoID)
{
    int nServoID;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams, nServoID, 1);
    servoID = nServoID;
    return bOk;
}

vecByteLst CBluetoothComandParser::getCmd_getAllServoOffset()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetAllServoOffset, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_getOneServoVersion(byte servoID)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetOneServoVersion, servoID);
}

bool CBluetoothComandParser::getCmdParams_getOneServoVersion(const uchar* pParamsOfCmd, int nLenParams, byte& servoID)
{
    int nServoID;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams, nServoID, 1);
    servoID = nServoID;
    return bOk;
}

vecByteLst CBluetoothComandParser::getCmd_getAllServoVersion()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetAllServoVersion, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_IsChargingPlaying(bool bIsChargingPlaying)
{
    byte data = eChargingPlayingEnable;
    if(!bIsChargingPlaying)
    {
        data = eChargingPlayingUnenable;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdIsChargingPlaying, data);
}

bool CBluetoothComandParser::getCmdParams_IsChargingPlaying(const uchar* pParamsOfCmd, int nLenParams, bool& bIsChargingPlaying)
{
    int param;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams, param, 1);
    if(!bOk)
    {
        return false;
    }

    bool bValidParams = true;
    switch (param)
    {
    case eChargingPlayingUnenable:
        bIsChargingPlaying = false;
        break;

    case eChargingPlayingEnable:
        bIsChargingPlaying = true;
        break;

    default:
        bValidParams = false;
        break;
    }
    return bValidParams;
}

vecByteLst CBluetoothComandParser::getCmd_readSN()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdReadWriteSN, eSNCmdRead);
}

vecByteLst CBluetoothComandParser::getCmd_writeSN(const byte *pName, int nLenName)
{
    if(nLenName > 16)
    {
        nLenName = 16;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdReadWriteSN, eSNCmdWrite,pName, nLenName);
}

bool CBluetoothComandParser::getCmdParams_ISWriteSN(const uchar* pParamsOfCmd, int nLenParams, bool& bWriteSN)
{
    int param;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams, param, 1);
    if(!bOk)
    {
        return false;
    }

    bool bValidParams = true;
    switch (param)
    {
    case eSNCmdRead:
        bWriteSN = false;
        break;

    case eSNCmdWrite:
        bWriteSN = true;
        break;

    default:
        bValidParams = false;
        break;
    }
    return bValidParams;
}

vecByteLst CBluetoothComandParser::getCmd_getUDID()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetUDID, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_getUTF8AndServoNum()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetUTF8AndServoNum, 0x00);
}

vecByteLst CBluetoothComandParser::getCmd_setAudioSource(emAudioSourceStat eAudioSet)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdSetAudioSource, (uchar)eAudioSet);
}

bool CBluetoothComandParser::getCmdParams_setAudioSource(const uchar* pParamsOfCmd, int nLenParams, emAudioSourceStat& eAudioSet)
{
    int param;
    bool bOk = CDataOperate::getValue(pParamsOfCmd, nLenParams, param, 1);
    if(!bOk)
    {
        return false;
    }

    bool bValidParams = true;
    switch (param)
    {
    case eSNCmdRead:
    case eSNCmdWrite:
        break;

    default:
        bValidParams = false;
        break;
    }
    return bValidParams;
}

vecByteLst CBluetoothComandParser::getCmd_getAudioSource()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetAudioSource);
}

vecByteLst CBluetoothComandParser::getCmd_GetWifisList(emWifiInfosStat eWifiInfo)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetWifisList, (uchar)eWifiInfo);
}

vecByteLst CBluetoothComandParser::getCmd_ConnectWifis(const byte *pData, int nLenData)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdConnectWifi, pData, nLenData);
}

vecByteLst CBluetoothComandParser::getCmd_ManalUpGrade()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdManalUpGrade);
}


///////////////////////////////////////////////////////////////////////////////////////////

vecByteLst CBluetoothComandParser::getAns_commonCmd(byte cmdOfBluetooth,
                                                    const byte *pParams,
                                                    int nLenParams)
{
    return CBluetoothComandParser::packageBTCmd(cmdOfBluetooth, pParams, nLenParams);
}

vecByteLst CBluetoothComandParser::getAns_getBTName(const byte *pBTName, int nLenName)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdHandshake, pBTName, nLenName);
}

vecByteLst CBluetoothComandParser::getAns_getDriveStatAudioStat(bool bMute)
{
    ////!!!!cmd is not the same to ans
    byte data = 0x00;
    if(bMute)
    {
        data = eAnsAudioMute;
    }
    else
    {
        data = eAnsAudioOpen;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetDriveStatus, 0x00, data);
}

vecByteLst CBluetoothComandParser::getAns_getDriveStatPlayActStat(bool bPause)
{
    byte data = 0x00;
    if(!bPause)
    {
        data = ePlayNoPause;
    }
    else
    {
        data = ePlayPause;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetDriveStatus, 0x01, data);
}

vecByteLst CBluetoothComandParser::getAns_getDriveStatAudioValue(int nValue)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetDriveStatus, 0x02, nValue);
}

vecByteLst CBluetoothComandParser::getAns_getDriveStatServoLedStat(bool bBright)
{
    byte data = 0x00;
    if(bBright)
    {
        data = eLedBright;
    }
    else
    {
        data = eLedExtinguish;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetDriveStatus, 0x03, data);
}

vecByteLst CBluetoothComandParser::getAns_getDriveStatTFStat(bool bExistTF)
{
    byte data = 0x00;
    if(bExistTF)
    {
        data = eFTNoExist;
    }
    else
    {
        data = eFTExist;
    }

    return CBluetoothComandParser::packageBTCmd(eBTCmdGetDriveStatus, 0x04, data);
}

vecByteLst CBluetoothComandParser::getAns_getActListPrepare()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetActList, 0x00);
}

vecByteLst CBluetoothComandParser::getAns_getActListSending(const byte *pName, int nLenName)
{
    return CBluetoothComandParser::packageBTCmd(0x80, pName, nLenName);
}

vecByteLst CBluetoothComandParser::getAns_getActListSendEnd(bool bOk)
{
    byte data = 0x00;
    if(!bOk)
    {
        data = eAnsErr;
    }
    else
    {
        data = eAnsOk;
    }
    return CBluetoothComandParser::packageBTCmd(0x81, data);
}

vecByteLst CBluetoothComandParser::getAns_playAct(emPlayActStat eStat)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdPlayAct, (uchar)eStat);
}

vecByteLst CBluetoothComandParser::getAns_stopPlayAct()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdStopPlayAct, 0X01);
}

vecByteLst CBluetoothComandParser::getAns_audioControl()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdAudioControl, 0X00);
}

vecByteLst CBluetoothComandParser::getAns_playActControl()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdPlayActControl, 0X00);
}

vecByteLst CBluetoothComandParser::getAns_heartbeat()
{
    return CBluetoothComandParser::getCmd_heartbeat();
}

vecByteLst CBluetoothComandParser::getAns_BTNameSet()
{
    //todo


    return vecByteLst();//
}

vecByteLst CBluetoothComandParser::getAns_setAudioValue()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdSetAudioValue, 0X00);
}

vecByteLst CBluetoothComandParser::getAns_servosPowerOff()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdServosPowerOff, 0X00);
}

vecByteLst CBluetoothComandParser::getAns_servosLedControl()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdServosLedControl, 0X00);
}

vecByteLst CBluetoothComandParser::getAns_adjustClock(bool bOk)
{
    byte data = eAnsErr;
    if(bOk)
    {
        data = eAnsOk;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdAdjustClock, data);
}

vecByteLst CBluetoothComandParser::getAns_getAlarmClockValue(bool bOpen, bool bAlarmEveryDay, uchar h, uchar m, uchar s, const byte *pActName, int nLenName)
{
    if(h > 24)
    {
        return CBluetoothComandParser::packageBTCmd(eBTCmdGetAlarmClockValue, 0x01);
    }

    return CBluetoothComandParser::getCmd_setAlarmClock(bOpen, bAlarmEveryDay,
                                                        h, m, s,
                                                        pActName, nLenName);
}

vecByteLst CBluetoothComandParser::getAns_setAlarmClock()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdSetAlarmClock, 0x00);
}

vecByteLst CBluetoothComandParser::getAns_getRobotSofeVer(const byte *pVersion, int nLen)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetRobotSofeVer, pVersion, nLen);
}

vecByteLst CBluetoothComandParser::getAns_deleteFile(emDeleteFileStat eDeleteStat)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdDeleteFile, eDeleteStat);
}

vecByteLst CBluetoothComandParser::getAns_modifyFile(bool bOk)
{
    byte data = eAnsErr;
    if(bOk)
    {
        data = eAnsOk;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdModifyFile, data);
}

vecByteLst CBluetoothComandParser::getAns_downloadBinEnd(emDownloadBinStat eStat)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdModifyFile, eStat);
}

vecByteLst CBluetoothComandParser::getAns_getBatteryStatus(emBatteryStat eStat, int mV, int nPercent)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetBatteryStatus,
                                                CDataOperate::getValue(mV, 1),//big endian
                                                CDataOperate::getValue(mV, 0),
                                                eStat,
                                                nPercent);
}

vecByteLst CBluetoothComandParser::getAns_lowPowWerAlert()
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdLowPowWerAlert, 0x00);
}

vecByteLst CBluetoothComandParser::getAns_getBootHardWareVersion(const uchar *pParams, int nLenParams)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetBootHardWareVersion, pParams, nLenParams);
}

vecByteLst CBluetoothComandParser::getAns_controlOneServo(int ID, emServoControlStat eStat)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdControlOneServo, ID, eStat);
}

vecByteLst CBluetoothComandParser::getAns_controlAllServo(const vecMotorData& vecData)
{
    byte dataCmdTemp[ROBOT_TOTAL_MOTOR_NUMBER] = {eServoControlNoAns};

    int nSizeVecData = vecData.size();
    if(nSizeVecData > ROBOT_TOTAL_MOTOR_NUMBER)
    {
        nSizeVecData = ROBOT_TOTAL_MOTOR_NUMBER;
    }

    for(int i = 0; i < nSizeVecData; ++i)
    {
        dataCmdTemp[i] = (byte)vecData.at(i).nAngle;
    }

    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(eBTCmdControlAllServo, dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::getAns_getOneServoAnglePoweroff(int ID, int nAngleOrGetServoAngleStat)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetOneServoAnglePoweroff, ID, nAngleOrGetServoAngleStat);
}

vecByteLst CBluetoothComandParser::getAns_getAllServoAnglePoweroff(const vecMotorData& vecData)
{
    byte dataCmdTemp[ROBOT_TOTAL_MOTOR_NUMBER] = {eGetServoAngleNoAns};

    int nSizeVecData = vecData.size();
    if(nSizeVecData > ROBOT_TOTAL_MOTOR_NUMBER)
    {
        nSizeVecData = ROBOT_TOTAL_MOTOR_NUMBER;
    }

    for(int i = 0; i < nSizeVecData; ++i)
    {
        dataCmdTemp[i] = (byte)vecData.at(i).nAngle;
    }

    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetAllServoAnglePoweroff, dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::getAns_setOneServoOffset(int ID, emSetServoOffsetStat eStat)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdSetOneServoOffset, ID, eStat);
}

vecByteLst CBluetoothComandParser::getAns_setAllServoOffset(const vecMotorData& vecData)
{
    byte dataCmdTemp[ROBOT_TOTAL_MOTOR_NUMBER] = {eGetServoAngleNoAns};

    int nSizeVecData = vecData.size();
    if(nSizeVecData > ROBOT_TOTAL_MOTOR_NUMBER)
    {
        nSizeVecData = ROBOT_TOTAL_MOTOR_NUMBER;
    }

    for(int i = 0; i < nSizeVecData; ++i)
    {
        dataCmdTemp[i] = (byte)vecData.at(i).nAngle;
    }

    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(eBTCmdSetAllServoOffset, dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::getAns_getOneServoOffset(int ID, int nOffsetOrGetServoOffsetStat)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetOneServoOffset,
                                                ID,
                                                CDataOperate::getValue(nOffsetOrGetServoOffsetStat, 1),
                                                CDataOperate::getValue(nOffsetOrGetServoOffsetStat, 0));//big endian
}

vecByteLst CBluetoothComandParser::getAns_getAllServoOffset(const vecMotorData& vecData)
{
    byte dataCmdTemp[ROBOT_TOTAL_MOTOR_NUMBER*2] = {(byte)eSetServoOffsetNoAns};

    int nSizeVecData = vecData.size();
    if(nSizeVecData > ROBOT_TOTAL_MOTOR_NUMBER)
    {
        nSizeVecData = ROBOT_TOTAL_MOTOR_NUMBER;
    }

    for(int i = 0; i < nSizeVecData; ++i)
    {
        dataCmdTemp[i*2] = (byte) CDataOperate::getValue(vecData.at(i).nAngle, 1);
        dataCmdTemp[i*2 + 1] = (byte) CDataOperate::getValue(vecData.at(i).nAngle, 0);//big endian
    }

    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(eBTCmdSetAllServoOffset, dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::getAns_getOneServoVersion(int ID, int nVer)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetOneServoVersion,
                                                ID,
                                                CDataOperate::getValue(nVer, 0),
                                                CDataOperate::getValue(nVer, 1),
                                                CDataOperate::getValue(nVer, 2),
                                                CDataOperate::getValue(nVer, 3));
}

vecByteLst CBluetoothComandParser::getAns_getAllServoVersion(const vecMotorData& vecData)
{
    byte dataCmdTemp[ROBOT_TOTAL_MOTOR_NUMBER*4] = {(byte)eGetServoVerNoAns};

    int nSizeVecData = vecData.size();
    if(nSizeVecData > ROBOT_TOTAL_MOTOR_NUMBER)
    {
        nSizeVecData = ROBOT_TOTAL_MOTOR_NUMBER;
    }

    for(int i = 0; i < nSizeVecData; ++i)
    {
        dataCmdTemp[i*2] = (byte) CDataOperate::getValue(vecData.at(i).nAngle, 0);
        dataCmdTemp[i*2 + 1] = (byte) CDataOperate::getValue(vecData.at(i).nAngle, 1);
        dataCmdTemp[i*2 + 2] = (byte) CDataOperate::getValue(vecData.at(i).nAngle, 2);
        dataCmdTemp[i*2 + 3] = (byte) CDataOperate::getValue(vecData.at(i).nAngle, 3);
    }

    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetAllServoVersion, dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::getAns_playActFinish(const uchar *pName, int nLenName)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdPlayActFinish, pName, nLenName);
}

vecByteLst CBluetoothComandParser::getAns_IsChargingPlaying(emChargingPlayingStat eStat)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdIsChargingPlaying, eStat);
}

vecByteLst CBluetoothComandParser::getAns_readSN(const uchar *pSN, int nLenSN)
{
    if(nLenSN > DEFAULT_MAX_LEN_SN)
    {
        nLenSN = DEFAULT_MAX_LEN_SN;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdReadWriteSN, eSNCmdRead , pSN, nLenSN);
}

vecByteLst CBluetoothComandParser::getAns_writeSN(bool bOk)
{
    byte data = eSNWriteErr ;
    if(bOk)
    {
        data = eSNWriteOk;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdReadWriteSN, eSNCmdWrite, data);
}

vecByteLst CBluetoothComandParser::getAns_getUDID(const uchar *pUDID, int nLenUDID)
{
    if(nLenUDID > DEFAULT_MAX_LEN_UDID)
    {
        nLenUDID = DEFAULT_MAX_LEN_UDID;
    }
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetUDID, pUDID, nLenUDID);
}

vecByteLst CBluetoothComandParser::getAns_setAudioSource(emAudioSourceStat eStat)
{
    return CBluetoothComandParser::packageBTCmd(eBTCmdSetAudioSource, eStat);
}

vecByteLst CBluetoothComandParser::getAns_getAudioSource(emAudioSourceStat eStat)
{
    return CBluetoothComandParser::getAns_setAudioSource(eStat);
}

vecByteLst CBluetoothComandParser::getAns_getAllServoID()
{
    byte arrServoIDs[ROBOT_TOTAL_MOTOR_NUMBER] = {0};
    for(int i = 0; i < ROBOT_TOTAL_MOTOR_NUMBER; i++)
    {
        arrServoIDs[i] = ROBOT_ID_DEFAULT_START_NUMBER + i;
    }
    int nLen = sizeof(arrServoIDs);
    return CBluetoothComandParser::packageBTCmd(eBTCmdGetUTF8AndServoNum, arrServoIDs, nLen);
}


////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////

int CBluetoothComandParser::getHeadSize()
{
    return m_nHeadSize;
}

int CBluetoothComandParser::getLenFlagSize()
{
    return m_nLenFlagSize;
}

int CBluetoothComandParser::getCmdIDFlagSize()
{
    return m_nCmdIDFlagSize;
}

int CBluetoothComandParser::getChksumSize()
{
    return m_nChksumSize;
}

int CBluetoothComandParser::getTailSize()
{
    return m_nTailSize;
}


////////////////////////////////////////////////////////////////////////////////////

int CBluetoothComandParser::getCmdLenByParamsLen(int nLenCmdAndParams)
{
    //长度:命令头2+长度1+<命令1+参数>+checksum1的长度.
    return (CBluetoothComandParser::getHeadSize()
            + 1
            + nLenCmdAndParams
            + CBluetoothComandParser::getChksumSize());
}

int CBluetoothComandParser::getCmdTotalLenByParamsLen(int nLenCmdAndParams)
{
    return CBluetoothComandParser::getCmdLenByParamsLen(nLenCmdAndParams) + CBluetoothComandParser::getTailSize();
}

vecByteLst CBluetoothComandParser::packageBTCmd(uchar cmd1, uchar cmd2, uchar cmd3, const uchar *pParams, int nLenParams)
{
    byte dataCmdTemp[DEFAULT_MAX_LEN_CMD_ARRAY] = {0x00};
    dataCmdTemp[0] = cmd1;
    dataCmdTemp[1] = cmd2;
    dataCmdTemp[2] = cmd3;
    dataCmdTemp[3] = nLenParams;
    memcpy(dataCmdTemp + 4, pParams, nLenParams);
    const int nCmdLen = nLenParams + 4;
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::packageBTCmd(uchar cmd1, uchar cmd2, const uchar *pParams, int nLenParams)
{
    byte dataCmdTemp[DEFAULT_MAX_LEN_CMD_ARRAY] = {0x00};
    dataCmdTemp[0] = cmd1;
    dataCmdTemp[1] = cmd2;
    memcpy(dataCmdTemp + 2, pParams, nLenParams);
    const int nCmdLen = nLenParams + 2;
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::packageBTCmd(uchar cmd, const uchar *pParams, int nLenParams)
{
    byte dataCmdTemp[DEFAULT_MAX_LEN_CMD_ARRAY] = {0x00};
    dataCmdTemp[0] = cmd;
    memcpy(dataCmdTemp + 1, pParams, nLenParams);
    const int nCmdLen = nLenParams + 1;
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

/**************************************************************************
 * 函数名: packageBTCmd
 * 功能:
 * 参数:
 *    @[int] pBufCmd: cmd and the params datas(命令(1B) +<参数1[参数2][参数3](nB) >)
 *    @[out]
 * 返回值: 命令数据vecByteLst
 * 时间: 20161207
 * 作者: asu
 */
vecByteLst CBluetoothComandParser::packageBTCmd(const uchar *pBufCmd, int nLenCmd)
{
    vecByteLst vecSaved;
    vecSaved.clear();

    if(!pBufCmd || (nLenCmd <= 0))
    {
        return vecSaved;
    }

    uchar head[] = {BT_CMD_HEAD, CDataOperate::swap(BT_CMD_HEAD)};

    const int nLenHead = sizeof(head);
    const int nLenCmdTemp = CBluetoothComandParser::getCmdLenByParamsLen(nLenCmd);

    //head
//    memcpy(pBufSaved, &head, nLenHead);
//    pBufSaved += nLenHead;
    CDataOperate::packageByAppend(vecSaved, head, nLenHead);

    //len
    vecSaved.push_back((uchar)nLenCmdTemp);

    //cmd
    CDataOperate::packageByAppend(vecSaved, pBufCmd, nLenCmd);

    //check
    uchar checkSum = (uchar)CDataOperate::checkSum(pBufCmd, nLenCmd);
    checkSum += (uchar)nLenCmdTemp;
    vecSaved.push_back(checkSum);

    //end
    vecSaved.push_back((uchar)BT_CMD_END);

    //all len
//    nSavedLen = CBluetoothComandParser::getCmdTotalLenByParamsLen(nLenCmd);

    return vecSaved;
}

vecByteLst CBluetoothComandParser::packageBTCmd(uchar pos1Param)
{
    byte dataCmdTemp[1] = {pos1Param};
    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::packageBTCmd(uchar pos1Param, uchar pos2Param)
{
    byte dataCmdTemp[] = {pos1Param, pos2Param};
    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::packageBTCmd(uchar pos1Param, uchar pos2Param, uchar pos3Param)
{
    byte dataCmdTemp[] = {pos1Param, pos2Param, pos3Param};
    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::packageBTCmd(uchar pos1Param, uchar pos2Param, uchar pos3Param, uchar pos4Param)
{
    byte dataCmdTemp[] = {pos1Param, pos2Param, pos3Param, pos4Param};
    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::packageBTCmd(uchar pos1Param, uchar pos2Param, uchar pos3Param,
                                                uchar pos4Param, uchar pos5Param)
{
    byte dataCmdTemp[] = {pos1Param, pos2Param, pos3Param, pos4Param, pos5Param};
    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

vecByteLst CBluetoothComandParser::packageBTCmd(uchar pos1Param, uchar pos2Param, uchar pos3Param,
                                                uchar pos4Param, uchar pos5Param, uchar pos6Param)
{
    byte dataCmdTemp[] = {pos1Param, pos2Param, pos3Param, pos4Param, pos5Param, pos6Param};
    int nCmdLen = sizeof(dataCmdTemp);
    return CBluetoothComandParser::packageBTCmd(dataCmdTemp, nCmdLen);
}

