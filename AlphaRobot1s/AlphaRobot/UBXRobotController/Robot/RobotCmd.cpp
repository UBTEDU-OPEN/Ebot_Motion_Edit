/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：RobotCmd.cpp
* 创建时间：2015/06/08 11:41
* 文件标识：
* 文件摘要：机器人操作命令类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/06/08 11:41
* 版本摘要：
*/
#include "stdafx.h"
#include "RobotCmd.h"
#include "CRC32.h"
#include "UBXPublic.h"


#include "robotcmdbtprotocol.h"


CRobotCmd::CRobotCmd(void)
{
    setRobotCmdProtocolType(eCmdProtocolTypeOldPCProtocol);
}


CRobotCmd::~CRobotCmd()
{
}

/**************************************************************************
* 函数名: GetCmd_ReadHardwareVer
* 功能: 获取读取硬件版本命令数据
* 参数:
*    @[out] nDataLenRet:返回的数据长度
* 返回值: 命令数据
* 时间: 2015/06/16 16:14
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_ReadHardwareVer(int& nDataLenRet)
{
	static byte databuf[] = {0xF1, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);
	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_ReadSoftwareVer
* 功能: 获取读取软件版本命令数据
* 参数:
*    @[out] nDataLenRet:返回的数据长度
* 返回值: 命令数据
* 时间: 2015/06/16 16:14
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_ReadSoftwareVer(int& nDataLenRet)
{	
	static byte databuf[] = {0xF5, 0x5F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xED};
	nDataLenRet = sizeof(databuf);
	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_DownloadBinPrepare
* 功能: 获取开始下载bin文件的命令数据
* 参数:
*    @[in ] szBinSrcPath: bin源文件路径
*    @[in ] sBinDestPath: bin目标文件路径
*    @[in ] bUTF8: 是否是UTF8目标文件路径
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2015/06/16 16:14
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_DownloadBinPrepare(const char* sBinSrcPath, const char* sBinDestPath, BOOL bUTF8, int& nDataLenRet)
{	
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

        const int framesize = TRANSFILE_FRAMESIZE_OLD;//数据帧长度
		int cmdlen = 1 + 1 + filenamelen + 2;
        int filesize = CUtil::GetFileSize(sSrcPath);
		int framecount = filesize%framesize ? filesize/framesize+1 : filesize/framesize;//总帧数

		nDataLenRet = 2 + 1 + 1 + filenamelen + 2 + 1 + 1;
		byte* databuf = new byte[nDataLenRet];
		memset(databuf, 0, nDataLenRet);

		byte* databuftemp = databuf;
		byte head[2] = {0xF8, 0x8F};

		memcpy(databuftemp, head, 2);
		databuftemp += 2;

		*databuftemp = cmdlen;
		databuftemp += 1;

		*databuftemp = filenamelen;
		databuftemp += 1;

		memcpy(databuftemp, sztempbinname, filenamelen);
		databuftemp += filenamelen;

		memcpy(databuftemp, &framecount, 2);
		databuftemp += 2;

		byte chksum = (byte)cmdlen + (byte)filenamelen;
		for(int i=0;i<filenamelen;i++)
		{
			chksum += sztempbinname[i];
		}

		byte tempcount[2] = {0};
		memcpy(tempcount, &framecount, 2);
		chksum += tempcount[0] + tempcount[1];		

		*databuftemp = chksum;
		databuftemp += 1;

		*databuftemp = 0xED;

		return databuf;
	}
	return NULL;
}

/**************************************************************************
* 函数名: GetCmd_DownloadBinIng
* 功能: 获取正在下载bin文件的命令数据
* 参数:
*    @[in ] curdata: 当前bin文件数据
*    @[in ] curframe: 当前传输帧
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2015/06/16 16:14
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_DownloadBinIng(const byte* curdata, const int curframe, int& nDataLenRet)
{
    const int framesize = TRANSFILE_FRAMESIZE_OLD;//数据帧长度
    const int sendsize = RobotCmdBTProtocol::m_nTransFilePackageSize; //发送长度
	nDataLenRet = sendsize;

	byte* sendbuf = new byte[sendsize];
	memset(sendbuf, 0, sendsize);

	byte* pbuf = sendbuf;

	byte head[2] = {0xF7, 0x7F};
	memcpy(pbuf, head, 2);
	pbuf += 2;

	memcpy(pbuf, &curframe, 2);
	pbuf += 2;

	memcpy(pbuf, curdata, framesize);
	pbuf += framesize;

	byte tempchk[2] = {0};
	memcpy(tempchk, &curframe, 2);

	byte chksum = tempchk[0] + tempchk[1];
	for(int i=0;i<framesize;i++)
	{
		chksum += curdata[i];
	}

	*pbuf = chksum;
	pbuf += 1;

	*pbuf = 0xED;

    return sendbuf;
}

/**************************************************************************
* 函数名: GetCmd_DownloadBinEnd
* 功能: 获取结束下载bin文件的命令数据
* 参数:
*    @[in ] curdata: 当前bin文件数据
*    @[in ] cursize: 当前传输数据长度
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2015/06/16 16:14
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_DownloadBinEnd(const byte* curdata, const int cursize, int& nDataLenRet)
{
	const int sendsize = 2 + 1 + cursize + 1 + 1;//发送长度
	nDataLenRet = sendsize;

	byte* sendbuf = new byte[sendsize];
    memset(sendbuf, 0, sendsize);

	byte* pbuf = sendbuf;

	byte head[2] = {0xF6, 0x6F};
	memcpy(pbuf, head, 2);
	pbuf += 2;

	byte cmdlen = 1 + cursize;//命令长度
	*pbuf = cmdlen;
	pbuf += 1;

	memcpy(pbuf, curdata, cursize);
	pbuf += cursize;

	byte chksum = cmdlen;
	for(int i=0;i<cursize;i++)
	{
		chksum += curdata[i];
	}

	*pbuf = chksum;
	pbuf += 1;

	*pbuf = 0xED;

	return sendbuf;
}

/**************************************************************************
* 函数名: GetCmd_TransFileCancel
* 功能: 获取取消文件传输的命令
* 参数:
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2015/07/18 16:14
* 作者:hwx
*/
byte *CRobotCmd::GetCmd_TransFileCancel(int &nDataLenRet)
{
    RobotCmdBTProtocol cmdTB;
    return cmdTB.GetCmd_TransFileCancel(nDataLenRet);
 #if 0
    static byte databuf[] = {0xFB, 0xBF, 0x00, 0x17, 0x01, 0xED};
    nDataLenRet = sizeof(databuf);
    databuf[2] = nDataLenRet - 1;
    databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
    return databuf;
#endif
}

