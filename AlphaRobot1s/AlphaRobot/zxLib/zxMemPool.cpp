/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxMemPool.cpp
* 创建时间：2015/08/27 21:48
* 文件标识：
* 文件摘要：内存池
* 
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/27 21:48
* 版本摘要：
*/
#include "StdAfx.h"
#include "zxMemPool.h" 

namespace zxLib
{    
//#pragma region 内存池实现
    static zxCriticalSection      m_crisec;          //内存块集合同步临界区

	zxMemPool::zxMemPool(void)
	{	
		m_nMemPoolMaxSize = 0;
		m_nTimerDelay = 1000;
		m_nIdleTimeout = 10000;
#ifdef Q_OS_WIN
        //m_nTimerId = timeSetEvent(m_nTimerDelay, 1, TimerCbk, (DWORD_PTR)this, TIME_PERIODIC);
        m_hThreadNeaton = (HANDLE)_beginthreadex(NULL, 0, ThreadNean, this, 0, NULL);
#else
        //pthread_mutex_init(&m_mutex, NULL);
        //m_nTimerId = startTimer(m_nTimerDelay);
        m_bQuitNean = FALSE;
        pthread_create(&m_hThreadNeaton, NULL, ThreadNean, this);
#endif
	}

	zxMemPool::~zxMemPool(void)
	{
        m_bQuitNean = TRUE;

#ifdef Q_OS_WIN
        WaitForSingleObject(m_hThreadNeaton, INFINITE);
        SAFE_CLOSEHANDLE(m_hThreadNeaton);
#else
        LPVOID status = 0;
        pthread_join(m_hThreadNeaton, &status);
        m_hThreadNeaton = NULL;
#endif

		ReleasePool();
	}

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
	void zxMemPool::InitMemPool(size_t nInitSize, size_t nMaxSize, int nIdleTimeout)
	{
		m_nIdleTimeout = nIdleTimeout;
		m_nMemPoolMaxSize = GetAllocSize(nMaxSize);

		CreateBlock(GetAllocSize(nInitSize));	
	}

	/**************************************************************************
	* 函数名: GetAllocSize
	* 功能: 获取申请的内存长度
	* 参数: 
	*    @[in ] nSize: 计划申请的内存长度
	* 返回值: 实际申请的内存长度
	* 时间: 2015/08/28 1:09
	* 作者: zdj
	*/
	size_t zxMemPool::GetAllocSize(size_t nSize)
	{
        return ((nSize - 1) / (MEMBLOCK_SIZE_MIN) + 1) * MEMBLOCK_SIZE_MIN;
	}
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
	byte* zxMemPool::Alloc(size_t nSize, size_t& nAllocSize)
	{ 	
		//NeatenPool();//整理内存
        m_crisec.Lock();
		byte* pRet = NULL;

		nAllocSize = GetAllocSize(nSize);//实际要申请的内存长度
		SetMemPoolBlockType::iterator itor = m_setMemPoolBlock.begin();	

		int nCount = 0;
		zxMemPoolBlock* pBlockUse = NULL;

		while(itor != m_setMemPoolBlock.end())
		{
			zxMemPoolBlock* pBlock = *itor;
			if(pBlock->m_eState == eMPBStateIdle && pBlock->m_nSize >= nAllocSize)
			{
				nCount++;
				if(nCount <= 1)
				{				
					pBlockUse = pBlock;
				}
				else
				{
					if(pBlock->m_nSize < pBlockUse->m_nSize)
					{					
						pBlockUse = pBlock;//使用最小的合适空闲块
					}
				}			
			}

			itor++;
		}

		if(pBlockUse)
		{
#if 1
			//拆分
			if(pBlockUse->m_nSize > nAllocSize && (pBlockUse->m_nSize - nAllocSize) % MEMBLOCK_SIZE_MIN == 0)
			{
				//print_msg("cut two buffer\n");			

				SplitBuffer(pBlockUse, nAllocSize);
			}
#endif
			pRet = pBlockUse->m_pBuffer;
			pBlockUse->m_eState = eMPBStateUsed;
			pBlockUse->m_nIdleTime = 0;
		}

		if(!pRet)
		{
			//没找到合适的内存块
			//print_msg("can't find a suitable block, try to add a block\n");

			zxMemPoolStatData statdata;
			GetMemPoolStat(statdata);
			if(statdata.m_nTotalSize >= m_nMemPoolMaxSize)
			{
				//print_msg("no idle memory to alloc\n");
			}
			else
			{
				zxMemPoolBlock* pBlock = CreateBlock(nAllocSize);//MEMBLOCK_SIZE_MIN的倍数
				if(pBlock)
					pRet = pBlock->m_pBuffer;
			}
		}

		if(!pRet)
		{  
#ifndef QT_NO_DEBUG
            print_msg("alloc failed\n");
#endif
		}

        m_crisec.Unlock();
		return pRet;
	}

