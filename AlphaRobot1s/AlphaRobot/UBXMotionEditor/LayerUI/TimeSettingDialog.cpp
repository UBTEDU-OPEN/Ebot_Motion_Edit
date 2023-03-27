//// TimeSettingDialog.cpp : 实现文件
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "TimeSettingDialog.h"
//#include "afxdialogex.h"


//// CTimeSettingDialog 对话框

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


//// CTimeSettingDialog 消息处理程序


//void CTimeSettingDialog::OnBnClickedOk()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	QString str;
//// 	GetDlgItem(IDC_TIME_SETTING_EDIT)->GetWindowText(str);
//// 	m_TimeScaleValue = str;

//	GetDlgItemText(IDC_TIME_SETTING_EDIT, str);
//	m_TimeScaleValue = _wtoi(str);

//	CDialogEx::OnOK();
//}
