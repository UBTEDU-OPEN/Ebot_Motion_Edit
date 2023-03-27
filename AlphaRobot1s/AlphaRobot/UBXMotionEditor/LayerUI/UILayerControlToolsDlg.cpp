//// UILayerControlToolsDlg.cpp : 实现文件
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "UILayerControlToolsDlg.h"
//#include "afxdialogex.h"


//// CUILayerControlToolsDlg 对话框

//IMPLEMENT_DYNAMIC(CUILayerControlToolsDlg, CDialog)

//CUILayerControlToolsDlg::CUILayerControlToolsDlg(QWidget* pParent /*=NULL*/)
//	: CDialog(CUILayerControlToolsDlg::IDD, pParent)
//	, m_pEvent(NULL)
//{

//}

//CUILayerControlToolsDlg::~CUILayerControlToolsDlg()
//{
//}

//void CUILayerControlToolsDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_LAYER_CONTROL_TOOLS_ADD, m_btnAddLayer);
//	DDX_Control(pDX, IDC_LAYER_CONTROL_TOOLS_DELETE, m_btnDeleteLayer);
//}


//BEGIN_MESSAGE_MAP(CUILayerControlToolsDlg, CDialog)
//	ON_WM_SIZE()
//	ON_BN_CLICKED(IDC_LAYER_CONTROL_TOOLS_ADD, OnBtnAddLayerClick)
//END_MESSAGE_MAP()


//// CUILayerControlToolsDlg 消息处理程序


//BOOL CUILayerControlToolsDlg::OnInitDialog()
//{
//	CDialog::OnInitDialog();

//	m_btnAddLayer.SetIcon(CSize(16,16), IDI_ICON_ADD_LAYER);
//	m_btnAddLayer.SetTooltip(_T("增加层"));
//	m_btnAddLayer.SetFlatStyle();
//	m_btnAddLayer.SetWindowPos(NULL, 0, 0, 16, 16, SWP_SHOWWINDOW);

//	m_btnDeleteLayer.SetIcon(CSize(16,16), IDI_ICON_DELETE_LAYER);
//	m_btnDeleteLayer.SetTooltip(_T("移除层"));
//	m_btnDeleteLayer.SetFlatStyle();
//	m_btnDeleteLayer.SetWindowPos(NULL, 0, 0, 16, 16, SWP_SHOWWINDOW);

//	m_dlgAddLayer.m_pEvnet = m_pEvent;

//	return TRUE;  // return TRUE unless you set the focus to a control
//	// 异常: OCX 属性页应返回 FALSE
//}


//void CUILayerControlToolsDlg::OnSize(UINT nType, int cx, int cy)
//{
//	CDialog::OnSize(nType, cx, cy);

//	if (m_btnAddLayer.GetSafeHwnd() == NULL)
//		return;

//	m_btnAddLayer.SetWindowPos(NULL, 2, 0, 0, 0, SWP_NOSIZE);
//	m_btnDeleteLayer.SetWindowPos(NULL, 24, 0, 0, 0, SWP_NOSIZE);
//}

//void CUILayerControlToolsDlg::OnBtnAddLayerClick()
//{
//	m_dlgAddLayer.m_pEvnet = m_pEvent;
//	if (m_dlgAddLayer.DoModal() == IDOK)
//	{
//		//m_dlgAddLayer.GetSelectLayer(&pIDS, nCount);

//		for(int i=0; i<m_dlgAddLayer.m_nSelectCount; i++)
//		{
//			if (m_pEvent)
//			{
//				m_pEvent->LayerControlToolsEvent_AddLayer(m_dlgAddLayer.m_pIDs[i]);
//			}
//		}
//	}
//}
