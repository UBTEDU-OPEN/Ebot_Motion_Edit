//// TimeSettingDialog.cpp : ʵ���ļ�
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "TimeSettingDialog.h"
//#include "afxdialogex.h"


//// CTimeSettingDialog �Ի���

//IMPLEMENT_DYNAMIC(CTimeSettingDialog, CDialogEx)

//CTimeSettingDialog::CTimeSettingDialog(QWidget* pParent /*=NULL*/)
//	: CDialogEx(CTimeSettingDialog::IDD, pParent)
//{

//	m_TimeScaleValue = 50;
//}

//CTimeSettingDialog::~CTimeSettingDialog()
//{
//}

//void CTimeSettingDialog::DoDataExchange(CDataExchange* pDX)
//{
//	CDialogEx::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_TIME_SETTING_EDIT, m_TimeScaleValue);
//	DWORD   dwStyle = GetDlgItem(IDC_TIME_SETTING_EDIT)->GetStyle();
//	dwStyle |= ES_NUMBER;
//	SetWindowLong(GetDlgItem(IDC_TIME_SETTING_EDIT)->m_hWnd, GWL_STYLE, dwStyle);
//	//m_EditPos.SetLimitText(MAX_ANGLE);
//}


//BEGIN_MESSAGE_MAP(CTimeSettingDialog, CDialogEx)
//	ON_BN_CLICKED(IDOK, &CTimeSettingDialog::OnBnClickedOk)
//END_MESSAGE_MAP()


//// CTimeSettingDialog ��Ϣ�������


//void CTimeSettingDialog::OnBnClickedOk()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	QString str;
//// 	GetDlgItem(IDC_TIME_SETTING_EDIT)->GetWindowText(str);
//// 	m_TimeScaleValue = str;

//	GetDlgItemText(IDC_TIME_SETTING_EDIT, str);
//	m_TimeScaleValue = _wtoi(str);

//	CDialogEx::OnOK();
//}
