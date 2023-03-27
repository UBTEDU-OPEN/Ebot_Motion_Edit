/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxCirCleQueueT.h
* 创建时间：2015/08/26 16:01
* 文件标识：
* 文件摘要：环形队列
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/08/26 16:01
* 版本摘要：
*/
#ifndef ZXCIRCLEQUEUE_H
#define ZXCIRCLEQUEUE_H

namespace zxLib
{
//#pragma region 环形队列

	template <class T>
	class ZXLIB_TEMPLATE zxCirCleQueueT
	{
	public:
		/**************************************************************************
		* 函数名: zxCirCleQueueT构造函数
		* 功能: 初始化
		* 参数: 
		*    @[in ] nSize: 数据个数	
		* 时间: 2015/08/25 22:20
		* 作者: zdj
		*/
		zxCirCleQueueT(size_t nSize)
		{ 		
			m_nBufferSize = nSize * sizeof(T);
			m_pBuffer = (T*)malloc(m_nBufferSize);
			m_pFront = m_pBuffer;
			m_pRear = m_pBuffer;
			m_nCount = 0;
		}

		virtual ~zxCirCleQueueT()
		{
			SAFE_FREE(m_pBuffer);
		}

		/**************************************************************************
		* 函数名: Push
		* 功能: 入队
		* 参数: 
		*    @[in ] data: 数据
		* 返回值: void
		* 时间: 2015/08/25 22:34
		* 作者: zdj
		*/
		BOOL Push(const T& data)
		{  		
			if(!IsFull())
			{
				*m_pRear = data;
				m_pRear = m_pBuffer + (m_pRear + 1 - m_pBuffer) % (m_nBufferSize / sizeof(T));
				m_nCount++;		

				return TRUE;
			}

			return FALSE;
		}

		/**************************************************************************
		* 函数名: Pop
		* 功能: 出队
		* 参数: 
		*    @[out] data: 数据
		* 返回值: void
		* 时间: 2015/08/25 22:39
		* 作者: zdj
		*/
		BOOL Pop(T& data)
		{
			if(!IsEmpty())
			{
				data = *m_pFront;
				m_pFront = m_pBuffer + (m_pFront + 1 - m_pBuffer) % (m_nBufferSize / sizeof(T));
				m_nCount--;

				return TRUE;
			}

			return FALSE;
		}

		/**************************************************************************
		* 函数名: GetHeadElem
		* 功能: 获取头部元素	
		* 返回值: 头部元素地址
		* 时间: 2015/08/25 22:40
		* 作者: zdj
		*/
		T* GetHeadElem()
		{
			if(!IsEmpty())
				return m_pFront;
			else
				return NULL;
		}

		/**************************************************************************
		* 函数名: GetLast
		* 功能: 获取缓冲区尾部地址
		* 时间: 2015/08/26 14:10
		* 作者: ZDJ
		*/
		T* GetLast()
		{
			return m_pBuffer + m_nBufferSize / sizeof(T) - 1;
		}

		/**************************************************************************
		* 函数名: GetTailElem
		* 功能: 获取尾部元素	
		* 返回值: 尾部元素地址
		* 时间: 2015/08/25 22:40
		* 作者: zdj
		*/
		T* GetTailElem()
		{
			if(!IsEmpty())
			{
				if(m_pRear > m_pBuffer)
					return m_pRear - 1;
				else
				{		
					return GetLast();
				}
			}

			return NULL;
		}

		/**************************************************************************
		* 函数名: IsFull
		* 功能: 判断队列是否已满
		* 时间: 2015/08/25 21:36
		* 作者: zdj
		*/
		BOOL IsFull()
		{
			return m_nCount * sizeof(T) >= m_nBufferSize;
		}

		/**************************************************************************
		* 函数名: IsEmpty
		* 功能: 判断队列是否已空
		* 时间: 2015/08/25 21:36
		* 作者: zdj
		*/
		BOOL IsEmpty()
		{
			return m_nCount <= 0;		
		}

