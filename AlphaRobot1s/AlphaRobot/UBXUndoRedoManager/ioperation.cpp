/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ioperation.cpp
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：操作类虚基类
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#include "ioperation.h"

IOperation::IOperation(Constants::emOperateCode eType) : m_eCode(eType)
  , m_nID(-1)
{

}

IOperation::~IOperation()
{

}

