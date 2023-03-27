#include "robotcmdbtprotocol.h"

#include "../common/cdataoperate.h"
#include "../common/ubtwifiutil.h"
#include "RobotCmd.h"
#include "ccomandparsernewbt.h"



int RobotCmdBTProtocol::m_nTransFilePackageSize = TRANSFILE_PKGSIZE_64;
int RobotCmdBTProtocol::m_nTransFilePerFrameSize = TRANSFILE_FRAMESIZE_NEW_FOR_128;



RobotCmdBTProtocol::RobotCmdBTProtocol()
{
    setRobotCmdProtocolType(eCmdProtocolTypeBluetoothPotocol);
}

RobotCmdBTProtocol::~RobotCmdBTProtocol()
{

}

byte *RobotCmdBTProtocol::GetCmd_ReadHardwareVer(int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_getBootHardWareVersion();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_ReadSoftwareVer(int &nDataLenRet)
{
    //todo
    vecByteLst lst = CBluetoothComandParser::getCmd_getRobotSofeVer();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_DownloadBinPrepare(const char *sBinSrcPath, const char *sBinDestPath, BOOL bUTF8, int &nDataLenRet)
{
    //do nothing
    vecByteLst lst;// = CBluetoothComandParser::getCmd_();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_DownloadBinIng(const byte *curdata, const int curframe, int &nDataLenRet)
{
    //todo
    vecByteLst lst;// = CBluetoothComandParser::getCmd_();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_DownloadBinEnd(const byte *curdata, const int cursize, int &nDataLenRet)
{
    //todo
    vecByteLst lst;// = CBluetoothComandParser::getCmd_();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_TransFileCancel(int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_transFileCancel();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorGetOffset(int nMotorID, int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_getOneServoOffset(nMotorID);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorSetOffset(int nMotorID, int nOffset, int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_setOneServoOffset(nMotorID, nOffset);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorVersion(int nMotorID, int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_getOneServoVersion(nMotorID);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorGetID(int &nDataLenRet)
{
    //todo



    vecByteLst lst;// = CBluetoothComandParser::getCmd_();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorSetID(int nOldMotorID, int nNewMotorID, int &nDataLenRet)
{
    //todo



    vecByteLst lst;// = CBluetoothComandParser::getCmd_();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorUpgradePrepare(int nMotorID, int &nDataLenRet)
{
    //todo



    vecByteLst lst;// = CBluetoothComandParser::getCmd_();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorUpgradeIng(int nMotorID, int nCurPage, byte *pUserData, int nUserDataLen, int &nDataLenRet)
{
    //todo



    vecByteLst lst;// = CBluetoothComandParser::getCmd_();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorUpgradeEnd(int nMotorID, int nCurPage, byte *pCheckData, int nCheckDataLen, int &nDataLenRet)
{
    //todo



    vecByteLst lst;// = CBluetoothComandParser::getCmd_();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorMove(int nMotorID, int nAngle, int nTime, int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_controlOneServo(nMotorID, nAngle, nTime, nTime);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorGetState(int nMotorID, int &nDataLenRet)
{
    //todo



    vecByteLst lst;// = CBluetoothComandParser::getCmd_();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_RobotDebug(MOTOR_DATA *pMotorBuf, int nMotorCount, int nRunTime, int nAllTime, int &nDataLenRet)
{
    //todo    
    vecMotorData vecData;
    for(int i = 0; i < nMotorCount; i++)
    {
        MOTOR_DATA motorData;
        motorData.nID = pMotorBuf[i].nID;
        motorData.nAngle = pMotorBuf[i].nAngle;
        vecData.push_back(motorData);
    }

    vecByteLst lst = CBluetoothComandParser::getCmd_controlAllServo(vecData,  nRunTime, nAllTime);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_SwitchMode(int nMode, int &nDataLenRet)
{
    //todo
   //SwitchMode of u


    vecByteLst lst;// = CBluetoothComandParser::getCmd_();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_ConnectRobot(int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_handshake();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_DisconnectRobot(int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_disconnect();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_RobotReset(MOTOR_DATA *pMotorBuf, int nMotorCount, int nRunTime, int nAllTime, int &nDataLenRet)
{
    //todo
    vecMotorData vecData;
    for (int i = 0; i < nMotorCount; i++)
    {
        MOTOR_DATA motoData;
        motoData.nID = i + ROBOT_ID_DEFAULT_START_NUMBER;
        motoData.nAngle = pMotorBuf[i].nAngle;
        vecData.push_back(motoData);
    }

    vecByteLst lst = CBluetoothComandParser::getCmd_controlAllServo(vecData, nRunTime*20, nAllTime*20);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_RobotReadSN(int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_readSN();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_RobotWriteSN(char *pszSN, int nSNlen, int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_writeSN((const byte*)pszSN, nSNlen);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_RobotReadUID(int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_getUDID();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_RobotCheckUTF8(int &nDataLenRet)
{
    //todo
    vecByteLst lst = CBluetoothComandParser::getCmd_getUTF8AndServoNum();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_MotorGetAngle(int nMotorID, int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_getOneServoAnglePoweroff(nMotorID);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_DownloadBinPrepare2(const char *sBinSrcPath, const char *sBinDestPath, BOOL bUTF8, int &nDataLenRet)
{
    //todo
    QString sSrcPath = AnsiToUnicode(sBinSrcPath);
    if(PathFileExists(sSrcPath))
    {
        const char* sztempbinname = sBinDestPath;
        int filenamelen = strlen(sztempbinname) + 1;

        string sUTF8;
        if(bUTF8)
        {
            QString sTempUnicode = AnsiToUnicode(sBinDestPath);
            char* pszUTF8 = NULL;
            int nLen = UnicodeToUtf8(sTempUnicode, pszUTF8);
            sUTF8 = pszUTF8;
            SAFE_DELETE_ARRAY(pszUTF8);

            if(sUTF8.length() <= 0)
            {
                nDataLenRet = 0;
                return NULL;
            }
            sztempbinname = sUTF8.c_str();
            filenamelen = sUTF8.length() + 1;
        }

        //const int framesize = TRANSFILE_FRAMESIZE_NEW;//数据帧长度
        const int framesize = RobotCmdBTProtocol::m_nTransFilePerFrameSize;//数据帧长度

        //字头(2B) +长度(1B) +命令(1B) +参数(nB) + checksum(1B)  + 结束符(1B)
        int cmdlen = 2 + 1 + 1 + (1 + filenamelen + 2 + 1) + 1;
        int filesize = CUtil::GetFileSize(sSrcPath);
        int framecount = filesize%framesize ? filesize/framesize+1 : filesize/framesize;//总帧数

        nDataLenRet = cmdlen + 1;
        byte* databuf = new byte[nDataLenRet];
        memset(databuf, 0, nDataLenRet);

        byte* databuftemp = databuf;
        byte head[2] = {0xFB, 0xBF};

        memcpy(databuftemp, head, 2);
        databuftemp += 2;

        *databuftemp = cmdlen;
        databuftemp += 1;

        *databuftemp = 0x14; //命令号
        databuftemp += 1;

        *databuftemp = filenamelen;
        databuftemp += 1;

        memcpy(databuftemp, sztempbinname, filenamelen);
        databuftemp += filenamelen;

        memcpy(databuftemp, &framecount, 2);
        databuftemp += 2;

        *databuftemp = 1;  //1为强制覆盖，0为不覆盖
        databuftemp += 1;

        byte chksum = GetChkSum(databuf + 2, nDataLenRet - 4);

        *databuftemp = chksum;
        databuftemp += 1;

        *databuftemp = 0xED;

        return databuf;
    }
    return NULL;
}

byte *RobotCmdBTProtocol::GetCmd_DownloadBinIng2(const int curframe, const byte *curdata, const int cursize, int &nDataLenRet, emUbtProductType eUbtProductType)
{
    if( eUbtProduct1E != eUbtProductType )
    {
        //todo
        const int framesize = cursize;//数据帧长度
        int cmdlen = 2 + 1 + 1 + 2 + framesize + 1;
        int sendsize = cmdlen + 1; //发送长度
        sendsize = ((sendsize - 1) / (RobotCmdBTProtocol::m_nTransFilePackageSize) + 1) * RobotCmdBTProtocol::m_nTransFilePackageSize; //64字节对齐
        nDataLenRet = sendsize;

        byte* sendbuf = new byte[sendsize];
        memset(sendbuf, 0, sendsize);

        byte* pbuf = sendbuf;

        byte head[2] = {0xFB, 0xBF};
        memcpy(pbuf, head, 2);
        pbuf += 2;

        memcpy(pbuf, &cmdlen, 1);
        pbuf += 1;

        *pbuf = 0x15; //命令号
        pbuf += 1;

        memcpy(pbuf, &curframe, 2);
        pbuf += 2;

        memcpy(pbuf, curdata, framesize);
        pbuf += framesize;

        byte chksum = GetChkSum(sendbuf + 2, nDataLenRet - 4);

        *pbuf = chksum;
        pbuf += 1;

        *pbuf = 0xED;

        return sendbuf;
    }


    /////////////for eUbtProduct1E, hid 1024
    ///
    //CComandParserNewBT::packageBTCmd( RobotCmdBTProtocol::eBTCmdDownloadBinIng,  , );

    const int nParam = cursize + 2;
    byte* sendbuf = new byte[nParam];
    memset(sendbuf, 0, nParam);
    memcpy(sendbuf, &curframe, 2);

    memcpy(sendbuf + 2, curdata, cursize);
    vecByteLst lst = CComandParserNewBT::packageBTCmd( eBTCmdDownloadBinIng,  sendbuf,  nParam);
    delete []sendbuf;
    sendbuf = NULL;

    //1024 字节对齐
    int nSizeCmdAll = lst.size();
    int nSendSize = ((nSizeCmdAll - 1) / (RobotCmdBTProtocol::m_nTransFilePackageSize) + 1) * RobotCmdBTProtocol::m_nTransFilePackageSize; //64字节对齐
    int nNeedAddZero = nSendSize - nSizeCmdAll;
    for(int i = 0; i < nNeedAddZero; i++)
    {
        lst.push_back((uchar)0x0);
    }

    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;

}

byte *RobotCmdBTProtocol::GetCmd_DownloadBinEnd2(const int curframe, const byte *curdata, const int cursize, int &nDataLenRet,
                                                 emUbtProductType eUbtProductType)
{
    //todo
    if( eUbtProduct1E != eUbtProductType )
    {
        const int framesize = cursize;//数据帧长度
        int cmdlen = 2 + 1 + 1 + 2 + framesize + 1;
        int sendsize = cmdlen + 1; //发送长度
        sendsize = ((sendsize - 1) / (RobotCmdBTProtocol::m_nTransFilePackageSize) + 1) * RobotCmdBTProtocol::m_nTransFilePackageSize; //64字节对齐
        nDataLenRet = sendsize;

        byte* sendbuf = new byte[sendsize];
        memset(sendbuf, 0, sendsize);

        byte* pbuf = sendbuf;

        byte head[2] = {0xFB, 0xBF};
        memcpy(pbuf, head, 2);
        pbuf += 2;

        memcpy(pbuf, &cmdlen, 1);
        pbuf += 1;

        *pbuf = 0x16; //命令号
        pbuf += 1;

        memcpy(pbuf, &curframe, 2);
        pbuf += 2;

        memcpy(pbuf, curdata, framesize);
        pbuf += framesize;

        byte chksum = GetChkSum(sendbuf + 2, nDataLenRet - 4);

        *pbuf = chksum;
        pbuf += 1;

        *pbuf = 0xED;

        return sendbuf;
    }


    /////////////for eUbtProduct1E, hid 1024
    ///
    const int nParam = cursize + 2;
    byte* sendbuf = new byte[nParam];
    memset(sendbuf, 0, nParam);
    memcpy(sendbuf, &curframe, 2);

    memcpy(sendbuf + 2, curdata, cursize);
    vecByteLst lst = CComandParserNewBT::packageBTCmd( eBTCmdDownloadBinEnd,  sendbuf,  nParam);
    delete []sendbuf;
    sendbuf = NULL;

    //1024 字节对齐
    int nSizeCmdAll = lst.size();
    int nSendSize = ((nSizeCmdAll - 1) / (RobotCmdBTProtocol::m_nTransFilePackageSize) + 1) * RobotCmdBTProtocol::m_nTransFilePackageSize; //64字节对齐
    int nNeedAddZero = nSendSize - nSizeCmdAll;
    for(int i = 0; i < nNeedAddZero; i++)
    {
        lst.push_back((uchar)0x0);
    }

    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_GetBatteryStatus(int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_getBatteryStatus();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_GetDeviceStatus(int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_getDriveStatus();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_GetActionList(char *pActionDir, int nActionDirLen, int &nDataLenRet)
{
    //todo
    vecByteLst lst = CBluetoothComandParser::getCmd_getActList(pActionDir, nActionDirLen);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_GetResInfos(int nResInfosType, int nResInfoCmdFlag, char *pParam, int nLenParam, int &nDataLenRet)
{
    vecByteLst lst;
    if(pParam && (nLenParam > 0))
    {
        lst = CBluetoothComandParser::getCmd_getSomeTypeResInfo((emGetSomeTypeResInfoResType)nResInfosType,
                                                                (emGetSomeTypeResInfoCmdFlag)nResInfoCmdFlag,
                                                                pParam, nLenParam);
    }
    else
    {
        lst = CBluetoothComandParser::getCmd_getSomeTypeResInfo((emGetSomeTypeResInfoResType)nResInfosType,
                                                                (emGetSomeTypeResInfoCmdFlag)nResInfoCmdFlag);
    }

    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_DeleteAction(const char *pActionName, int nActionNameLen, int &nDataLenRet)
{
#ifdef Q_OS_WIN0 //Q_OS_WIN
    // 需要将文件名转为ascii码
    char *pAsciiActionName = NULL;
    int nAsciiActionNameLen = Utf8ToAnsi(pActionName, pAsciiActionName);
    vecByteLst lst = CBluetoothComandParser::getCmd_deleteFile((const byte *)pAsciiActionName, nAsciiActionNameLen);
    SAFE_DELETE_ARRAY(pAsciiActionName);

#else
    //mac时，不转换，1p\1e正确
    vecByteLst lst = CBluetoothComandParser::getCmd_deleteFile((const byte *)pActionName, nActionNameLen);

#endif

    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_WifiScan(int nWifiStat, int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_GetWifisList((emWifiInfosStat)nWifiStat);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_ConnectWifi(char *pSSID, int nLenSSID, char *pPassKey, int nLenPassKey, int &nDataLenRet)
{
    string strSSID = pSSID;
    string strPasskey = pPassKey;
    string strParams = UBTWifiUtil::getJsonStr_ConnectWifi(eBTCmdConnectWifi, strSSID, strPasskey);
    vecByteLst lst = CBluetoothComandParser::getCmd_ConnectWifis((const byte*)strParams.c_str(), strParams.length());
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_PlayAction(char *pActionName, int nActionNameLen, int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_playAct((const byte *)pActionName, nActionNameLen);
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_SetTime(const QDateTime *pDateTime, int &nDataLenRet)
{
    byte* pByte = NULL;
    nDataLenRet = 0;
    if(pDateTime && pDateTime->isValid())
    {
        vecByteLst lst = CBluetoothComandParser::getCmd_adjustClock(CRobotCmd::getTimeValue(eTimeTypeYear, pDateTime),
                                                                    CRobotCmd::getTimeValue(eTimeTypeMonth, pDateTime),
                                                                    CRobotCmd::getTimeValue(eTimeTypeDay, pDateTime),
                                                                    CRobotCmd::getTimeValue(eTimeTypeHour, pDateTime),
                                                                    CRobotCmd::getTimeValue(eTimeTypeMinute, pDateTime),
                                                                    CRobotCmd::getTimeValue(eTimeTypeSecond, pDateTime));
        nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    }
    return pByte;
}

byte *RobotCmdBTProtocol::GetCmd_ManalUpGrade(int &nDataLenRet)
{
    vecByteLst lst = CBluetoothComandParser::getCmd_ManalUpGrade();
    byte* pByte = NULL;
    nDataLenRet = CDataOperate::vectToArr(lst, pByte);
    return pByte;
}

