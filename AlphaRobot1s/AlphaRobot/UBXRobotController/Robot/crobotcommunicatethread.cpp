/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：crobotcommunicatethread.cpp
* 创建时间：2016/10/20
* 文件标识：
* 文件摘要：发送数据命令到机器人的线程
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/10/20
* 版本摘要：
*/

#include "alpharobotlog.h"
#include "crobotcommunicatethread.h"

CRobotCommunicateThread::CRobotCommunicateThread(QObject *parent) :
    QThread(parent)
{
    m_queue.setCapacity(CAPACITY_SEND_QUEUE);
}

CRobotCommunicateThread::~CRobotCommunicateThread()
{
    abortAllData();
}

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
bool CRobotCommunicateThread::pushData(emRobotCmdType eRobotCmd, byte *pData, int nLen)
{
    CRobotCmdData *pCmdData = new CRobotCmdData(eRobotCmd, pData, nLen);

    // 插入队列时，需要根据优先级进行排序
    int nPos = getInsertPos(pCmdData);
    AlphaRobotLog::getInstance()->printDebug(QString("Insert cmd %1 at index: %2").arg(pCmdData->getRobotCmdType().getCmdDisplayName()).arg(nPos));
    return m_queue.syncInsert(nPos, pCmdData, PUSH_DATA_TIME_OUT);
}

/**************************************************************************
* 函数名: getInsertPos
* 功能: 获取插入数据的位置
* 参数:
*    @[in ] pCmdData: 命令数据
* 返回值: 插入队列的位置
* 时间: 2016/10/20
* 作者: hwx
*/
int CRobotCommunicateThread::getInsertPos(CRobotCmdData *pCmdData)
{
    if (!pCmdData)
    {
        return -1;
    }

    int nSize = m_queue.size();
    int nPos = nSize;
    for (int index = 0; index < nSize; index++)
    {
        CRobotCmdData *pQueueData = m_queue.at(index);
        if (!pQueueData)
        {
            continue;
        }

        if ((pCmdData->getRobotCmdPriority()) > (pQueueData->getRobotCmdPriority()))
        {
            nPos = index;
            break;
        }
    }

    return nPos;
}

void CRobotCommunicateThread::abortAllData()
{
    while (!m_queue.isEmpty())
    {
        CRobotCmdData *pCmdData = NULL;
        if (m_queue.pop(pCmdData))
        {
            delete pCmdData;
        }
        pCmdData = NULL;
    }
    m_queue.clear();
}

/**************************************************************************
* 函数名: runThread
* 功能: 启动线程
* 参数:
* 返回值:
* 时间: 2016/10/20
* 作者: hwx
*/
void CRobotCommunicateThread::runThread()
{
    if (!isRunning())
    {
        start();
    }
}

/**************************************************************************
* 函数名: run
* 功能:发送队列中所有数据到串口
* 参数:
* 返回值: void
* 时间: 2016/10/20
* 作者: hwx
*/
void CRobotCommunicateThread::run()
{
    if (!g_pRobotController)
    {
        return;
    }

    int nBusyRtyTime = 0;
    while (!m_queue.isEmpty())
    {
        if (g_pRobotController->getCurrentPerformRobotCmd() != eRobotCmdTypeNone)
        {
            nBusyRtyTime++;

            if((m_queue.size() > 2) && (nBusyRtyTime > 50))
            {
                emRobotCmdType eRobotCmdType = g_pRobotController->getCurrentPerformRobotCmd();
                if( (eRobotCmdType != eRobotCmdTypeTransFilePrepare)
                        && (eRobotCmdType != eRobotCmdTypeTransFileIng)
                        && (eRobotCmdType != eRobotCmdTypeTransFileEnd) )
                {
                    ////////////////////////////////////
                    /// for maybe lost hid data, and going the busying status. so break the bad busying status
                    g_pRobotController->setCurrentPerformRobotCmd(eRobotCmdTypeNone);
                    QString strLog = QString("\n\ncurrent type=%d,  maybe lost hid data, and going the busying status. so break the bad busying status\n\n\n").arg(eRobotCmdType);
                    qDebug()<< strLog;
                    AlphaRobotLog::getInstance()->printDebug(strLog);
                    continue;
                }
            }

            AlphaRobotLog::getInstance()->printDebug("Serial port is busy, wait");
            msleep(WAIT_SERIAL_PORT_IDLE_TIME);
            continue;
        }

        nBusyRtyTime = 0;

        CRobotCmdData *pCmdData = NULL;
        if (!m_queue.pop(pCmdData))
        {
            AlphaRobotLog::getInstance()->printError("pop data error in CThreadSendRobotCmd::run()");
            continue;
        }

        if (!pCmdData)
        {
            AlphaRobotLog::getInstance()->printError("pop null data in CThreadSendRobotCmd::run()");
            continue;
        }

        byte *pData = NULL;
        int nDatalen = pCmdData->getData(&pData);
        CCommandType oCmdType = pCmdData->getRobotCmdType();
        emRobotCmdType eCmdType = oCmdType.getCommand();
        if (nDatalen <= 0)
        {
            continue;
        }

        ////////////////////////////////Log//////////////////////
        if(g_bIsRecordLogValue)
        {
            QString strLog = QString("Send robot data: %1\n").arg(oCmdType.getCmdDisplayName());
            for (int i = 0; i < nDatalen; i++)
            {
                QString strByte;
                strByte.sprintf("%02X ", (byte)pData[i]);
                strLog += strByte;
            }
            AlphaRobotLog::getInstance()->printDebug(strLog);
        }
        ////////////////////////////////Log//////////////////////

        if(pData && (nDatalen > 0))
        {
            IMyPortInterface*  pPort = g_pRobotController->getPortInterface();         //端口实例
            if(pPort && pPort->IsOpen())
            {
                // 设置当前正在执行的操作
                g_pRobotController->sendRobotCommand(oCmdType);

                if((eCmdType == eRobotCmdTypeTransFileIng || eCmdType == eRobotCmdTypeTransFileEnd) && g_pRobotController->IsSupportNewProtocol())
                {
                    for(int i = 0; i < nDatalen; i += RobotCmdBTProtocol::m_nTransFilePackageSize)
                    {
                        pPort->SendData(pData + i, RobotCmdBTProtocol::m_nTransFilePackageSize); //每次发送64字节
                        if(g_nSmallFrameDelay > 0)
                        {
                            Sleep(g_nSmallFrameDelay);
                        }
                    }
                }
                else
                {
                    if(g_bIsRecordLogValue && (eCmdType != eRobotCmdTypeTransFileIng))
                    {
                        qDebug() << "cmd before send" << endl;
                    }

                    pPort->SendData(pData, nDatalen);

                    if(g_bIsRecordLogValue && (eCmdType != eRobotCmdTypeTransFileIng))
                    {
                        qDebug() << "cmd after send" << endl;
                    }
                }
            }
        }  // end if pData

        SAFE_DELETE(pCmdData);
    } // end while
}

