//// BPMSettingDlg.cpp : ʵ���ļ�
////

//#include "stdafx.h"
//#include "BPMSettingDlg.h"
//#include "afxdialogex.h"


//// CBPMSettingDlg �Ի���

//IMPLEMENT_DYNAMIC(CBPMSettingDlg, CDialogEx)

//CBPMSettingDlg::CBPMSettingDlg(CWnd* pParent /*=NULL*/)
//	: CDialogEx(CBPMSettingDlg::IDD, pParent)
//{
//	m_nBpmValue = (int)(SecsPerMinute / 50);
//}

//CBPMSettingDlg::~CBPMSettingDlg()
//{
//}

//void CBPMSettingDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialogEx::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_EDIT_BPMVALUE, m_nBpmValue);
//	DWORD   dwStyle = GetDlgItem(IDC_EDIT_BPMVALUE)->GetStyle();
//	dwStyle |= ES_NUMBER;
//	SetWindowLong(GetDlgItem(IDC_EDIT_BPMVALUE)->m_hWnd, GWL_STYLE, dwStyle);
//}


//BEGIN_MESSAGE_MAP(CBPMSettingDlg, CDialogEx)
//	ON_BN_CLICKED(IDOK, &CBPMSettingDlg::OnBnClickedOk)
//END_MESSAGE_MAP()


//// CBPMSettingDlg ��Ϣ�������


//void CBPMSettingDlg::OnBnClickedOk()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	CString str;
//	GetDlgItemText(IDC_EDIT_BPMVALUE,str);
//	m_nBpmValue = _wtoi(str);
//	CDialogEx::OnOK();
//}
