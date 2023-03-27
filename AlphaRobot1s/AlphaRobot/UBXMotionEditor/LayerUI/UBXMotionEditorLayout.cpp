//// UBXMotionEditorLayout.cpp : 实现文件
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "UBXMotionEditorLayout.h"
//#include "afxdialogex.h"
//#include "ILayerUI.h"
//#include "IUILayerManager.h"
//#include "ILayerOptionHandler.h"
//#include "UIMusicLayer.h"

//bool CreateIActionLayerManager(IActionLayerManager** ppManager)
//{
//	CUBXMotionEditorLayout* pProxy = new CUBXMotionEditorLayout();
//	if (pProxy)
//	{
//		*ppManager = pProxy;

//		return true;
//	}

//	return false;
//}

//// CUBXMotionEditorLayout 对话框

//IMPLEMENT_DYNAMIC(CUBXMotionEditorLayout, CDialog)

//CUBXMotionEditorLayout::CUBXMotionEditorLayout(QWidget* pParent /*=NULL*/)
//	: CDialog(CUBXMotionEditorLayout::IDD, pParent)
//	, m_pMainLayer(NULL)
//	, m_pLayerControl(NULL)
//	, m_pSocketSession(NULL)
//{

//}

//CUBXMotionEditorLayout::~CUBXMotionEditorLayout()
//{
//}

//void CUBXMotionEditorLayout::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//}

//bool CUBXMotionEditorLayout::Connect(QWidget* pWnd)
//{
//	if (Create(CUBXMotionEditorLayout::IDD, pWnd) == false)
//		return false;

//	ShowWindow(SW_SHOW);
//	return true;
//}
//void CUBXMotionEditorLayout::ILayerManager_EnablePlay(int nLayerType ,bool bState)
//{
//	if(m_pLayerControl)
//	{
//		m_pLayerControl->LayerControlToolsEvent_EnablePlay(nLayerType , bState);
//	}
//}
//// void CUBXMotionEditorLayout::EnableControlPlay(int nLayerType , bool bState)
//// {
//// 	if (m_pLayerControl)
//// 	{
//// 	}
//// }
//void CUBXMotionEditorLayout::ReleaseConnection()
//{


//	if (m_pMainLayer)
//	{
//		m_pMainLayer->ReleaseConnection();
//		m_pMainLayer = NULL;
//	}

//	if (m_pLayerControl)
//	{
//		m_pLayerControl->ReleaseConnection();
//		m_pLayerControl = NULL;
//	}

//	if (m_pLayerOption)
//	{
//		m_pLayerOption->ReleaseConnection();
//		m_pLayerOption = NULL;
//	}
//	if (GetSafeHwnd())
//	{
//		DestroyWindow();
//	}
//	delete this;
//}


//BEGIN_MESSAGE_MAP(CUBXMotionEditorLayout, CDialog)
//	ON_WM_SIZE()
//	ON_WM_HSCROLL()
//	ON_MESSAGE(WM_SPLITTER_MOVED, OnSpliteMove)
//END_MESSAGE_MAP()


//// CUBXMotionEditorLayout 消息处理程序

//LRESULT CUBXMotionEditorLayout::OnSpliteMove(WPARAM wParam, LPARAM lParam)
//{
//	CRect rc;

//	GetClientRect(rc);

//	PostMessage(WM_SIZE, rc.Width(), rc.Height());

//	return 0;
//}


//bool CUBXMotionEditorLayout::OnInitDialog()
//{
//	__super::OnInitDialog();

//	CreateLayerManager(&m_pMainLayer);
//	m_pMainLayer->Connect(this, this);

//	CreateLayerControl(&m_pLayerControl);
//	m_pLayerControl->Connect(this, this);
//	CreateLayerOption(&m_pLayerOption);
//	m_pLayerOption->Connect(this);

