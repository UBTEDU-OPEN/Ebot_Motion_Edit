/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ubxundoredomanager.h
* 创建时间：2017/01/09
* 文件标识：
* 文件摘要：撤销重做管理模块
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2017/01/09
* 版本摘要：
*/

#ifndef UBXUNDOREDOMANGAER_H
#define UBXUNDOREDOMANAGER_H

#include <QList>
#include <QObject>

#include "ioperation.h"
#include "zxObserverSubjectBase.h"
#include "ubxundoredomanager_global.h"
#include "chandlemarkline.h"
#include "chandlemotiondata.h"
#include "chandlestring.h"
#include "chandleint.h"
#include "chandletemplatefile.h"

using namespace zxLib;
using namespace Constants;

extern UBXUNDOREDOMANAGER_API const UINT WM_MSG_EXECUTE_UNDO;  //执行undo操作
extern UBXUNDOREDOMANAGER_API const UINT WM_MSG_EXECUTE_REDO;  //执行redo操作

class UBXUNDOREDOMANAGER_API UBXUndoRedoManager : public QObject
{
    Q_OBJECT

public:
    /************************************
    * 名称: getInstance
    * 功能: 获取单例
    * 参数：
    * 返回:   撤销管理模块对象指针
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    static UBXUndoRedoManager *getInstance();

    /************************************
    * 名称: notifyOberservers
    * 功能: 通知观察者
    * 参数: [in] eType: 操作类型
                 [in] eCode: 操作码
                 [in] lParam: 参数1
                 [in] wParam: 参数2
    * 返回:
    * 时间:   2017/01/13
    * 作者:   hwx
    ************************************/
    void notifyOberservers(emExecuteType eType, emOperateCode eCode, LPARAM lParam, WPARAM wParam);

    /************************************
    * 名称: createOperation
    * 功能: 工厂模式，根据类型码创建操作对象
    * 参数: [in] eType: 操作类型码
    * 返回: operation对象
    * 时间:   2017/01/13
    * 作者:   hwx
    ************************************/
    IOperation *createOperation(emOperateCode eCode);

    // 生成ID
    int createID();

public:
    /************************************
    * 名称: pushUndoOperation
    * 功能: 添加undo操作
    * 参数: [in] pItem: 操作项
    * 返回:  添加后的size
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    int pushUndoOperation(IOperation *pItem);

    /************************************
    * 名称: executeUndoOperation
    * 功能: 执行undo操作
    * 参数:
    * 返回:  bool
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    bool executeUndoOperation();

    /************************************
    * 名称: pushRedoOperation
    * 功能: 添加redo操作
    * 参数: [in] pItem: 操作项
    * 返回:  添加后的size
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    int pushRedoOperation(IOperation *pItem);

    /************************************
    * 名称: executeRedoOperation
    * 功能: 执行redo操作
    * 参数:
    * 返回: bool
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    bool executeRedoOperation();

    /************************************
    * 名称: clearAllList
    * 功能: 清空撤销重做列表
    * 参数:
    * 返回: bool
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void clearAllList();

    /************************************
    * 名称: clearRedoList
    * 功能: 清空重做列表
    * 参数:
    * 返回: bool
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void clearRedoList();

    /************************************
    * 名称: clearUndoList
    * 功能: 清空撤销列表
    * 参数:
    * 返回: bool
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void clearUndoList();

    // 判断撤销、重做队列是否为空
    bool isUndoListEmpty();
    bool  isRedoListEmpty();

signals:
    /************************************
    * 名称: sigUndoListSizeChanged
    * 功能: Undo列表size发生变化
    * 参数: [in] nSize: Undo列表最新的size
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void sigUndoListSizeChanged(int nSize);

    /************************************
    * 名称: sigRedoListSizeChanged
    * 功能: Rndo列表size发生变化
    * 参数: [in] nSize: Rndo列表最新的size
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void sigRedoListSizeChanged(int nSize);

protected:
    explicit UBXUndoRedoManager(QObject *parent = NULL);
    virtual ~UBXUndoRedoManager();

private:
    // 清空列表
    void clearList(QList<IOperation *> &lstOperation);

private:
    QAtomicInt  m_nID;  // 操作ID

    QList<IOperation *>  m_lstRedoOperation;  // redo操作列表
    QList<IOperation *>   m_lstUndoOperation;  // undo操作列表
};

#endif // UBXUNDOREDOMANAGER_H
