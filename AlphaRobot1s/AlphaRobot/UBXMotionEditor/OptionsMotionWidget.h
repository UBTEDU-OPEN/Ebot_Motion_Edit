/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：UIMainLayerLayout.h
* 创建时间：2015/12/04 14:00
* 文件标识：
* 文件摘要：动作编辑模块界面设计类
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2015/12/04 14:00
* 版本摘要：
*/
#ifndef OPTIONOTIONWIDGET_H
#define OPTIONOTIONWIDGET_H

#include <QWidget>
#include "LayerUI/IActionLayerManager.h"
#include "LayerUI/ILayerManager.h"
#include "UBXMotionEditor.h"
#include "LayerUI/ILayerOptionHandler.h"
#include "IUBXRobot3DView.h"
#include "ubxundoredomanager.h"
#include "mp3projconstants.h"
#include "../zxLib/widget/ubxwidget.h"

#include <QTableWidget>
#include <QListWidget>
#include <QTimer>
#include <QStyleOption>
#include <QPainter>


typedef QVector<int> VectorInt;

class ActItemCopyData
{
public:
    ActItemCopyData();
    ~ActItemCopyData();

public:
    void initData();

public:
    int nRow;
    int nColumn;
    QList<VectorInt> listItem;
};


namespace Ui {
class COptionsMotionWidget;
}

class CActionBlockGroupManager;
class COptionsMotionWidget : public UBXWidget, public ILayerOptionHandler, public zxObserverBaseEx
{
    Q_OBJECT

public:
    explicit COptionsMotionWidget(QWidget *parent = 0);
    //    virtual void paintEvent(QPaintEvent *event);
    ~COptionsMotionWidget();

    // 观察者模式通知消息
    virtual void OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    virtual void customEvent(QEvent * event) override;
    //public:
    //    virtual void mousePressEvent(QMouseEvent *event);
public:
    //    // 实现IActionLayerManager接口
    //    virtual bool Connect(QWidget* pParent);
    //    // 释放资源
    //    virtual void Disconnect();

    virtual bool GetData(char** ppData,  int &nLen);
    virtual bool SetData(char* pData, int nLen);
    virtual void AdjustData();
    virtual void UBXMotionEditor_Set3DRobotViewInterface(IUBXRobot3DView* pRobotView);
    virtual void UBXMotionEditor_SetSocketSession(CIUBXSocketSession* pSession);
    virtual void UBXMotionEditor_UpdateScrollBar();
    virtual void UBXMotionEditor_SetMusicResource(QString strFilePath);
    virtual void UBXMotionEditor_StopDebug();
    virtual void UBXMotionEditor_ReleaseMusicSource();
    virtual void UBXMotionEditor_SetTreeView(ILibTreeNavigation* pView);
    virtual void UBXMotionEditor_TreeToView(char *pData, int nLen, QPoint* nPoint, QString FileName);
    virtual bool UBXMotionEditor_IsProjectModity();
    virtual bool UBXMotionEditor_IsRobotDebuging();
    virtual void UBXMotionEditor_SetProjectModity(bool bState);
public:
    // ILayerOptionHandler接口实现
    virtual bool Connect(QWidget* pParent, ILayerOptionHandler_Event* pEvent);
    virtual void ReleaseConnection();
    virtual QWidget* GetWidget();
    virtual void SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID);
    virtual void Set3DRobotViewInterface(IUBXRobot3DView* pRobotView);
    virtual void SetControlState(bool controlState, int nLayerType = 1);
    virtual void InsertGroupByMenu(int nCol);

    //hels 动作播放停止或者结束
    virtual void MotionPlayStop(int nPlayStatus) Q_DECL_OVERRIDE;
    virtual void FinishPlayingMusic();
    virtual void MusicLoaded(const QString& title, int time);//Motion模块更新名称和时长,时间单位为s
    virtual void UpdateMusicPosition(quint64 position);//音乐播放的位置时间单位为ms

    // hwx 动作编辑模块操作
    virtual void copyMotionFrame() Q_DECL_OVERRIDE;
    virtual void pasteMotionFrame() Q_DECL_OVERRIDE;
    virtual QMimeData *getCopyFrameData() Q_DECL_OVERRIDE;
    virtual void deleteMotionFrame() Q_DECL_OVERRIDE;
    virtual void modifyMotionFrame() Q_DECL_OVERRIDE;  // 修改动作帧
    virtual void insertMotionFrame() Q_DECL_OVERRIDE; // 插入动作帧
    virtual void splitInsertMotionFrame() Q_DECL_OVERRIDE; // 插入拆分动作帧
    virtual void addFrameToPreMotionBlock() Q_DECL_OVERRIDE; // 向前一个动作帧添加动作帧
    virtual void createFrameInNewBlock() Q_DECL_OVERRIDE; // 在新建动作帧组中创建动作帧
    virtual void mergeGroup() Q_DECL_OVERRIDE;//合并选中动作组

    virtual bool hasCopyFrame() Q_DECL_OVERRIDE; // 判断是否拷贝了动作帧

    RETRANSLATEUIFUNC

