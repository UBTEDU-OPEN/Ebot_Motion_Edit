/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：DataPool.cpp
* 创建时间：2015/07/22 16:22
* 文件标识：
* 文件摘要：数据缓冲池
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/07/22 16:22
* 版本摘要：
*/
#include "stdafx.h"
#include "DataPool.h"
#include "Util.h"

#include <QString>

map<int, int> g_mapProtocolAnswerLen;//协议应答长度


/**************************************************************************
* 函数名: GetProtocolAnswerLen
* 功能: 获取协议应答长度
* 参数: 
*    @[in ] nProtoId: 协议号
* 返回值: 成功返回数据长度，失败返回-1
* 时间: 2015/07/16 17:40
* 作者: ZDJ
*/
int GetProtocolAnswerLen(int nProtoId)
{
	map<int, int>::iterator itor = g_mapProtocolAnswerLen.find(nProtoId);
	if(itor != g_mapProtocolAnswerLen.end())
		return itor->second;
	return -1;
}

/**************************************************************************
* 函数名: GetProtocolAnswerLen
* 功能: 设置协议应答长度
* 参数:
*    @[in ] nProtoId: 协议号
*    @[in ] nProtoLen: 协议数据长度
* 返回值: 成功返回true，失败返回false
* 时间: 2015/07/16 17:40
* 作者: ZDJ
*/
bool SetProtocolAnswerLen(int nProtoId, int nProtoLen)
{
    map<int, int>::iterator itor = g_mapProtocolAnswerLen.find(nProtoId);
    if(itor != g_mapProtocolAnswerLen.end())
    {
        itor->second = nProtoLen;
        return true;
    }
    return false;
}

/**************************************************************************
* 函数名: InitProtocolAnswerLen
* 功能: 初始化协议应答数据长度
* 时间: 2015/07/16 18:03
* 作者: ZDJ
*/
void InitProtocolAnswerLen()
{
	g_mapProtocolAnswerLen[0xFD] = 10;   //正在升级舵机，结束升级舵机
	g_mapProtocolAnswerLen[0xFC] = 10;   //开始升级舵机
	g_mapProtocolAnswerLen[0xFB] = 10;   //动作调试
	g_mapProtocolAnswerLen[0xFA] = 10;   //读写偏移值，读写ID，回读等
	g_mapProtocolAnswerLen[0xF9] = 4;    //PC软件转U盘
	g_mapProtocolAnswerLen[0xF8] = 4;    //开始传输文件
	g_mapProtocolAnswerLen[0xF7] = 4;    //正在传输文件
	g_mapProtocolAnswerLen[0xF6] = 4;    //传输文件结束
	g_mapProtocolAnswerLen[0xF5] = 15;   //读软件版本
	g_mapProtocolAnswerLen[0xF4] = 4;    //工具连接
	g_mapProtocolAnswerLen[0xF3] = 4;    //开始播放
	g_mapProtocolAnswerLen[0xF2] = 5;    //停止播放
	g_mapProtocolAnswerLen[0xF1] = -1;   //读硬件版本，变长数据
	g_mapProtocolAnswerLen[0xEF] = 8;	 //读舵机运行状态
	//g_mapProtocolAnswerLen[0x0D] = -1;	 //打印信息
}

/**************************************************************************
* 函数名: ExistProtocol
* 功能: 是否存在协议
* 参数: 
*    @[in ] nProtoId: 协议号
* 返回值: 存在返回TRUE，否则返回FALSE
* 时间: 2015/09/21 20:32
* 作者: ZDJ
*/
BOOL ExistProtocol(int nProtoId)
{
	map<int, int>::iterator itor = g_mapProtocolAnswerLen.find(nProtoId);
	if(itor != g_mapProtocolAnswerLen.end())
		return TRUE;
	return FALSE;
}

/**************************************************************************
* 函数名: IsBlueToothCmd
* 功能: 判断是否是蓝牙指令
* 参数: 
*    @[in ] pData: 指令数据
*    @[in ] nLen: 指令长度
* 返回值: 是否是蓝牙指令
* 时间: 2015/11/16 10:57
* 作者: ZDJ
*/
BOOL IsBlueToothCmd(byte* pData, int nLen)
{
    if(pData[0] == 0xFB && pData[1] == 0xBF)
    {
        if(pData[2] == 0x01 && nLen == 10)
            return FALSE;
        else
            return TRUE;
    }
    return FALSE;
}

