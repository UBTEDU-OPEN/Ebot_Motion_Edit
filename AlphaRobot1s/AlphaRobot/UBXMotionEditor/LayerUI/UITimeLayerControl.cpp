//// UITimeLayerControl.cpp : ʵ���ļ�
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "UITimeLayerControl.h"
//#include "afxdialogex.h"
//#include "TimeSettingDialog.h"
//#include "UILayerControlManager.h"

//// CUITimeLayerControl �Ի���

//IMPLEMENT_DYNAMIC(CUITimeLayerControl, CDialog)

//CUITimeLayerControl::CUITimeLayerControl(CWnd* pParent /*=NULL*/)
//	: CDialog(CUITimeLayerControl::IDD, pParent)
//{

//}

//CUITimeLayerControl::~CUITimeLayerControl()
//{
//}

//void CUITimeLayerControl::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_BUTTON_SET_TIME_SCALE, m_TimeScale);
//}


//BEGIN_MESSAGE_MAP(CUITimeLayerControl, CDialog)
//	ON_BN_CLICKED(IDC_BUTTON_SET_TIME_SCALE, &CUITimeLayerControl::OnBnClickedButtonSetTimeScale)
//	ON_WM_PAINT()
//END_MESSAGE_MAP()


//// CUITimeLayerControl ��Ϣ�������
//int CUITimeLayerControl::GetType()
//{
//	return ACTION_LAYER_TIME;
//}

//CWnd* CUITimeLayerControl::GetCWnd()
//{
//	return this;
//}

//bool CUITimeLayerControl::Connect(CWnd* pParent)
//{
//	if (Create(CUITimeLayerControl::IDD, pParent) == FALSE)
//		return false;

//	SetWindowPos(NULL, 0, 0, 100, 22, SWP_SHOWWINDOW);

//	return true;
//}
//void CUITimeLayerControl::ReleaseConnection()
//{
//		delete this;
//}

//void CUITimeLayerControl::OnBnClickedButtonSetTimeScale()
//{
//	CTimeSettingDialog m_dlg;

//	CUILayerControlManager* dlg = (CUILayerControlManager*)GetParent();
//	m_dlg.m_TimeScaleValue = dlg->GetTimeScaleValue();
//	UpdateData(FALSE);
//	if (IDOK == m_dlg.DoModal())
//	{
//		if (m_dlg.m_TimeScaleValue == 0)
//		{
//			MessageBox(language_loadstring(IDS_STRING_MUSIC_INVALIDSCALE));
//			return;
//		}
//		CUILayerControlManager* dlg = (CUILayerControlManager*)GetParent();
//		dlg->SetTimeScaleValue(m_dlg.m_TimeScaleValue);
//	}

//}


//BOOL CUITimeLayerControl::OnInitDialog()
//{
//	__super::OnInitDialog();

//	m_TimeScale.SetIcon(CSize(16,16), IDI_ICON_SETTING_RANGE);
//	//m_TimeScale.SetTooltip(_T("���ÿ̶�"));
//	m_TimeScale.SetFlatStyle();
//	m_TimeScale.EnableWindow(FALSE);
//	m_TimeScale.ShowWindow(SW_HIDE);

//	return TRUE;  // return TRUE unless you set the focus to a control
//	// �쳣: OCX ����ҳӦ���� FALSE
//}

//void CUITimeLayerControl::LayerDebugStop()
//{
//	this->EnableWindow(TRUE);
//}

//void CUITimeLayerControl::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting

//	CRect   rc;
//	GetClientRect( &rc );// ��ȡ�ͻ���
//	dc.FillSolidRect(&rc, RGB(250, 250, 220));

//	CDialog::OnPaint();
//}

//void CUITimeLayerControl::OnOK()
//{
//	// TODO: �ڴ����ר�ô����/����û���

//	//__super::OnOK();
//}


//void CUITimeLayerControl::OnCancel()
//{
//	// TODO: �ڴ����ר�ô����/����û���

//	//__super::OnCancel();
//}