		/**************************************************************************
		* 函数名: GetCount
		* 功能: 获取数据个数
		* 时间: 2015/08/25 21:55
		* 作者: zdj
		*/
		int GetCount()
		{
			return m_nCount;
		}

		/**************************************************************************
		* 函数名: print
		* 功能: 打印数据
		* 时间: 2015/08/27 20:38
		* 作者: zdj
		*/
		void print()
		{	
#ifndef QT_NO_DEBUG

			char szTip[STRING_LEN_128] = {0};
			strcpy(szTip, "datalist[");

			if(m_pFront < m_pRear)
			{
				for(T* pTemp = m_pBuffer; pTemp < m_pFront; pTemp++)
				{
					char szTemp[8] = {0};
					sprintf(szTemp, "%02d ", 0);
					strcat(szTip, szTemp);
				}

				for(T* pTemp = m_pFront; pTemp < m_pRear; pTemp++)
				{
					char szTemp[8] = {0};
					sprintf(szTemp, "%02d ", *pTemp);
					strcat(szTip, szTemp);
				}		

				for(T* pTemp = m_pRear; pTemp <= GetLast(); pTemp++)
				{
					if(pTemp < GetLast())
					{
						char szTemp[8] = {0};
						sprintf(szTemp, "%02d ", 0);
						strcat(szTip, szTemp);
					}
					else
					{
						char szTemp[8] = {0};
						sprintf(szTemp, "%02d", 0);
						strcat(szTip, szTemp);
					}
				}
			}
			else if(m_pFront > m_pRear)
			{
				for(T* pTemp = m_pBuffer; pTemp < m_pRear; pTemp++)
				{
					char szTemp[8] = {0};
					sprintf(szTemp, "%02d ", *pTemp);
					strcat(szTip, szTemp);
				}

				for(T* pTemp = m_pRear; pTemp < m_pFront; pTemp++)
				{
					char szTemp[8] = {0};
					sprintf(szTemp, "%02d ", 0);
					strcat(szTip, szTemp);
				}		

				for(T* pTemp = m_pFront; pTemp <= GetLast(); pTemp++)
				{
					if(pTemp < GetLast())
					{
						char szTemp[8] = {0};
						sprintf(szTemp, "%02d ", *pTemp);
						strcat(szTip, szTemp);
					}
					else
					{
						char szTemp[8] = {0};
						sprintf(szTemp, "%02d", *pTemp);
						strcat(szTip, szTemp);
					}
				}
			}
			else
			{
				if(m_nCount > 0)
				{
					for(T* pTemp = m_pBuffer; pTemp <= GetLast(); pTemp++)
					{
						if(pTemp < GetLast())
						{
							char szTemp[8] = {0};
							sprintf(szTemp, "%02d ", *pTemp);
							strcat(szTip, szTemp);
						}
						else
						{
							char szTemp[8] = {0};
							sprintf(szTemp, "%02d", *pTemp);
							strcat(szTip, szTemp);
						}
					}
				}
				else
				{
					for(T* pTemp = m_pBuffer; pTemp <= GetLast(); pTemp++)
					{
						if(pTemp < GetLast())
						{
							char szTemp[8] = {0};
							sprintf(szTemp, "%02d ", 0);
							strcat(szTip, szTemp);
						}
						else
						{
							char szTemp[8] = {0};
							sprintf(szTemp, "%02d", 0);
							strcat(szTip, szTemp);
						}
					}
				}
			}

			char szTemp[STRING_LEN_32] = {0};
			sprintf(szTemp, "] datacount:%d\n", m_nCount);
			strcat(szTip, szTemp);

			print_msg(szTip);
#endif

		}

	private:
		T*  m_pBuffer;     //缓冲区首地址
		int m_nBufferSize; //缓冲区字节数
		T*  m_pFront;      //头部游标
		T*  m_pRear;       //尾部游标
		int m_nCount;      //数据个数
	};

//#pragma endregion
}

#endif