/**************************************************************************
* 函数名: GetChkSum
* 功能: 获取校验值
* 参数:
*    @[in ] pDataBuf: 数据缓冲区
*    @[in ] nDataLen: 数据长度
* 返回值: 校验值
* 时间: 2015/10/29 9:14
* 作者: ZDJ
*/
byte GetChkSum(byte* pDataBuf, int nDataLen)
{
    byte byRet = 0;
    for(int i = 0; i < nDataLen; i++)
    {
        byRet += pDataBuf[i];
    }
    return byRet;
}

PKGDATA::PKGDATA()
{
	m_bBegin = FALSE;
	m_bEnd = FALSE;
}

/**************************************************************************
* 函数名: GetByteData
* 功能: 获取数据包，成功时需在外释放
* 时间: 2015/07/20 17:16
* 作者: ZDJ
*/
byte* PKGDATA::GetByteData()
{
	byte* databuf = new byte[m_myDataList.size()];
	for(int i=0;i<m_myDataList.size();i++)
	{
		databuf[i] = m_myDataList[i];
	}
	return databuf;
}

/**************************************************************************
* 函数名: GetVecData
* 功能: 获取数据包
* 时间: 2015/07/20 17:16
* 作者: ZDJ
*/
vector<byte> PKGDATA::GetVecData()
{
	return m_myDataList;
}

/**************************************************************************
* 函数名: PushData
* 功能: 存储数据
* 时间: 2015/07/20 17:17
* 作者: ZDJ
*/
void PKGDATA::PushData(byte data)
{			
	int nCmdId = -1;

	if(!m_bBegin)
	{
		nCmdId = data; //接收第一个字节，即指令id
		m_bBegin = TRUE;
		m_bEnd = FALSE;
	}
	else
	{
		if(m_myDataList.size() > 0)
			nCmdId = m_myDataList[0];
	}	

	if(m_bBegin)
		m_myDataList.push_back(data);

	int nAnswerLen = GetProtocolAnswerLen(nCmdId);
    if(m_myDataList.size() >= 3)
	{		
		switch(nCmdId)
		{
		case 0xF1:
			nAnswerLen = m_myDataList[2] + 5;//版本号长度+5
			break;	
		case 0xFB:
			{
				if(IsBlueToothCmd(m_myDataList.data(), nAnswerLen))
				{
					nAnswerLen = m_myDataList[2] + 1;
				}
			}
			break;
		}		
	}

	if(nAnswerLen > -1 && m_myDataList.size() >= nAnswerLen)
	{
		m_bBegin = FALSE;
		m_bEnd = TRUE;
	}
}

/**************************************************************************
* 函数名: ClearData
* 功能: 清理数据
* 时间: 2015/07/20 17:18
* 作者: ZDJ
*/
void PKGDATA::ClearData()
{
	m_bBegin = FALSE;
	m_bEnd = FALSE;
	m_myDataList.clear();
}

/**************************************************************************
* 函数名: IsEnd
* 功能: 数据包是否接收完毕
* 时间: 2015/07/20 17:18
* 作者: ZDJ
*/
BOOL PKGDATA::IsEnd()
{
	return !m_bBegin && m_bEnd;
}

/**************************************************************************
* 函数名: IsBegin
* 功能: 数据包是否开始接收
* 时间: 2015/07/20 17:18
* 作者: ZDJ
*/
BOOL PKGDATA::IsBegin()
{
	return m_bBegin;
}

/**************************************************************************
* 函数名: GetHead
* 功能: 获取数据包的指令id
* 时间: 2015/07/20 17:18
* 作者: ZDJ
*/
byte PKGDATA::GetHead()
{
	if(m_myDataList.size() > 0)
		return m_myDataList[0];
	return 0;
}

/**************************************************************************
* 函数名: GetSize
* 功能: 获取数据长度
* 时间: 2015/07/27 15:39
* 作者: ZDJ
*/
int PKGDATA::GetSize()
{
	return m_myDataList.size();
}

