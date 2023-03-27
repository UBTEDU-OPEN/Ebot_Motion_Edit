/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ioperation.h
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：操作类虚基类
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#ifndef IOPERATION_H
#define IOPERATION_H

#include "ubxundoredomanager_global.h"
#include "ubxundoredomanagerconst.h"
#include "globaldef.h"

class UBXUNDOREDOMANAGER_API  IOperation
{
public:
    explicit IOperation(Constants::emOperateCode eType);
    virtual ~IOperation();

    // 设置、获取ID
    void setID(int nId) { m_nID = nId; }
    int getId() { return m_nID; }

public:
    virtual bool execute() = 0; // 执行（重做）接口
    virtual bool undo() = 0;  // 撤销接口

    virtual bool cloneDataBeforeChanged(LPARAM lParam, WPARAM wParam) = 0; // 拷贝修改前的数据
    virtual bool cloneDataAfterChanged(LPARAM lParam, WPARAM wParam) = 0; // 拷贝修改后的数据

protected:
    int m_nID;  // 操作ID，唯一标识符
    Constants::emOperateCode      m_eCode;  // 操作类型
};

#endif // IOPERATION_H
