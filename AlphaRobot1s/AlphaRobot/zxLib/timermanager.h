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

#ifndef CTIMERMANAGER_H
#define CTIMERMANAGER_H

#include <QObject>
#include <QTimer>
#include <QMap>


#include "../common/singleton.h"


class ZXLIB_DLL CTimerManager : public QObject
{
    Q_OBJECT

public:
    SINGLETON(CTimerManager)

public:
    //!!!please have connected sinTimeout before using addTimer().
    bool addTimer(const int& nTimerID, const int& msec);

    /**************************************************************************
    * 函数名: removeTimer
    * 功能: remove the timer for timer out by timerID
    * 参数:
    *    @[in ] nTimerID: timer id

    * 返回值: 成功返回true
    * 时间: 2016/08/17
    * 作者: asu
    */
    bool removeTimer(const int& nTimerID);

    bool existTimer(const int& nTimerID);

signals:
    void sigTimeout(int nTimerID);


private slots:
    void onTimeout();

private:
    QMap<int, QTimer *> m_mapTimer;
};

#endif // CTIMERMANAGER_H
