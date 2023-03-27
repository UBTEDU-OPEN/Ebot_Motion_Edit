/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cubxudiskmanager.h
* 创建时间：2016/11/12
* 文件标识：
* 文件摘要：机器人移动盘管理
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/11/12
* 版本摘要：
*/

#include <QDir>
#include <QProcess>

#include "cubxudiskmanager.h"
#include "alpharobotlog.h"

#ifdef Q_OS_WIN
/**************************************************************************
* 函数名: GetDiskFlag
* 功能: 根据设备名称查找盘符，不带冒号
* 参数:
*    @[in ] sDeviceLink: 设备链接
* 返回值: 成功返回盘符字母，失败返回空字符串
* 时间: 2016/11/2
* 作者: ZDJ
*/
QString CUBXUdiskManager::GetDiskFlag(const QString &sDeviceLink)
{
    QString sDiskFlag;  //盘符
    QString sTestValue; //查找关键字符串

    QString strDeviceLink = sDeviceLink;
    if (strDeviceLink.isEmpty())
    {
        strDeviceLink = zxLibConstConstants::UBT_DeviceLink;
    }

    HKEY hKey = NULL;
    if(RegOpenKey(HKEY_LOCAL_MACHINE, QString("System\\CurrentControlSet\\Services\\Disk\\Enum").toStdWString().c_str(), &hKey) == ERROR_SUCCESS)
    {
        for(int i = 0; i < 1024; i++)
        {
            DWORD dwValueLen = 1024;
            WCHAR szValue[1024] = {0};

            QString sTemp;
            sTemp.sprintf("%d", i);
            DWORD dwType = REG_SZ;
            if(RegQueryValueEx(hKey, sTemp.toStdWString().c_str(), 0, &dwType, (byte*)szValue, &dwValueLen) == ERROR_SUCCESS)
            {
                if(QString::fromWCharArray(szValue).indexOf(strDeviceLink) != -1)
                {
                    sTestValue = QString::fromWCharArray(szValue);
                    sTestValue.replace('\\', '#');
                    break;
                }
            }
        }
        RegCloseKey(hKey);
    }

    if(!sTestValue.isEmpty())
    {
        if(RegOpenKey(HKEY_LOCAL_MACHINE, QString("System\\MountedDevices").toStdWString().c_str(), &hKey) == ERROR_SUCCESS)
        {
            for(int i = 0; i < 26; i++)
            {
                QString sTempDiskFlag; //临时盘符
                sTempDiskFlag.sprintf("%c", 'A' + i);

                QString sTempPath = "\\DosDevices\\" + sTempDiskFlag + ":";

                DWORD dwValueLen = 1024;
                WCHAR szValue[1024] = {0};

                DWORD dwType = REG_BINARY;
                if(RegQueryValueEx(hKey, sTempPath.toStdWString().c_str(), 0, &dwType, (byte*)szValue, &dwValueLen) == ERROR_SUCCESS)
                {
                    if(QString::fromWCharArray(szValue).indexOf(sTestValue) != -1)
                    {
                        sDiskFlag = sTempDiskFlag;
                        break;
                    }
                }
            }
            RegCloseKey(hKey);
        }
    }

    return sDiskFlag;
}
#else
bool CUBXUdiskManager::getUBTUSBDeviceByIOLib(QString &strVolumnPath, QString &strBSDPath)
{
    MyUSBDevice *pDev = MyUSBDevice::Allocate();
    kern_return_t kr = pDev->initialize();
    if(kr != kIOReturnSuccess)
    {
        printf("initialize device failed: %08x", kr);
        return false;
    }

    strVolumnPath = pDev->GetDeviceVolumePath(zxLibConstConstants::UBT_VID, zxLibConstConstants::UBT_PID); //获取设备卷路径
    strBSDPath = pDev->GetDeviceBSDPath(zxLibConstConstants::UBT_VID, zxLibConstConstants::UBT_PID); //获取设备bsd路径

    if (pDev)
    {
        pDev->Release();
    }

    return (!strVolumnPath.isEmpty() && !strBSDPath.isEmpty());
}


