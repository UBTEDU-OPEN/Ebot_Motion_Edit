/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CTimerManager
* 创建时间：20160817
* 文件标识：
* 文件摘要：the timer manager
*
* 当前版本：1.0.0.0
* 作    者：asu
*/


#include "timermanager.h"


#include <QVariant>


#include "../common/UBXPublic.h"


const QString TIMER_ID = "timerID";


CTimerManager::CTimerManager()
{

}

CTimerManager::~CTimerManager()
{
    //remove timer of map
    QMap<int, QTimer *>::const_iterator iTimer = m_mapTimer.constBegin();
    while (iTimer != m_mapTimer.constEnd())
    {
        const int nTimerID = iTimer.key();
        removeTimer(nTimerID);

        ++iTimer;
    }
}

//!!!please have connected sinTimeout before using addTimer().
bool CTimerManager::addTimer(const int &nTimerID, const int &msec)
{
    if(msec > 0)
    {
        if(existTimer(nTimerID))
        {
            return false;
        }

        QTimer *pTimer = new QTimer(this);
        m_mapTimer.insert(nTimerID , pTimer);
        pTimer->setProperty(TIMER_ID.toStdString().c_str(), QVariant(nTimerID));
        connect(pTimer, &QTimer::timeout, this, &CTimerManager::onTimeout);
        pTimer->start(msec);

        return true;
    }

    return false;
}

/**************************************************************************
* 函数名: removeTimer
* 功能: remove the timer for timer out by timerID
* 参数:
*    @[in ] nTimerID: timer id

* 返回值: 成功返回true
* 时间: 2016/08/17
* 作者: asu
*/
bool CTimerManager::removeTimer(const int &nTimerID)
{
    bool bOk = false;
    QTimer *pTimer = m_mapTimer.value(nTimerID);
    if(pTimer)
    {
        QVariant vTimerID = pTimer->property(TIMER_ID.toStdString().c_str());

        if(vTimerID.isValid())
        {
            const int nFindTimerID = vTimerID.toInt();
            if(nFindTimerID == nTimerID)
            {
                pTimer->stop();
                m_mapTimer.remove(nFindTimerID);
                SAFE_DELETE(pTimer);

                bOk = true;
            }
        }
    }

    return bOk;
}

bool CTimerManager::existTimer(const int &nTimerID)
{
    bool bOk = false;
    QTimer *pTimer = m_mapTimer.value(nTimerID);
    if(pTimer)
    {
        QVariant vTimerID = pTimer->property(TIMER_ID.toStdString().c_str());

        if(vTimerID.isValid())
        {
            const int nFindTimerID = vTimerID.toInt();
            if(nFindTimerID == nTimerID)
            {
                bOk = true;
            }
        }
    }

    return bOk;
}

//slot to timeout
void CTimerManager::onTimeout()
{
    QTimer *pTimer = dynamic_cast<QTimer*>(sender());
    if(pTimer)
    {
        pTimer->stop();
        QVariant vTimerID = pTimer->property(TIMER_ID.toStdString().c_str());

        if(vTimerID.isValid())
        {
            const int nTimerID = vTimerID.toInt();
            removeTimer(nTimerID);

            emit sigTimeout(nTimerID);
        }
    }
}