	/**************************************************************************
	* 函数名: Recycle
	* 功能: 回收申请的内存
	* 参数: 
	*    @[in ] pBuf: 内存地址
	* 返回值: 回收的内存长度
	* 时间: 2015/08/28 0:51
	* 作者: zdj
	*/
	size_t zxMemPool::Recycle(byte* pBuf)
	{
		m_crisec.Lock();
		size_t nRet = 0;

		SetMemPoolBlockType::iterator itor = m_setMemPoolBlock.begin();
		while(itor != m_setMemPoolBlock.end())
		{
			zxMemPoolBlock* pBlock = *itor;
			if(pBlock->m_pBuffer == pBuf)
			{
				pBlock->m_eState = eMPBStateIdle;	
				pBlock->m_nIdleTime = 0;

				nRet = pBlock->m_nSize;
				break;
			}

			itor++;
		} 

		m_crisec.Unlock();
		//NeatenPool();//整理内存
		return nRet;
	}

	/**************************************************************************
	* 函数名: ReleasePool
	* 功能: 释放内存池
	* 时间: 2015/08/28 0:23
	* 作者: zdj
	*/
	void zxMemPool::ReleasePool()
	{
        m_crisec.Lock();

		SetMemPoolBlockType::iterator itor = m_setMemPoolBlock.begin();
        while(itor != m_setMemPoolBlock.end())
		{
			zxMemPoolBlock* pBlock = *itor;

			if(pBlock->m_bIsBase)
				SAFE_FREE(pBlock->m_pBuffer);
            SAFE_DELETE(pBlock);

            //itor = m_setMemPoolBlock.erase(itor);
            m_setMemPoolBlock.erase(itor++);
		}

        m_crisec.Unlock();
	}

	/**************************************************************************
	* 函数名: NeatenPool
	* 功能: 整理内存块
	* 时间: 2015/08/27 23:00
	* 作者: zdj
	*/
	void zxMemPool::NeatenPool()
	{
        m_crisec.Lock();

		if(m_setMemPoolBlock.size() > 0)
		{
			SetMemPoolBlockType::iterator itorfront = m_setMemPoolBlock.begin();
			SetMemPoolBlockType::iterator itorrear = itorfront;
			itorrear++;

			while(itorrear != m_setMemPoolBlock.end())
			{
				zxMemPoolBlock* pBlockFront = *itorfront;
				zxMemPoolBlock* pBlockRear = *itorrear;

				if(pBlockFront->m_eState == eMPBStateIdle && pBlockRear->m_eState == eMPBStateIdle
					&& (pBlockFront->m_pBuffer + pBlockFront->m_nSize == pBlockRear->m_pBuffer)
					&& !pBlockRear->m_bIsBase)
				{
					//找到相邻的两个空闲块
                    //print_msg("find two adjacent blocks\n");
					pBlockFront->m_nSize += pBlockRear->m_nSize;//扩展前内存块的长度
					pBlockFront->m_nIdleTime = 0;//合并后空闲时间置0

					SAFE_DELETE(pBlockRear);//释放块
                    //itorrear = m_setMemPoolBlock.erase(itorrear); //删除后内存块对象
                    m_setMemPoolBlock.erase(itorrear++); //删除后内存块对象
				}
				else
				{
					itorfront++;
					itorrear++;
				}
			}
		}

        m_crisec.Unlock();
	}

