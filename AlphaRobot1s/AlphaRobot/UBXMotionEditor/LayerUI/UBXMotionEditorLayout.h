//#pragma once

//#include "resource.h"
//// CUBXMotionEditorLayout 对话框

//#include "IActionLayerManager.h"
//#include "ILayerManager.h"
//#include "ILayerControl.h"
//#include "ILayerOption.h"

//class CActionBlockDataManager;
//class CUBXMotionEditorLayout : public CDialog, public IActionLayerManager, public ILayerControlEvent, public ILayerManager_Event
//{
//	DECLARE_DYNAMIC(CUBXMotionEditorLayout)

//public:
//    CUBXMotionEditorLayout(QWidget* pParent = NULL);   // 标准构造函数
//	virtual ~CUBXMotionEditorLayout();

//// 对话框数据
//	enum { IDD = IDD_DIALOG_MOTIONEDITOR_NEW };

//private:
//	ILayerManager*			m_pMainLayer;
//	ILayerControl*			m_pLayerControl;
//	ILayerOption*			m_pLayerOption;
//	CXSplitterWnd			m_SplitterWnd;
//	CIUBXSocketSession*		m_pSocketSession;
//	bool                    m_bModified;

//public:
//	// IActionLayerManager接口实现
//    virtual bool Connect(QWidget* pWnd);
//	virtual void ReleaseConnection();
//    virtual QWidget* GetQWidget() {return this;};
//	virtual bool GetData(char** ppData, int &nLen);
//	virtual bool SetData(char* pData, int nLen);
//	virtual void AdjustData();
//    virtual void ActionLayerManager_Set3DRobotViewInterface(IUBXRobot3DView* pRobotView);
//	virtual void ActionLayerManager_SetSocketSession(CIUBXSocketSession* pSession);
//	virtual void ActionLayerManager_SetMusicResource(const TCHAR *szFilePath);
//	virtual void ActionLayerManager_UpdateScrollBar();
//	virtual void ActionLayerManager_StopDebug();
//	virtual void ActionLayerManager_ReleaseMusicSource() ;
//	virtual void ActionLayerManager_SetTreeView(ITreeNavigation* pView);
//    virtual void ActionLayerManager_TreeToView(char *pData, int nLen, QPoint* nPoint, QString FileName);
//	virtual bool ActionLayerManager_IsFileModified();
//	virtual bool ActionLayerManager_IsRobotDebuging() ;
//	virtual void ActionLayerManager_SetFileModified(bool bState);
//public:
//	// ILayerControlEvent 接口实现
//	virtual void ILayerControlEvent_GetUnUseLayers(int **pLayerID, int& nLayerCount);
//    virtual QString ILayerControlEvent_GetLayerName(int nLayerID);
//	virtual void ILayerControlEvent_AddLayer(int nLayerID);
//	virtual void ILayerControlEvent_AddLayerControl(int nLayerType);
//	virtual void LayerControlEvent_DeleteLayer(int nLayerID);
//	virtual void LayerControlEvent_SetTimeScaleValue(int timeScale);
//	virtual int LayerControlEvent_GetTimeScaleValue();

//	virtual void LayerControlEvent_LayerPlaying(int layerType);
//	virtual void LayerControlEvent_AddMusic() ;
//	virtual void LayerControlEvent_StopLayerDebug();
//	virtual void LayerControlEvent_LayerStop() ;
//	virtual void LayerControlEvent_SetCurrentOption(int layerType);
//	virtual void LayerControlEvent_SetMusicSource(int nLayerType , void *pData , int nDataLen , int nDataID);

//public:
//	// ILayerManager_Event 接口实现
//	virtual void ILayerManager_SetLayOptionHandler(ILayerOptionHandler* pHandler);
//	virtual void ILayerManager_SetMusicLayerSourceHandler(ILayerOptionHandler* pHandler , int nLayerType , void *pData , int nDataLen , int nDataID);
//	virtual void ILayerManager_SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID);

//	virtual void ILayerManager_LayerDebugStop(int layerType);
//	virtual void ILayerManager_DeleteMusciFile(ILayerOptionHandler* pHandler) ;
//	virtual ILayerOptionHandler* ILayerManager_GetOptionHandler();
//	virtual void ILayerManager_OnDeletOptionHandler(ILayerOptionHandler* pHandler);
//	virtual void ILayerManager_EnablePlay(int nLayerType ,bool bState)  ;
//	//virtual void ILayerManager_EnablePlay(int nLayerType , bool bState)  ;
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

//	DECLARE_MESSAGE_MAP()
//public:
//	virtual bool OnInitDialog();
//    afx_msg void OnSize(uint nType, int cx, int cy);
//	afx_msg LRESULT OnSpliteMove(WPARAM wParam, LPARAM lParam);
//};
