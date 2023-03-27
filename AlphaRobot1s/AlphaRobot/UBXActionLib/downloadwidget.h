/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：downloadwidget.h
* 创建时间：2016/03/30
* 文件标识：
* 文件摘要：下载项组件，用于显示和管理下载项
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/30
* 版本摘要：
*/

#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H
#include "actupdatemangr.h"
#include <QWidget>
#include <QCheckBox>
#include <QProgressBar>
#include <QPushButton>
#include <QMap>
#include "UBXPublic.h"
#include "../zxLib/widget/ubxwidget.h"



namespace Ui {
class DownloadWidget;
}


class UBXACTIONLIBSHARED_EXPORT DownloadWidget : public UBXWidget
{
    Q_OBJECT

public:
    explicit DownloadWidget(QWidget *parent = 0);
    ~DownloadWidget();

    /************************************
    * 名称: setDefaultStyle
    * 功能: 设置窗体默认样式
    * 返回:   void
    * 时间:   2016/03/31
    * 作者:   hels
    ************************************/
    void setDefaultStyle();

    RETRANSLATEUIFUNC

signals:
    /************************************
    * 名称: SigAddToSyncList
    * 功能: 添加到同步列表
    * 参数: [in]vecActtion 动作文件路径
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void SigAddToSyncList(QVector<ActItemData> &vecActtion);

protected:
    /************************************
    * 名称: resizeEvent
    * 功能: 窗体大小改变事件，用于重新计算组件大小
    * 参数: [in]parameter-name parameter description
    * 返回:   void
    * 时间:   2016/03/30
    * 作者:   hels
    ************************************/
    void resizeEvent(QResizeEvent *event);

    //重写以下函数实现移动窗体功能
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *);

    /************************************
    * 名称: eventFilter
    * 功能: 重写父类函数，当鼠标离开tablewidget时，将hover状态清空
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
    /************************************
    * 名称: OnStartDownload
    * 功能: 通知外部开始下载
    * 参数: [in]requestID 对应的请求ID
    * 参数: [in]itemData 下载项
    * 参数: [in]bytesReceived 已经接收到的字节数
    * 参数: [in]bytesTotal 总共的字节数
    * 返回:   void
    * 时间:   2016/04/05
    * 作者:   hels
    ************************************/
    void OnStartDownload(int requestID, ActItemData itemData,  qint64 bytesReceived, qint64 bytesTotal);


    /************************************
    * 名称: OnDownloadProgress
    * 功能: 响应下载进度
    * 参数: [in]requestID 对应的请求ID
    * 参数: [in]nActionID 下载项的ID
    * 参数: [in]bytesReceived 已经接收到的字节数
    * 参数: [in]bytesTotal 总共的字节数
    * 返回:   void
    * 时间:   2016/04/05
    * 作者:   hels
    ************************************/
    void OnDownloadProgress(int requestID, int nActionID,qint64 bytesReceived, qint64 bytesTotal);

    /************************************
    * 名称: OnItemCheckBoxClicked
    * 功能: 响应下载项checkbox槽函数
    * 参数: [in]checked 按钮check状态
    * 返回:   void
    * 时间:   2016/04/05
    * 作者:   hels
    ************************************/
    void OnItemCheckBoxClicked(bool checked);

    /************************************
    * 名称: OnSelAllCheckBoxClicked
    * 功能: 选中所有按钮槽函数
    * 参数: [in]checked 按钮check状态
    * 返回:   void
    * 时间:   2016/04/08
    * 作者:   hels
    ************************************/
    void OnSelAllCheckBoxClicked(bool checked);

    /************************************
    * 名称: OnPauseBtnClicked
    * 功能: 暂停按钮槽函数
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void OnPauseBtnClicked();

    /************************************
    * 名称: OnDeleteBtnClicked
    * 功能: 删除按钮槽函数
    * 返回:   void
    * 时间:   2016/04/07
    * 作者:   hels
    ************************************/
    void OnDeleteBtnClicked();

    /************************************
    * 名称: OnOpenBtnClicked
    * 功能: 打开文件按钮槽函数
    * 返回:   void
    * 时间:   2016/04/07
    * 作者:   hels
    ************************************/
    void OnOpenBtnClicked();

    /************************************
    * 名称: OnCellEntered
    * 功能: 响应鼠标进入item项，实现hover效果
    * 参数: [in]int row 当前Item行
    * 参数: [in]int column 当前Item列
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void OnCellEntered(int row, int column);

    /************************************
    * 名称: OnHasPausedDownload
    * 功能: 下载停止后由下载线程通知
    * 参数: [in]nActionID 暂停的action ID
    * 返回:   void
    * 时间:   2016/04/06
    * 作者:   hels
    ************************************/
    void OnHasPausedDownload(int nActionID);

    /************************************
    * 名称: OnPauseSelBtnClicked
    * 功能: 批量暂停按钮槽函数
    * 返回:   void
    * 时间:   2016/04/08
    * 作者:   hels
    ************************************/
    void OnPauseSelBtnClicked();

    /************************************
    * 名称: OnStartAllBtnClicked
    * 功能: 全部开始按钮槽函数
    * 返回:   void
    * 时间:   2016/04/08
    * 作者:   hels
    ************************************/
    void OnStartAllBtnClicked();

    /************************************
    * 名称: OnDeleteSelBtnClicked
    * 功能: 批量删除按钮槽函数
    * 返回:   void
    * 时间:   2016/04/08
    * 作者:   hels
    ************************************/
    void OnDeleteSelBtnClicked();

    /************************************
    * 名称: OnOpenFolderBtnClicked
    * 功能: 打开目录按钮槽函数
    * 返回:   void
    * 时间:   2016/04/08
    * 作者:   hels
    ************************************/
    void OnOpenFolderBtnClicked();

    /************************************
    * 名称: OnAddToSyncListclicked
    * 功能: 添加到同步列表
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void OnAddToSyncListclicked();

    /************************************
    * 名称: onRowCountChanged
    * 功能: the slot to tabel row count
    * 返回:   void
    * 时间:   20161108
    * 作者:   asu
    ************************************/
    void onRowCountChanged(int nRowCount);
    
    /************************************
    * 名称: slotOnTableVScrollValueChanged
    * 功能: 垂直滚动条值发生变化的槽函数，此时需要更新删除、
    * 返回:   void
    * 时间:   20161108
    * 作者:   asu
    ************************************/
    void slotOnTableVScrollValueChanged(int nValue);

