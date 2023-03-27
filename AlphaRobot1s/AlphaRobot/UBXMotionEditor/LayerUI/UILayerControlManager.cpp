//// UILayerControlManager.cpp : 实现文件
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "UILayerControlManager.h"
//#include "afxdialogex.h"
//#include "UILayerControl.h"


//// CUILayerControlManager 对话框

//IMPLEMENT_DYNAMIC(CUILayerControlManager, CDialog)

//CUILayerControlManager::CUILayerControlManager(QWidget* pParent /*=NULL*/)
//	: CDialog(CUILayerControlManager::IDD, pParent)
//	, m_bDataPlaying(FALSE)
//	, m_bMusicPlaying(FALSE)
//{
//}

//CUILayerControlManager::~CUILayerControlManager()
//{
//	if (GetSafeHwnd())
//	{
		
//		DestroyWindow();
//	}

//	DeleteAllLayerControl();
////	delete this;
//}

//void CUILayerControlManager::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//}


//BEGIN_MESSAGE_MAP(CUILayerControlManager, CDialog)
//	ON_WM_SIZE()
//	ON_WM_PAINT()
//END_MESSAGE_MAP()


//// CUILayerControlManager 消息处理程序


//BOOL CUILayerControlManager::OnInitDialog()
//{
//	CDialog::OnInitDialog();

//	return TRUE;  // return TRUE unless you set the focus to a control
//	// 异常: OCX 属性页应返回 FALSE
//}


//void CUILayerControlManager::OnSize(UINT nType, int cx, int cy)
//{
//	CDialog::OnSize(nType, cx, cy);

//	IUILayerControl* itemFound = NULL;
//	for (int i = 0; i < m_listActionLayer.GetCount(); i++)
//	{
//		itemFound = m_listActionLayer.GetAt(m_listActionLayer.FindIndex(i));
//		if (itemFound->GetQWidget() != NULL)
//		{
//			if (itemFound->GetQWidget()->GetSafeHwnd() != NULL)
//			{
//				CRect controlRC;
//				itemFound->GetQWidget()->GetClientRect(controlRC);
//				int nHeight = 19;
//				int nYPos = 0;
//				if(i == 0)
//				{
//					nHeight = 19;
//					nYPos = 20;
//				}
//				else if (i == 1)
//				{
//					nHeight = 69;
//					nYPos = 50;
//				}
//				else if (i == 2)
//				{
//					nHeight = 19;
//					nYPos = 120;
//				}
//				itemFound->GetQWidget()->SetWindowPos(NULL, 3, nYPos, controlRC.Width() + 20, nHeight, SWP_SHOWWINDOW);
//			}
//		}
//	}
//}

//bool CUILayerControlManager::AddLayerControl(int nLayerID)
//{
//	CAutolockRes res;
//	IUILayerControl* pLayerControl = NULL;
//	if (nLayerID == ACTION_LAYER_TIME)
//	{
//		pLayerControl = new CUITimeLayerControl();
//		pLayerControl->Connect(this);
//	}
//	else if (nLayerID == ACTION_LAYER_MOTION)
//	{
//		pLayerControl = new CUIMotionLayerControl();
//		pLayerControl->Connect(this);
//	}
//// 	else if (nLayerID == ACTION_LAYER_EYE)
//// 	{
//// 		pLayerControl = new CUIEyesLayerControl();
//// 		pLayerControl->Connect(this);
//// 	}
//// 	else if (nLayerID == ACTION_LAYER_EAR)
//// 	{
//// 		pLayerControl = new CUIEarsLayerControl();
//// 		pLayerControl->Connect(this);
//// 	}
//	else if (nLayerID == ACTION_LAYER_MUSIC)
//	{
//		pLayerControl = new CUIMusicLayerControl();
//		pLayerControl->Connect(this);
//	}

//	return AddLayerControl(pLayerControl);
//}

//bool CUILayerControlManager::AddLayerControl(IUILayerControl* pLayerControl)
//{
//	IUILayerControl* itemFound = NULL;

//	if (pLayerControl == NULL)
//		return false;

//	for (int i=0; i<m_listActionLayer.GetCount(); i++)
//	{
//		itemFound = m_listActionLayer.GetAt(m_listActionLayer.FindIndex(i));
//		if (itemFound && itemFound->GetType() == pLayerControl->GetType())
//		{
//			return false;
//		}
//	}

//	m_listActionLayer.AddTail(pLayerControl);
//	CRect rc;
//	GetClientRect(rc);
//	PostMessage(WM_SIZE, rc.Width(), rc.Height());
////	Invalidate(FALSE);
//	return true;
//}

//bool CUILayerControlManager::DeleteLayerControl(int nTypeID)
//{
//	CUILayerControl* pDlg = (CUILayerControl*)GetParent();
//	pDlg->LayerControlToolsEvent_DeleteLayer(nTypeID);

//	IUILayerControl* itemFound = NULL;
//	for (int i=0; i<m_listActionLayer.GetCount(); i++)
//	{
//		itemFound = m_listActionLayer.GetAt(m_listActionLayer.FindIndex(i));
//		if (itemFound && itemFound->GetType() == nTypeID)
//		{
//			itemFound->ReleaseConnection();
//			m_listActionLayer.RemoveAt(m_listActionLayer.FindIndex(i));

//			CRect rc;
//			GetClientRect(rc);
//			PostMessage(WM_SIZE, rc.Width(), rc.Height());
//			return true;
//		}
//	}

////	Invalidate(FALSE);
//	return false;
//}

