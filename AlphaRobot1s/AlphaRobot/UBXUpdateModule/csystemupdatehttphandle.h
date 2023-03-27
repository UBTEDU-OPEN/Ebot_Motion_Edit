#ifndef CSYSTEMUPDATEHTTPHANDLE_H
#define CSYSTEMUPDATEHTTPHANDLE_H

/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CSystemUpdateHttpHandle
* 创建时间：2017/07/23 16:17
* 文件标识：
* 文件摘要：更新嵌入式版本, only for alpha 1e robot update
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：2017/07/23 16:17
* 版本摘要：
*/


#include <QString>


#include "global.h"
#include "ciniconfig.h"


#define STR_SYS_UPDATE_ROBOT_ROBOTTYPE  "Alpha1E"
#define STR_SYS_UPDATE_ROBOT_ROBOTID  "NBA19123ECC7A26A341B2E51ACC08AB2"



class CSystemUpdateHttpHandle
{
public:
    CSystemUpdateHttpHandle();
    ~CSystemUpdateHttpHandle();

public:
    /**************************************************************************
    * 函数名: CompareVersion
    * 功能: 比较版本
    * 参数:
    *    @[in ] sLocalVer: 本地版本，版本号如1.0.0.1
    *    @[in ] sUpdateVer: 更新版本，版本号如1.0.0.1
    *    @[in ] chktype: 更新类型
    * 返回值: 更新版本较新返回1，更新版本较旧返回-1，相等返回0
    * 时间: 2017/07/18 9:49
    * 作者: asu
    */
    static int compareVersion(const QString &sLocalVer, const QString &sUpdateVer);

    /**************************************************************************
    * 函数名: getOnlyNumVersion
    * 功能:   alphaV1.0.0.1 => 1.0.0.1
    * 参数:
    *    @[in ] sLocalVer: 本地版本，版本号如1.0.0.1
    * 返回值:
    * 时间: 2017/07/18 9:49
    * 作者: asu
    */
    static QString getOnlyNumVersion(const QString &sLocalVer);

public:
    /**************************************************************************
    * 函数名: checkUpdate
    * 功能: 检查更新
    * 参数:
    * 	@[in ] sLocalVer: 本地机器人版本
    * 返回值: 检测到新版本返回1， 未检测到新版本返回0，出错返回2
    * 时间: 2015/05/15 9:32
    * 作者: ZDJ
    */
    static int checkUpdate(const QString &sLocalVer, FILEINFO& fileinfo);
    static QString getUpdateToken();

public:
    //for robot 1e
    static QString getUrlRobotUpdateLoad();
    static QString getUrlRobotUpdateToken();

    static QString getUrlParamsRobotUpdateLoad(const QString& strUpdateToken, const QString& strCurrentVersion,
                                               const QString& strRobotID, const QString& strRobotType);
    static QString getUrlParamsRobotUpdateToken();

    //parse result
    static QString getUpdateTokenFromRes(const QString& strResJsonToken);
    static QString getVersionFromRes(const QString& strResJsonToken);
};

#endif // CSYSTEMUPDATEHTTPHANDLE_H
