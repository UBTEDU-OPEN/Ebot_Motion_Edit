//// UIMotionLayerControl.cpp : ʵ���ļ�
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "UIMotionLayerControl.h"
//#include "afxdialogex.h"
//#include "UILayerControlManager.h"

//// CUIMotionLayerControl �Ի���

//IMPLEMENT_DYNAMIC(CUIMotionLayerControl, CDialog)

//CUIMotionLayerControl::CUIMotionLayerControl(CWnd* pParent /*=NULL*/)
//	: CDialog(CUIMotionLayerControl::IDD, pParent)
//	, m_PlayState(PLAYSTATE_STOP)
//{

//}

//CUIMotionLayerControl::~CUIMotionLayerControl()
//{
//}

//void CUIMotionLayerControl::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	//	DDX_Control(pDX, IDC_BUTTON_DELETE_MOTION, m_deleteLayer);
//	DDX_Control(pDX, IDC_BUTTON_DEBUG_MOTION, m_debugMotion);
//	DDX_Control(pDX, IDC_BUTTON_STOP_MOTION, m_btnStop);
//}


//BEGIN_MESSAGE_MAP(CUIMotionLayerControl, CDialog)
////	ON_BN_CLICKED(IDC_BUTTON_DELETE_MOTION, &CUIMotionLayerControl::OnBnClickedButtonDeleteMotion)
//	ON_BN_CLICKED(IDC_BUTTON_DEBUG_MOTION, &CUIMotionLayerControl::OnBnClickedButtonDebugMotion)
//	ON_WM_PAINT()
//	ON_BN_CLICKED(IDC_BUTTON_STOP_MOTION, &CUIMotionLayerControl::OnBnClickedButtonStopMotion)
//END_MESSAGE_MAP()


//// CUIMotionLayerControl ��Ϣ�������


//// void CUIMotionLayerControl::OnBnClickedButtonDeleteMotion()
//// {
//// }

//void CUIMotionLayerControl::EnablePlay(bool bState)
//{
//	if (m_debugMotion.GetSafeHwnd())
//	{
//		m_debugMotion.EnableWindow(bState);
//	}
//	if (m_btnStop.GetSafeHwnd())
//	{
//		m_btnStop.EnableWindow(bState);
//	}
//}
//void CUIMotionLayerControl::OnBnClickedButtonDebugMotion()
//{
//	CUILayerControlManager* dlg = (CUILayerControlManager*)GetParent();
//	dlg->SetSelectedOption(ACTION_LAYER_MOTION);
//	if (m_PlayState == PLAYSTATE_PLAYING)
//	{
//		m_debugMotion.EnableWindow(FALSE);
//		m_PlayState = PLAYSTATE_PAUSING;
//		dlg->StopLayerDebug();
//		m_debugMotion.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//		m_debugMotion.SetTooltip(language_loadstring(IDS_STRING_LAYERCONTROL_DEBUG));
//		m_debugMotion.EnableWindow(true);
//	}
//	else
//	{
//		dlg->StopLayerDebug();
//		m_btnStop.EnableWindow(FALSE);
//		m_debugMotion.EnableWindow(FALSE);
//		m_debugMotion.SetIcon(CSize(16,16), IDI_ICON_PAUSE);
//		m_debugMotion.SetTooltip(language_loadstring(IDS_STRING_LAYERCONTROL_PAUSE));

//		m_PlayState = PLAYSTATE_PLAYING;
//		dlg->LayerPlaying(ACTION_LAYER_MOTION);
//		m_btnStop.EnableWindow(TRUE);
//		m_debugMotion.EnableWindow(TRUE);
//		dlg->m_bDataPlaying = TRUE;
//	}
//}


//BOOL CUIMotionLayerControl::OnInitDialog()
//{
//	__super::OnInitDialog();

//// 	m_deleteLayer.SetIcon(CSize(16,16), IDI_ICON_DELETE_LAYER);
//// 	m_deleteLayer.SetTooltip(_T("ɾ����"));
//// 	m_deleteLayer.SetFlatStyle();

//	m_debugMotion.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//	m_debugMotion.SetTooltip(language_loadstring(IDS_STRING_LAYERCONTROL_DEBUG));
//	m_debugMotion.SetFlatStyle();
//	m_btnStop.SetIcon(CSize(16,16), IDI_ICON_STOP);
//	m_btnStop.SetTooltip(language_loadstring(IDS_STRING_LAYERCONTROL_STOP));
//	m_btnStop.SetFlatStyle();
//	return TRUE;  // return TRUE unless you set the focus to a control
//	// �쳣: OCX ����ҳӦ���� FALSE
//}

//int CUIMotionLayerControl::GetType()
//{
//	return ACTION_LAYER_MOTION;
//}

//CWnd* CUIMotionLayerControl::GetCWnd()
//{
//	return this;
//}

//bool CUIMotionLayerControl::Connect(CWnd* pParent)
//{
//	if (Create(CUIMotionLayerControl::IDD, pParent) == FALSE)
//		return false;

//	SetWindowPos(NULL, 0, 0, 100, 22, SWP_SHOWWINDOW);

//	return true;
//}
//void CUIMotionLayerControl::ReleaseConnection()
//{
//	delete this;
//}

//void CUIMotionLayerControl::LayerDebugStop()
//{
//	m_debugMotion.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//	m_PlayState = PLAYSTATE_STOP;
//}

//void CUIMotionLayerControl::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting

//	CRect   rc;
//	GetClientRect( &rc );// ��ȡ�ͻ���
//	dc.FillSolidRect(&rc, RGB(250, 250, 220));

//	CDialog::OnPaint();
//}

//void CUIMotionLayerControl::OnOK()
//{
//	// TODO: �ڴ����ר�ô����/����û���

//	//__super::OnOK();
//}


//void CUIMotionLayerControl::OnCancel()
//{
//	// TODO: �ڴ����ר�ô����/����û���

//	//__super::OnCancel();
//}


//void CUIMotionLayerControl::OnBnClickedButtonStopMotion()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	CUILayerControlManager* dlg = (CUILayerControlManager*)GetParent();
//	//ֹͣ��Ҫ�ѹ�����·ŵ���ʼλ�ã�0�������ԭ�����õĿ�ʼ����ʱ�䣩
//	if(m_PlayState != PLAYSTATE_STOP)
//	{
//		m_debugMotion.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//		m_debugMotion.SetTooltip(language_loadstring(IDS_STRING_LAYERCONTROL_DEBUG));
//		m_PlayState = PLAYSTATE_STOP;     //ֹͣ����
//		dlg->LayerStop();
//	}
//}