//	m_SplitterWnd.m_nTop = 10;
//	m_SplitterWnd.m_nBottom = 10;
//	m_SplitterWnd.BindWithControl(this, IDC_STATIC_X);
//    m_SplitterWnd.AddUpWnd(m_pMainLayer->GetQWidget());
//    m_SplitterWnd.AddDownWnd(m_pLayerOption->GetQWidget());
//	m_SplitterWnd.SetBkColor(RGB(230, 231, 233));

//	return true;  // return true unless you set the focus to a control
//	// 异常: OCX 属性页应返回 false
//}


//void CUBXMotionEditorLayout::OnSize(uint nType, int cx, int cy)
//{
//	__super::OnSize(nType, cx, cy);

//    if (m_pMainLayer==NULL || m_pMainLayer->GetQWidget()->GetSafeHwnd()==NULL)
//		return;

//	CRect rc;
//	int	top = 0;

//    m_pMainLayer->GetQWidget()->GetWindowRect(rc);
//	top = rc.Height();

//	CRect rcControl;
//    m_pLayerControl->GetQWidget()->GetWindowRect(rcControl);

//	GetClientRect(rc);
//	cx = rc.Width();
//	cy = rc.Height();

//    m_pLayerControl->GetQWidget()->SetWindowPos(NULL, 0, 0, 115, top, SWP_SHOWWINDOW);
//    m_pMainLayer->GetQWidget()->SetWindowPos(NULL, 115, 0, cx-115, top, SWP_SHOWWINDOW);
//	m_SplitterWnd.SetWindowPos(NULL, 0, top, cx, 4, SWP_SHOWWINDOW);
//    m_pLayerOption->GetQWidget()->SetWindowPos(NULL, 0, top+4, cx, cy-top-4, 0);
//    //m_pShowAlphaInfo->GetQWidget()->SetWindowPos(NULL, 0, top+SPLITTER_CX, cx, cy-top-SPLITTER_CX, 0);
//}

//bool CUBXMotionEditorLayout::GetData(char** ppData, int &nLen)
//{
//	if (m_pMainLayer)
//	{
//		return m_pMainLayer->GetData(ppData, nLen);
//	}

//	return false;
//}
//void CUBXMotionEditorLayout::AdjustData()
//{
//	if (m_pMainLayer)
//	{
//		//return m_pMainLayer->SetData(pData, nLen);
//		m_pMainLayer->AdjustData();
//	}

//}
//bool CUBXMotionEditorLayout::SetData(char* pData, int nLen)
//{
//	if (m_pLayerControl)
//	{
//		m_pLayerControl->LayerControl_DeleteAllControl();
//	}

//	if (m_pLayerOption)
//	{
//		m_pLayerOption->ResetState();
//	}

//	if (m_pMainLayer)
//	{
//		return m_pMainLayer->SetData(pData, nLen);
//	}

//	return true;
//}

//void CUBXMotionEditorLayout::ILayerControlEvent_GetUnUseLayers(int **pLayerID, int& nLayerCount)
//{
//	if (m_pMainLayer)
//	{
//		return m_pMainLayer->LayerManager_GetUnUseLayers(pLayerID, nLayerCount);
//	}
//}

//QString CUBXMotionEditorLayout::ILayerControlEvent_GetLayerName(int nLayerID)
//{
//	if (m_pMainLayer)
//	{
//		return m_pMainLayer->LayerManager_GetLayerName(nLayerID);
//	}

//	return L"";
//}
//void CUBXMotionEditorLayout::LayerControlEvent_AddMusic()
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_AddMusic();
//	}
//}
//void CUBXMotionEditorLayout::ILayerControlEvent_AddLayer(int nLayerID)
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_AddLayer(nLayerID);
//	}
//}

//void CUBXMotionEditorLayout::ILayerControlEvent_AddLayerControl(int nLayerType)
//{
//	if (m_pLayerControl)
//	{
//		m_pLayerControl->LayerControlToolsEvent_AddLayerControl(nLayerType);
//	}
//}

//void CUBXMotionEditorLayout::LayerControlEvent_DeleteLayer(int nLayerID)
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_DeleteLayer(nLayerID);
//	}
//}

