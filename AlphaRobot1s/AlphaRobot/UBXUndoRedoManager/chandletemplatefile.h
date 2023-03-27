/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：chandletemplatefile.h
* 创建时间：2017/01/18
* 文件标识：
* 文件摘要：处理动作模板文件撤销重做功能
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/18
* 版本摘要：
*/

#ifndef CHANDLETEMPLATEFILE_H
#define CHANDLETEMPLATEFILE_H

#include "ioperation.h"
#include "ubxundoredomanagerconst.h"
#include "ubxundoredomanager_global.h"
#include "datamodel/cstringlistcontainer.h"

class UBXUNDOREDOMANAGER_API CHandleTemplateFile : public IOperation
{
public:
    explicit CHandleTemplateFile(Constants::emOperateCode eCode);
    virtual ~CHandleTemplateFile();

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
    /************************************
    * 名称: cloneDataAfterChanged
    * 功能: 拷贝数据
    * 参数:[in ] pSrcData: 拷贝源数据
                [in ] pDesData: 拷贝目的数据
    * 返回: void
    * 时间:   2017/01/18
    * 作者:   hwx
    ************************************/
    void cloneData(CStringListContainer *pSrcData, CStringListContainer **pDesData);

private:
    CStringListContainer *m_pDataBeforeChanged;
    CStringListContainer *m_pDataAfterChanged;
};

#endif // CHANDLETEMPLATEFILE_H