/**************************************************************************
* 函数名: GetCmd_MotorGetOffset
* 功能: 获取舵机偏移量回读指令
* 参数: 
*    @[in ] nMotorID: 舵机ID
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 偏移量回读指令
* 时间: 2015/09/08 13:15
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorGetOffset(int nMotorID, int& nDataLenRet)
{	
	static byte databuf[] = {0xFA, 0xAF, 0x0F, 0xD4, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nMotorID;

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_MotorSetOffset
* 功能: 获取舵机偏移量设置指令
* 参数: 
*    @[in ] nMotorID: 舵机ID
*    @[in ] nOffset: 偏移值
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 偏移量设置指令
* 时间: 2015/09/08 13:31
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorSetOffset(int nMotorID, int nOffset, int& nDataLenRet)
{
	static byte databuf[] = {0xFA, 0xAF, 0x0F, 0xD2, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nMotorID;

//	CString str;
//	str.Format(_T("%02X"), nOffset);
//	str = str.Right(2);

//	int iRes;
//	StrToIntEx((L"0x"+str), STIF_SUPPORT_HEX, &iRes);

	if (nOffset >= 0)
	{
		databuf[7] = nOffset;
		databuf[6] = 0x00;
	}
	else
	{
		databuf[6] = 0xFF;
        databuf[7] = nOffset;
	}

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_MotorVersion
* 功能: 获取舵机读版本指令
* 参数: 
*    @[in ] nMotorID: 舵机ID
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 读版本指令
* 时间: 2015/09/08 13:31
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorVersion(int nMotorID, int& nDataLenRet)
{
	static byte databuf[] = {0xFC, 0xCF, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nMotorID;

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_MotorGetID
* 功能: 获取舵机读ID指令
* 参数: 
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 读ID指令
* 时间: 2015/09/08 13:15
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorGetID(int& nDataLenRet)
{
	return GetCmd_MotorGetOffset(0, nDataLenRet);
}

/**************************************************************************
* 函数名: GetCmd_MotorSetID
* 功能: 获取舵机写ID指令
* 参数: 
*    @[in ] nOldMotorID: 旧舵机ID
*    @[in ] nNewMotorID: 新舵机ID
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 写ID指令
* 时间: 2015/09/08 13:31
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorSetID(int nOldMotorID, int nNewMotorID, int& nDataLenRet)
{
	static byte databuf[] = {0xFA, 0xAF, 0x01, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nOldMotorID;
	databuf[5] = nNewMotorID;

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_MotorUpgradePrepare
* 功能: 获取进入舵机升级模式指令
* 参数: 
*    @[in ] nMotorID: 舵机ID
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 舵机升级模式指令
* 时间: 2015/09/08 13:31
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorUpgradePrepare(int nMotorID, int& nDataLenRet)
{
	static byte databuf[] = {0xFC, 0xCF, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nMotorID;

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_MotorUpgradeIng
* 功能: 获取正在升级舵机指令
* 参数: 
*    @[in ] nMotorID: 舵机ID
*    @[in ] nCurPage: 当前帧
*    @[in ] pUserData: 用户数据
*    @[in ] nUserDataLen: 用户数据长度
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 正在升级舵机指令
* 时间: 2015/09/08 13:31
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorUpgradeIng(int nMotorID, int nCurPage, byte* pUserData, int nUserDataLen, int& nDataLenRet)
{
	const int datalen = 71;
    const int framesize = 64;

	static byte databuf[datalen] = {0};
	memset(databuf, 0, sizeof(databuf));

	nDataLenRet = datalen;	
	
	byte head[2] = {0xFD, 0xDF};

	memcpy(databuf, head, 2);
	databuf[2] = nMotorID;
	databuf[3] = 0x01;
	databuf[4] = nCurPage;

	int nCopyLen = nUserDataLen <= framesize ? nUserDataLen:framesize;
	memcpy(databuf + 5, pUserData, nCopyLen);

	for(int i = 2; i <= datalen-3; i++)
	{
		databuf[datalen-2] += databuf[i];
	}

	databuf[datalen-1] = 0xED;

	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_MotorUpgradeEnd
* 功能: 获取结束升级舵机指令
* 参数: 
*    @[in ] nMotorID: 舵机ID
*    @[in ] nCurPage: 当前帧
*    @[in ] pCheckData: 需要校验的用户数据
*    @[in ] nCheckDataLen: 需要校验的用户数据长度
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 结束升级舵机指令
* 时间: 2015/09/08 13:31
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorUpgradeEnd(int nMotorID, int nCurPage, byte* pCheckData, int nCheckDataLen, int& nDataLenRet)
{
	const int datalen = 71;
	static byte databuf[datalen] = {0};
	memset(databuf, 0, sizeof(databuf));

	nDataLenRet = datalen;	

	byte head[2] = {0xFD, 0xDF};

	memcpy(databuf, head, 2);
	databuf[2] = nMotorID;
	databuf[3] = 0x02;
    databuf[4] = nCurPage;

    //checkdata需64字节对齐
    byte* pNewBuf = NULL;
    int nNewBufLen = 0;
    bool bAligned = false; //对齐标识

    if(nCheckDataLen % 64 == 0)
    {
        //已对齐
        bAligned = true;

        pNewBuf = pCheckData;
        nNewBufLen = nCheckDataLen;
    }
    else
    {
        bAligned = false;

        int nAddLen = 64 - nCheckDataLen % 64;
        nNewBufLen = nCheckDataLen + nAddLen;
        pNewBuf = new byte[nNewBufLen];
        memset(pNewBuf, 0, nNewBufLen);
        memcpy(pNewBuf, pCheckData, nCheckDataLen);
    }

    UINT nCRC32 = crc32_custom(CRC32_INIT_VALUE, pNewBuf, nNewBufLen);
    memcpy(databuf + 5, &nCRC32, sizeof(nCRC32));

    if(!bAligned)
    {
        SAFE_DELETE_ARRAY(pNewBuf);
    }

	for(int i = 2; i <= datalen-3; i++)
	{
		databuf[datalen-2] += databuf[i];
	}

	databuf[datalen-1] = 0xED;

	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_MotorMove
* 功能: 舵机运行命令
* 参数: 
*    @[in ] nMotorID: 舵机ID
*    @[in ] nAngle: 角度
*    @[in ] nTime: 时间
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 舵机运行命令数据
* 时间: 2015/09/15 13:17
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorMove(int nMotorID, int nAngle, int nTime, int& nDataLenRet)
{
	static byte databuf[] = {0xFA, 0xAF, 0x01, 0x01, 0, 0x01, 0, 0x01, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nMotorID;
	databuf[4] = nAngle;
    databuf[5] = nTime; //运行时间

    byte tempdata[2] = {0};
    memcpy(tempdata, &nTime, 2);
    databuf[6] = tempdata[1];
    databuf[7] = tempdata[0];

	databuf[8] = GetChkSum(databuf + 2, 6);

	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_MotorGetState
* 功能: 获取舵机状态
* 参数: 
*    @[in ] nMotorID: 舵机ID
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据
* 时间: 2015/10/29 14:37
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorGetState(int nMotorID, int& nDataLenRet)
{
	static byte databuf[] = {0xEF, 0xFE, 0, 0, 0, 0, 0, 0, 0, 0xED };
	nDataLenRet = sizeof(databuf);
	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_RobotDebug
* 功能: 动作调试
* 参数: 
*    @[in ] pMotorBuf: 舵机数据缓冲区
*    @[in ] nMotorCount: 舵机数量
*    @[in ] nRunTime: 运行时间
*    @[in ] nAllTime: 总时间
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据
* 时间: 2015/10/29 14:44
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_RobotDebug(MOTOR_DATA* pMotorBuf, int nMotorCount, int nRunTime, int nAllTime, int& nDataLenRet)
{
	const int nMaxMotorCount = 20; //最大舵机数量
	if(nMotorCount > nMaxMotorCount)
	{
		nDataLenRet = 0;
		return NULL;
	}

	const int nMoveDataLen = nMotorCount + 2;
	int* pMoveData = new int[nMoveDataLen];

	for(int i = 0; i < nMotorCount; i++)
	{
		pMoveData[i] = pMotorBuf[i].nAngle;
	}
	pMoveData[nMotorCount] = nRunTime;
	pMoveData[nMotorCount + 1] = nAllTime;

	static byte databuf[33] = 
	{
		0xFB, 0xBF, 0x01, 0, 0, 0, 0, 0, 
		0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x3C, 0x4C, 0x6E, 
		0x5A, 0x5A, 0x78, 0x68, 0x46, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 
		0x32, 0, 0x32, 0x6D, 0xED
	};

	nDataLenRet = sizeof(databuf);

	for (int i = 0; i < nMotorCount; i++)
	{
		databuf[8+i] = pMoveData[i];
	}

	databuf[28] = pMoveData[nMotorCount] / 20;

	if (pMoveData[nMotorCount+1] >= 60)
	{
		pMoveData[nMotorCount+1] = pMoveData[nMotorCount+1] - 40;
	}

	if ((pMoveData[nMotorCount+1] / 20) < 256)
	{
		databuf[29] = 0;
		databuf[30] = pMoveData[nMotorCount+1] / 20;

		if (databuf[30] == 256)
		{
			databuf[29] = databuf[29] + 1;
			databuf[30] = 0x00;
		}
		else
		{
			if(databuf[30] == 0x00)
			{
				databuf[30] = 1;
			}
		}	
	}
	else // 暂时
	{
		databuf[29] = (pMoveData[nMotorCount+1] / 20)/256;
		databuf[30] = (pMoveData[nMotorCount+1] / 20)%256;

		if (databuf[30] == 256)
		{
			databuf[29] = databuf[29] + 1;
			databuf[30] = 0x00;
		}
		else
		{
			if(databuf[30] == 0x00)
			{
				databuf[30] = 1;
			}
		}
	}

	// 校验位
	byte chksum = 0;
	for (int n = 2; n < 31; n++)
	{
		chksum += databuf[n];
	}
	databuf[31] = chksum;

	SAFE_DELETE_ARRAY(pMoveData);

	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_SwitchMode
* 功能: 获取切换模式命令数据
* 参数: 
*    @[in ] nMode: 模式
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据，无需在外释放
* 时间: 2015/10/19 13:06
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_SwitchMode(int nMode, int& nDataLenRet)
{
    static byte databuf[] = {0xF9, 0x9F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xED};
    databuf[2] = nMode;
    databuf[8] = GetChkSum(databuf + 2, 6);
	nDataLenRet = sizeof(databuf);
	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_ConnectRobot
* 功能: 连接机器人
* 参数: 
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据，无需在外释放
* 时间: 2015/10/29 9:18
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_ConnectRobot(int& nDataLenRet)
{
    static byte databuf[] = {0xF4, 0x4F, 0x01, 0x01, 0, 0, 0, 0, 0, 0xED};
    nDataLenRet = sizeof(databuf);

    databuf[8] = GetChkSum(databuf + 2, 6);
    return databuf;
}

/**************************************************************************
* 函数名: GetCmd_DisconnectRobot
* 功能: 断开机器人
* 参数: 
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据，无需在外释放
* 时间: 2015/10/29 9:18
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_DisconnectRobot(int& nDataLenRet)
{
	static byte databuf[] = {0xF4, 0x4F, 0x01, 0x00, 0, 0, 0, 0, 0, 0xED};

	nDataLenRet = sizeof(databuf);
	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_RobotReset
* 功能: 机器人复位
* 参数: 
*    @[in ] pMotorBuf: 舵机数据缓冲区
*    @[in ] nMotorCount: 舵机数量
*    @[in ] nRunTime: 运行时间
*    @[in ] nAllTime: 总时间
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据
* 时间: 2015/10/29 14:44
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_RobotReset(MOTOR_DATA* pMotorBuf, int nMotorCount, int nRunTime, int nAllTime, int& nDataLenRet)
{
	const int nMaxMotorCount = 20; //最大舵机数量
	if(nMotorCount > nMaxMotorCount)
	{
		nDataLenRet = 0;
		return NULL;
	}	

	static byte databuf[33] = 
	{
		0xFB, 0xBF, 0x01, 0, 0, 0, 0, 0, 
		0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x3C, 0x4C, 0x6E, 
		0x5A, 0x5A, 0x78, 0x68, 0x46, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 
		0x32, 0, 0x32, 0x6D, 0xED
	};

	nDataLenRet = sizeof(databuf);

	for (int i = 0; i < nMotorCount; i++)
	{
		databuf[8+i] = pMotorBuf[i].nAngle;
	}

//    databuf[28] = nRunTime / 20;

//    if (nAllTime >= 60)
//    {
//        nAllTime = nAllTime - 40;
//    }

//    if ((nAllTime / 20) < 256)
//    {
//        databuf[29] = 0;
//        databuf[30] = nAllTime / 20;

//        if (databuf[30] == 256)
//        {
//            databuf[29] = databuf[29] + 1;
//            databuf[30] = 0x00;
//        }
//        else
//        {
//            if(databuf[30] == 0x00)
//            {
//                databuf[30] = 1;
//            }
//        }
//    }
//    else // 暂时
//    {
//        databuf[29] = (nAllTime / 20)/256;
//        databuf[30] = (nAllTime / 20)%256;

//        if (databuf[30] == 256)
//        {
//            databuf[29] = databuf[29] + 1;
//            databuf[30] = 0x00;
//        }
//        else
//        {
//            if(databuf[30] == 0x00)
//            {
//                databuf[30] = 1;
//            }
//        }
//    }

    databuf[28] = nRunTime;

    byte tempdata[2] = {0};
    memcpy(tempdata, &nAllTime, 2);
    databuf[29] = tempdata[1];
    databuf[30] = tempdata[0];

	// 校验位
	byte chksum = 0;
	for (int n = 2; n <= 30; n++)
	{
		chksum += databuf[n];
	}
	databuf[31] = chksum;

	//SAFE_DELETE_ARRAY(pMoveData);

	return databuf;
}

/**************************************************************************
* 函数名: GetCmd_RobotReadSN
* 功能: 读机器人序列号
* 参数: 
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据，无需在外释放
* 时间: 2015/10/29 9:18
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_RobotReadSN(int& nDataLenRet)
{
#if 1
	static byte databuf[] = {0xFB, 0xBF, 0x00, 0x33, 0x00, 0, 0xED};

	nDataLenRet = sizeof(databuf);
	databuf[2] = nDataLenRet - 1;

	databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
	return databuf;
#else
	return NULL;
#endif
}

/**************************************************************************
* 函数名: GetCmd_RobotWriteSN
* 功能: 写机器人序列号
* 参数: 
*    @[in ] pszSN: 序列号数据
*    @[in ] nSNlen: 序列号数据长度
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据，无需在外释放
* 时间: 2015/10/29 9:18
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_RobotWriteSN(char* pszSN, int nSNlen, int& nDataLenRet)
{
#if 1
	static byte databuf[] = {0xFB, 0xBF, 0x00, 0x33, 0x01, 
		0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0xED};

	nDataLenRet = sizeof(databuf);

	databuf[2] = nDataLenRet - 1;

	if(nSNlen > 16)
		nSNlen = 16;
	memcpy(databuf + 5, pszSN, nSNlen);

	databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
	return databuf;
#else
	return NULL;
#endif
}

/**************************************************************************
* 函数名: GetCmd_RobotReadUID
* 功能: 读机器人UID
* 参数: 
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据，无需在外释放
* 时间: 2015/10/29 9:18
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_RobotReadUID(int& nDataLenRet)
{
#if 1
	static byte databuf[] = {0xFB, 0xBF, 0x00, 0x34, 0x00, 0, 0xED};

	nDataLenRet = sizeof(databuf);
	databuf[2] = nDataLenRet - 1;

	databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
	return databuf;
#else
	return NULL;
#endif
}

/**************************************************************************
* 函数名: GetCmd_RobotCheckUTF8
* 功能: 检查是否支持UTF8
* 参数: 
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据，无需在外释放
* 时间: 2015/10/29 9:18
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_RobotCheckUTF8(int& nDataLenRet)
{
#if 1
	static byte databuf[] = {0xFB, 0xBF, 0x00, 0x35, 0x00, 0, 0xED};

	nDataLenRet = sizeof(databuf);
	databuf[2] = nDataLenRet - 1;

	databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
	return databuf;
#else
	return NULL;
#endif
}

/**************************************************************************
* 函数名: GetCmd_MotorGetAngle
* 功能: 获取舵机角度命令
* 参数:
*    @[in ] nMotorID: 舵机ID
*    @[out] nDataLenRet: 指令数据长度
* 返回值: 命令数据，无需在外释放
* 时间: 2015/10/29 9:18
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_MotorGetAngle(int nMotorID, int& nDataLenRet)
{
    static byte databuf[] = {0xFA, 0xAF, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};

    nDataLenRet = sizeof(databuf);
    databuf[2] = nMotorID;
    databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
    return databuf;
}

/**************************************************************************
* 函数名: GetCmd_DownloadBinPrepare2
* 功能: 获取开始下载bin文件的命令数据（蓝牙协议）
* 参数:
*    @[in ] szBinSrcPath: bin源文件路径
*    @[in ] sBinDestPath: bin目标文件路径
*    @[in ] bUTF8: 是否是UTF8目标文件路径
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2015/06/16 16:14
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_DownloadBinPrepare2(const char* sBinSrcPath, const char* sBinDestPath, BOOL bUTF8, int& nDataLenRet)
{
    RobotCmdBTProtocol cmdBTProtocol;
    return cmdBTProtocol.GetCmd_DownloadBinPrepare2(sBinSrcPath, sBinDestPath, bUTF8, nDataLenRet);

#if 0
    QString sSrcPath = AnsiToUnicode(sBinSrcPath);
    if(PathFileExists(sSrcPath))
    {
        const char* sztempbinname = sBinDestPath;
        int filenamelen = strlen(sztempbinname) + 1;
        if(bUTF8)
        {
            QString sTempUnicode = AnsiToUnicode(sBinDestPath);
            char* pszUTF8 = NULL;
            int nLen = UnicodeToUtf8(sTempUnicode, pszUTF8);
            string sUTF8 = pszUTF8;
            SAFE_DELETE_ARRAY(pszUTF8);

            if(sUTF8.length() <= 0)
            {
                nDataLenRet = 0;
                return NULL;
            }
            sztempbinname = sUTF8.c_str();
            filenamelen = sUTF8.length() + 1;
        }

        const int framesize = TRANSFILE_FRAMESIZE_NEW;//数据帧长度
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
 #endif

}

/**************************************************************************
* 函数名: GetCmd_DownloadBinIng2
* 功能: 获取正在下载bin文件的命令数据（蓝牙协议）
* 参数:
*    @[in ] curframe: 当前传输帧
*    @[in ] curdata: 当前bin文件数据
*    @[in ] cursize: 当前传输数据长度
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2015/06/16 16:14
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_DownloadBinIng2(const int curframe, const byte* curdata, const int cursize, int& nDataLenRet , emUbtProductType eUbtProductType)
{
    RobotCmdBTProtocol cmdBTProtocol;
    return cmdBTProtocol.GetCmd_DownloadBinIng2(curframe,  curdata, cursize, nDataLenRet, eUbtProductType);
#if 0
    const int framesize = cursize;//数据帧长度
    int cmdlen = 2 + 1 + 1 + 2 + framesize + 1;
    int sendsize = cmdlen + 1; //发送长度
    sendsize = ((sendsize - 1) / (TRANSFILE_PKGSIZE) + 1) * TRANSFILE_PKGSIZE; //64字节对齐
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
#endif
}

/**************************************************************************
* 函数名: GetCmd_DownloadBinEnd2
* 功能: 获取结束下载bin文件的命令数据（蓝牙协议）
* 参数:
*    @[in ] curframe: 当前传输帧
*    @[in ] curdata: 当前bin文件数据
*    @[in ] cursize: 当前传输数据长度
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2015/06/16 16:14
* 作者: ZDJ
*/
byte* CRobotCmd::GetCmd_DownloadBinEnd2(const int curframe, const byte* curdata, const int cursize, int& nDataLenRet, emUbtProductType eUbtProductType)
{
    RobotCmdBTProtocol cmdBTProtocol;
    return cmdBTProtocol.GetCmd_DownloadBinEnd2( curframe,  curdata,  cursize, nDataLenRet, eUbtProductType);
#if 0
    const int framesize = cursize;//数据帧长度
    int cmdlen = 2 + 1 + 1 + 2 + framesize + 1;
    int sendsize = cmdlen + 1; //发送长度
    sendsize = ((sendsize - 1) / (TRANSFILE_PKGSIZE) + 1) * TRANSFILE_PKGSIZE; //64字节对齐
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
#endif
}

