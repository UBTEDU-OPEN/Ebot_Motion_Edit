/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxObserverBaseEx.cpp
* 创建时间：2015/08/24 22:18
* 文件标识：
* 文件摘要：观察者基类扩展
* 
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/24 22:18
* 版本摘要：
*/
#include "stdafx.h"
#include "zxObserverBaseEx.h"
#include "zxObserverSubjectBaseEx.h"

namespace zxLib
{

//#pragma region 观察者基类实现

	zxObserverBaseEx::zxObserverBaseEx()
	{
		m_bIsNotify = TRUE;
	}

	zxObserverBaseEx::~zxObserverBaseEx()
	{ 	
		RemoveAllSubscribe();
	}

	/**************************************************************************
	* 函数名: OnPreNotify
	* 功能: 预处理通知消息
	* 参数: 
	*    @[in ] nMsgID: 消息ID
	*    @[in ] dwContext: 消息上下文
	*    @[in ] wParam: 参数1
	*    @[in ] lParam: 参数2
	* 返回值: 返回0则继续处理消息，否则丢弃消息
	* 时间: 2015/08/22 19:09
	* 作者: zdj
	*/
	int zxObserverBaseEx::OnPreNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}

	/**************************************************************************
	* 函数名: OnNotify
	* 功能: 通知消息
	* 参数: 
	*    @[in ] nMsgID: 消息ID
	*    @[in ] dwContext: 消息上下文
	*    @[in ] wParam: 参数1
	*    @[in ] lParam: 参数2
	* 返回值: void
	* 时间: 2015/08/22 19:09
	* 作者: zdj
	*/
	void zxObserverBaseEx::OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
	{

	}

	/**************************************************************************
	* 函数名: AddSubscribe
	* 功能: 添加订阅
	* 参数: 
	*    @[in ] dwMsg: 消息集合，0表示任何消息
	* 返回值: void
	* 时间: 2015/08/22 19:08
	* 作者: zdj
	*/
	void zxObserverBaseEx::AddSubscribe(DWORD dwMsg)
	{
		zxObserverSubjectBaseEx::RegisterObserver(this, dwMsg);
	}

	/**************************************************************************
	* 函数名: AddSubscribe
	* 功能: 添加订阅
	* 参数: 
	*    @[in ] setMsg: 消息集合
	* 返回值: void
	* 时间: 2015/08/22 19:08
	* 作者: zdj
	*/
	void zxObserverBaseEx::AddSubscribe(const SetMsgType& setMsg)
	{
		zxObserverSubjectBaseEx::RegisterObserver(this, setMsg);
	}

	/**************************************************************************
	* 函数名: RemoveSubscribe
	* 功能: 移除订阅
	* 参数: 
	*    @[in ] setMsg: 消息集合
	* 返回值: void
	* 时间: 2015/08/22 19:08
	* 作者: zdj
	*/
	void zxObserverBaseEx::RemoveSubscribe(const SetMsgType& setMsg)
	{
		zxObserverSubjectBaseEx::UnregisterObserver(this, setMsg);//反注册		
	}

	/**************************************************************************
	* 函数名: RemoveAllSubscribe
	* 功能: 移除所有订阅
	* 时间: 2015/08/22 19:08
	* 作者: zdj
	*/
	void zxObserverBaseEx::RemoveAllSubscribe()
	{
		zxObserverSubjectBaseEx::UnregisterObserver(this);//反注册		
	}

	/**************************************************************************
	* 函数名: ModifySubscribe
	* 功能: 修改订阅消息
	* 参数: 
	*    @[in ] dwAddMsg: 添加的消息，0则不添加任何消息
	*    @[in ] dwRemoveMsg: 移除的消息，0则不移除任何消息
	* 返回值: void
	* 时间: 2015/08/24 13:13
	* 作者: ZDJ
	*/
	void zxObserverBaseEx::ModifySubscribe(DWORD dwAddMsg, DWORD dwRemoveMsg)
	{
		if(dwAddMsg)
			zxObserverSubjectBaseEx::RegisterObserver(this, dwAddMsg);//注册消息
		if(dwRemoveMsg)
			zxObserverSubjectBaseEx::UnregisterObserver(this, dwRemoveMsg);//移除消息
	}

	/**************************************************************************
	* 函数名: EnableRecvNotify
	* 功能: 设置是否接收通知
	* 参数: 
	*    @[in ] bRecv: 是否接收通知
	* 返回值: void
	* 时间: 2015/08/22 19:07
	* 作者: zdj
	*/
	void zxObserverBaseEx::EnableRecvNotify(BOOL bRecv)
	{
		m_bIsNotify = bRecv;
	}

	/**************************************************************************
	* 函数名: IsEnableRecvNotify
	* 功能: 是否接收通知
	* 返回值: 接收通知标识
	* 时间: 2015/08/22 19:07
	* 作者: zdj
	*/
	BOOL zxObserverBaseEx::IsEnableRecvNotify()
	{
		return m_bIsNotify;
	}

//#pragma endregion
}