/**************************************************************************
* 函数名: IsValid
* 功能: 检测是否是合法协议
* 时间: 2015/09/22 9:44
* 作者: ZDJ
*/
BOOL PKGDATA::IsValid()
{
    if(m_myDataList.size() >= 4)
	{
		byte tempbyte = m_myDataList[1];				
		tempbyte = (tempbyte >> 4 & 0x0F) | (tempbyte << 4 & 0xF0); //高低位互换

        byte headdata = m_myDataList[0];
        if(headdata == tempbyte)
        {
#if 1
            map<int, int>::iterator itor = g_mapProtocolAnswerLen.find(headdata);
            if(itor != g_mapProtocolAnswerLen.end())
            {

                if(m_myDataList[3] == 0xED && m_myDataList.size() == 4
                        && (m_myDataList[2] == 0xAA || m_myDataList[2] == 0xEE))
                    return TRUE;
                else if(m_myDataList.size() > 4)
                {
                    int nCmdLen = itor->second;

                    // add by huangwuixan 2016.7.23
                    // 如果是0xF1/0x1F参数（获取硬件版本），则命令的长度为m_myDataList[2] + 5
                    if (m_myDataList[0] == 0xF1 && m_myDataList[1] == 0x1F)
                    {
                        nCmdLen = m_myDataList[2] + 5;
                    }

                    if(IsBlueToothCmd(m_myDataList.data(), m_myDataList.size()))
                        nCmdLen = m_myDataList[2] + 1;

                    int taildata = m_myDataList[nCmdLen-1];
                    int chksum = m_myDataList[nCmdLen-2];

                    byte* pData = GetByteData();
                    int chksumtemp = GetChkSum(pData + 2, m_myDataList.size() - 4);
                    SAFE_DELETE_ARRAY(pData);

                    if(chksum == chksumtemp && taildata == 0xED)
                    {
                        return TRUE;
                    }
                    else
                    {
                        //////////////////////////////////////////////Log/////////////////////////////////////
                        QString sCalculate, sReceive;
                        sCalculate.sprintf("%02X ", (byte)chksumtemp);
                        sReceive.sprintf("%02X", (byte) chksum);
                        QString strLog = QString("check sum error: calculate result: %1 while receive %2").arg(sCalculate).arg(sReceive);
                        AlphaRobotLog::getInstance()->printError(strLog);
                        ///////////////////////////////////////////////////////////////////////////////////////
                    }
                }
            }
#endif

        }
	}
	return FALSE;
}

CDataPool::CDataPool()
{
	m_pCurPkg = NULL;
}

CDataPool::~CDataPool(void)
{
	CUtil::ReleaseVector(m_vecPkgList);
}

/**************************************************************************
* 函数名: PushData
* 功能: 存储数据
* 参数: 	
*    @[in ] myData: 数据
* 返回值: 接收完毕返回指令id，否则返回-1
* 时间: 2015/07/17 17:09
* 作者: ZDJ
*/
int CDataPool::PushData(byte myData)
{
    int nRet = -1;

	if(!m_pCurPkg)
	{
		if(ExistProtocol(myData))
		{
			PKGDATA* newpkg = new PKGDATA();
			newpkg->PushData(myData);
			m_vecPkgList.push_back(newpkg);

			m_pCurPkg = newpkg;
		}
	}
	else
	{
		if(!m_pCurPkg->IsEnd())
			m_pCurPkg->PushData(myData);

        if(m_pCurPkg->IsEnd())
        {
            int nCmdId = m_pCurPkg->GetHead();
            if(m_pCurPkg->IsValid())
                nRet = nCmdId;
            else
                ClearData(nCmdId);
            m_pCurPkg = NULL;
        }
	}
    return nRet;
}

/**************************************************************************
* 函数名: GetPkg
* 功能: 获取数据包
* 参数: 
*    @[in ] cmdId: 指令id
* 返回值: 成功返回数据包，失败返回NULL
* 时间: 2015/07/20 17:12
* 作者: ZDJ
*/
PKGDATA* CDataPool::GetPkg(int cmdId)
{
	for(int i=0;i<m_vecPkgList.size();i++)
	{
		PKGDATA* pPkg = m_vecPkgList[i];
		if(pPkg)
		{
			if(pPkg->GetHead() == cmdId)
			{
				return pPkg;
			}
		}
	}
	return NULL;
}

/**************************************************************************
* 函数名: ClearData
* 功能: 清空数据包
* 参数: 
*    @[in ] cmdId: 指令id
* 时间: 2015/07/17 17:51
* 作者: ZDJ
*/
void CDataPool::ClearData(int cmdId)
{
	vector<PKGDATA*>::iterator itor = m_vecPkgList.begin();
	while(itor != m_vecPkgList.end())
	{
		PKGDATA* pPkg = *itor;
		if(pPkg)
		{				
			if(pPkg->GetHead() == cmdId)
			{
				m_vecPkgList.erase(itor);
				SAFE_DELETE(pPkg);
				break;
			}				
		}

		itor++;
	}
}
