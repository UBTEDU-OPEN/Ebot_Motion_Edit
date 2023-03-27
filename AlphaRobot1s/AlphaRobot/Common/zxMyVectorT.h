/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：zxMyVectorT.h
* 创建时间：2015/10/16 9:17
* 文件标识：
* 文件摘要：扩展vector
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/10/16 9:17
* 版本摘要：
*/
#pragma once

#include <vector>
using namespace std;

template <class T>
class zxMyVector : public vector<T>
{
public:
	zxMyVector();
	~zxMyVector();

	/**************************************************************************
	* 函数名: find
	* 功能: 查找指定值
	* 时间: 2015/10/16 9:21
	* 作者: ZDJ
	*/
	bool find(const T& var)
	{
		vector<T>::iterator itor = begin();
		for(; itor != end(); itor)
		{
			if((*itor) == var)
				return true;
		}

		return false;
	}
};