/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CCommandType.h
* 创建时间：2016/11/22
* 文件标识：
* 文件摘要：机器人命令
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/11/22
* 版本摘要：
*/

#ifndef CCOMANDTYPE_H
#define CCOMANDTYPE_H

#include <QString>
#include <QMetaType>

#include "ubxrobotcontrolconst.h"

class CCommandType
{
public:
    explicit CCommandType();
    explicit CCommandType(emRobotCmdType eCmdType);
    CCommandType(const CCommandType &item);
    virtual ~CCommandType();

public:
    /**************************************************************************
    * 函数名: getCommand
    * 功能: 获取机器人命令
    * 参数:
    *    @
    * 返回值: 机器人命令
    * 时间: 2016/11/22
    * 作者: hwx
    */
    emRobotCmdType getCommand() const { return m_eRobotCmdType; }

    QString getCmdDisplayName();

    /**************************************************************************
    * 函数名: getCommand
    * 功能: 获取机器人命令
    * 返回值: 机器人命令优先级
    * 时间: 2016/11/22
    * 作者: hwx
    */
    emRobotCmdPriority getPriority();

    /**************************************************************************
    * 函数名: getTimeoutValue
    * 功能: 获取命令对应的超时时间值
    * 返回值: 超时时间值
    * 时间: 2016/11/22
    * 作者: hwx
    */
    int getTimeoutValue();

    CCommandType &operator = (const CCommandType& item);

private:
    emRobotCmdType  m_eRobotCmdType;  // 机器人命令
};

Q_DECLARE_METATYPE(CCommandType)

#endif // CCOMANDTYPE_H
