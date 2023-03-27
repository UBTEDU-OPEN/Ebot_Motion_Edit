/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：LocalActSyncWidget.h
* 创建时间：2016/01/28 15:33
* 文件标识：
* 文件摘要：本地动作同步窗口
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/01/28 15:33
* 版本摘要：
*/
#ifndef LOCALACTSYNCWIDGET_H
#define LOCALACTSYNCWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QUuid>
#include <QFileDialog>
#include <QDateTime>
#include <QProgressBar>
#include <QTime>
#include <QCheckBox>
#include <QTextCodec>
#include <QStyleOption>

#include <map>

#include "UBXPublic.h"
#include "ilocalactsyncwidget.h"
#include "QCheckboxHeaderView.h"
#include "../zxLib/widget/ubxwidget.h"




using namespace std;



namespace Ui {
class LocalActSyncWidget;
}

//动作文件数据
struct ACTFILEDATA : public QObjectUserData
{
    QString m_sFileName;    //文件名
    QString m_sFilePath;    //文件路径
    QString m_sFileModTime; //文件修改时间
    qint64  m_nFileSize;    //文件大小
    QString m_sGuid;        //guid
    bool    m_bLocalFile;   //是否是本地文件

    ACTFILEDATA()
    {
        m_sFileName = NULLSTR;
        m_sFilePath = NULLSTR;
        m_sFileModTime = NULLSTR;
        m_nFileSize = 0;
        m_sGuid = QUuid::createUuid().toString();
        m_bLocalFile = true; //默认是本地文件
    }

    ACTFILEDATA(const QString& sFileName, const QString& sFilePath, const QString& sFileModTime, qint64 nFileSize, const QString& sGuid, bool bLocalFile)
    {
        m_sFileName = sFileName;
        m_sFilePath = sFilePath;
        m_sFileModTime = sFileModTime;
        m_nFileSize = nFileSize;
        m_sGuid = sGuid;
        m_bLocalFile = bLocalFile;
    }

    ACTFILEDATA(const ACTFILEDATA& filedata)
    {
        m_sFileName = filedata.m_sFileName;
        m_sFilePath = filedata.m_sFilePath;
        m_sFileModTime = filedata.m_sFileModTime;
        m_nFileSize = filedata.m_nFileSize;
        m_sGuid = filedata.m_sGuid;
        m_bLocalFile = filedata.m_bLocalFile;
    }

    ACTFILEDATA& operator = (const ACTFILEDATA& filedata)
    {
        m_sFileName = filedata.m_sFileName;
        m_sFilePath = filedata.m_sFilePath;
        m_sFileModTime = filedata.m_sFileModTime;
        m_nFileSize = filedata.m_nFileSize;
        m_sGuid = filedata.m_sGuid;
        m_bLocalFile = filedata.m_bLocalFile;
        return *this;
    }
};

class LOCALACTSYNCWIDGET_API LocalActSyncWidget : public UBXWidget, public zxObserverBase, public ILocalActSyncWidget
{
    Q_OBJECT

public:
    explicit LocalActSyncWidget(QWidget *parent = 0);
    ~LocalActSyncWidget();

    /**************************************************************************
    * 函数名: OnNotify
    * 功能: 通知消息
    * 参数:
    *    @[in ] pSubject: 被观察者
    *    @[in ] nMsgID: 消息ID
    *    @[in ] dwContext: 消息上下文
    *    @[in ] wParam: 参数1
    *    @[in ] lParam: 参数2
    * 返回值: void
    * 时间: 2015/08/22 19:09
    * 作者: zdj
    */
    virtual void OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

    /*
    * brief:  初始化
    * author: hels
    * date:   2015-12-24
    * param:  [in] parent 父窗体指针
    * param:  [in] pEvent 事件通知接口指针
    * return: void
    */
    virtual void Connect(QWidget* parent, ILocalActSyncWidgetEvent* pEvent);

    /*
    * brief:  释放资源,注意会销毁对象本身，调用此函数后，对象指针失效
    * author: hels
    * date:   2015-12-24
    * param:  void
    * return: void
    */
    virtual void Disconnect();

    //获取窗体
    virtual QWidget* GetWidget();
    RETRANSLATEUIFUNC

signals:
    /**************************************************************************
    * 函数名: PortOpenChanged
    * 功能: 端口改变事件
    * 时间: 2016/01/28 17:39
    * 作者: ZDJ
    */
    void PortOpenChanged(BOOL bOpen);

