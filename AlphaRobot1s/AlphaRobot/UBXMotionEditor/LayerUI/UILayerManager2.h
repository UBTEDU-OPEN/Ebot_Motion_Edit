#pragma once
//#include "afxwin.h"

#include "IUILayerManager.h"
#include "IGroupDataPlay.h"
#include "MusicChooseDialog.h"
#include "stable.h"

const uint ID_FLOWGRAPH_CONTROL = 1354;

class CUILayerManager :public QWidget, public IUILayerManager, public IGroupDataPlayEvent
{
public:
    CUILayerManager(void);
    ~CUILayerManager(void);

//	IUBXRobot3DView* Get3dView(){return m_pRobot3dView;};

public:
    virtual bool Connect(QWidget* pWidget, IUILayerManager_Event* pEvent);
    virtual void ReleaseConnection();
    virtual QWidget* GetWidget();
    virtual void OnDraw(/*CDC* pDC*/);
    virtual bool SetData(char* pData, int nLen);
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
    virtual IUBXRobot3DView* Get3DRobotViewInterface() {return m_pRobot3dView;};
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
    virtual void LayerStop();
    virtual void LayerDebugStop(int layerType);
    virtual void PlayEvent_OnDebugStop(int nLayerType);
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
    virtual void IUILayerManager_SingleBlockPlaying(int nLayerType ,CActionBlockData *pActionData) ;
    virtual void IUILayerManager_SingleGroupPlaying(int nLayerType , CActionBlockGroupManager* pGroupManager);
    virtual void IUILayerManager_SingleBlockPlayingStop(int nLayerType);

//	DECLARE_DYNCREATE( CUILayerManager )

    virtual bool Create( QString lpszClassName, QString lpszWindowName, ulong dwStyle, const QRect& rect, QWidget* pParentWnd, uint nID/*, QCreateContext* pContext = NULL */);
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
    bool	m_IsMusicPlaying;
    bool    m_IsDataPlaying;
    bool    m_bModified;
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


//public:

//	DECLARE_MESSAGE_MAP()
//	afx_msg void OnPaint();
//	afx_msg void OnHScroll(uint nSBCode, uint nPos, CScrollBar* pScrollBar);
//	afx_msg void OnLButtonDown(uint nFlags, QPoint point);
//	afx_msg void OnLButtonUp(uint nFlags, QPoint point);
//	afx_msg void OnSize(uint nType, int cx, int cy);
//	afx_msg void OnLButtonDblClk(uint nFlags, QPoint point);
//	afx_msg void OnRButtonDown(uint nFlags, QPoint point);
//	afx_msg void OnMouseMove(uint nFlags, QPoint point);
//	afx_msg void OnTimer(uint_PTR nIDEvent);
//	virtual bool PreTranslateMessage(MSG* pMsg);
};



