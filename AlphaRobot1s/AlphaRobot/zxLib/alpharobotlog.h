/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：alpharobotlog.h
* 创建时间：2016/06/07
* 文件标识：
* 文件摘要：参数设置控件
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/06/07
* 版本摘要：
*/

#ifndef ALPHAROBOTLOG_H
#define ALPHAROBOTLOG_H

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

const QString LOG_ENABLE_KEYWORD = "Log";

// Log文件的路径，调试时可以根据路径打开该log文件查看
//  Mac系统: user/library/application support/Alpharobot/Log
//  windows系统: QCoreApplication::applicationDirPath() + "/Log"
class ZXLIB_DLL AlphaRobotLog
{
public:

    /************************************
    * 名称: getInstance
    * 功能: 获取单例
    * 参数：
    * 返回:   AlphaRobotLog *：log对象
    * 时间:   2016/06/15
    * 作者:   hwx
    ************************************/
    static AlphaRobotLog *getInstance();

    /************************************
    * 名称: printDebug
    * 功能: 打印Debug信息
    * 参数：
    * 返回:
    * 时间:   2016/06/15
    * 作者:   hwx
    ************************************/
    void printDebug(const QString &strDebug);

    /************************************
    * 名称: printInfo
    * 功能: 打印info信息
    * 参数：
    * 返回:
    * 时间:   2016/06/15
    * 作者:   hwx
    ************************************/
    void printInfo(const QString &strInfo);

    /************************************
    * 名称: printWarning
    * 功能: 打印warning信息
    * 参数：
    * 返回:
    * 时间:   2016/06/15
    * 作者:   hwx
    ************************************/
    void printWarning(const QString &strWarning);

    /************************************
    * 名称: printError
    * 功能: 打印Error信息
    * 参数：
    * 返回:
    * 时间:   2016/06/15
    * 作者:   hwx
    ************************************/
    void printError(const QString &strError);

private:
    explicit AlphaRobotLog(QObject *pParent = NULL);
    ~AlphaRobotLog();

private:
    void printToLog(const QString &strLog, const QString &strLogType);

    // 读取配置文件判断是否开启log开关
//    bool isLogEnable();

private:
    QString m_strDateTimeFormat;  // 日期时间格式

    bool  m_bEnableLog;  // 开关，debug模式下才会进行操作，发布时可以将其设置为false

    QFile m_Logfile;  // log文件
    QTextStream m_OutputStream;  // 打印文件数据流
    QMutex m_Mutex; // 互斥锁
};

#endif // ALPHAROBOTLOG