    /************************************
    * 名称: SigAddedAction
    * 功能: 添加动作文件后发送信号（供下载列表等更新状态）
    * 参数: [in]filePath 动作列表文件路径
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void SigAddedAction(const QString &filePath);

    /************************************
    * 名称: SigRemoveAction
    * 功能: 移除动作文件后发送信号（供下载列表等更新状态）
    * 参数: [in]filePath 动作列表文件路径
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void SigRemoveAction(const QString &filePath);

public slots:
    /**************************************************************************
    * 函数名: OnPortOpenChanged
    * 功能: 端口改变消息响应
    * 时间: 2016/01/28 17:39
    * 作者: ZDJ
    */
    void OnPortOpenChanged(BOOL bOpen);

    /************************************
    * 名称: OnSyncingAction
    * 功能: 响应添加动作到同步列表，主要提供给外部模块同步动作（如下载列表模块）
    * 参数: [in]vecActtion 动作文件路径
    * 返回:   void
    * 时间:   2016/04/08
    * 作者:   hels
    ************************************/
    void OnSyncingAction(QVector<ActItemData> &vecActtion);

    /************************************
    * 名称: slotOnReceiveRobotTFCardStatus
    * 功能: 机器人没有插入TF卡的响应函数，同步动作按钮设置为不可用状态
    * 参数: bHasTFCard: 是否存在TF卡标识
    * 返回:   void
    * 时间:   2016/10/25
    * 作者:   hwx
    ************************************/
    void slotOnReceiveRobotTFCardStatus(bool bHasTFCard);

    /************************************
    * 名称: slotOnIsSystemEmbedVersionNeedCopyUpdate
    * 功能: 嵌入式系统是否需要切换到U盘模式拷贝升级的槽函数
    * 参数: bMustUpdate: 是否需要切U盘拷贝升级
    * 返回:   void
    * 时间:   2016/11/09
    * 作者:   hwx
    ************************************/
    void slotOnIsSystemEmbedVersionNeedCopyUpdate(bool bMustUpdate);

protected:
    /**************************************************************************
    * 函数名: OnDownloadProgress
    * 功能: 下载进度消息响应
    * 时间: 2015/10/18 13:43
    * 作者: ZDJ
    */
    LRESULT OnDownloadProgress(WPARAM wParam, LPARAM lParam);

    /**************************************************************************
    * 函数名: OnDownloadEnd
    * 功能: 下载结束消息响应
    * 时间: 2015/10/18 13:43
    * 作者: ZDJ
    */
    LRESULT OnDownloadEnd(WPARAM wParam, LPARAM lParam);

    /**************************************************************************
    * 函数名: onTranFileCancel
    * 功能: 取消文件传输结果响应
    * 时间: 2016/7/18 13:43
    * 作者: hwx
    */
    LRESULT onTranFileCancel(WPARAM wParam, LPARAM lParam);

    // 处理内存卡容量应答消息
    void processTFCardCapacityRsp(WPARAM wParam, LPARAM lParam);

private:
    /**************************************************************************
    * 函数名: FindItemProgress
    * 功能: 获取行进度条
    * 参数:
    *    @[in ] sGuid：guid值
    * 返回值: 行进度条
    * 时间: 2016/01/13 17:49
    * 作者: ZDJ
    */
    QProgressBar* FindItemProgress(const QString& sGuid);

    /**************************************************************************
    * 函数名: FindItem
    * 功能: 获取列表项
    * 参数:
    *    @[in ] sGuid：guid值
    * 返回值: 列表项
    * 时间: 2016/01/13 17:49
    * 作者: ZDJ
    */
    QTableWidgetItem* FindItem(const QString& sGuid);

    /**************************************************************************
    * 函数名: FindItemIndex
    * 功能: 获取行索引
    * 参数:
    *    @[in ] sGuid：guid值
    * 返回值: 行索引
    * 时间: 2016/01/13 17:49
    * 作者: ZDJ
    */
    int FindItemIndex(const QString& sGuid);

    /**************************************************************************
    * 函数名: EnableCtrls
    * 功能: 设置控件启用状态
    * 参数:
    *    @[in ] bEnable: 启用标识
    *    @[in ] bIsSyncing: 同步状态
    * 返回值: void
    * 时间: 2016/01/13 14:40
    * 作者: ZDJ
    */
    void EnableCtrls(bool bEnable, bool bIsSyncing = false);

