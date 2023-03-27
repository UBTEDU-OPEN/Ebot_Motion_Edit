/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：chandleruntime.h
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：动作编辑模块配置动作帧运行时间撤销重做
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#ifndef CHANDLEINT_H
#define CHANDLEINT_H

#include "ioperation.h"
#include "ubxundoredomanagerconst.h"

class UBXUNDOREDOMANAGER_API CHandleInt : public IOperation
{
public:
    explicit CHandleInt(Constants::emOperateCode eType);
    virtual ~CHandleInt();

public:
    /************************************
    * 名称: execute
    * 功能: 执行redo操作
    * 参数:
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    virtual bool execute() override;

    /************************************
    * 名称: undo
    * 功能: 撤销操作
    * 参数:
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    virtual bool undo() override;

    /************************************
    * 名称: cloneDataBeforeChanged
    * 功能: 拷贝修改前的数据，作为历史数据，以便进行撤销操作
    * 参数: [in ] lParam: 参数1
                 [in ] wParam: 参数2
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    virtual bool cloneDataBeforeChanged(LPARAM lParam, WPARAM wParam) override;

    /************************************
    * 名称: cloneDataAfterChanged
    * 功能: 拷贝修改后的数据，以便进行重做操作
    * 参数:[in ] lParam: 参数1
                [in ] wParam: 参数2
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    virtual bool cloneDataAfterChanged(LPARAM lParam, WPARAM wParam) override;

private:
    int m_nDataBeforeChanged; // 修改前的值
    int m_nDataAfterChanged;  // 修改后的值
};

#endif // CHANDLEINT_H
