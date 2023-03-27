//// UIMainLayerLayoutToolsDlg.cpp : 实现文件
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "UIMainLayerLayoutToolsDlg.h"
//#include "afxdialogex.h"
//#include "ILayerUI.h"
//#include "IUILayerManager.h"


//// CUIMainLayerLayoutToolsDlg 对话框

//IMPLEMENT_DYNAMIC(CUIMainLayerLayoutToolsDlg, CDialog)

//CUIMainLayerLayoutToolsDlg::CUIMainLayerLayoutToolsDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(CUIMainLayerLayoutToolsDlg::IDD, pParent)
//	, m_pEvent(NULL)
//	, m_pLayerManager(NULL)
//{
//	m_bMusicPlayState = false;
//	m_bMotionPlayState = false;
//}

//CUIMainLayerLayoutToolsDlg::~CUIMainLayerLayoutToolsDlg()
//{
//}

//void CUIMainLayerLayoutToolsDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_BTN_LAYERTOOLS_PLAY, m_btnPlay);
//	DDX_Control(pDX, IDC_BTN_LAYERTOOLS_STOP, m_btnStop);
//	DDX_Control(pDX, IDC_STATIC_CURRENTCELL, m_stcCurrentCell);
//	DDX_Control(pDX, IDC_STATIC_CURRENT_CELLTIME, m_stcCurentCellTime);
//	DDX_Control(pDX, IDC_STATIC_CURRENT_TIME, m_stcCurrentTime);
//	DDX_Control(pDX, IDC_SCROLLBAR_H, m_srcH);
//}


//BEGIN_MESSAGE_MAP(CUIMainLayerLayoutToolsDlg, CDialog)
//	ON_WM_SIZE()
////	ON_NOTIFY(NM_THEMECHANGED, IDC_SCROLLBAR_H, &CUIMainLayerLayoutToolsDlg::OnNMThemeChangedScrollbarH)
//	ON_WM_HSCROLL()
//	ON_BN_CLICKED(IDC_BTN_LAYERTOOLS_PLAY, &CUIMainLayerLayoutToolsDlg::OnBnClickedBtnLayertoolsPlay)
//	//ON_BN_CLICKED(IDC_BTN_LAYERTOOLS_STOP, &CUIMainLayerLayoutToolsDlg::LayertoolsStop)
//	ON_BN_CLICKED(IDC_BTN_LAYERTOOLS_STOP, &CUIMainLayerLayoutToolsDlg::OnBnClickedBtnLayertoolsStop)
//END_MESSAGE_MAP()


//// CUIMainLayerLayoutToolsDlg 消息处理程序


//BOOL CUIMainLayerLayoutToolsDlg::OnInitDialog()
//{
//	CDialog::OnInitDialog();

//	m_btnPlay.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//	m_btnPlay.SetTooltip(language_loadstring(IDS_STRING_MAINCONTROLTOOLS_DEBUG));
//	m_btnPlay.SetFlatStyle();
//	m_btnPlay.SetWindowPos(NULL, 0, 0, 16, 16,SWP_SHOWWINDOW);

//	m_btnStop.SetIcon(CSize(16,16), IDI_ICON_STOP);
//	m_btnStop.SetTooltip(language_loadstring(IDS_STRING_MAINCONTROLTOOLS_STOP));
//	m_btnStop.SetFlatStyle();
//	m_btnStop.SetWindowPos(NULL, 0, 0, 24, 16,SWP_SHOWWINDOW);

//	m_stcCurrentCell.SetWindowTextW(L"80");
//	m_stcCurentCellTime.SetWindowTextW(L"20 ms");
//	m_stcCurrentTime.SetWindowTextW(L"1.2 S");

//	ShowStatus(0, m_pLayerManager->GetTimeScaleValues());
//	return TRUE;  // return TRUE unless you set the focus to a control
//	// 异常: OCX 属性页应返回 FALSE
//}


//void CUIMainLayerLayoutToolsDlg::OnSize(UINT nType, int cx, int cy)
//{
//	CDialog::OnSize(nType, cx, cy);

//	if (m_btnPlay.GetSafeHwnd() == NULL)
//		return;

//	int xPos = 2;

//	CRect btnPlay;
//	m_btnPlay.GetClientRect(btnPlay);
//	m_btnPlay.SetWindowPos(NULL, xPos, 0, btnPlay.Width(), btnPlay.Height(), SWP_SHOWWINDOW);
//	xPos += btnPlay.Width()+6;

