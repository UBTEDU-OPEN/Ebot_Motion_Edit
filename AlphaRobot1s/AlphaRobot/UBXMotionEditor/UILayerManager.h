#ifndef UILAYERMANAGER_H
#define UILAYERMANAGER_H

#include "LayerUI/IUILayerManager.h"
#include "LayerUI/IGroupDataPlay.h"
#include "stable.h"
#include <QStyleOption>
#include <QPainter>

class STMotionData;

namespace Ui {
class CUILayerManager;
}
class CUILayerManager :public QWidget, public IUILayerManager, public IGroupDataPlayEvent, public zxObserverBaseEx
{
    Q_OBJECT

public:
    explicit CUILayerManager(QWidget *parent = 0);
    ~CUILayerManager();

    // 观察者模式通知消息
    virtual void OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

private:
    // 更新tab显示名称，如果存在修改，则文件名称后加*
    void updateTabName();

    /************************************
    * 名称: updateMotionData
    * 功能: 更新动作数据
    * 参数: [in] pData: 数据
    * 返回: void
    * 时间:   2017/01/09
    * 作者:   hwx
    ************************************/
    void updateMotionData(STMotionData *pData);

    /************************************
    * 名称: doProcessSimMsg
    * 功能: 处理仿真消息
    * 参数: [in] nMsgID: 消息ID
    * 参数: [in] eStatus: 播放状态
    * 返回: void
    * 时间:   2017/03/18
    * 作者:   hwx
    ************************************/
    void doProcessSimMsg(int nMsgID, emPlayStatus eStatus);

signals:
    void sigOnMotionPosChanged(int nTime);

private slots:
    void slotOnMotionPosChanged(int nTime);

private:
    Ui::CUILayerManager *ui;

//	IUBXRobot3DView* Get3dView(){return m_pRobot3dView;};

protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *);
    virtual void customEvent(QEvent * event) override;
public:
    virtual bool Connect(QWidget* pParent, IUILayerManager_Event* pEvent);
    virtual void ReleaseConnection();
    virtual QWidget* GetWidget();
    virtual void OnDraw(/*CDC* pDC*/);
    virtual bool SetData(char* pData, int nLen);
    virtual bool setMotionLayerData(char *pData, int nLen);
    virtual void AdjustData() ;
    //virtual void SetMusicSource(QString strMusicFile);
    virtual bool GetData(char** ppData, int &nLen);
    virtual QString GetLayerName(int nLayerID);
    virtual int GetCurrentSelTime();
    virtual int SetCurrentSelectCol(int nCol);
    virtual void SetActionDataGroupManagerToOptions(CActionBlockDataManager* pGroupManager, int nCurrentGroupID,int nCurrentActionID);
    //virtual void SetActionToOptions(int nCurrentGroupID , int nCurrentActionID) ;
    virtual void UpdateScrollBar();
    virtual void AddMusic();
    virtual void ReleaseMusic() ;
    virtual bool AddActionLayer(ILayerUI* pLayer);
    virtual bool AddAcionLayer(int nLayerID);
//	virtual void SetPosition(int PositionX, int PositionY);
    // 查找层
    virtual ILayerUI* FindLayer(int nTypeID);
    // 删除层
    virtual bool DeleteActionLayer(int nTypeID);
    // 删除所有层
    virtual void DeleteAllLayer();
    // 获取LAY数量
    virtual int GetLayerCount();
    // 根据序号查找层
    virtual ILayerUI* FindLayerByIndex(int nIndex);
    virtual void SetHScrollOffset(uint nPos);
    virtual void SetHScrollPos(int nCol) ;
    virtual void Set3DRobotViewInterface(IUBXRobot3DView* pRobotView);
    virtual IUBXRobot3DView* Get3DRobotViewInterface() {return m_pRobot3dView;}
    virtual int GetNewMaxCellCount(int nMax);
    virtual void SetTreeView(ILibTreeNavigation* pView);
    virtual void TreeToView(char *pData, int nLen, QPoint* nPoint, QString FileName);

    virtual void SetTimeScaleValues(int timeScale);
    virtual void FreshTimeScaleValues(int timeSacle);
    virtual void SetMusicBeatsTime(int nTime);
    virtual int GetTimeScaleValues();
    virtual int GetLineThick();
    virtual void SetLineThick(int nThick);
    virtual int GetCellWidth();
    virtual void SetCellWidth(int nWidth);
    virtual int GetCellCount();
    virtual void SetCellCount(int nCount);

    virtual int GetCurrentSelectCol();

    virtual void LayerPlaying(int layerType, ILayerOptionHandler* optionHandle);
    virtual void StopLayerDebug();
    virtual void PauseLayerDebug();
    virtual void LayerStop();
    virtual void LayerDebugStop(int layerType);
    virtual void PlayEvent_OnDebugStop(int nLayerType, int nPlayStatus) Q_DECL_OVERRIDE;
    virtual void PlayEvent_OnSetPos(int nCol);
    virtual void PlayEvent_OnSetPosChangeToOptions(int nLayerType , int nCol);
    virtual void PlayEvent_OnEnableCtrl(int nLayerType , bool bState);
    virtual void PlayEvent_SetDebugType(int nDebugTyoe);
    virtual int  PlayEvent_GetDebugType();
    virtual void SetOriginPos(int nCol);