//void CUBXMotionEditorLayout::LayerControlEvent_SetTimeScaleValue(int timeScale)
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_SetTimeScale(timeScale);
//	}
//}

//int CUBXMotionEditorLayout::LayerControlEvent_GetTimeScaleValue()
//{
//	if (m_pMainLayer)
//	{
//		return m_pMainLayer->LayerManager_GetTimeScaleValue();
//	}

//	return -1;
//}
//void CUBXMotionEditorLayout::ILayerManager_SetMusicLayerSourceHandler(ILayerOptionHandler* pHandler , int nLayerType , void *pData , int nDataLen , int nDataID)
//{
//	if (m_pLayerOption)
//	{
//		//m_pLayerOption->SetOptionHandler(pHandler);
//		m_pLayerOption->SetMusicOptionSources(pHandler,nLayerType,pData , nDataLen , nDataID);
//	}
//	if (m_pMainLayer)
//	{
//	}
//}
//void CUBXMotionEditorLayout::ILayerManager_DeleteMusciFile(ILayerOptionHandler* pHandler)
//{
//	if (m_pLayerOption)
//	{
//		m_pLayerOption->DeleteMusicResource(pHandler);
//	}
//}
//void CUBXMotionEditorLayout::ActionLayerManager_SetMusicResource(const TCHAR *szFilePath)
//{
//// 	if(m_pLayerOption->GetOptionHandler())
//// 	{
//// 		QString str;
//// 		str.Format(QObject::tr("%s"),szFilePath);
//// 		ILayerOptionHandler *pOptionHandler= m_pLayerOption->GetOptionHandler();
//// 		//pOptionHandler->SetMusicResource(str);
//// 	}
//	if (m_pMainLayer)
//	{
//        QString str;
//        str.Format(QObject::tr("%s"),szFilePath);
//// 		ILayerOptionHandler *pOptionHandler= m_pLayerOption->GetOptionHandler();
//// 		pOptionHandler->SetMusicResource(str);
//		m_pMainLayer->LayerManager_SetMusicResource(str);
//	}
//}
//void CUBXMotionEditorLayout::ILayerManager_SetLayOptionHandler(ILayerOptionHandler* pHandler)
//{
//	if (m_pLayerOption)
//	{
//		m_pLayerOption->SetOptionHandler(pHandler);
//	}
//}

//void CUBXMotionEditorLayout::ILayerManager_OnDeletOptionHandler(ILayerOptionHandler* pHandler)
//{
//	if (m_pLayerOption)
//	{
//		m_pLayerOption->OnDeletOptionHandler(pHandler);
//	}
//}

//void CUBXMotionEditorLayout::ILayerManager_SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID)
//{
//	if (m_pLayerOption)
//	{
//		m_pLayerOption->SetActionBlockDataManager(pManager, nCurrentGroupID,nCurrentActionID);
//	}
//}

//void CUBXMotionEditorLayout::ActionLayerManager_Set3DRobotViewInterface(IUBXRobot3DView* pRobotView)
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_Set3DRobotViewInterface(pRobotView);
//	}
//}

//void CUBXMotionEditorLayout::ActionLayerManager_SetTreeView(ITreeNavigation* pView)
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_SetTreeView(pView);
//	}
//}