private:
    int GetCurrenGroupManagerID();
    void SetShowID(int *pID, int nCount);
    /**************************************************************************
     * 函数名: AddToTableWidget
     * 功能: 添加数据到列表
     * 参数:
     *    @[in ] pData: 数据缓冲区
     *    @[in ] nLen: 数据长度
     *    @[in ] BlockType: 动作类型
     *    @[in ] fRunTime: 已变换的运行时间
     *    @[in ] fAllTime: 已变换的总时间
     *    @[in ] ActionBlockID: 动作id
     * 返回值: void
     * 时间: 2015/08/04 16:29
     * 作者: ZDJ
     */
    int AddToTableWidget(char* pData, int nLen, QString BlockType, float fRunTime, float fAllTime, int ActionBlockID);

    /**************************************************************************
     * 函数名: UpdateTableWidgetShow
     * 功能: 更新列表显示
     * 参数:
     *    @[in ] nRow: 行索引
     *    @[in ] pActionBlock: 动作数据
     * 返回值: void
     * 时间: 2015/08/07 13:12
     * 作者: ZDJ
     */
    void UpdateTableWidgetShow(int nRow, CActionBlockData* pActionBlock);
    bool IsFullRowSelect(int nRow);
    bool isFullRowSelect(const QList<QTableWidgetSelectionRange>& selectRange);
    bool isContinueRange(const QList<QTableWidgetSelectionRange>& selectRange);
    bool getRangeData(ActItemCopyData& itemData, const QList<QTableWidgetSelectionRange>& selectRange, bool bOnlyRowColumn);
    bool IsTableHasSelectRow();
    // 显示选中的ID
    void setSelectRow(int row);
    void initTableWidget();
    void deleteSampleListItem(int nRow);
    void InsertSampleListToTableWidget(int nCurRow);
    bool CopySampleSelsctItem();

    void UpdateUIByData();
    void SetCurrentSelectItem(QListWidgetItem * item, int nIndex, int nSelRow);
    void UpdataGridByGroupManager( int nSelRow, CActionBlockGroupManager* pGroup);
    void initWidget();
    void onMotionFrameCopy();
    void onMotionFramePasteToEnd();
    void onMotionFramePaste();
//    int  getSelectRowCount();
    int  getFirstSelectRowID();
    int  getLastSelectRowID();
    MOTOR_DATA* GetMotorData(char* pData, int nLen, int nID);
    void SetMotorAngle(char* pData, int nLen, int nID, int nAngle);
    void ModleView(int nActionBlockID);
    void PlayBlock(int nActionBlockID);

    /**************************************************************************
     * 函数名: addMotionFrame
     * 功能: 添加动作帧到指定的block
     * 参数:
     *    @[in ] nBlockId: block id
     * 返回值: void
     * 时间: 2016/12/08
     * 作者: hwx
     */
    void addMotionFrame(int nBlockId);

    void CreateGroupListPopMenuActions();
    void CreateGridListPopMenuActions();
    void onGroupListDelete();

private:

    // 更新动作编辑的运行时间
    void updateConfigRuntime(int nTimeMs);

    // 拷贝修改前、后的动作数据
    void cloneBeforeChangedMotionData(IOperation *pHandle);
    void  cloneAfterChangedMotionData(IOperation *pHandle);

    /**************************************************************************
     * 函数名: getCurrentSelTimeCol
     * 功能: 获取当前时间，以粒度为单位
     * 参数:
     * 返回值: void
     * 时间: 2017/03/17
     * 作者: hwx
     */
    quint64 getCurrentSelTimeCol();

    // 获取界面管理
    IUILayerManager *getLayerManger();

    // 复位播放时间
    void resetPlayTime();

   // 订阅消息
    void subscribeMsg();

    /**************************************************************************
     * 函数名: isMotionSimulating
     * 功能: 判断是否正在进行动作仿真或联合仿真
     * 参数:
     * 返回值: void
     * 时间: 2017/03/17
     * 作者: hwx
     */
    bool isMotionSimulating();

    // 判断是否存在拷贝动作帧
    bool isCopyFrame();

    /**************************************************************************
     * 函数名: doProcessSimulateMsg
     * 功能: 处理音乐层音乐、动作播放消息，主要用于播放时禁用当前界面按钮
     * 参数:
     *    @[in ] nMsgId: 消息ID
     *    @[in ] eStatus: 播放状态
     * 返回值: void
     * 时间: 2017/03/15
     * 作者: hwx
     */
    void doProcessSimulateMsg(int nMsgId, emPlayStatus eStatus);

    // 停止播放
    void doProcessStopPlayMsg(int nMsgID, emPlayType eType);

    // 仿真按钮使能状态设置
    void setSimulateButtonState(bool bEnable);

    // 动作编辑按钮状态设置，仿真时禁止动作编辑
    void setMotionEditButtonState(bool bEnalbe);