//	virtual void PlayEvent_SetLayerMenuState(bool menuState);

    virtual void TotalLayerPlaying();
    virtual void StopTotalLayerDebug();
    virtual void PauseTotalLayerDebug();
    virtual ILayerOptionHandler* GetOptionHandler();
    virtual void IUILayerManager_SetSocketSession(CIUBXSocketSession* pSession);
    virtual void IUILayerManager_SwitchLayer(int nLayerID);
    virtual void IUILayerManager_SetMusicFile(int nLayerType , void *pData , int nDataLen , int nDataID) ;
    virtual void IUILayerManager_DeleteMusicFile(int nLayerType) ;
    virtual void IUILayerManager_SetModifiedState(bool bState) ;
    virtual bool IUILayerManager_GetModifiedState() ;
    virtual bool IUILayerManager_IsRobotDebuging() ;
    virtual bool IUILayerManager_IsSimulating() Q_DECL_OVERRIDE;
    virtual void IUILayerManager_SingleBlockPlaying(int nLayerType ,CActionBlockData *pActionData) ;
    virtual void IUILayerManager_SingleGroupPlaying(int nLayerType , CActionBlockGroupManager* pGroupManager);
    virtual void IUILayerManager_SingleBlockPlayingStop(int nLayerType);
    virtual emPlayType IUILayerManager_GetCurPlayType() Q_DECL_OVERRIDE;

    virtual void IUILayerManager_SetMusicModifiedState(bool bState);
    virtual bool IUILayerManager_GetMusicModifiedState();

    // 设置tabwidget中tab文本和tooltip
    virtual void IUILayerManager_SetTabWidgetTabText(QWidget *pParent, const QString &sFile) override;

    //hels 通知MP3模块更新界面
    virtual void UpdateMP3Widget(CActionBlockDataManager* pGroupManager, int nCurrentGroupID , int nCurrentActionID );
    virtual void UpdateMP3Action(int nGroupID, float fTime);
    virtual void FinishMusicPlaying();//音乐播放完成时通知

    /************************************
    * 名称: updateMotionWidget
    * 功能: 刷新动作编辑界面
    * 参数: [in] nTime: 时间值，并非真实时间，需要x时间粒度
    * 返回: void
    * 时间:   2017/03/02
    * 作者:   hwx
    ************************************/
    virtual void updateMotionWidget(int nTime) Q_DECL_OVERRIDE;

    /************************************
    * 名称: PauseMusicPlaying
    * 功能: 暂停播放音乐
    * 参数:
    * 返回: void
    * 时间:   2017/03/17
    * 作者:   hwx
    ************************************/
    virtual void PauseMusicPlaying() Q_DECL_OVERRIDE;

    /************************************
    * 名称: StartMusicPlaying
    * 功能: 开始播放音乐
    * 参数:
    * 返回: void
    * 时间:   2017/03/17
    * 作者:   hwx
    ************************************/
    virtual void StartMusicPlaying() Q_DECL_OVERRIDE;

    virtual void HasMotionData(bool hasData);

    QString GetModuleDir();
    //总的单元块数
    int m_CellCount;
    // 线宽
    int m_lineThick;
    // 单元格宽度
    int m_nCellWidth;
    // 时间粒度
    int m_scaleValue;
    // 仿真类型
    int m_nDebugType;
protected:
    // 时间粒度
    static int		m_nTimeAtomic;
    QList<ILayerUI*> m_listActionLayer;
//	SCROLLINFO m_hSi;

    IUILayerManager_Event* m_pEvent;
    IGroupDataPlay*	m_pGroupDataPlay;
    IGroupDataPlay* m_pMusicDataPlay;
//	IGroupDataManagerPlay* m_ppManagerPlay;
    CIUBXSocketSession*		m_pSocketSession;
    int m_nCurCol;
    QList<IGroupDataPlay*> m_lstGroupPlay;
private:
    IUBXRobot3DView* m_pRobot3dView;

    QString m_strFileName;  // 正在编辑的动作文件名称，不含路径

    emPlayType  m_ePlayType; // 仿真类型

    bool	   m_IsMusicPlaying;
    bool    m_IsDataPlaying;
    bool    m_bModified;
    bool    m_bMusicModified;

    IGroupDataPlay*	m_pGroupPlay;
//	CMusicChooseDialog *m_pChooseDlg;
    void ResetHScroll();
    //void SetHScrollOffset(int offset);
    ILayerUI* GetLastLayer();
    void ClearSelectState();
    void SetScrollPos(int nPos);
public:
    bool ParseRawData(char* pData, int nLen);
    void AdjustLayerPosition();
    // 根据X坐标获取列号
    int GetColIndexByPosX(int x);
    // 根据Y坐标获取行号
    int GetRowIndexByPosY(int y);

    void DeleteLayerData();
};

#endif // UILAYERMANAGER_H

