/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxSingletonMangr.h
* 创建时间：2015/08/24 22:13
* 文件标识：
* 文件摘要：单例实例管理类
* 
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/24 22:13
* 版本摘要：
*/
#ifndef ZXSINGLETONMANGR_H
#define ZXSINGLETONMANGR_H

namespace zxLib
{

//#pragma region 单例实例管理类声明

	class ZXLIB_DLL zxSingletonMangr
	{
	private:
		zxSingletonMangr();
		~zxSingletonMangr();
	public:
		/**************************************************************************
		* 函数名: RegSingletonRelease
		* 功能: 注册单例类的释放函数
		* 参数: 
		*    @[in ] pFunc: 释放函数
		* 返回值: void
		* 时间: 2015/08/22 23:31
		* 作者: zdj
		*/
		static void RegSingletonRelease(PVOIDFUNC pFunc);

		/**************************************************************************
		* 函数名: UnregSingletonRelease
		* 功能: 反注册单例类的释放函数
		* 参数: 
		*    @[in ] pFunc: 释放函数
		* 返回值: void
		* 时间: 2015/08/22 23:31
		* 作者: zdj
		*/
		static void UnregSingletonRelease(PVOIDFUNC pFunc);

		/**************************************************************************
		* 函数名: ReleaseSingletons
		* 功能: 释放所有的单例类实例
		* 时间: 2015/08/22 23:33
		* 作者: zdj
		*/
		static void ReleaseSingletons();
	private:
		static set<PVOIDFUNC> m_setSingletonRelease;//所有单例实例释放函数地址的集合
	}; 
//#pragma endregion

}

#endif
