//#pragma once
////#include "afxwin.h"
////#include "resource.h"
//#include "stable.h"
//class ILayoutTools_Event
//{
//public:
//	virtual void OnSrcollChange(int nPos) = 0;
//	virtual void EnablePlay(int nLayerType , bool bState) = 0;
//};
//// CUIMainLayerLayoutToolsDlg 对话框

//class IUILayerManager;
//class CUIMainLayerLayoutToolsDlg
//{
////	DECLARE_DYNAMIC(CUIMainLayerLayoutToolsDlg)

//public:
//    CUIMainLayerLayoutToolsDlg(QWidget* pParent = NULL);   // 标准构造函数
//	virtual ~CUIMainLayerLayoutToolsDlg();

//// 对话框数据
//	ILayoutTools_Event* m_pEvent;
//	IUILayerManager*	m_pLayerManager;

//    void ResetHScroll(QWidget* pWnd, int nMax);
//	void ShowStatus(int nCurrentCell, int nScales);
//    void SetHScrollPos(QWidget* pWnd, int nMax,int nPos);

//private:
//	bool m_bMusicPlayState;
//	bool m_bMotionPlayState;
//protected:

//public:
//    virtual bool OnInitDialog();
//	void StopTotalDebug(int nlayerType);
//	void LayertoolsStop();
//	virtual void OnOK();
//};
