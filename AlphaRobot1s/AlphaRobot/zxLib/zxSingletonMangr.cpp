/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxSingletonMangr.cpp
* 创建时间：2015/08/24 22:13
* 文件标识：
* 文件摘要：单例实例管理类
* 
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/24 22:13
* 版本摘要：
*/
#include "stdafx.h"
#include "zxSingletonMangr.h"

namespace zxLib
{

//#pragma region 单例实例管理类实现

	set<PVOIDFUNC> zxSingletonMangr::m_setSingletonRelease = set<PVOIDFUNC>();

	zxSingletonMangr::zxSingletonMangr()
	{
	}
	zxSingletonMangr::~zxSingletonMangr()
	{
	}

	/**************************************************************************
	* 函数名: RegSingletonRelease
	* 功能: 注册单例类的释放函数
	* 参数: 
	*    @[in ] pFunc: 释放函数
	* 返回值: void
	* 时间: 2015/08/22 23:31
	* 作者: zdj
	*/
	void zxSingletonMangr::RegSingletonRelease(PVOIDFUNC pFunc)
	{
		if(m_setSingletonRelease.find(pFunc) == m_setSingletonRelease.end())
			m_setSingletonRelease.insert(pFunc);
	}

	/**************************************************************************
	* 函数名: UnregSingletonRelease
	* 功能: 反注册单例类的释放函数
	* 参数: 
	*    @[in ] pFunc: 释放函数
	* 返回值: void
	* 时间: 2015/08/22 23:31
	* 作者: zdj
	*/
	void zxSingletonMangr::UnregSingletonRelease(PVOIDFUNC pFunc)
	{
		if(m_setSingletonRelease.find(pFunc) != m_setSingletonRelease.end())
		{		
			m_setSingletonRelease.erase(pFunc);
		}
	}

	/**************************************************************************
	* 函数名: ReleaseSingletons
	* 功能: 释放所有的单例类实例
	* 时间: 2015/08/22 23:33
	* 作者: zdj
	*/
	void zxSingletonMangr::ReleaseSingletons()
	{
		set<PVOIDFUNC>::iterator itor = m_setSingletonRelease.begin();
        while(itor != m_setSingletonRelease.end())
		{		
			(*itor)();//执行释放函数
            m_setSingletonRelease.erase(itor++);
		}
	}

//#pragma endregion

}