bool CUBXUdiskManager::getUBTUSBDeviceByCommand(QString& strVolumnPath, QString& strBSDPath)
{
    strBSDPath.clear();
    strVolumnPath.clear();

    QProcess p;
    p.start("system_profiler", QStringList()<<"SPUSBDataType");
    p.waitForStarted();
    p.waitForFinished();

    QString strInfos = QString::fromLocal8Bit(p.readAllStandardOutput());
    QString strVar;
    QStringList strDevList;
    foreach (strVar, strInfos.split("\n\n"))
    {
        if(strVar.contains("Product ID:") &&
                strVar.contains("0x5720") &&
                strVar.contains("Vendor ID:") &&
                strVar.contains("Manufacturer:") &&
                strVar.contains("UBTicroelectronics") &&
                strVar.contains("BSD Name:") &&
                strVar.contains("Mount Point:") &&
                strVar.contains("Volume UUID:") )
        {
            strDevList.append(strVar.trimmed());
        }
    }

    if(strDevList.count() == 1)
    {
        foreach (strVar, strDevList.at(0).split("\n"))
        {
            if(strBSDPath.isEmpty())
            {
                strBSDPath = getValueByLabel(strVar, "BSD Name", ":");
            }
            else if (strVolumnPath.isEmpty())
            {
                strVolumnPath = getValueByLabel(strVar, "Mount Point", ":");
            }
        }
    }

    const QString strTempStartWith = "/dev/";
    if(!strBSDPath.isEmpty() && !strBSDPath.startsWith(strTempStartWith))
    {
        strBSDPath.prepend(strTempStartWith);
    }

    return  (!strBSDPath.isEmpty() && !strVolumnPath.isEmpty());
}

QString CUBXUdiskManager::getValueByLabel(const QString& strIn, const QString& strLabel, const QString& strDelimiter)
{
    QString strValue;
    if(strIn.contains(strLabel))
    {
        QStringList strList = strIn.split(strDelimiter);
        if(strList.count() == 2)
        {
            strValue = QString(strList.at(1)).trimmed();
        }
    }

    return strValue;
}
#endif

CUBXUdiskManager::CUBXUdiskManager(QObject *parent) : QObject(parent)
{

}

CUBXUdiskManager::~CUBXUdiskManager()
{

}

QString CUBXUdiskManager::getUdiskRootPath()
{
    QString strDiskName;
    QString strBsdName;
#ifdef Q_OS_WIN
    strDiskName = GetDiskFlag();
#else
    if (!getUBTUSBDeviceByIOLib(strDiskName, strBsdName))
    {
        if (!getUBTUSBDeviceByCommand(strDiskName, strBsdName))
        {
            AlphaRobotLog::getInstance()->printError("Failed to get UBT usb device");
            return "";
        }
    }
#endif

    if (strDiskName.isEmpty())
    {
        return "";
    }
#ifdef Q_OS_WIN
    return QString("%1:\\").arg(strDiskName); // Windows系统返回H
#else
    return strDiskName;  // Mac系统返回的Volumn/NONAME
#endif
}

QString CUBXUdiskManager::getUdiskName(QString &strBsdPath)
{
#ifdef Q_OS_WIN
    return GetDiskFlag(); // Windows系统返回盘符，如H
#else
    QString strVolumnPath;
    if (!getUBTUSBDeviceByIOLib(strVolumnPath, strBsdPath))
    {
        if (!getUBTUSBDeviceByCommand(strVolumnPath, strBsdPath))
        {
            AlphaRobotLog::getInstance()->printError("Failed to get UBT usb device");
            return "";
        }
    }

    QDir dir(strVolumnPath);  // Mac系统返回Volumn/NONAME格式
    return dir.dirName();
#endif
}
