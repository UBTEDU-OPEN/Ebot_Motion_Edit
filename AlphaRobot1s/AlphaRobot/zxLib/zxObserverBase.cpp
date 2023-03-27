/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：zxObserverBase.cpp
* 创建时间：2015/08/24 22:18
* 文件标识：
* 文件摘要：观察者基类
*
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/24 22:18
* 版本摘要：
*/
#include "stdafx.h"
#include "zxObserverBase.h"

namespace zxLib
{

//#pragma region 观察者基类实现

zxObserverBase::zxObserverBase()
{
}

zxObserverBase::~zxObserverBase()
{
    RemoveAllSubscribe();
}

/**************************************************************************
    * 函数名: OnPreNotify
    * 功能: 预处理通知消息
    * 参数:
    *    @[in ] pSubject: 被观察者
    *    @[in ] nMsgID: 消息ID
    *    @[in ] dwContext: 消息上下文
    *    @[in ] wParam: 参数1
    *    @[in ] lParam: 参数2
    * 返回值: 返回0则继续处理消息，否则丢弃消息
    * 时间: 2015/08/22 19:09
    * 作者: zdj
    */
int zxObserverBase::OnPreNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

/**************************************************************************
    * 函数名: OnNotify
    * 功能: 通知消息
    * 参数:
    *    @[in ] pSubject: 被观察者
    *    @[in ] nMsgID: 消息ID
    *    @[in ] dwContext: 消息上下文
    *    @[in ] wParam: 参数1
    *    @[in ] lParam: 参数2
    * 返回值: void
    * 时间: 2015/08/22 19:09
    * 作者: zdj
    */
void zxObserverBase::OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{

}

/**************************************************************************
* 函数名: OnNotify
* 功能: 通知消息
* 参数:
*    @[in ] receiver 观察者
*    @[in ] pSubject: 被观察者
*    @[in ] nMsgID: 消息ID
*    @[in ] dwContext: 消息上下文
*    @[in ] wParam: 参数1
*    @[in ] lParam: 参数2
* 返回值: void
* 时间: 2015/08/22 19:09
* 作者: asu
*/
void zxObserverBase::OnNotify(QObject *pReceiver, const zxObserverSubjectBase* pSubject,
                              int eventType, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    zxEvent* pNewEvent = new zxEvent(nMsgID);
    pNewEvent->eEventType = (zxEventType)eventType;//zxEventType
    pNewEvent->lReceiver = (long)pReceiver;
    pNewEvent->dwMsgId = nMsgID;
    pNewEvent->wParam = wParam;
    pNewEvent->lParam = lParam;
    QCoreApplication::postEvent(pReceiver, pNewEvent);
}

/**************************************************************************
    * 函数名: AddSubscribe
    * 功能: 添加订阅
    * 参数:
    *    @[in ] pSubject: 被观察者
    *    @[in ] dwMsg: 消息集合，0表示任何消息
    *    @[in ] bRecv: 是否接收通知
    * 返回值: void
    * 时间: 2015/08/22 19:08
    * 作者: zdj
    */
void zxObserverBase::AddSubscribe(zxObserverSubjectBase* pSubject, DWORD dwMsg, BOOL bRecv)
{
    pSubject->RegisterObserver(this, dwMsg);
    m_mapSubscribe[pSubject] = bRecv;
}

/**************************************************************************
    * 函数名: AddSubscribe
    * 功能: 添加订阅
    * 参数:
    *    @[in ] pSubject: 被观察者
    *    @[in ] setMsg: 消息集合
    *    @[in ] bRecv: 是否接收通知
    * 返回值: void
    * 时间: 2015/08/22 19:08
    * 作者: zdj
    */
void zxObserverBase::AddSubscribe(zxObserverSubjectBase* pSubject, const SetMsgType& setMsg, BOOL bRecv)
{
    pSubject->RegisterObserver(this, setMsg);
    m_mapSubscribe[pSubject] = bRecv;
}

/**************************************************************************
    * 函数名: RemoveSubscribe
    * 功能: 移除订阅
    * 参数:
    *    @[in ] pSubject: 被观察者
    * 返回值: void
    * 时间: 2015/08/22 19:08
    * 作者: zdj
    */
void zxObserverBase::RemoveSubscribe(zxObserverSubjectBase* pSubject)
{
    MapSubscribeSubject::iterator itor = m_mapSubscribe.find(pSubject);
    if(itor != m_mapSubscribe.end())
    {
        itor->second = FALSE;
        itor->first->UnregisterObserver(this);//反注册
        m_mapSubscribe.erase(itor++);
    }
}

/**************************************************************************
    * 函数名: RemoveAllSubscribe
    * 功能: 移除所有订阅
    * 时间: 2015/08/22 19:08
    * 作者: zdj
    */
void zxObserverBase::RemoveAllSubscribe()
{
    MapSubscribeSubject::iterator itor = m_mapSubscribe.begin();
    while(itor != m_mapSubscribe.end())
    {
        itor->second = FALSE;
        itor->first->UnregisterObserver(this);//反注册
        m_mapSubscribe.erase(itor++);
    }
}

/**************************************************************************
    * 函数名: ModifySubscribe
    * 功能: 修改订阅消息
    * 参数:
    *    @[in ] pSubject: 被观察者
    *    @[in ] dwAddMsg: 添加的消息，0则不添加任何消息
    *    @[in ] dwRemoveMsg: 移除的消息，0则不移除任何消息
    * 返回值: void
    * 时间: 2015/08/24 13:13
    * 作者: ZDJ
    */
void zxObserverBase::ModifySubscribe(zxObserverSubjectBase* pSubject, DWORD dwAddMsg, DWORD dwRemoveMsg)
{
    MapSubscribeSubject::iterator itor = m_mapSubscribe.find(pSubject);
    if(itor != m_mapSubscribe.end())
    {
        if(dwAddMsg)
            itor->first->RegisterObserver(this, dwAddMsg);//注册消息
        if(dwRemoveMsg)
            itor->first->UnregisterObserver(this, dwRemoveMsg);//移除消息
    }
}

/**************************************************************************
    * 函数名: EnableRecvNotify
    * 功能: 设置是否接收通知
    * 参数:
    *    @[in ] pSubject: 被观察者
    *    @[in ] bRecv: 是否接收通知
    * 返回值: void
    * 时间: 2015/08/22 19:07
    * 作者: zdj
    */
void zxObserverBase::EnableRecvNotify(zxObserverSubjectBase* pSubject, BOOL bRecv)
{
    MapSubscribeSubject::iterator itor = m_mapSubscribe.find(pSubject);
    if(itor != m_mapSubscribe.end())
    {
        itor->second = bRecv;
    }
}

/**************************************************************************
    * 函数名: IsEnableRecvNotify
    * 功能: 是否接收通知
    * 参数:
    *    @[in ] pSubject: 被观察者
    * 返回值: 接收通知标识
    * 时间: 2015/08/22 19:07
    * 作者: zdj
    */
BOOL zxObserverBase::IsEnableRecvNotify(zxObserverSubjectBase* pSubject)
{
    MapSubscribeSubject::iterator itor = m_mapSubscribe.find(pSubject);
    if(itor != m_mapSubscribe.end())
    {
        return itor->second;
    }
    return FALSE;
}

//#pragma endregion
}
