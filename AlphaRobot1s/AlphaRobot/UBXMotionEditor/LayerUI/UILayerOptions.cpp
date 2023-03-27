//// UILayerOptions.cpp : 实现文件
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "UILayerOptions.h"
//#include "afxdialogex.h"
//#include "ILayerOptionHandler.h"
//#include "OptionsMusicDlg.h"

//bool CreateLayerOption(ILayerOption** ppOption)
//{
//	CUILayerOptions* pProxy = new CUILayerOptions();
//	if (pProxy)
//	{
//		*ppOption = pProxy;

//		return true;
//	}

//	return false;
//}
//// CUILayerOptions 对话框

//IMPLEMENT_DYNAMIC(CUILayerOptions, CDialog)

//CUILayerOptions::CUILayerOptions(CWnd* pParent /*=NULL*/)
//	: CDialog(CUILayerOptions::IDD, pParent)
//	, m_pOptionHandler(NULL)
//	, m_pGroupManager(NULL)
//{

//}

//CUILayerOptions::~CUILayerOptions()
//{
//}

//void CUILayerOptions::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//}


//BEGIN_MESSAGE_MAP(CUILayerOptions, CDialog)
//	ON_WM_SIZE()
//END_MESSAGE_MAP()


//bool CUILayerOptions::Connect(CWnd* pParent)
//{
//	if (Create(CUILayerOptions::IDD, pParent))
//	{
//		ShowWindow(SW_SHOW);

//		m_pOptionHandler = NULL;
//		return true;
//	}

//	return false;
//}

//void CUILayerOptions::ReleaseConnection()
//{
//	if (GetSafeHwnd())
//	{
//		DestroyWindow();
//	}

//	delete this;
//}

//CWnd* CUILayerOptions::GetCWnd()
//{
//	return this;
//}

//void CUILayerOptions::SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID)
//{
//	m_pGroupManager = pManager;
//	if (m_pOptionHandler)
//	{
//		m_pOptionHandler->SetActionBlockDataManager(pManager, nCurrentGroupID,nCurrentActionID);
//	}
//}

//void CUILayerOptions::ResetState()
//{
//	m_pOptionHandler = NULL;
//}

//void CUILayerOptions::OnDeletOptionHandler(ILayerOptionHandler* pHandler)
//{
//	if (m_pOptionHandler == pHandler)
//		m_pOptionHandler = NULL;
//}
//void CUILayerOptions::DeleteMusicResource(ILayerOptionHandler* pHandler)
//{
//	COptionsMusicDlg *pDlg = (COptionsMusicDlg*)(pHandler->GetCWnd());
//	if (pDlg)
//	{
//		//pDlg->AddLayerBlock1S(pData , nDataLen , nDataID);
//		pDlg->DeleteMusicFile();
//	}
//}
//void CUILayerOptions::SetMusicOptionSources(ILayerOptionHandler* pHandler , int nLayerType , void *pData , int nDataLen , int nDataID)
//{
//// 	if (m_pOptionHandler == pHandler)
//// 		return;
//	m_pOptionHandler = pHandler;
//	COptionsMusicDlg *pDlg = (COptionsMusicDlg*)(pHandler->GetCWnd());
//	if (pDlg)
//	{
//		pDlg->AddLayerBlock1S(pData , nDataLen , nDataID);
//	}
//}
//void CUILayerOptions::SetOptionHandler(ILayerOptionHandler* pHandler)
//{
//	if (m_pOptionHandler == pHandler)
//		return;

//	if (m_pOptionHandler)
//	{
//		if (m_pOptionHandler->GetCWnd()->GetSafeHwnd())
//			m_pOptionHandler->GetCWnd()->ShowWindow(SW_HIDE);
//	}
//	m_pOptionHandler = pHandler;
//	if (m_pOptionHandler == NULL)
//		return;

//	m_pOptionHandler->GetCWnd()->SetParent(this);
//	//UpdateWindow();

//	PostMessage(WM_SIZE, 0, 0);
//}

//ILayerOptionHandler* CUILayerOptions::GetOptionHandler()
//{
//	return m_pOptionHandler;
//}

//// CUILayerOptions 消息处理程序


//void CUILayerOptions::OnSize(UINT nType, int cx, int cy)
//{
//	__super::OnSize(nType, cx, cy);

//	if (m_pOptionHandler == NULL)
//		return;

//	if (m_pOptionHandler->GetCWnd()->GetSafeHwnd() == NULL)
//		return;

//	CRect rcClient;
//	GetClientRect(rcClient);
//	m_pOptionHandler->GetCWnd()->SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height(), SWP_SHOWWINDOW);
//}

//void CUILayerOptions::OnOK()
//{
//	// TODO: 在此添加专用代码和/或调用基类

//	//__super::OnOK();
//}


//void CUILayerOptions::OnCancel()
//{
//	// TODO: 在此添加专用代码和/或调用基类

//	//__super::OnCancel();
//}
