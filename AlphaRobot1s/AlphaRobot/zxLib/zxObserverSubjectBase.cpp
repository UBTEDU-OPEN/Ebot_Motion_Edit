/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxObserverSubjectBase.cpp
* 创建时间：2015/08/24 22:16
* 文件标识：
* 文件摘要：被观察者基类
* 
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/24 22:16
* 版本摘要：
*/
#include "stdafx.h"
#include "zxObserverSubjectBase.h"
#include "zxObserverBase.h"

namespace zxLib
{

//#pragma region 被观察者基类实现

	/**************************************************************************
	* 函数名: RegisterObserver
	* 功能: 注册观察者
	* 参数: 
	*    @[in ] pObserver: 观察者
	*    @[in ] dwMsg: 消息集合，0表示任何消息
	* 返回值: void
	* 时间: 2015/08/22 19:13
	* 作者: zdj
	*/
	void zxObserverSubjectBase::RegisterObserver(zxObserverBase* pObserver, DWORD dwMsg)
	{
		MapObserverType::iterator itor = m_mapObserverList.find(pObserver);
		if(itor == m_mapObserverList.end())
		{
			SetMsgType setMsg;
			setMsg.insert(dwMsg);
			m_mapObserverList[pObserver] = setMsg;
		}
		else
		{	
			itor->second.insert(dwMsg);
		}
	}

	/**************************************************************************
	* 函数名: RegisterObserver
	* 功能: 注册观察者
	* 参数: 
	*    @[in ] pObserver: 观察者
	*    @[in ] setMsg: 消息集合
	* 返回值: void
	* 时间: 2015/08/22 19:13
	* 作者: zdj
	*/
	void zxObserverSubjectBase::RegisterObserver(zxObserverBase* pObserver, const SetMsgType& setMsg)
	{
		MapObserverType::iterator itor = m_mapObserverList.find(pObserver);
		if(itor == m_mapObserverList.end())
		{ 			
			m_mapObserverList[pObserver] = setMsg;
		}
		else
		{	
			SetMsgType::iterator itormsg = setMsg.begin();
			for(; itormsg != setMsg.end(); itormsg++)
				itor->second.insert(*itormsg);
		}
	}

	/**************************************************************************
	* 函数名: UnregisterObserver
	* 功能: 反注册观察者
	* 参数: 
	*    @[in ] pObserver: 观察者
	*    @[in ] dwMsg: 消息集合，0表示任何消息
	* 返回值: void
	* 时间: 2015/08/22 19:13
	* 作者: zdj
	*/
	void zxObserverSubjectBase::UnregisterObserver(zxObserverBase* pObserver, DWORD dwMsg)
	{
		MapObserverType::iterator itor = m_mapObserverList.find(pObserver);
		if(itor != m_mapObserverList.end())
		{		
			if(dwMsg == 0)
				m_mapObserverList.erase(itor);
			else
				itor->second.erase(dwMsg);
		}
	}

	/**************************************************************************
	* 函数名: UnregisterObserver
	* 功能: 反注册观察者
	* 参数: 
	*    @[in ] pObserver: 观察者
	*    @[in ] setMsg: 消息集合
	* 返回值: void
	* 时间: 2015/08/22 19:13
	* 作者: zdj
	*/
	void zxObserverSubjectBase::UnregisterObserver(zxObserverBase* pObserver, const SetMsgType& setMsg)
	{
		SetMsgType::iterator itor = setMsg.begin();
		for(; itor != setMsg.end(); itor++)
		{
			UnregisterObserver(pObserver, *itor);
		}
	}

	/**************************************************************************
	* 函数名: NotifyObservers
	* 功能: 通知所有观察者
	* 参数: 
	*    @[in ] nMsgID: 消息ID
	*    @[in ] dwContext: 消息上下文
	*    @[in ] wParam: 参数1
	*    @[in ] lParam: 参数2
	* 返回值: void
	* 时间: 2015/08/22 19:09
	* 作者: zdj
	*/
	void zxObserverSubjectBase::NotifyObservers(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
	{
		MapObserverType::iterator itor = m_mapObserverList.begin();
		for(;itor != m_mapObserverList.end();itor++)
		{
			zxObserverBase* pObserver = itor->first;
			SetMsgType setCareMsg = itor->second;	//观察者所关心的消息集合
			if(pObserver->IsEnableRecvNotify(this)) 
			{	
				int nMsgSize = setCareMsg.size();  //消息集合长度
				BOOL bFindZero = setCareMsg.find(0) != setCareMsg.end();  //查找0
				BOOL bFindMsg = setCareMsg.find(nMsgID) != setCareMsg.end(); //查找关心的消息
				if(nMsgSize == 0 || (nMsgSize == 1 && bFindZero) || (nMsgSize >= 1 && bFindMsg))					
				{
					if(pObserver->OnPreNotify(this, nMsgID, dwContext, wParam, lParam) == 0)
						pObserver->OnNotify(this, nMsgID, dwContext, wParam, lParam);
				}
			}
		}		
	}

//#pragma endregion

}
