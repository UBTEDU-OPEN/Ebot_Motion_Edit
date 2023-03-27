/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CCommandType.cpp
* 创建时间：2016/11/22
* 文件标识：
* 文件摘要：机器人命令
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/11/22
* 版本摘要：
*/

#include "ccommandtype.h"

CCommandType::CCommandType()
{

}

CCommandType::CCommandType(emRobotCmdType eCmdType) : m_eRobotCmdType(eCmdType)
{

}

CCommandType::CCommandType(const CCommandType &item)
{
    m_eRobotCmdType = item.getCommand();
}

CCommandType::~CCommandType()
{

}

QString CCommandType::getCmdDisplayName()
{
    QString strCmdName = "";
    switch (m_eRobotCmdType) {
    case eRobotCmdTypeTransFilePrepare:
    {
        strCmdName = "eRobotCmdTypeTransFilePrepare";
        break;
    }
    case eRobotCmdTypeTransFileIng:
    {
        strCmdName = "eRobotCmdTypeTransFileIng";
        break;
    }
    case eRobotCmdTypeTransFileEnd:
    {
        strCmdName = "eRobotCmdTypeTransFileEnd";
        break;
    }
    case eRobotCmdTypeTransFileCancel:
    {
        strCmdName = "eRobotCmdTypeTransFileCancel";
        break;
    }
    case eRobotCmdTypeGetOffset:
    {
        strCmdName = "eRobotCmdTypeGetOffset";
        break;
    }
    case eRobotCmdTypeSetOffset:
    {
        strCmdName = "eRobotCmdTypeSetOffset";
        break;
    }
    case eRobotCmdTypeGetAngle:
    {
        strCmdName = "eRobotCmdTypeGetAngle";
        break;
    }
    case eRobotCmdTypeMove:
    {
        strCmdName = "eRobotCmdTypeMove";
        break;
    }
    case eRobotCmdTypeGetOneAngle:
    {
        strCmdName = "eRobotCmdTypeGetOneAngle";
        break;
    }
    case eRobotCmdTypeGetID:
    {
        strCmdName = "eRobotCmdTypeGetID";
        break;
    }
    case eRobotCmdTypeSetID:
    {
        strCmdName = "eRobotCmdTypeSetID";
        break;
    }
    case eRobotCmdTypeGetMotorVersion:
    {
        strCmdName = "eRobotCmdTypeGetMotorVersion";
        break;
    }
    case eRobotCmdTypeUpgradePrepare:
    {
        strCmdName = "eRobotCmdTypeUpgradePrepare";
        break;
    }
    case eRobotCmdTypeUpgradeIng:
    {
        strCmdName = "eRobotCmdTypeUpgradeIng";
        break;
    }
    case eRobotCmdTypeUpgradeEnd:
    {
        strCmdName = "eRobotCmdTypeUpgradeEnd";
        break;
    }
    case eRobotCmdTypeConnectRobot:
    {
        strCmdName = "eRobotCmdTypeConnectRobot";
        break;
    }
    case eRobotCmdTypeDisconnectRobot:
    {
        strCmdName = "eRobotCmdTypeDisconnectRobot";
        break;
    }
    case eRobotCmdTypeDebug:
    {
        strCmdName = "eRobotCmdTypeDebug";
        break;
    }
    case eRobotCmdTypeSwitchMode:
    {
        strCmdName = "eRobotCmdTypeSwitchMode";
        break;
    }
    case eRobotCmdTypeReset:
    {
        strCmdName = "eRobotCmdTypeReset";
        break;
    }
    case eRobotCmdTypeReadSN:
    {
        strCmdName = "eRobotCmdTypeReadSN";
        break;
    }
    case eRobotCmdTypeWriteSN:
    {
        strCmdName = "eRobotCmdTypeWriteSN";
        break;
    }
    case eRobotCmdTypeReadUID:
    {
        strCmdName = "eRobotCmdTypeReadUID";
        break;
    }
    case eRobotCmdTypeCheckUtf8:
    {
        strCmdName = "eRobotCmdTypeCheckUtf8";
        break;
    }
    case eRobotCmdTypeGetRobotSysEmbeddedVersion:
    {
        strCmdName = "eRobotCmdTypeGetRobotSysEmbeddedVersion";
        break;
    }
    case eRobotCmdTypeGetRobotHardwareVer:
    {
        strCmdName = "eRobotCmdTypeGetRobotHardwareVer";
        break;
    }
    case eRobotCmdTypeGetBatteryStatus:
    {
        strCmdName = "eRobotCmdTypeGetBatteryStatus";
        break;
    }
    case eRobotCmdTypeGetDeviceStatus:
    {
        strCmdName = "eRobotCmdTypeGetDeviceStatus";
        break;
    }

    case eRobotCmdTypeGetSomeResInfos:
    {
        strCmdName = "eRobotCmdTypeGetSomeResInfos";
        break;
    }
    case eRobotCmdTypeGetActionList:
    {
        strCmdName = "eRobotCmdTypeGetActionList";
        break;
    }

    case eRobotCmdTypeSetTime:
    {
        strCmdName = "eRobotCmdTypeSetTime";
        break;
    }

    case eRbootCmdTypeDeleteAction:
    {
        strCmdName = "eRbootCmdTypeDeleteAction";
        break;
    }

    default:
    {
        strCmdName = "eRobotCmdTypeUnkonwn";
        break;
    }
    }

    return strCmdName;
}

