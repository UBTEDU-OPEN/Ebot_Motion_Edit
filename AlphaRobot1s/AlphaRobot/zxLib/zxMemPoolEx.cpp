/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxMemPoolEx.cpp
* 创建时间：2015/09/07 13:59
* 文件标识：
* 文件摘要：内存池扩展类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/09/07 13:59
* 版本摘要：
*/
#include "StdAfx.h"
#include "zxMemPoolEx.h"
namespace zxLib
{
//#pragma region 内存池扩展类实现    

	zxMemPoolEx::zxMemPoolEx(void)
	{
        //pthread_mutex_init(&m_mutex, NULL);
	}

	zxMemPoolEx::~zxMemPoolEx(void)
	{
        //pthread_mutex_destroy(&m_mutex);
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
	zxMemPoolBlock* zxMemPoolEx::CreateBlock(size_t nBufSize)
	{
        m_crisec.Lock();

		zxMemPoolBlock* pRet = NULL;
		byte* pNewBuffer = NULL;
		BOOL bIsBase = TRUE;//是否是缓冲区基地址

		if(m_setMemPoolBlock.size() <= 0)
		{
			pNewBuffer = (byte*)malloc(nBufSize);
			bIsBase = TRUE;
		}
		else
		{		
			SetMemPoolBlockType::iterator itor = m_setMemPoolBlock.begin();
			zxMemPoolBlock* pFirstBlock = *itor;//首内存块
			int nTotalSize = 0;//内存块总长度

			while(itor != m_setMemPoolBlock.end())
			{
				nTotalSize += (*itor)->m_nSize;
				itor++;
			}

			pNewBuffer = (byte*)realloc(pFirstBlock->m_pBuffer, nTotalSize + nBufSize);//扩展缓冲区
			if(pNewBuffer)
			{
				pFirstBlock->m_pBuffer = pNewBuffer;//修改原缓冲区地址
				pNewBuffer = pFirstBlock->m_pBuffer + nTotalSize;//新缓冲区地址

				SetMemPoolBlockType::iterator itorFront = m_setMemPoolBlock.begin();
				SetMemPoolBlockType::iterator itorRear = itorFront;

				if(m_setMemPoolBlock.size() > 1)
				{
					itorRear++;

					while(itorRear != m_setMemPoolBlock.end() && !((*itorRear)->m_bIsBase))
					{				
						(*itorRear)->m_pBuffer = (*itorFront)->m_pBuffer + (*itorFront)->m_nSize;//重新设置内存地址
						itorFront++;
						itorRear++;
					}
				}
			}

			bIsBase = FALSE;
		}

		if(pNewBuffer)
		{
			zxMemPoolBlock* pNewBlock = new zxMemPoolBlock;
			pNewBlock->m_nSize = nBufSize;
			pNewBlock->m_pBuffer = pNewBuffer;
			pNewBlock->m_eState = eMPBStateIdle;
			pNewBlock->m_nIdleTime = 0;
			pNewBlock->m_bIsBase = bIsBase;

			m_setMemPoolBlock.insert(pNewBlock);

			pRet = pNewBlock;
		}

        m_crisec.Unlock();
		return pRet;
	}

//#pragma endregion
}
