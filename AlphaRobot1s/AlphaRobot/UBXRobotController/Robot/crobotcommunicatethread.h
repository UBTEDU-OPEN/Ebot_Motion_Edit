/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：crobotcommunicatethread.h
* 创建时间：2016/10/20
* 文件标识：
* 文件摘要：发送数据命令到机器人的线程
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/10/20
* 版本摘要：
*/

#ifndef CROBOTCOMMUNICATETHREAD_H
#define CROBOTCOMMUNICATETHREAD_H

#include <QObject>
#include <QThread>

#include "czxqueue.h"
#include "crobotcmddata.h"
#include "UBXRobotController.h"

class CRobotCommunicateThread : public QThread
{
    Q_OBJECT
public:
    explicit CRobotCommunicateThread(QObject *parent = 0);
    ~CRobotCommunicateThread();

public:
    /**************************************************************************
    * 函数名: pushData
    * 功能: 缓存数据到队列中
    * 参数:
    *    @[in ] eRobotCmd: 命令类型
    *    @[in ] pData: 数据
    *    @[in ] nLen: 数据长度
    * 返回值: 成功返回true，失败false
    * 时间: 2016/10/20
    * 作者: hwx
    */
    bool pushData(emRobotCmdType eRobotCmd, byte *pData, int nLen);

    /**************************************************************************
    * 函数名: getInsertPos
    * 功能: 获取插入数据的位置
    * 参数:
    *    @[in ] pCmdData: 命令数据
    * 返回值: 插入队列的位置
    * 时间: 2016/10/20
    * 作者: hwx
    */
    int getInsertPos(CRobotCmdData *pCmdData);

    // 丢弃所有数据
    void abortAllData();

    /**************************************************************************
    * 函数名: runThread
    * 功能: 启动线程
    * 参数:
    * 返回值:
    * 时间: 2016/10/20
    * 作者: hwx
    */
    void runThread();

protected:
    /**************************************************************************
    * 函数名: run
    * 功能:发送队列中所有数据到串口
    * 参数:
    * 返回值: void
    * 时间: 2016/10/20
    * 作者: hwx
    */
    virtual void run() override;

signals:
    /**************************************************************************
    * 函数名: sigOnSendCommand
    * 功能: 发送指定命令的信号
    * 参数:
        *    @[out ] eCmd: 机器人命令
    * 返回值: void
    * 时间: 2016/10/20
    * 作者: hwx
    */
    void sigOnSendCommand(emRobotCmdType eCmd);

public slots:

private:

private:
    CZXQueue<CRobotCmdData *> m_queue;
};

#endif // CROBOTCOMMUNICATETHREAD_H
