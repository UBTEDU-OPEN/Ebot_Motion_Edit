/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxMemPoolEx.h
* 创建时间：2015/09/07 13:59
* 文件标识：
* 文件摘要：内存池扩展类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/09/07 13:59
* 版本摘要：
*/
#ifndef ZXMEMPOOLEX_H
#define ZXMEMPOOLEX_H

namespace zxLib
{
//#pragma region 内存池扩展类声明

	class ZXLIB_DLL zxMemPoolEx : public zxMemPool
	{
	public:
		zxMemPoolEx(void);
		virtual ~zxMemPoolEx(void);

		/**************************************************************************
		* 函数名: CreateBlock
		* 功能: 创建内存块
		* 参数: 
		*    @[in ] nBufSize: 内存块缓冲区长度
		* 返回值: 返回内存块地址
		* 时间: 2015/08/28 0:18
		* 作者: zdj
		*/
		virtual zxMemPoolBlock* CreateBlock(size_t nBufSize);	

    private:
        //pthread_mutex_t   m_mutex;
        zxCriticalSection      m_crisec;          //内存块集合同步临界区
	};

//#pragma endregion

}

#endif