byte *CRobotCmd::GetCmd_GetBatteryStatus(int &nDataLenRet)
{
//    static byte databuf[] = {ROBOT_CMD_BLUE_TOOTH_START_FLAG0,
//                             ROBOT_CMD_BLUE_TOOTH_START_FLAG1,
//                             0x00,
//                             eBTCmdGetBatteryStatus,
//                             0x00,
//                             ROBOT_CMD_BLUE_TOOTH_END_FLAG};

//    nDataLenRet = sizeof(databuf);
//    databuf[eBTCmdPosCmdLength] = nDataLenRet - 1;
//    databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
//    return databuf;

    RobotCmdBTProtocol cmdBT;
    return cmdBT.GetCmd_GetBatteryStatus(nDataLenRet);
}

/**************************************************************************
* 函数名: GetCmd_GetDeviceStatus
* 功能: 读取机器人设备状态
* 参数:
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2016/10/25
* 作者: hwx
*/
byte *CRobotCmd::GetCmd_GetDeviceStatus(int &nDataLenRet)
{
    static byte databuf[] = {0xFB, 0xBF, 0x00, 0x0A, 0x00, 0x00, 0xED};

    nDataLenRet = sizeof(databuf);
    databuf[2] = nDataLenRet - 1;
    databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
    return databuf;
}