	/**************************************************************************
	* 函数名: CreateBlock
	* 功能: 创建内存块
	* 参数: 
	*    @[in ] nBufSize: 内存块缓冲区长度
	* 返回值: 返回内存块地址
	* 时间: 2015/08/28 0:18
	* 作者: zdj
	*/
	zxMemPoolBlock* zxMemPool::CreateBlock(size_t nBufSize)
	{
        //m_crisec.Lock();

		zxMemPoolBlock* pRet = NULL;
		byte* pNewBuffer = (byte*)malloc(nBufSize);
		if(pNewBuffer)
		{
			zxMemPoolBlock* pNewBlock = new zxMemPoolBlock;
			pNewBlock->m_nSize = nBufSize;
			pNewBlock->m_pBuffer = pNewBuffer;
			pNewBlock->m_eState = eMPBStateIdle;
			pNewBlock->m_nIdleTime = 0;
			pNewBlock->m_bIsBase = TRUE;

			m_setMemPoolBlock.insert(pNewBlock);

			pRet = pNewBlock;
		}

        //m_crisec.Unlock();
		return pRet;
	}

	/**************************************************************************
	* 函数名: GetMemPoolStat
	* 功能: 统计内存池使用情况
	* 参数: 
	*    @[out] statdata: 统计数据
	* 返回值: void
	* 时间: 2015/08/28 9:52
	* 作者: ZDJ
	*/
	void zxMemPool::GetMemPoolStat(zxMemPoolStatData& statdata)
	{	
        //m_crisec.Lock();

		SetMemPoolBlockType::iterator itor = m_setMemPoolBlock.begin();
		for(; itor != m_setMemPoolBlock.end(); itor++)
		{
			zxMemPoolBlock* pBlock = *itor;
			statdata.m_nTotalSize += pBlock->m_nSize;

			if(pBlock->m_eState == eMPBStateIdle)
				statdata.m_nIdleSize += pBlock->m_nSize;
			else if(pBlock->m_eState == eMPBStateUsed)
				statdata.m_nUsedSize += pBlock->m_nSize;
		}

		statdata.m_nBlockCount = m_setMemPoolBlock.size();

        //m_crisec.Unlock();
	}

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
    UINT STDCALL zxMemPool::ThreadNean(LPVOID lpParam)
#else
    LPVOID zxMemPool::ThreadNean(LPVOID lpParam)
#endif
	{
        zxMemPool* pMemPool = (zxMemPool*)lpParam;
        while(pMemPool)
		{
            if(pMemPool->m_bQuitNean)
                break;

			pMemPool->NeatenPool();//整理内存

            m_crisec.Lock();

			SetMemPoolBlockType::iterator itor = pMemPool->m_setMemPoolBlock.begin();
			while(itor != pMemPool->m_setMemPoolBlock.end())
			{
				zxMemPoolBlock* pBlock = *itor;

				if(pBlock->m_eState == eMPBStateIdle)
				{
					pBlock->m_nIdleTime += pMemPool->m_nTimerDelay;

					//空闲超时的块将被释放
					if(pBlock->m_nIdleTime >= pMemPool->m_nIdleTimeout)
					{
						if(pBlock->m_bIsBase)
						{
							BOOL bRelease = FALSE;//是否释放内存块

							if(pMemPool->m_setMemPoolBlock.size() <= 1)
							{
								bRelease = TRUE;//只有一个内存块时释放此块
							}
							else
							{
								SetMemPoolBlockType::iterator itorRear = itor;
								itorRear++;
								if(itorRear != pMemPool->m_setMemPoolBlock.end() && (*itorRear)->m_bIsBase)
								{
									bRelease = TRUE;//后继内存块缓冲区为动态申请时释放此块								
								}
							}

							if(bRelease)
							{
								SAFE_FREE(pBlock->m_pBuffer);
								SAFE_DELETE(pBlock);
                                pMemPool->m_setMemPoolBlock.erase(itor++);
								continue;
							}
						}
					}
				}

				itor++;
			}

            m_crisec.Unlock();

            Sleep(1000);
		}

        return 0;
	}

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
	void zxMemPool::SplitBuffer(zxMemPoolBlock* pBlockUse, int nAllocSize)
	{	
		zxMemPoolBlock* pNewBlock = new zxMemPoolBlock;
		if(pNewBlock)
		{
			pNewBlock->m_eState = eMPBStateIdle;
			pNewBlock->m_nIdleTime = 0;
			pNewBlock->m_nSize = pBlockUse->m_nSize - nAllocSize;
			pNewBlock->m_pBuffer = pBlockUse->m_pBuffer + nAllocSize;
			pNewBlock->m_bIsBase = FALSE;

			m_setMemPoolBlock.insert(pNewBlock); 

			pBlockUse->m_nSize = nAllocSize;
		}	
	}

//#pragma endregion
}