//	m_btnStop.GetClientRect(btnPlay);
//	m_btnStop.SetWindowPos(NULL, xPos, 0, btnPlay.Width(), btnPlay.Height(), SWP_SHOWWINDOW);
//	xPos += btnPlay.Width()+12;

//	m_stcCurrentCell.GetWindowRect(btnPlay);
//	m_stcCurrentCell.SetWindowPos(NULL, xPos, 0, btnPlay.Width(), btnPlay.Height(), SWP_SHOWWINDOW);
//	xPos += btnPlay.Width()+4;

//	m_stcCurentCellTime.GetWindowRect(btnPlay);
//	m_stcCurentCellTime.SetWindowPos(NULL, xPos, 0, btnPlay.Width(), btnPlay.Height(), SWP_SHOWWINDOW);
//	xPos += btnPlay.Width()+4;

//	m_stcCurrentTime.GetWindowRect(btnPlay);
//	m_stcCurrentTime.SetWindowPos(NULL, xPos, 0, btnPlay.Width(), btnPlay.Height(), SWP_SHOWWINDOW);
//	xPos += btnPlay.Width()+4;

//	m_srcH.GetWindowRect(btnPlay);
//	m_srcH.SetWindowPos(NULL, xPos, 0, cx-xPos, btnPlay.Height(), SWP_SHOWWINDOW);
//}
//void CUIMainLayerLayoutToolsDlg::SetHScrollPos(CWnd* pWnd, int nMax,int nPos)
//{
//	if (pWnd == NULL && (!pWnd->GetSafeHwnd()) && nPos < 0)
//	{
//		return;
//	}
//	CRect rc;
//	pWnd->GetClientRect(rc);

//	m_srcH.GetScrollInfo(&m_hSi );

//	m_hSi.cbSize = sizeof( SCROLLINFO );
//	m_hSi.nMin = 0;
//	m_hSi.nPage = 1/*ILayerUI::m_nCellWidth+ILayerUI::m_lineThick*/;
//	int nLineThick = m_pLayerManager->GetLineThick();
//	int nCellWidth = m_pLayerManager->GetCellWidth();
//	m_hSi.nMax = nMax - rc.Width()/(nLineThick+nCellWidth);
//	if (nPos <= nMax)
//	{
//		m_hSi.nPos = nPos;
//	}
//	else
//		m_hSi.nPos = nMax;
//	if (nMax < (rc.Width()/(nLineThick + nCellWidth)))
//	{
//// 		if (m_pEvent)
//// 		{
//// 			m_pEvent->OnSrcollChange(0);
//// 		}
//	}
//	m_srcH.SetScrollInfo(&m_hSi );
//}
//void CUIMainLayerLayoutToolsDlg::ResetHScroll(CWnd* pWnd, int nMax)
//{
//	if (pWnd == NULL)
//		return;

//	CRect rc;
//	pWnd->GetClientRect(rc);

//	m_srcH.GetScrollInfo(&m_hSi );

//	m_hSi.cbSize = sizeof( SCROLLINFO );
//	m_hSi.nMin = 0;
//	m_hSi.nPage = 1/*ILayerUI::m_nCellWidth+ILayerUI::m_lineThick*/;
//	int nLineThick = m_pLayerManager->GetLineThick();
//	int nCellWidth = m_pLayerManager->GetCellWidth();
	
//	m_hSi.nMax = nMax - rc.Width()/(nLineThick+nCellWidth)+1;
//	if (m_hSi.nMax < 0)
//	{
//		m_hSi.nMax = 0;
//	}
//	m_srcH.SetScrollInfo(&m_hSi );
	
//	if (nMax < (rc.Width()/(nLineThick + nCellWidth)))
//	{
//		if (m_pEvent)
//		{
//			m_pEvent->OnSrcollChange(0);
//		}
//	}
//}

////void CUIMainLayerLayoutToolsDlg::OnNMThemeChangedScrollbarH(NMHDR *pNMHDR, LRESULT *pResult)
////{
////	// 该功能要求使用 Windows XP 或更高版本。
////	// 符号 _WIN32_WINNT 必须 >= 0x0501。
////	// TODO: 在此添加控件通知处理程序代码
////
////	*pResult = 0;
////}


//void CUIMainLayerLayoutToolsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	int maxpos;
//	maxpos = pScrollBar->GetScrollLimit();
//	int curpos = pScrollBar->GetScrollPos();
//	switch(nSBCode)
//	{
//	case SB_THUMBPOSITION:
//		curpos = nPos;
//		break;
//// 	case SB_ENDSCROLL:
//// 		return;
//	case SB_LINEDOWN:
//		if (curpos < maxpos)
//			curpos++;
//		break;
//	case SB_LINEUP:
//		if (curpos > 0)
//			curpos--;
//		break;
//	case SB_THUMBTRACK:
//		curpos = nPos;
//		break;
//	}