/**************************************************************************
* 函数名: GetCmd_GetActionList
* 功能: 获取机器人动作列表
* 参数:
*    @[in] pActionDir: 指定动作路径
*    @[in] nActionDirLen: 动作路径长度
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2016/12/01
* 作者: hwx
*/
byte *CRobotCmd::GetCmd_GetActionList(char *pActionDir, int nActionDirLen, int &nDataLenRet)
{
    // 支持传入空
//    if (!pActionDir || nActionDirLen <= 0)
//    {
//        return NULL;
//    }

    QString strActionDir(pActionDir);

    // 需要将文件名转为ascii码
    char *pAsciiActionDir = NULL;
    int nAsciiActionDirLen = Utf8ToAnsi(pActionDir, pAsciiActionDir);
//    if (!pAsciiActionDir || nAsciiActionDirLen <= 0)
//    {
//        return NULL;
//    }

    // 2byte命令起始标志 + 1byte命令长度 + 1byte命令类型 + 1byte动作路径长度 + nByte动作路径长度 + 1byte校验 + 1byte结束标志
    int cmdlen = 2 + 1 + 1 + 1 +  nAsciiActionDirLen + 1;
    int sendsize = cmdlen + 1;
    sendsize = ((sendsize - 1) / (RobotCmdBTProtocol::m_nTransFilePackageSize) + 1) * RobotCmdBTProtocol::m_nTransFilePackageSize; //64字节对齐
    nDataLenRet = sendsize;

    byte* sendbuf = new byte[sendsize];
    memset(sendbuf, 0, sendsize);

    byte* pbuf = sendbuf;

    byte head[2] = {ROBOT_CMD_BLUE_TOOTH_START_FLAG0, ROBOT_CMD_BLUE_TOOTH_START_FLAG1};
    memcpy(pbuf, head, 2);
    pbuf += 2;

    memcpy(pbuf, &cmdlen, 1);
    pbuf += 1;

    *pbuf = (byte)eBTCmdGetActList; //命令号
    pbuf += 1;

    *pbuf = nAsciiActionDirLen; // 路径长度
    pbuf += 1;

    if (pAsciiActionDir && nAsciiActionDirLen >0)
    {
        memcpy(pbuf, pAsciiActionDir, nAsciiActionDirLen);
        pbuf += nAsciiActionDirLen;
    }

    byte chksum = GetChkSum(sendbuf + 2, nDataLenRet - 4);

    *pbuf = chksum;
    pbuf += 1;

    *pbuf = ROBOT_CMD_BLUE_TOOTH_END_FLAG;

    SAFE_DELETE(pAsciiActionDir);
    return sendbuf;
}