public slots:
    /************************************
    * 名称: OnPortOpenChanged
    * 功能: 响应机器人连接状态改变
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void OnPortOpenChanged(BOOL bOpen);

    /************************************
    * 名称: OnAddedAction
    * 功能: 同步界面添加动作文件响应函数，更新状态用
    * 参数: [in] sFilePath 动作列表文件路径
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void OnAddedAction(const QString& sFilePath);

    /************************************
    * 名称: OnRemoveAction
    * 功能: 同步界面移除动作文件响应函数，更新状态
    * 参数: [in] sFilePath 动作列表文件路径
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void OnRemoveAction(const QString& sFilePath);

    /************************************
    * 名称: OnCheckBoxNotAddedClicked
    * 功能: 仅显示未添加槽函数
    * 参数: [in]checked 按钮的check状态
    * 返回:   void
    * 时间:   2016/04/11
    * 作者:   hels
    ************************************/
    void OnCheckBoxNotAddedClicked(bool checked);

    /************************************
    * 名称: slotOnDownloadFinish
    * 功能: 动作下载完成后，更新进度条和下载状态
    * 参数: nActionId：动作id
    * 返回:   void
    * 时间:   2016/07/14
    * 作者:   hwx
    ************************************/
    void slotOnDownloadFinish(int nActionId);

    /************************************
    * 名称: slotOnReceiveRobotTFCardStatus
    * 功能: 机器人没有插入TF卡的响应函数，同步动作按钮设置为不可用状态
    * 参数: bHasTFCard: 是否存在TF卡标识
    * 返回:   void
    * 时间:   2016/10/25
    * 作者:   hwx
    ************************************/
    void slotOnReceiveRobotTFCardStatus(bool bHasTFCard);

private slots:
    void onCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);


