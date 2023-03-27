/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxMemPool.h
* 创建时间：2015/08/27 21:48
* 文件标识：
* 文件摘要：内存池
* 
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/27 21:48
* 版本摘要：
*/
#ifndef ZXMEMPOOL_H
#define ZXMEMPOOL_H

#include <QObject>
#include <QTimerEvent>

namespace zxLib
{
    #define MEMBLOCK_SIZE_MIN 1024     //内存块最小1K

    //内存块状态
    enum MemPoolBlockState
	{
		eMPBStateIdle = 0, //空闲
		eMPBStateUsed,     //使用中
	};

	//内存块
	typedef struct zxMemPoolBlock
	{
		byte*              m_pBuffer;   //缓冲区地址
		size_t             m_nSize;		//缓冲区长度
		MemPoolBlockState  m_eState;	//缓冲区状态
		int64              m_nIdleTime; //空闲时间

		BOOL               m_bIsBase;   //是否是缓冲区基地址

		STRUCT_INIT(zxMemPoolBlock);
	}zxMemPoolBlock;

	//内存池统计数据
	typedef struct zxMemPoolStatData
	{
		size_t  m_nTotalSize; //内存池缓冲区的总内存
		size_t  m_nUsedSize;  //使用中的缓冲区内存
		size_t  m_nIdleSize;  //空闲的缓冲区内存
		int     m_nBlockCount;//内存块数量

		STRUCT_INIT(zxMemPoolStatData);
	}zxMemPoolStatData;

//#pragma region 内存池声明

    class ZXLIB_DLL zxMemPool
	{
	public:
		zxMemPool(void);
		virtual ~zxMemPool(void);

		/**************************************************************************
		* 函数名: InitMemPool
		* 功能: 初始化内存池
		* 参数: 
		*    @[in ] nInitSize: 初始化缓冲区长度
		*    @[in ] nMaxSize: 内存池最大长度
		*    @[in ] nIdleTimeout: 空闲超时时间，单位ms
		* 返回值: void
		* 时间: 2015/08/28 13:16
		* 作者: ZDJ
		*/
		void InitMemPool(size_t nInitSize, size_t nMaxSize, int nIdleTimeout);

		/**************************************************************************
		* 函数名: GetAllocSize
		* 功能: 获取申请的内存长度
		* 参数: 
		*    @[in ] nSize: 计划申请的内存长度
		* 返回值: 实际申请的内存长度
		* 时间: 2015/08/28 1:09
		* 作者: zdj
		*/
		size_t GetAllocSize(size_t nSize);

		/**************************************************************************
		* 函数名: Alloc
		* 功能: 申请内存
		* 参数: 
		*    @[in ] nSize: 计划申请的内存长度
		*    @[out] nAllocSize: 实际申请的内存长度
		* 返回值: 成功返回内存地址，失败返回NULL
		* 时间: 2015/08/28 0:14
		* 作者: zdj
		*/
		byte* Alloc(size_t nSize, size_t& nAllocSize);

		/**************************************************************************
		* 函数名: Recycle
		* 功能: 回收申请的内存
		* 参数: 
		*    @[in ] pBuf: 内存地址
		* 返回值: 回收的内存长度
		* 时间: 2015/08/28 0:51
		* 作者: zdj
		*/
		size_t Recycle(byte* pBuf);

		/**************************************************************************
		* 函数名: ReleasePool
		* 功能: 释放内存池
		* 时间: 2015/08/28 0:23
		* 作者: zdj
		*/
		void ReleasePool();

		/**************************************************************************
		* 函数名: NeatenPool
		* 功能: 整理内存块
		* 时间: 2015/08/27 23:00
		* 作者: zdj
		*/
		void NeatenPool();

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

		/**************************************************************************
		* 函数名: GetMemPoolStat
		* 功能: 统计内存池使用情况
		* 参数: 
		*    @[out] statdata: 统计数据
		* 返回值: void
		* 时间: 2015/08/28 9:52
		* 作者: ZDJ
		*/
		void GetMemPoolStat(zxMemPoolStatData& statdata);

		/**************************************************************************
		* 函数名: SplitBuffer
		* 功能: 拆分内存块缓冲区
		* 参数: 
		*    @[in ] pBlockUse: 要拆分的内存块
		*    @[in ] nAllocSize: 分配的缓冲区长度
		* 返回值: void
		* 时间: 2015/09/07 11:18
		* 作者: ZDJ
		*/
		void SplitBuffer(zxMemPoolBlock* pBlockUse, int nAllocSize);

protected:
#ifdef Q_OS_WIN
        /**************************************************************************
        * 函数名: ThreadNean
        * 功能: 内存整理
        * 参数:
        *    @[in ] lpParam: 线程参数
        * 返回值: 线程处理结果
        * 时间: 2016/01/08
        * 作者: ZDJ
        */
        static UINT STDCALL ThreadNean(LPVOID lpParam);
#else
        static LPVOID ThreadNean(LPVOID lpParam);
#endif
	protected:
		//set比较器，按缓冲区地址升序排列
		typedef struct stBlockComparerBuf
		{
			bool operator()(const zxMemPoolBlock* pBlock1, const zxMemPoolBlock* pBlock2)
			{
				return pBlock1->m_pBuffer < pBlock2->m_pBuffer;
			}
		}stBlockComparerBuf;

		typedef set<zxMemPoolBlock*, stBlockComparerBuf> SetMemPoolBlockType;
		SetMemPoolBlockType  m_setMemPoolBlock; //内存块集合，自动按缓冲区地址升序排列	

		size_t               m_nMemPoolMaxSize; //内存池的最大长度
		int                  m_nTimerDelay;     //计时器延时
		int                  m_nIdleTimeout;    //空闲超时时间
        //zxCriticalSection      m_crisec;          //内存块集合同步临界区
#ifdef Q_OS_WIN
        HANDLE           m_hThreadNeaton;
#else
        pthread_t            m_hThreadNeaton; //内存整理线程
#endif
        BOOL              m_bQuitNean; //退出内存整理线程
	};

//#pragma endregion
}

#endif

