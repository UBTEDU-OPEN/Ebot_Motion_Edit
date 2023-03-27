/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxSqLinkT.h
* 创建时间：2015/08/26 15:54
* 文件标识：
* 文件摘要：链表
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/08/26 15:54
* 版本摘要：
*/
#ifndef ZXSQLINK_H
#define ZXSQLINK_H

namespace zxLib
{

//#pragma region 链表声明

	//链式结点
	template <class T>
	class ZXLIB_TEMPLATE zxLinkNodeT
	{
	public:
		T            m_data;   //数据
		zxLinkNodeT* m_pNext;  //下一个数据
	};

	//链表
	template <class T>
	class ZXLIB_TEMPLATE zxSqLinkT
	{
	public:
		zxSqLinkT();

		virtual ~zxSqLinkT();

		/**************************************************************************
		* 函数名: Append
		* 功能: 新增数据结点
		* 参数: 
		*    @[in ] data: 数据
		* 返回值: 
		* 时间: 2015/08/25 22:54
		* 作者: zdj
		*/
		void Append(const T& data);

		/**************************************************************************
		* 函数名: Remove
		* 功能: 移除指定位置的结点
		* 参数: 
		*    @[in ] nIndex: 索引值
		* 返回值: void
		* 时间: 2015/08/25 23:07
		* 作者: zdj
		*/
		void Remove(int nIndex);

		/**************************************************************************
		* 函数名: GetAt
		* 功能: 获取指定位置的结点
		* 参数: 
		*    @[in ] nIndex: 索引值
		* 返回值: 指定位置的结点地址
		* 时间: 2015/08/25 23:07
		* 作者: zdj
		*/
        zxLinkNodeT<T>* GetAt(int nIndex);

		/**************************************************************************
		* 函数名: operator []
		* 功能: 获取指定位置的结点
		* 参数: 
		*    @[in ] nIndex: 索引值
		* 返回值: 指定位置的结点地址
		* 时间: 2015/08/25 23:07
		* 作者: zdj
		*/
        zxLinkNodeT<T>* operator [] (int nIndex);

	private:
		zxLinkNodeT<T>* m_pHeadNode;//头结点地址，不存储数据
	};

//#pragma endregion

//#pragma region 链表实现

	template <class T>
	zxSqLinkT<T>::zxSqLinkT()
	{
		m_pHeadNode = new zxLinkNodeT<T>();		
		m_pHeadNode->m_pNext = NULL;
	}

	template <class T>
	zxSqLinkT<T>::~zxSqLinkT()
	{
        zxLinkNodeT<T>* pTempNode = m_pHeadNode;
		while(pTempNode)
		{
            zxLinkNodeT<T>* pNextNode = pTempNode->m_pNext;
			SAFE_DELETE(pTempNode);

			pTempNode = pNextNode;
		}
	}

	/**************************************************************************
	* 函数名: Append
	* 功能: 新增数据结点
	* 参数: 
	*    @[in ] data: 数据
	* 返回值: 
	* 时间: 2015/08/25 22:54
	* 作者: zdj
	*/
	template <class T>
	void zxSqLinkT<T>::Append(const T& data)
	{
        zxLinkNodeT<T>* pNewNode = new zxLinkNodeT<T>();
		pNewNode->m_data = data;
		pNewNode->m_pNext = NULL;

        zxLinkNodeT<T>* pTempNode = m_pHeadNode;
		while(pTempNode->m_pNext)
			pTempNode = pTempNode->m_pNext;

		pTempNode->m_pNext = pNewNode;
	}

	/**************************************************************************
	* 函数名: Remove
	* 功能: 移除指定位置的结点
	* 参数: 
	*    @[in ] nIndex: 索引值
	* 返回值: void
	* 时间: 2015/08/25 23:07
	* 作者: zdj
	*/
	template <class T>
	void zxSqLinkT<T>::Remove(int nIndex)
	{
		int nCurIndex = 0;

        zxLinkNodeT<T>* pFront = m_pHeadNode;
        zxLinkNodeT<T>* pTempNode = m_pHeadNode;

		while(pTempNode->m_pNext)
		{ 		
			pFront = pTempNode;
			pTempNode = pTempNode->m_pNext;

			if(nCurIndex == nIndex)
			{
				//释放结点
				pFront->m_pNext = pTempNode->m_pNext;
				SAFE_DELETE(pTempNode);

				break;
			}
			nCurIndex++;
		}
	}

	/**************************************************************************
	* 函数名: GetAt
	* 功能: 获取指定位置的结点
	* 参数: 
	*    @[in ] nIndex: 索引值
	* 返回值: 指定位置的结点地址
	* 时间: 2015/08/25 23:07
	* 作者: zdj
	*/
	template <class T>
	zxLinkNodeT<T>* zxSqLinkT<T>::GetAt(int nIndex)
	{
		int nCurIndex = 0;

        zxLinkNodeT<T>* pTempNode = m_pHeadNode;
		while(pTempNode->m_pNext)
		{ 		
			pTempNode = pTempNode->m_pNext;

			if(nCurIndex == nIndex)
			{
				return pTempNode;
			}
			nCurIndex++;
		}

		return NULL;
	}

	/**************************************************************************
	* 函数名: operator []
	* 功能: 获取指定位置的结点
	* 参数: 
	*    @[in ] nIndex: 索引值
	* 返回值: 指定位置的结点地址
	* 时间: 2015/08/25 23:07
	* 作者: zdj
	*/
	template <class T>
	zxLinkNodeT<T>* zxSqLinkT<T>::operator [] (int nIndex)
	{
		return GetAt(nIndex);
	}


//#pragma endregion

}

#endif