byte *CRobotCmd::GetCmd_GetResInfos(int nResInfosType, int nResInfoCmdFlag, char *pParam, int nLenParam, int &nDataLenRet)
{
    return NULL;
}

/**************************************************************************
* 函数名: GetCmd_DeleteAction
* 功能: 删除机器人动作
* 参数:
*    @[out] pActionName: 动作名称
*    @[out] nActionNameLen: 动作名称长度
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2016/12/01
* 作者: hwx
*/
byte *CRobotCmd::GetCmd_DeleteAction(const char *pActionName, int nActionNameLen, int &nDataLenRet)
{  
    //mac时，转换，1p正确(无有1E)
    // 需要将文件名转为ascii码
    char *pAsciiActionName = NULL;
    int nAsciiActionNameLen = Utf8ToAnsi(pActionName, pAsciiActionName);

    // 2byte命令起始标志 + 1byte命令长度 + 1byte命令类型 + nByte动作名称长度 + 1byte校验 + 1byte结束标志
    int cmdlen = 2 + 1 + 1 +  nAsciiActionNameLen + 1;
    int sendsize = cmdlen + 1;
    sendsize = ((sendsize - 1) / (RobotCmdBTProtocol::m_nTransFilePackageSize) + 1) * RobotCmdBTProtocol::m_nTransFilePackageSize; //64字节对齐
    nDataLenRet = sendsize;

    byte* sendbuf = new byte[sendsize];
    memset(sendbuf, 0, sendsize);

    byte* pbuf = sendbuf;

    byte head[2] = {ROBOT_CMD_BLUE_TOOTH_START_FLAG0, ROBOT_CMD_BLUE_TOOTH_START_FLAG1};
    memcpy(pbuf, head, 2);
    pbuf += 2;

    memcpy(pbuf, &cmdlen, 1);
    pbuf += 1;

    *pbuf = eBTCmdDeleteRobotAction; //命令号
    pbuf += 1;

    memcpy(pbuf, pAsciiActionName, nAsciiActionNameLen);
    pbuf += nAsciiActionNameLen;

    byte chksum = GetChkSum(sendbuf + 2, nDataLenRet - 4);

    *pbuf = chksum;
    pbuf += 1;

    *pbuf = ROBOT_CMD_BLUE_TOOTH_END_FLAG;

    SAFE_DELETE(pAsciiActionName);

    return sendbuf;
}