signals:
    void SigMotionPlayStop(emPlayStatus eStatus);
    void SigSetControlState(bool controlState, int nLayerType);

public slots:
    void slotOnAngleValueCheckWay(emAngleChectWayType nWayType);
    void onItemClicked(QTableWidgetItem *item);

private slots:
    //hels 用于判断选中行，以便通知MP3模块切换选中项
    void onSectionClicked(int logicalIndex);
    //hels 响应行号双击事件
    void onSectionDoubleClicked(int logicalIndex);

    //hels 响应编辑Item信号
    void onItemCommitData(QWidget * editor);
    void modifyItemData(int nRow, int nColumn, bool bSaveToRedo);

    //hels 单击Motion Action时，触发，通知3D模型改变舵机角度
    void OnTimeOut();

    void OnMotionPlayStop(emPlayStatus eStatus);
    void OnSetControlState(bool controlState, int nLayerType);

    void on_pbAdd_clicked();

    void on_pbInsert_clicked();

    void on_pbInsertSplit_clicked();

    void on_pbModify_clicked();

    void onMotionFrameDelete();

//    void on_btnAddGroup_clicked();

    //    void on_m_ListActionGroup_clicked(const QModelIndex &index);

    //    void on_m_ListActionGroup_itemClicked(QListWidgetItem *item);

    //void on_btnDeleteGroup_clicked();

    void slotOnGroupItemPressed(QListWidgetItem *item);

    void on_twActionList_itemPressed(QTableWidgetItem *item);

    void on_btnUnitedSimulation_clicked();

    void on_btnStop_clicked();

    void onGroupModifyName();
    void onGroupListCopy();
    void onGroupListPaste();
    void onGroupListMerge();

    void slotOnEditFrameRunTimeFinished();

protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *);


    //////
    void autoModifyItemValue();
    void warningItemValueInvalid();
    void setWarningItemValueInvalid(int nRow, int nColum);
    void changeItemColor(Qt::GlobalColor color);

    bool removeGroupListItem(int nGroupID);
    QListWidgetItem * getListWidgetItem(int nGroupID);

private:
    Ui::COptionsMotionWidget *ui;
    ILayerManager* m_pMainLayer;
    ILayerOptionHandler_Event* m_pEvent;    
    int m_nCol;
    QTimer* m_pTimerClicked;//单击时启动的定时器，用于更新3D模型
                            //若双击触发，会先停止此定时器，再进行平滑动画

    int m_nCurrentGroupListSelectItem;
    int m_nCurrentGroupID;
    //QList<int> m_lstGroupIDSelect;
    vector<int> m_ShowId;
    bool m_bLbtnDownDebug;
    CActionBlockDataManager* m_pGroupManager;
    IUBXRobot3DView* m_pRobotView;
    QTableWidget * m_pGridTable;

    //    QList<QTableWidgetItem*>m_pItemList;
    QListWidget * m_pGroupListWidget;
    bool IsCompensatory;
    QString m_strTotalTime;
    int m_nTotalTime;
    int m_nServoCount;  //舵机数量
    int m_nRunTimeCol;  //运行时间列索引
    int m_nAllTimeCol;  //总时间列索引

    emPlayStatus m_eTotalPlayStatus;//联合仿真状态
    bool m_bIsMotionSimulating; // 正在音乐或动作仿真的标志

    QString m_strFrameRunTime;  // 帧动作运行时间

    QMimeData* m_pMineData;



    ///////
    Qt::GlobalColor m_EditItemDefaultColor;
    Qt::GlobalColor m_EditItemAutoEditedColor;
    Qt::GlobalColor m_EditItemInvalidColor;
    bool m_bAutoModifyItemData;

    //for copy data
    ActItemCopyData m_ActItemCopyData;

};


#endif // OPTIONOTIONWIDGET_H