//void CUBXMotionEditorLayout::ActionLayerManager_TreeToView(char *pData, int nLen, QPoint* nPoint, QString FileName)
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_TreeToView(pData, nLen, nPoint, FileName);
//	}
//}
//void CUBXMotionEditorLayout::ActionLayerManager_SetFileModified(bool bState)
//{
//	ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(ACTION_LAYER_MOTION);
//	if (pLayer == NULL)
//		return ;
//	IUILayerManager* pLayerManager = pLayer->GetLayerManager();
//	if(pLayerManager)
//	{
//		//pLayerManager->IUILayerManager_SetModifiedState(true);
//		//bMofied = pLayerManager->IUILayerManager_GetModifiedState();
//		pLayerManager->IUILayerManager_SetModifiedState(bState);
//	}
//}
//bool CUBXMotionEditorLayout::ActionLayerManager_IsRobotDebuging()
//{
//	ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(ACTION_LAYER_MOTION);
//	if (pLayer == NULL)
//		return false;
//	IUILayerManager* pLayerManager = pLayer->GetLayerManager();
//	bool bMofied =false;
//	if(pLayerManager)
//	{
//		//pLayerManager->IUILayerManager_SetModifiedState(true);
//		bMofied = pLayerManager->IUILayerManager_IsRobotDebuging();
//	}
//	return bMofied;
//}
//bool CUBXMotionEditorLayout::ActionLayerManager_IsFileModified()
//{
//	ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(ACTION_LAYER_MOTION);
//	if (pLayer == NULL)
//		return false;
//	IUILayerManager* pLayerManager = pLayer->GetLayerManager();
//	bool bMofied =false;
//	if(pLayerManager)
//	{
//		//pLayerManager->IUILayerManager_SetModifiedState(true);
//		bMofied = pLayerManager->IUILayerManager_GetModifiedState();
//	}
//	return bMofied;
//}

//void CUBXMotionEditorLayout::LayerControlEvent_LayerPlaying(int layerType)
//{
//	if (m_pMainLayer)
//	{
//		ILayerOptionHandler* optionHandle = m_pLayerOption->GetOptionHandler();
//		m_pMainLayer->LayerManager_LayerPlaying(layerType, optionHandle);
//	}
//}
//void CUBXMotionEditorLayout::LayerControlEvent_LayerStop()
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_LayerStop();
//	}
//}
//void CUBXMotionEditorLayout::LayerControlEvent_StopLayerDebug()
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_StopLayerDebug();
//	}
//}

//void CUBXMotionEditorLayout::ILayerManager_LayerDebugStop(int layerType)
//{
//    if (m_pLayerControl && m_pLayerControl->GetQWidget())
//	{
//		m_pLayerControl->LayerControl_LayerDebugStop(layerType);
//	}
//}

//ILayerOptionHandler* CUBXMotionEditorLayout::ILayerManager_GetOptionHandler()
//{
//	return m_pLayerOption->GetOptionHandler();
//}


//void CUBXMotionEditorLayout::ActionLayerManager_SetSocketSession(CIUBXSocketSession* pSession)
//{
//	m_pSocketSession = pSession;
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_SetSocketSession(pSession);
//	}
//}
//void CUBXMotionEditorLayout::LayerControlEvent_SetMusicSource(int nLayerType , void *pData , int nDataLen , int nDataID)
//{
//	if (m_pMainLayer == NULL)
//	{
//		return;
//	}
//	ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(nLayerType);
//	if (pLayer == NULL)
//		return;

//	IUILayerManager* pLayerManager = pLayer->GetLayerManager();
//	if (pLayerManager == NULL)
//		return;
//	pLayerManager->IUILayerManager_SetMusicFile(nLayerType,pData,nDataLen,nDataID);
//}
//void CUBXMotionEditorLayout::LayerControlEvent_SetCurrentOption(int layerType)
//{
//	if (m_pMainLayer == NULL)
//		return;

//	ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(layerType);
//	if (pLayer == NULL)
//		return;

//	IUILayerManager* pLayerManager = pLayer->GetLayerManager();
//	if (pLayerManager == NULL)
//		return;

//	pLayerManager->IUILayerManager_SwitchLayer(layerType);
//}

//void CUBXMotionEditorLayout::ActionLayerManager_UpdateScrollBar()
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_UpdateScrollBar();
//	}
//}
//void CUBXMotionEditorLayout::ActionLayerManager_ReleaseMusicSource()
//{
//	if (m_pMainLayer)
//	{
//		//m_pMainLayer->LayerManager_StopLayerDebug();
//		//m_pMainLayer->la
//		m_pMainLayer->LayerManager_ReleaseMusic();
//	}
//}
//void CUBXMotionEditorLayout::ActionLayerManager_StopDebug()
//{
//	if (m_pMainLayer)
//	{
//		m_pMainLayer->LayerManager_StopLayerDebug();
//	}
//}