/**************************************************************************
* 函数名: GetCmd_DeleteAction
* 功能: 删除机器人动作
* 参数:
*    @[int] nWifiStat:
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2017/02/27
* 作者: asu
*/
byte *CRobotCmd::GetCmd_WifiScan(int nWifiStat, int &nDataLenRet)
{
    RobotCmdBTProtocol cmdBT;
    return cmdBT.GetCmd_WifiScan(nWifiStat, nDataLenRet);
}

/**************************************************************************
* 函数名: GetCmd_ConnectWifi
* 功能: connect wifi
* 参数:
*    @[int] nWifiStat:
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2017/02/27
* 作者: asu
*/
byte *CRobotCmd::GetCmd_ConnectWifi(char *pSSID, int nLenSSID, char *pPassKey, int nLenPassKey, int &nDataLenRet)
{
    RobotCmdBTProtocol cmdBT;
    return cmdBT.GetCmd_ConnectWifi(pSSID, nLenSSID, pPassKey, nLenPassKey, nDataLenRet);
}

/**************************************************************************
* 函数名: GetCmd_PlayAction
* 功能:
* 参数:
*    @[in] pActionName: action name
*    @[in] nActionNameLen: action name length
*    @[out] nDataLenRet: 返回的命令数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2017/02/27
* 作者: asu
*/
byte *CRobotCmd::GetCmd_PlayAction(char *pActionName, int nActionNameLen, int &nDataLenRet)
{
    RobotCmdBTProtocol cmdBT;
    return cmdBT.GetCmd_PlayAction(pActionName, nActionNameLen, nDataLenRet);
}

