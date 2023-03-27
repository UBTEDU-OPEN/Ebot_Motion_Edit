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


#ifndef CUBXUDISKMANAGER_H
#define CUBXUDISKMANAGER_H

#include <QObject>

#include "zxlibconst.h"

#ifdef Q_OS_MAC
#include "myusbdevice.h"
#else
#include <windows.h>
#endif

class ZXLIB_DLL CUBXUdiskManager : public QObject
{
    Q_OBJECT
public:

    // 获取移动盘目录
    static QString getUdiskRootPath();

    // 获取移动盘名称, windows系统下strBsdPath返回空
    static QString getUdiskName(QString &strBsdPath);

#ifdef Q_OS_WIN
    /**************************************************************************
    * 函数名: GetDiskFlag
    * 功能: Windows根据设备名称查找盘符，不带冒号，如H
    * 参数:
    *    @[in ] sDeviceLink: 设备链接
    * 返回值: 成功返回盘符字母，失败返回空字符串
    * 时间: 2016/11/2
    * 作者: ZDJ
    */
    static QString GetDiskFlag(const QString& sDeviceLink = ""); // 通过注册表获取
#else
    /**************************************************************************
    * 函数名: getUBTUSBDeviceByIOLib
    * 功能: Mac根据VID和PID获取UBT移动盘路径和盘符名称
    * 参数:
    *    @[in ] strVolumnPath: 挂载的位置，如Volumn/NONAME
    *    @[in ] strBSDPath: 设备BSD路径，如dev/disk2
    * 返回值: 成功返回盘符字母，失败返回空字符串
    * 时间: 2016/11/12
    * 作者: ZDJ
    */
    static bool getUBTUSBDeviceByIOLib(QString &strVolumnPath, QString &strBSDPath); // 通过调用IO库获取
    static bool getUBTUSBDeviceByCommand(QString& strVolumnPath, QString& strBSDPath); // 通过系统命令解析获取
#endif

signals:

public slots:

protected:
    explicit CUBXUdiskManager(QObject *parent = 0);
    ~CUBXUdiskManager();

private:
#ifdef Q_OS_WIN
#else
    static QString getValueByLabel(const QString& strIn, const QString& strLabel, const QString& strDelimiter = ":");
#endif
};

#endif // CUBXUDISKMANAGER_H
