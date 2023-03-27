/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ILocalActSyncWidget
* 创建时间：2016/01/28 15:33
* 文件标识：
* 文件摘要：本地动作同步窗口接口
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/01/28 15:33
* 版本摘要：
*/
#ifndef ILOCALACTSYNCWIDGET
#define ILOCALACTSYNCWIDGET

#ifdef LOCALACTSYNCWIDGET_EXPORTS
#define LOCALACTSYNCWIDGET_API Q_DECL_EXPORT
#else
#define LOCALACTSYNCWIDGET_API Q_DECL_IMPORT
#endif

#include <QWidget>

//本地动作同步窗口事件代理
class LOCALACTSYNCWIDGET_API ILocalActSyncWidgetEvent
{
public:
};

//本地动作同步窗口接口
class LOCALACTSYNCWIDGET_API ILocalActSyncWidget
{
public:
    /*
    * brief:  初始化
    * author: hels
    * date:   2015-12-24
    * param:  [in] parent 父窗体指针
    * param:  [in] pEvent 事件通知接口指针
    * return: void
    */
    virtual void Connect(QWidget* parent, ILocalActSyncWidgetEvent* pEvent) = 0;

    /*
    * brief:  释放资源,注意会销毁对象本身，调用此函数后，对象指针失效
    * author: hels
    * date:   2015-12-24
    * param:  void
    * return: void
    */
    virtual void Disconnect() = 0;

    //获取窗体
    virtual QWidget* GetWidget() = 0;
};

bool LOCALACTSYNCWIDGET_API CreateLocalActSyncWidget(ILocalActSyncWidget** ppLocalActSyncWidget);

#endif // ILOCALACTSYNCWIDGET