//void CUILayerControlManager::DeleteAllLayerControl()
//{
//	for (int i=0; i<m_listActionLayer.GetCount(); i++)
//	{
//		IUILayerControl* pcui = m_listActionLayer.GetAt(m_listActionLayer.FindIndex(i));
//		if (pcui)
//		{
//			pcui->ReleaseConnection();
////			delete pcui;
//			//m_mapActionLayer.RemoveAt(m_mapActionLayer.FindIndex(i));
//		}
//	}
//	m_listActionLayer.RemoveAll();
//}

//void CUILayerControlManager::SetTimeScaleValue(int timeScale)
//{
//	CUILayerControl* pDlg = (CUILayerControl*)GetParent();
//	pDlg->LayerControlToolsEvent_SetTimeScaleValue(timeScale);
//}

//int CUILayerControlManager::GetTimeScaleValue()
//{
//	CUILayerControl* pDlg = (CUILayerControl*)GetParent();
//	return pDlg->LayerControlToolsEvent_GetTimeScaleValue();
//}

//void CUILayerControlManager::LayerPlaying(int layerType)
//{
//	IUILayerControl* itemFound = NULL;
//	itemFound = m_listActionLayer.GetAt(m_listActionLayer.FindIndex(0));
//	itemFound->GetQWidget()->EnableWindow(FALSE);

//	CUILayerControl* pDlg = (CUILayerControl*)GetParent();
//// 	if (m_IsPlaying)
//// 	{
//// 		pDlg->LayerControl_StopLayerDebug();
//// 	}
//	if (pDlg)
//	{
//		pDlg->LayerControl_LayerPlaying(layerType);
//	}
//}
//void CUILayerControlManager::AddMusic()
//{
//	CUILayerControl* pParentDlg = (CUILayerControl*)GetParent();
//	if (pParentDlg)
//	{
//		pParentDlg->AddMusic();
//	}
//}
//void CUILayerControlManager::EnablePlay(int nLayerType ,bool bState)
//{
//	int nType = 0;
//	if (nLayerType == ACTION_LAYER_TIME)
//	{
//		nType = 0;
//	}
//	else if (nLayerType == ACTION_LAYER_MOTION)
//	{
//		nType = 2;
//	}
//	else if (nLayerType == ACTION_LAYER_MUSIC)
//	{
//		nType =1;
//	}
//	else
//		return;
//	for (int i= 0 ;i<m_listActionLayer.GetCount();i++)
//	{
//		if (i != nType)
//		{
//			continue;
//		}
//		IUILayerControl *pControl = m_listActionLayer.GetAt(m_listActionLayer.FindIndex(i));
//		if (pControl)
//		{
//			pControl->EnablePlay(bState);
//		}
//	}
//}
//void CUILayerControlManager::LayerStop()
//{
//	m_bDataPlaying = FALSE;
//	m_bMusicPlaying =FALSE;
//	CUILayerControl* pDlg = (CUILayerControl*)GetParent();
//	//pDlg->LayerControl_StopLayerDebug();
//	if (pDlg)
//	{
//		pDlg->LayerControl_LayerStop();
//	}
//}
//void CUILayerControlManager::StopLayerDebug()
//{
//	m_bDataPlaying = FALSE;
//	m_bMusicPlaying = FALSE;
//	CUILayerControl* pDlg = (CUILayerControl*)GetParent();
//	pDlg->LayerControl_StopLayerDebug();
//}

//void CUILayerControlManager::LayerDebugStop(int layerType)
//{
//	if (layerType == ACTION_LAYER_MOTION)
//	{
//		m_bDataPlaying = FALSE;
//	}
//	else if(layerType == ACTION_LAYER_MUSIC)
//	{
//	    m_bMusicPlaying = FALSE;
//	}
//	IUILayerControl* itemFound = NULL;
//	for (int i=0; i<m_listActionLayer.GetCount(); i++)
//	{
//		itemFound = m_listActionLayer.GetAt(m_listActionLayer.FindIndex(i));
//		if (itemFound && itemFound->GetType() == layerType)
//		{
//			itemFound->LayerDebugStop();
//		}
//		if (itemFound && itemFound->GetType() == 0)
//		{
//			itemFound->LayerDebugStop();
//		}
//	}
//}
//void CUILayerControlManager::EnableControl(int bState)
//{
//	for (int i = 0 ;i<m_listActionLayer.GetCount();i++ )
//	{
//		IUILayerControl *pControl = m_listActionLayer.GetAt(m_listActionLayer.FindIndex(i));
//		if (pControl)
//		{
//			pControl->EnablePlay(bState);
//		}
//		//m_listActionLayer
//	}
//}
//void CUILayerControlManager::SetSelectedOption(int layerType)
//{
//	CUILayerControl* pDlg = (CUILayerControl*)GetParent();
//	pDlg->LayerControl_SetCurrentOption(layerType);
//}
//void CUILayerControlManager::SetMusicFile(int nLayerType , void *pData , int nDataLen , int nDataID)
//{
//	CUILayerControl* pDlg = (CUILayerControl*)GetParent();
//	//pDlg->LayerControl_SetCurrentOption(layerType);
//	if (pDlg)
//	{
//		pDlg->LayerControl_SetMusicSource(nLayerType , pData , nDataLen , nDataID);
//	}
//}

//void CUILayerControlManager::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
	
//	CRect   rc;
//	GetClientRect( &rc );// 获取客户区
//	dc.FillSolidRect(&rc, RGB(250, 250, 220));

//	CDialog::OnPaint();
//}