/**************************************************************************
* 函数名: GetCmd_SetTime
* 功能: 获取设置机器人时间命令
* 参数:
*    @[in] pDateTime: 时间信息
*    @[out] nDataLenRet: 输出数据长度
* 返回值: 命令数据，需在外释放
* 时间: 2017/03/14
* 作者: hwx
*/
byte *CRobotCmd::GetCmd_SetTime(const QDateTime *pDateTime, int &nDataLenRet)
{
    if (!pDateTime)
    {
        return 0;
    }

    // 2byte命令起始标志 + 1byte命令长度 + 1byte命令类型 + 6Byte动作名称长度 + 1byte校验 + 1byte结束标志
    int cmdlen = 2 + 1 + 1 +  6 + 1;
    int sendsize = cmdlen + 1;
    //sendsize = ((sendsize - 1) / (TRANSFILE_PKGSIZE) + 1) * TRANSFILE_PKGSIZE; //64字节对齐
    nDataLenRet = sendsize;

    byte* sendbuf = new byte[sendsize];
    memset(sendbuf, 0, sendsize);

    byte* pbuf = sendbuf;

    // 命令头
    byte head[2] = {ROBOT_CMD_BLUE_TOOTH_START_FLAG0, ROBOT_CMD_BLUE_TOOTH_START_FLAG1};
    memcpy(pbuf, head, 2);
    pbuf += 2;

    // 命令长度
    memcpy(pbuf, &cmdlen, 1);
    pbuf += 1;

    //命令号
    *pbuf = eBTCmdSetTime;
    pbuf += 1;

    // 时间信息
    byte year = getTimeValue(eTimeTypeYear, pDateTime);
    byte month = getTimeValue(eTimeTypeMonth, pDateTime);
    byte day = getTimeValue(eTimeTypeDay, pDateTime);
    byte hour = getTimeValue(eTimeTypeHour, pDateTime);
    byte minute = getTimeValue(eTimeTypeMinute, pDateTime);
    byte second = getTimeValue(eTimeTypeSecond, pDateTime);

    *pbuf = year;
    ++pbuf;
    *pbuf = month;
    ++pbuf;
    *pbuf = day;
    ++pbuf;
    *pbuf = hour;
    ++pbuf;
    *pbuf = minute;
    ++pbuf;
    *pbuf = second;
    ++pbuf;

    byte chksum = GetChkSum(sendbuf + 2, nDataLenRet - 4);

    *pbuf = chksum;
    pbuf += 1;

    *pbuf = ROBOT_CMD_BLUE_TOOTH_END_FLAG;
    return sendbuf;
}

