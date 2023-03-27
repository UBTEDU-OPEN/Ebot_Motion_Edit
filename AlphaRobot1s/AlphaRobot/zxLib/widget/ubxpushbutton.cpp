/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ubxpushbutton.cpp
* 创建时间：2017.01.24
* 文件标识：
* 文件摘要：UBX定制按钮
*
* 当前版本：1.0.0.0
* 作    者：hwx
*/

#include "ubxpushbutton.h"

UBXPushButton::UBXPushButton(QWidget *parent) : QPushButton(parent)
{
    m_bSetPressInterval = false;
    m_nMsecondPressInterval = 1000;
    m_pTimerPressInterval = NULL;
    connect(this, &UBXPushButton::released, this, &UBXPushButton::onReleased);
}

UBXPushButton::~UBXPushButton()
{
    if(m_pTimerPressInterval)
    {
        delete m_pTimerPressInterval;
    }
}

void UBXPushButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit sigOnMouseDoubleClicked();
    QPushButton::mouseDoubleClickEvent(event);
}

void UBXPushButton::onReleased()
{
    QPushButton::setEnabled(false);

    if(!m_pTimerPressInterval)
    {
        m_pTimerPressInterval = new QTimer(this);
        connect(m_pTimerPressInterval, &QTimer::timeout, this, &UBXPushButton::onTimeoutInterval);
    }
    m_pTimerPressInterval->start(m_nMsecondPressInterval);
}

void UBXPushButton::onTimeoutInterval()
{
    m_pTimerPressInterval->stop();
    QPushButton::setEnabled(true);
}

int UBXPushButton::pressInterval() const
{
    return m_nMsecondPressInterval;
}

void UBXPushButton::setPressInterval(int nMsecondPressInterval)
{
    if(nMsecondPressInterval > 0)
    {
        m_nMsecondPressInterval = nMsecondPressInterval;
    }
}

void UBXPushButton::setEnabled(bool bEnable)
{
    if(m_pTimerPressInterval)
    {
        m_pTimerPressInterval->stop();
    }
    QPushButton::setEnabled(bEnable);
}

void UBXPushButton::setDisabled(bool bEnable)
{
    if(m_pTimerPressInterval)
    {
        m_pTimerPressInterval->stop();
    }
    QPushButton::setDisabled(bEnable);
}

bool UBXPushButton::isPressInterval() const
{
    return m_bSetPressInterval;
}

void UBXPushButton::enablePressInterval(bool bSetPressInterval)
{
    m_bSetPressInterval = bSetPressInterval;
}

