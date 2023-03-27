/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：chandlemarkline.h
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：修改标记线
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#ifndef CHANDLEMARKLINE_H
#define CHANDLEMARKLINE_H

#include <QList>

#include "ioperation.h"
#include "datamodel/cmarkline.h"
#include "ubxundoredomanager_global.h"

// 封装成container的目的是为了参数传递list<T>, 和LPARAM lParam进行强制转换
class UBXUNDOREDOMANAGER_API CMarkLineContainer
{
public:
    CMarkLineContainer();
     ~CMarkLineContainer();

    /************************************
    * 名称: addLine
    * 功能: 添加标记线
    * 参数: [in] pLine 标记线
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void addLine(CMarkLine *pLine);

    /************************************
    * 名称: deleteLine
    * 功能: 删除标记线
    * 参数: [in] pLine 标记线
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void deleteLine(CMarkLine *pLine);

    /************************************
    * 名称: cloneLines
    * 功能: 深拷贝所有动作数据
    * 参数: [in] pSrcLines: 源数据
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void cloneLines(CMarkLineContainer *pSrcLines);

    /************************************
    * 名称: removeAllLines
    * 功能: 删除所有标记线
    * 参数:
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void removeAllLines();

    QList<CMarkLine *> &getAllLines() { return m_lstMarkLines; }

private:

private:
    QList<CMarkLine *> m_lstMarkLines;

};

class UBXUNDOREDOMANAGER_API CHandleMarkLine : public IOperation
{
public:
    explicit CHandleMarkLine(Constants::emOperateCode eType);
    virtual ~CHandleMarkLine();

public:
    CMarkLineContainer *getDataBeforeChanged() { return m_pMarkLinesBeforeChanged; }
    CMarkLineContainer *getDataAfterChanged() { return m_pMarkLinesAfterChanged; }

//    /************************************
//    * 名称: clone
//    * 功能: 拷贝数据
//    * 参数: [in] item 待拷贝的数据
//    * 返回: void
//    * 时间:   2017/01/09
//    * 作者:   hwx
//    ************************************/
//    bool clone(CHandleMarkLine &item);

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
    * 名称: cloneMarkLines
    * 功能: 深拷贝所有动作数据
    * 参数: [in] pLinesSrc: 源数据
    *           [in] pLinesDes: 目的数据
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void cloneMarkLines(CMarkLineContainer  *pLinesSrc, CMarkLineContainer  **pLinesDes);

private:
    CMarkLineContainer *m_pMarkLinesBeforeChanged;  // 修改前动作帧、动作组标记线
    CMarkLineContainer  *m_pMarkLinesAfterChanged;  // 修改后动作帧、动作组标记线
};

#endif // CHANDLEMARKLINE_H
