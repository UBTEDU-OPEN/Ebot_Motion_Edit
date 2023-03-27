//// UILayerControl.cpp : 实现文件
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "UILayerControl.h"
//#include "afxdialogex.h"


//bool CreateLayerControl(ILayerControl** ppLayerControl)
//{
//	CUILayerControl* pProxy = new CUILayerControl();
//	if (pProxy)
//	{
//		*ppLayerControl = pProxy;

//		return true;
//	}

//	return false;
//}
//// CUILayerControl 对话框

//IMPLEMENT_DYNAMIC(CUILayerControl, CDialog)

//CUILayerControl::CUILayerControl(QWidget* pParent /*=NULL*/)
//	: CDialog(CUILayerControl::IDD, pParent)
//	, m_pEvent(NULL)
//{

//}

//CUILayerControl::~CUILayerControl()
//{
//}

//void CUILayerControl::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//}


//BEGIN_MESSAGE_MAP(CUILayerControl, CDialog)
//	ON_WM_SIZE()
//END_MESSAGE_MAP()


//bool CUILayerControl::Connect(QWidget* pParent, ILayerControlEvent* pEvent)
//{
//	if (Create(CUILayerControl::IDD, pParent))
//	{
//		ShowWindow(SW_SHOW);
//		m_pEvent = pEvent;

//		m_dlgTools.m_pEvent = this;
//		return true;
//	}

//	return false;
//}

//void CUILayerControl::ReleaseConnection()
//{
//	if (GetSafeHwnd())
//	{
//		DestroyWindow();
//	}

//	delete this;
//}

//QWidget* CUILayerControl::GetQWidget()
//{
//	return this;
//}


//BOOL CUILayerControl::OnInitDialog()
//{
//	__super::OnInitDialog();

// 	m_controlManagerDlg.Create(CUILayerControlManager::IDD, this);
// 	m_controlManagerDlg.SetWindowPos(NULL, 0, 0, 125, 22, SWP_SHOWWINDOW);

//// 	m_dlgTools.Create(CUILayerControlToolsDlg::IDD, this);
//// 	m_dlgTools.SetWindowPos(NULL, 0, 0, 50, 22, SWP_SHOWWINDOW);

//	return TRUE;  // return TRUE unless you set the focus to a control
//	// 异常: OCX 属性页应返回 FALSE
//}


//void CUILayerControl::OnSize(UINT nType, int cx, int cy)
//{
//	__super::OnSize(nType, cx, cy);

//	if (m_controlManagerDlg.GetSafeHwnd() == NULL)
//		return;

//	CRect rcTools;
//// 	m_dlgTools.GetWindowRect(rcTools);
//// 	m_dlgTools.MoveWindow(0, cy-rcTools.Height(), cx, rcTools.Height());

//	m_controlManagerDlg.MoveWindow(0, 0, cx, cy - rcTools.Height());
//}

//void CUILayerControl::LayerControlToolsEvent_GetUnUseLayers(int **pLayerID, int& nLayerCount)
//{
//	if (m_pEvent)
//	{
//		return m_pEvent->ILayerControlEvent_GetUnUseLayers(pLayerID, nLayerCount);
//	}
//}

//QString CUILayerControl::LayerControlToolsEvent_GetLayerName(int nLayerID)
//{
//	if (m_pEvent)
//	{
//		return m_pEvent->ILayerControlEvent_GetLayerName(nLayerID);
//	}

//	return L"";
//}

//void CUILayerControl::LayerControlToolsEvent_AddLayer(int nLayerID)
//{
//	if (m_pEvent)
//	{
//		m_pEvent->ILayerControlEvent_AddLayer(nLayerID);
//	}
//}
//void CUILayerControl::LayerControlToolsEvent_EnablePlay(int nLayerType ,bool bState)
//{
//	m_controlManagerDlg.EnablePlay(nLayerType ,bState);
//}
//void CUILayerControl::LayerControlToolsEvent_AddLayerControl(int nLayerID)
//{
//	m_controlManagerDlg.AddLayerControl(nLayerID);
//}

//void CUILayerControl::LayerControl_DeleteAllControl()
//{
//	m_controlManagerDlg.DeleteAllLayerControl();
//}

//void CUILayerControl::LayerControlToolsEvent_DeleteLayer(int nLayerID)
//{
//	if (m_pEvent)
//	{
//		m_pEvent->LayerControlEvent_DeleteLayer(nLayerID);
//	}
//}

//void CUILayerControl::LayerControlToolsEvent_SetTimeScaleValue(int timeScale)
//{
//	if (m_pEvent)
//	{
//		m_pEvent->LayerControlEvent_SetTimeScaleValue(timeScale);
//	}
//}

//int CUILayerControl::LayerControlToolsEvent_GetTimeScaleValue()
//{
//	if (m_pEvent)
//	{
//		return m_pEvent->LayerControlEvent_GetTimeScaleValue();
//	}

//	return -1;
//}
//void CUILayerControl::AddMusic()
//{
//	if (m_pEvent)
//	{
//		m_pEvent->LayerControlEvent_AddMusic();
//	}
//}
//void CUILayerControl::LayerControl_LayerPlaying(int layerType)
//{
//	if (m_pEvent)
//	{
//		m_pEvent->LayerControlEvent_LayerPlaying(layerType);
//	}
//}
//void CUILayerControl::LayerControl_LayerStop()
//{
//	if (m_pEvent)
//	{
//		m_pEvent->LayerControlEvent_LayerStop();
//		//m_pEvent->
//	}
//}
//void CUILayerControl::LayerControl_StopLayerDebug()
//{
//	if (m_pEvent)
//	{
//		m_pEvent->LayerControlEvent_StopLayerDebug();
//	}
//}

//void CUILayerControl::LayerControl_LayerDebugStop(int layerType)
//{
//	if (m_controlManagerDlg.GetSafeHwnd())
//	{
//		m_controlManagerDlg.LayerDebugStop(layerType);
//	}
//}

//void CUILayerControl::LayerControl_SetCurrentOption(int layerType)
//{
//	if (m_pEvent)
//	{
//		m_pEvent->LayerControlEvent_SetCurrentOption(layerType);
//	}
//}
//void CUILayerControl::LayerControl_SetMusicSource(int nLayerType , void *pData , int nDataLen , int nDataID)
//{
//	if (m_pEvent)
//	{
//		m_pEvent->LayerControlEvent_SetMusicSource(nLayerType , pData , nDataLen ,nDataID);
//	}
//}