byte *CRobotCmd::GetCmd_ManalUpGrade(int &nDataLenRet)
{
    RobotCmdBTProtocol cmdBT;
    return cmdBT.GetCmd_ManalUpGrade(nDataLenRet);
}

/**************************************************************************
* 函数名: getTimeValue
* 功能: 根据时间参数类型获取时间值
* 参数:
*    @[in] eTimeType: 时间类型
*    @[in] pDateTime: 时间信息
* 返回值: 时间值
* 时间: 2017/03/14
* 作者: hwx
*/
byte CRobotCmd::getTimeValue(emTimeType eTimeType, const QDateTime *pDateTime)
{
    byte byteValue = 0;
    if (!pDateTime || !pDateTime->isValid())
    {
        return byteValue;
    }

    QDate date = pDateTime->date();
    QTime time = pDateTime->time();

    switch (eTimeType) {
    case eTimeTypeYear:
    {
        int nYear = date.year();
        byteValue = (QString("%1").arg(nYear)).right(2).toInt();
        break;
    }
    case eTimeTypeMonth:
    {
        byteValue = date.month();
        break;
    }
    case eTimeTypeDay:
    {
        byteValue = date.day();
        break;
    }
    case eTimeTypeHour:
    {
        byteValue = time.hour();
        break;
    }
    case eTimeTypeMinute:
    {
        byteValue = time.minute();
        break;
    }
    case eTimeTypeSecond:
    {
        byteValue = time.second();
        break;
    }
    default:
        break;
    }

    return byteValue;
}
