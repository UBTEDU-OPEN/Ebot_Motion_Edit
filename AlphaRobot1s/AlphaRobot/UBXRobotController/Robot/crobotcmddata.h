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

#include "ubxrobotcontrolconst.h"
#include "ccommandtype.h"

#ifndef CROBOTCMDDATA_H
#define CROBOTCMDDATA_H

class CRobotCmdData
{
public:
    explicit CRobotCmdData(emRobotCmdType eRobotCmd, byte *pData, int nLen);
    ~CRobotCmdData();

public:
    /**************************************************************************
    * 函数名: getData
    * 功能: 获取数据
    * 参数:
    *    @[in ] pData: 指向数据地址的指针
    * 返回值: 数据的长度
    * 时间: 2016/10/20
    * 作者: hwx
    */
    int getData(byte **pData);

    /**************************************************************************
    * 函数名: getRobotCmdType
    * 功能: 获取指令信息
    * 参数:
    * 返回值: 指令信息
    * 时间: 2016/10/20
    * 作者: hwx
    */
    CCommandType& getRobotCmdType() { return *m_pRobotCmdType; }

    /**************************************************************************
    * 函数名: getRobotCmdPriority
    * 功能: 获取指令优先级
    * 参数:
    * 返回值: 指令优先级
    * 时间: 2016/10/20
    * 作者: hwx
    */
    emRobotCmdPriority getRobotCmdPriority();

private:
    CCommandType *m_pRobotCmdType; // 数据命令

    byte *m_pData; // 数据
    int m_nDataLen; // 数据长度
};

#endif // CROBOTCMDDATA_H
