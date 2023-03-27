/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxObserverBaseEx.h
* 创建时间：2015/08/24 22:18
* 文件标识：
* 文件摘要：观察者基类扩展
* 
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/24 22:18
* 版本摘要：
*/
#ifndef ZXOBSERVERBASEEX_H
#define ZXOBSERVERBASEEX_H


namespace zxLib
{ 
	class zxObserverSubjectBaseEx;

//#pragma region 观察者基类声明

	class ZXLIB_DLL zxObserverBaseEx
	{
	public:
		zxObserverBaseEx();
		virtual ~zxObserverBaseEx();

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
		virtual int OnPreNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

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
		virtual void OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

		/**************************************************************************
		* 函数名: AddSubscribe
		* 功能: 添加订阅
		* 参数: 
		*    @[in ] dwMsg: 消息集合，0表示任何消息
		* 返回值: void
		* 时间: 2015/08/22 19:08
		* 作者: zdj
		*/
		void AddSubscribe(DWORD dwMsg = 0);

		/**************************************************************************
		* 函数名: AddSubscribe
		* 功能: 添加订阅
		* 参数: 
		*    @[in ] setMsg: 消息集合
		* 返回值: void
		* 时间: 2015/08/22 19:08
		* 作者: zdj
		*/
		void AddSubscribe(const SetMsgType& setMsg);

		/**************************************************************************
		* 函数名: RemoveSubscribe
		* 功能: 移除订阅
		* 参数: 
		*    @[in ] setMsg: 消息集合
		* 返回值: void
		* 时间: 2015/08/22 19:08
		* 作者: zdj
		*/
		void RemoveSubscribe(const SetMsgType& setMsg);

		/**************************************************************************
		* 函数名: RemoveAllSubscribe
		* 功能: 移除所有订阅
		* 时间: 2015/08/22 19:08
		* 作者: zdj
		*/
		void RemoveAllSubscribe();

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
		void ModifySubscribe(DWORD dwAddMsg, DWORD dwRemoveMsg);

		/**************************************************************************
		* 函数名: EnableRecvNotify
		* 功能: 设置是否接收通知
		* 参数: 
		*    @[in ] bRecv: 是否接收通知
		* 返回值: void
		* 时间: 2015/08/22 19:07
		* 作者: zdj
		*/
		void EnableRecvNotify(BOOL bRecv = TRUE);

		/**************************************************************************
		* 函数名: IsEnableRecvNotify
		* 功能: 是否接收通知
		* 返回值: 接收通知标识
		* 时间: 2015/08/22 19:07
		* 作者: zdj
		*/
		BOOL IsEnableRecvNotify();

	private:
		BOOL m_bIsNotify;
	};

//#pragma endregion

}

#endif
