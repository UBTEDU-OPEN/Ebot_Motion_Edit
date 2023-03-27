//#pragma once

//#include "resource.h"
//#include "UIMotionLayerControl.h"
//// #include "UIEarsLayerControl.h"
//// #include "UIEyesLayerControl.h"
//#include "UIMusicLayerControl.h"
//#include "UITimeLayerControl.h"
//#include "IUILayerManager.h"
//#include "IGroupDataManagerPlay.h"
//#include "stdafx.h"

//// CUILayerControlManager 对话框

//class CUILayerControlManager : public CDialog
//{
//	DECLARE_DYNAMIC(CUILayerControlManager)

//public:
//	CUILayerControlManager(QWidget* pParent = NULL);   // 标准构造函数
//	virtual ~CUILayerControlManager();

//// 对话框数据
//	enum { IDD = IDD_DIALOG_LAYER_UI_MANAGER };

//public:
//	bool AddLayerControl(int nLayerID);
//	bool AddLayerControl(IUILayerControl* pLayerControl);
//	bool DeleteLayerControl(int nTypeID);
//	void DeleteAllLayerControl();
//	void SetTimeScaleValue(int timeScale);
//	int  GetTimeScaleValue();
//	void EnablePlay(int nLayerType , bool bState);
//	void LayerPlaying(int layerType);
//	void AddMusic();
//	void LayerStop();
//	void StopLayerDebug();
//	void LayerDebugStop(int layerType);
//	void SetSelectedOption(int layerType);
//	void EnableControl(int bState);
//	void SetMusicFile(int nLayerType , void *pData , int nDataLen , int nDataID);

////public:
////	virtual void LayerDebugStop(int layerType);

//protected:
//	CList<IUILayerControl*, IUILayerControl*> m_listActionLayer;

//public:
//	BOOL m_bDataPlaying;
//	BOOL m_bMusicPlaying;

//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

//	DECLARE_MESSAGE_MAP()
//public:
//	virtual BOOL OnInitDialog();
//	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnPaint();
//};