    /**************************************************************************
    * 函数名: GetNewID
    * 功能: 获取新增ID
    * 返回值: 新增ID
    * 时间: 2016/01/15 15:37
    * 作者: ZDJ
    */
    int GetNewID();

    /**************************************************************************
    * 函数名: popUpRobotUdisk
    * 功能: 弹出机器人移动U盘
    * 返回值:
    * 时间: 2016/11/12 15:37
    * 作者: hwx
    */
    void popUpRobotUdisk();

    /**************************************************************************
    * 函数名: openDirectory
    * 功能: 调用系统接口打开指定目录
    *    @[in ] strDir: 指定打开的路径
    * 返回值:
    * 时间: 2016/11/12 15:37
    * 作者: hwx
    */
    void openDirectory(const QString &strDir);

    /**************************************************************************
    * 函数名: syncNextAction
    * 功能: 同步下一个动作
    *
    * 返回值:
    * 时间: 2017/04/05
    * 作者: hwx
    */
    void syncNextAction();

public slots:
    /**************************************************************************
    * 函数名: onPortOpenClick
    * 功能: 打开端口事件响应
    * 时间: 2016/01/15 15:37
    * 作者: ZDJ
    */
    void onPortOpenClick();

    /**************************************************************************
    * 函数名: onRowCountChanged
    * 功能: refrash the tabel checkbox state
    * 返回值: void
    * 时间: 20161107
    * 作者: asu
    */
    void onRowCountChanged(int);

private slots:
    /**************************************************************************
    * 函数名: on_pushButton_addact_clicked
    * 功能: 添加动作事件响应
    * 时间: 2016/01/15 15:37
    * 作者: ZDJ
    */
    void on_pushButton_addact_clicked();

    /**************************************************************************
    * 函数名: on_pushButton_removeact_clicked
    * 功能: 移除动作事件响应
    * 时间: 2016/01/15 15:37
    * 作者: ZDJ
    */
    void on_pushButton_removeact_clicked();

    /**************************************************************************
    * 函数名: on_pushButton_downloadact_clicked
    * 功能: 同步动作事件响应
    * 时间: 2016/01/15 15:37
    * 作者: ZDJ
    */
    void on_pushButton_downloadact_clicked();

    /**************************************************************************
    * 函数名: on_pushButton_udiskmode_clicked
    * 功能: U盘模式点击事件响应
    * 时间: 2016/01/15 15:37
    * 作者: ZDJ
    */
    void on_pushButton_udiskmode_clicked();

    /**************************************************************************
    * 函数名: onSelallClicked
    * 功能: 全选复选框点击事件响应
    * 时间: 2016/01/15 15:37
    * 作者: ZDJ
    */
    void onSelallClicked();

    /**************************************************************************
    * 函数名: onTableWidgetCheckBoxClicked
    * 功能: 行首复选框点击响应
    * 参数:
    *    @[in ] bChecked: 选择标识
    * 返回值: void
    * 时间: 2016/01/15 15:37
    * 作者: ZDJ
    */
    void onTableWidgetCheckBoxClicked(bool bChecked);

    void on_pbAddLocalAct_clicked();

    //add the info to table widget
    void addToTableWidget(int row, int nId, QString fileName, qint64 nFileSize, QString fileModifyTime);


protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

    void resizeEvent(QResizeEvent *);
    virtual void timerEvent(QTimerEvent *event);

private:
    void setDefaultStyle();

private:
    Ui::LocalActSyncWidget *ui;    

    int          m_nProgressCol;  //进度条的列索引
    int          m_nFileNameCol; //文件名称的列索引
    bool         m_bUdiskMode;  //u盘模式

    int          m_nWaitForUdiskShowUpTimeoutId; // 等待U盘可用超时ID

    int  m_nTryGetUdiskTime;  // 尝试获取移动盘的次数

    bool         m_bSyncing;     //同步中
    bool       m_bIsChangedUdiskQuiet;  // 切换到U盘模式后不提示提示信息

    QMap<QString, int> m_mapGuidActID;//记录guid对应的action ID，以便同步过程中通知外部

    ILocalActSyncWidgetEvent*  m_pEvent;
    QCheckBox* m_pCheckBoxSelAll;
};

#endif // WIDGET_H