//	pScrollBar->SetScrollPos(curpos);
//	if (m_pEvent)
//	{
//		m_pEvent->OnSrcollChange(/*m_hSi.nPos*/curpos);
//	}

//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
//}

//void CUIMainLayerLayoutToolsDlg::ShowStatus(int nCurrentCell, int nScales)
//{
//	CString Disp;
//	Disp.Format(L"%d", nCurrentCell);
//	m_stcCurrentCell.SetWindowText(Disp);

//	Disp.Format(L"%d ms", nScales);
//	m_stcCurentCellTime.SetWindowText(Disp);

//	Disp.Format(L"%0.2f s", (float)(ceil(nCurrentCell*nScales*1.0))/1000);
//	m_stcCurrentTime.SetWindowText(Disp);
//}


//void CUIMainLayerLayoutToolsDlg::OnBnClickedBtnLayertoolsPlay()
//{
//	if (!m_bMusicPlayState && !m_bMotionPlayState)
//	{
//		//如果当前层在仿真，先清除当前层的仿真状态
//		m_btnPlay.EnableWindow(FALSE);
//		if (m_pLayerManager)
//		{
//			m_pLayerManager->PauseTotalLayerDebug();
//		}
//		m_btnPlay.EnableWindow(TRUE);
//		m_btnPlay.SetTooltip(language_loadstring(IDS_STRING_MAINCONTROLTOOLS_STOP));
//		m_btnPlay.SetIcon(CSize(16,16), IDI_ICON_PAUSE);
//		m_bMusicPlayState = TRUE;
//		m_bMotionPlayState = TRUE;
//		if (m_pEvent)
//		{
//			m_pEvent->EnablePlay(ACTION_LAYER_TIME,false);
//			m_pEvent->EnablePlay(ACTION_LAYER_MUSIC,false);
//			m_pEvent->EnablePlay(ACTION_LAYER_MOTION,false);
			
//		}
//		m_pLayerManager->TotalLayerPlaying();
//	}
//	else
//	{
//		m_btnPlay.EnableWindow(FALSE);
//		if (m_pLayerManager)
//		{
//			m_pLayerManager->PauseTotalLayerDebug();
//		}
//		m_btnPlay.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//		m_btnPlay.SetTooltip(language_loadstring(IDS_STRING_MAINCONTROLTOOLS_DEBUG));
//		m_bMusicPlayState = FALSE;
//		m_bMotionPlayState = FALSE;
//       // LayertoolsStop();
//		m_btnPlay.EnableWindow(TRUE);
//	}
//}

//void CUIMainLayerLayoutToolsDlg::LayertoolsStop()
//{
//// 	if (m_pLayerManager)
//// 	{
//// 		m_pLayerManager->StopTotalLayerDebug();
//// 	}
//	if (m_pEvent)
//	{
//		m_pEvent->EnablePlay(ACTION_LAYER_TIME,true);
//		m_pEvent->EnablePlay(ACTION_LAYER_MUSIC,true);
//		m_pEvent->EnablePlay(ACTION_LAYER_MOTION,true);
//	}
//	m_bMusicPlayState = FALSE;
//	m_bMotionPlayState = FALSE;
//	m_btnPlay.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//	m_btnPlay.SetTooltip(language_loadstring(IDS_STRING_MAINCONTROLTOOLS_DEBUG));
//}

//void CUIMainLayerLayoutToolsDlg::StopTotalDebug(int nlayerType)
//{
//	if (nlayerType == ACTION_LAYER_MUSIC)
//	{
//		m_bMusicPlayState = false;
//	}
//	else if(nlayerType == ACTION_LAYER_MOTION)
//	{
//		m_bMotionPlayState = false;
//	}
//	if (!m_bMusicPlayState   && !m_bMotionPlayState)
//	{
//		//m_btnPlay.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//		LayertoolsStop();
//	}
	
//}


//void CUIMainLayerLayoutToolsDlg::OnOK()
//{
//	// TODO: 在此添加专用代码和/或调用基类

//	//CDialog::OnOK();
//}

//void CUIMainLayerLayoutToolsDlg::OnBnClickedBtnLayertoolsStop()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	m_btnPlay.EnableWindow(FALSE);
//	if (m_pLayerManager)
//	{
//		m_pLayerManager->StopTotalLayerDebug();
//	}
//	// LayertoolsStop();
//	m_btnPlay.EnableWindow(TRUE);
//}