private:
    enum ItemStatus
    {
        UnknownState = 0x0000,//未知状态
        DownloadingState = 0x0001,//下载中
        FinishedState = 0x0002,//下载完成
        PausedState = 0x0004,//暂停下载（未完成下载也属于此种状态）
        NotAddedState = 0x0008,//未添加至同步列表状态
        AddedState = 0x0010,//已经添加
        SynchronizedState = 0x0020//已同步
    };

private:
    /************************************
    * 名称: InitItems
    * 功能: 初始化下载项
    * 返回:   void
    * 时间:   2016/04/07
    * 作者:   hels
    ************************************/
    void InitItems();

    /************************************
    * 名称: AddDownloadItem
    * 功能: 添加下载项
    * 参数: [in]requestID 对应的请求ID
    * 参数: [in]itemData 下载项
    * 参数: [in]bytesReceived 已经接收到的字节数
    * 参数: [in]bytesTotal 总共的字节数
    * 返回:   void
    * 时间:   2016/04/05
    * 作者:   hels
    ************************************/
    void AddDownloadItem(int requestID, ActItemData itemData, qint64 bytesReceived, qint64 bytesTotal);


    /************************************
    * 名称: getRowByActionID
    * 功能: get row number
    * 参数: [in]nActionID 下载项的ID
    * 返回:   int
    * 时间:   2016/05/31
    * 作者:   suhaiwu
    ************************************/
    int getRowByActionID(const int& nActionID)const;

    /************************************
    * 名称: getActionIDByRow
    * 功能: get ActionID
    * 参数: [in]nRowTab  table row
    * 返回:   int
    * 时间:   2016/05/31
    * 作者:   suhaiwu
    ************************************/
    int getActionIDByRow(const int& nRowTab)const;

    /************************************
    * 名称: getActionIDByProgressBar
    * 功能: get ActionID
    * 参数: [in]pProgressBar  table progressBar
    * 返回:   int
    * 时间:   2016/05/31
    * 作者:   suhaiwu
    ************************************/
    int getActionIDByProgressBar(const QProgressBar* const pProgressBar)const;

    /************************************
    * 名称: addItemStatus
    * 功能: set item status of nActionID
    * 参数: [in]nActionID     load item action id
    * 参数: [in]eItemStatus   item status
    * 返回:   bool
    * 时间:   2016/06/01
    * 作者:   suhaiwu
    ************************************/
    bool addItemStatus(const int nActionID, const ItemStatus eItemStatus);

    /************************************
    * 名称: removeItemStatus
    * 功能: remove item status of nActionID
    * 参数: [in]nActionID     load item action id
    * 参数: [in]eItemStatus   item status
    * 返回:   bool
    * 时间:   2016/06/01
    * 作者:   suhaiwu
    ************************************/
    bool removeItemStatus(const int nActionID, const ItemStatus eItemStatus);

    /************************************
    * 名称: getItemStatus
    * 功能: get item status of nActionID
    * 参数: [in]nActionID     load item action id
    * 返回:   ItemStatus
    * 时间:   2016/06/01
    * 作者:   suhaiwu
    ************************************/
    int getItemStatus(const int nActionID)const;

    /************************************
    * 名称: isTtemStatus
    * 功能: just item status of nActionID
    * 参数: [in]nActionID     load item action id
    * 参数: [in]eItemStatus   item status
    * 返回:   bool
    * 时间:   2016/06/01
    * 作者:   suhaiwu
    ************************************/
    bool isItemStatus(const int nActionID, const ItemStatus eItemStatus)const;

private:
    Ui::DownloadWidget *ui;
    int m_nRowPrevHover;//记录上一次hover的行，当次要清除hover背景
    QPushButton* m_pBtnPause;//暂停按钮
    QPushButton* m_pBtnDelete;//删除
    QPushButton* m_pBtnOpen;//打开目录
    int m_nCurActionID;//当前选中的Action ID
    int m_nCurRow;//记录当前行列信息
    int m_nCurCol;
    QMap<int, ActItemData> m_mapItemData;
    QMap<int, int> m_mapItemStatus;//下载项状态
    QMap<int, QProgressBar*> m_mapProgressBar;//下载进度

    QPoint m_ptPre;
    bool m_bLeftDown;
    QCheckBox *m_pCheckBoxSelAll;
};

#endif // DOWNLOADWIDGET_H
