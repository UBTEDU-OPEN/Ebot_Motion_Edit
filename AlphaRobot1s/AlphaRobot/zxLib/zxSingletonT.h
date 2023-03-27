/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxSingletonT.hpp
* 创建时间：2015/08/24 22:11
* 文件标识：
* 文件摘要：单例基类
* 
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/24 22:11
* 版本摘要：
*/
#ifndef ZXSINGLETONT_H
#define ZXSINGLETONT_H

#include "zxSingletonMangr.h"

namespace zxLib
{

//#pragma region 单例基类声明

    class zxSingletonMangr;

	template <class T>
	class ZXLIB_TEMPLATE zxSingletonT
	{
	protected:
		zxSingletonT(void);
		virtual ~zxSingletonT(void);

	public:
		/**************************************************************************
		* 函数名: GetInstance
		* 功能: 获取T类型的实例
		* 时间: 2015/08/22 23:29
		* 作者: zdj
		*/
		static T* GetInstance();

		/**************************************************************************
		* 函数名: Release
		* 功能: 释放T类型的实例
		* 时间: 2015/08/22 23:30
		* 作者: zdj
		*/
		static void Release();
	private:
		static T* m_pInstance; //T类型的实例
	};

//#pragma endregion

//#pragma region 单例基类实现

	template <class T>
    SELECTANY T* zxSingletonT<T>::m_pInstance = NULL;

	template <class T>
	zxSingletonT<T>::zxSingletonT(void)
	{	
	}

	template <class T>
	zxSingletonT<T>::~zxSingletonT(void)
	{
	}

	/**************************************************************************
	* 函数名: GetInstance
	* 功能: 获取T类型的实例
	* 时间: 2015/08/22 23:29
	* 作者: zdj
	*/
	template <class T>
	T* zxSingletonT<T>::GetInstance()
	{
		if(!m_pInstance)
		{
			m_pInstance = new T();
			zxSingletonMangr::RegSingletonRelease(zxSingletonT<T>::Release);//将实例的释放函数地址加入管理器
		}
		return m_pInstance;
	}

	/**************************************************************************
	* 函数名: Release
	* 功能: 释放T类型的实例
	* 时间: 2015/08/22 23:30
	* 作者: zdj
	*/
	template <class T>
	void zxSingletonT<T>::Release()
	{	
#ifndef QT_NO_DEBUG
        print_msg("0x%08X zxSingletonT<T>::Release()\n", m_pInstance);
#endif
		SAFE_DELETE(m_pInstance);
	}
//#pragma endregion

}

#endif
