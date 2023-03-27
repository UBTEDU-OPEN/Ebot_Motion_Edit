//#pragma once

//#include "resource.h"
//#include "UILayerControlImpl.h"
//#include "afxwin.h"
//// CUITimeLayerControl 对话框

//class CUITimeLayerControl : public CDialog, public CUILayerControlImpl
//{
//	DECLARE_DYNAMIC(CUITimeLayerControl)

//public:
//	CUITimeLayerControl(CWnd* pParent = NULL);   // 标准构造函数
//	virtual ~CUITimeLayerControl();

//// 对话框数据
//	enum { IDD = IDD_UI_TIME_LAYER_CONTROL };

//public:
//	virtual int GetType();
//	virtual CWnd* GetCWnd();
//	virtual bool Connect(CWnd* pParent);
//	virtual void ReleaseConnection();
//	virtual void LayerDebugStop();
//	virtual void EnablePlay( bool bState){};

//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

//	DECLARE_MESSAGE_MAP()
//public:
//	CXTPButton m_TimeScale;
//	afx_msg void OnBnClickedButtonSetTimeScale();
//	afx_msg void OnPaint();
//	virtual BOOL OnInitDialog();
//	virtual void OnOK();
//	virtual void OnCancel();
//};
