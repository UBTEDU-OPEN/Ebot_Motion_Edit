/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：crobotcmddata.h
* 创建时间：2016/10/20
* 文件标识：
* 文件摘要：抽象后发送给机器人的命令数据
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/10/20
* 版本摘要：
*/

#include "crobotcmddata.h"
#include "alpharobotlog.h"

CRobotCmdData::CRobotCmdData(emRobotCmdType eRobotCmd, byte *pData, int nLen)
{
    m_pRobotCmdType = new CCommandType(eRobotCmd);
    m_nDataLen = nLen;
    m_pData = new byte[m_nDataLen];
    memset(m_pData, 0, m_nDataLen);
    memcpy(m_pData, pData, m_nDataLen);
}

CRobotCmdData::~CRobotCmdData()
{
    if (m_pData)
    {
        delete m_pData;
    }
    m_pData = NULL;

    if (m_pRobotCmdType)
    {
        delete m_pRobotCmdType;
    }
    m_pRobotCmdType = NULL;
}

/**************************************************************************
* 函数名: getData
* 功能: 获取数据
* 参数:
*    @[in ] pData: 指向数据地址的指针
* 返回值: 数据的长度
* 时间: 2016/10/20
* 作者: hwx
*/
int CRobotCmdData::getData(byte **pData)
{
    if (m_nDataLen <= 0)
    {
        return 0;
    }

    *pData = m_pData;
    return m_nDataLen;
}

/**************************************************************************
* 函数名: getRobotCmdPriority
* 功能: 获取指令优先级
* 参数:
* 返回值: 指令优先级
* 时间: 2016/10/20
* 作者: hwx
*/
emRobotCmdPriority CRobotCmdData::getRobotCmdPriority()
{
    if (!m_pRobotCmdType)
    {
        return eRobotCmdPriorityMiddle;
    }

    m_pRobotCmdType->getPriority();
}