/**************************************************************************
* 函数名: getCommand
* 功能: 获取机器人命令
* 返回值: 机器人命令优先级
* 时间: 2016/11/22
* 作者: hwx
*/
emRobotCmdPriority CCommandType::getPriority()
{
    emRobotCmdPriority ePriority = eRobotCmdPriorityMiddle;
    switch (m_eRobotCmdType)
    {
        case eRobotCmdTypeTransFilePrepare:
        case eRobotCmdTypeUpgradePrepare:
        {
            ePriority = eRobotCmdPriorityLow;
            break;
        }
        case eRobotCmdTypeTransFileCancel:
        {
            ePriority = eRobotCmdPriorityHigh;
            break;
        }
        default:
        {
            ePriority = eRobotCmdPriorityMiddle;
            break;
        }
    }
    return ePriority;
}

/**************************************************************************
* 函数名: getTimeoutValue
* 功能: 获取命令对应的超时时间值
* 参数:
*    @
* 返回值: 超时时间值
* 时间: 2016/11/22
* 作者: hwx
*/
int CCommandType::getTimeoutValue()
{
    int nTimeout = TIMEOUT_300;
    switch (m_eRobotCmdType) {
    case eRobotCmdTypeTransFilePrepare:
    {
        nTimeout = TIMEOUT_CMD_TRANSFILE_PREPARE;
        break;
    }
    case eRobotCmdTypeTransFileIng:
    {
        nTimeout = TIMEOUT_CMD_TRANSFILE_ING;
        break;
    }
    case eRobotCmdTypeTransFileEnd:
    {
        nTimeout = TIMEOUT_CMD_TRANSFILE_END;
        break;
    }
    case eRobotCmdTypeTransFileCancel:
    {
        nTimeout = TIMIEOUT_CMD_TRANSFILE_CANCEL;
        break;
    }
    case eRobotCmdTypeGetOffset:
    {
        nTimeout = TIMEOUT_CMD_GET_OFFSET;
        break;
    }
    case eRobotCmdTypeSetOffset:
    {
        nTimeout = TIMEOUT_CMD_SET_OFFSET;
        break;
    }
    case eRobotCmdTypeGetAngle:
    {
        nTimeout = TIMEOUT_CMD_GET_ANGLE;
        break;
    }
    case eRobotCmdTypeMove:
    {
        nTimeout = TIMEOUT_CMD_MOVE;
        break;
    }
    case eRobotCmdTypeGetOneAngle:
    {
        nTimeout = TIMEOUT_CMD_GET_ONE_ANGLE;
        break;
    }
    case eRobotCmdTypeGetID:
    {
        nTimeout = TIMEOUT_CMD_GET_ID;
        break;
    }
    case eRobotCmdTypeSetID:
    {
        nTimeout = TIMEOUT_CMD_SET_ID;
        break;
    }
    case eRobotCmdTypeGetMotorVersion:
    {
        nTimeout = TIMEOUT_CMD_GET_MOTOR_VERSION;
        break;
    }
    case eRobotCmdTypeUpgradePrepare:
    {
        nTimeout = TIMEOUT_CMD_TRANSFILE_PREPARE;
        break;
    }
    case eRobotCmdTypeUpgradeIng:
    {
        nTimeout = TIMEOUT_CMD_TRANSFILE_ING;
        break;
    }
    case eRobotCmdTypeUpgradeEnd:
    {
        nTimeout = TIMEOUT_CMD_TRANSFILE_END;
        break;
    }
    case eRobotCmdTypeConnectRobot:
    {
        nTimeout = TIMEOUT_CMD_CONNECT_ROBOT;
        break;
    }
    case eRobotCmdTypeDisconnectRobot:
    {
        nTimeout = TIMEOUT_CMD_DISCONNECT_ROBOT;
        break;
    }
    case eRobotCmdTypeDebug:
    {
        nTimeout = TIMEOUT_CMD_DEBUG;
        break;
    }
    case eRobotCmdTypeSwitchMode:
    {
        nTimeout = TIMEOUT_CMD_SWITCH_MODE;
        break;
    }
    case eRobotCmdTypeReset:
    {
        nTimeout = TIMEOUT_CMD_RESET;
        break;
    }
    case eRobotCmdTypeReadSN:
    {
        nTimeout = TIMEOUT_CMD_READ_SN;
        break;
    }
    case eRobotCmdTypeWriteSN:
    {
        nTimeout = TIMEOUT_CMD_WRITE_SN;
        break;
    }
    case eRobotCmdTypeReadUID:
    {
        nTimeout = TIMEOUT_CMD_READ_UID;
        break;
    }
    case eRobotCmdTypeCheckUtf8:
    {
        nTimeout = TIMEOUT_CMD_CHECK_UTF8;
        break;
    }
    case eRobotCmdTypeGetRobotSysEmbeddedVersion:
    {
        nTimeout = TIMEOUT_CMD_GET_SYS_EMBEDED_VERSION;
        break;
    }
    case eRobotCmdTypeGetRobotHardwareVer:
    {
        nTimeout = TIMEOUT_CMD_GET_HARDWARE_VERSION;
        break;
    }
    case eRobotCmdTypeGetBatteryStatus:
    {
        nTimeout = TIMEOUT_CMD_GET_BATTERY_STATUS;
        break;
    }
    case eRobotCmdTypeGetDeviceStatus:
    {
        nTimeout = TIMEOUT_CMD_GET_DEVICE_STATUS;
        break;
    }

    case eRobotCmdTypeGetSomeResInfos:
    {
        nTimeout = TIMEOUT_CMD_GET_ROBOT_RES_INFOS;
        break;
    }

    case eRobotCmdTypeGetActionList:
    {
        nTimeout = TIMEOUT_CMD_GET_ROBOT_ACTION_LIST;
        break;
    }
    case eRobotCmdTypeDeleteAction:
    {
        nTimeout = TIMEOUT_CMD_DELETE_ROBOT_ACTION;
        break;
    }

    case eRobotCmdTypeWifiScan:
    {
        nTimeout = TIMEOUT_CMD_GET_ROBOT_WIFIINFO_LIST;
        break;
    }

    case eRobotCmdTypeConnectWifi:
    {
        nTimeout = TIMEOUT_CMD_ROBOT_CONNECT_WIFII;
        break;
    }

    case eRobotCmdTypeManalUpGrade:
    {
        nTimeout = TIMEOUT_CMD_ROBOT_MANAL_UPGRADE;
        break;
    }


    default:
        break;
    }

    return nTimeout;
}

CCommandType &CCommandType::operator =(const CCommandType &item)
{
    if (&item == this)
    {
        return *this;
    }

    this->m_eRobotCmdType = item.getCommand();

    return *this;
}
