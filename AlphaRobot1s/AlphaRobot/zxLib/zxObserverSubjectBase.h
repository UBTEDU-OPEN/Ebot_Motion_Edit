/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxObserverSubjectBase.h
* 创建时间：2015/08/24 22:16
* 文件标识：
* 文件摘要：被观察者基类
* 
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/24 22:16
* 版本摘要：
*/
#ifndef ZXOBSERVERSUBJECTBASE_H
#define ZXOBSERVERSUBJECTBASE_H

namespace zxLib
{
	class zxObserverBase;

	typedef set<DWORD>  SetMsgType;   //消息集合类型

//#pragma region 被观察者基类声明

	class ZXLIB_DLL zxObserverSubjectBase
	{
	public:

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
		virtual void RegisterObserver(zxObserverBase* pObserver, DWORD dwMsg = 0);

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
		virtual void RegisterObserver(zxObserverBase* pObserver, const SetMsgType& setMsg);

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
		virtual void UnregisterObserver(zxObserverBase* pObserver, DWORD dwMsg = 0);

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
		virtual void UnregisterObserver(zxObserverBase* pObserver, const SetMsgType& setMsg);

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
		virtual void NotifyObservers(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

	private:
		typedef map<zxObserverBase*, SetMsgType> MapObserverType;// <观察者，消息集合>
		MapObserverType m_mapObserverList;  //观察者集合	
	};


//#pragma endregion

}

#endif
