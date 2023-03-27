//// UIMusicLayerControl.cpp : 实现文件
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "UIMusicLayerControl.h"
//#include "afxdialogex.h"
//#include "UILayerControlManager.h"


//// CUIMusicLayerControl 对话框

//IMPLEMENT_DYNAMIC(CUIMusicLayerControl, CDialog)

//CUIMusicLayerControl::CUIMusicLayerControl(CWnd* pParent /*=NULL*/)
//	: CDialog(CUIMusicLayerControl::IDD, pParent)
//	, m_PlayState(PLAYSTATE_STOP)
//{
//	//int m_MusicLength = 0;
//	m_pMP3Session = NULL;
//}

//CUIMusicLayerControl::~CUIMusicLayerControl()
//{
//}

//void CUIMusicLayerControl::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	//	DDX_Control(pDX, IDC_BUTTON_DELETE_MUSIC, m_deleteMusic);
//	DDX_Control(pDX, IDC_BUTTON_DEBUG_MUSIC, m_debugMusic);
//	DDX_Control(pDX, IDC_BUTTON_DEBUG_STOP, m_btnStop);
//	DDX_Control(pDX, IDC_BUTTON_ADDMUSIC, m_btnAddMusic);
//}


//BEGIN_MESSAGE_MAP(CUIMusicLayerControl, CDialog)
////	ON_BN_CLICKED(IDC_BUTTON_DELETE_MUSIC, &CUIMusicLayerControl::OnBnClickedButtonDeleteMusic)
//	ON_BN_CLICKED(IDC_BUTTON_DEBUG_MUSIC, &CUIMusicLayerControl::OnBnClickedButtonDebugMusic)
//	ON_WM_PAINT()
//	ON_BN_CLICKED(IDC_BUTTON_DEBUG_STOP, &CUIMusicLayerControl::OnBnClickedButtonDebugStop)
//	ON_BN_CLICKED(IDC_BUTTON_ADDMUSIC, &CUIMusicLayerControl::OnBnClickedButtonAddmusic)
//END_MESSAGE_MAP()


//// CUIMusicLayerControl 消息处理程序


////void CUIMusicLayerControl::OnBnClickedButtonDeleteMusic()
////{
//// 	CUILayerControlManager* dlg = (CUILayerControlManager*)GetParent();
//// 	dlg->DeleteLayerControl(ACTION_LAYER_MUSIC);

//// 	CString strUBXFileEx = _T("UBX Files(*.mp3)|*.mp3||");
//// 	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |OFN_ALLOWMULTISELECT|OFN_ENABLESIZING,strUBXFileEx,NULL);
//// 	dlg.m_ofn.lpstrTitle = _T("Choose a mp3 file");
//// 	dlg.m_ofn.nMaxFile = 10 * MAX_PATH;
//// 	dlg.m_ofn.lpstrFile = new TCHAR[dlg.m_ofn.nMaxFile];
//// 	memset(dlg.m_ofn.lpstrFile , 0 , sizeof(TCHAR)*dlg.m_ofn.nMaxFile);
////
//// 	CString strFilename;
////
//// 	if (IDOK == dlg.DoModal())
//// 	{
//// 		POSITION pos_file;
//// 		pos_file = dlg.GetStartPosition();
//// 		while(pos_file != NULL)
//// 		{
//// 			strFilename = dlg.GetNextPathName(pos_file);
//// 		}
//// 	}
//// 	delete []dlg.m_ofn.lpstrFile;
////
//// 	if (!strFilename.IsEmpty())
//// 	{
//// 		if (m_pMP3Session->isSongInTheQuene() )
//// 		{
//// 			//此处可能需要清除音乐层的绘图，并删除之前的音乐层
//// 			if (!m_pMP3Session->AddSong(strFilename.GetBuffer(0)))
//// 			{
//// 				MessageBox(strFilename + language_loadstring(IDS_MUSIC_FILEERROR));
//// 				return;
//// 			}
//// 		}
//// 		else
//// 		{
//// 			if (!m_pMP3Session->Connect(strFilename.GetBuffer(0)))
//// 			{
//// 				MessageBox(strFilename + language_loadstring(IDS_MUSIC_FILEERROR));
//// 				return;
//// 			}
//// 		}
//// 		CUILayerControlManager* dlg = (CUILayerControlManager*)GetParent();
//// 		int nBPM , nLength = 0;
//// 		TCHAR *pFileTitle = NULL;
//// 		if(m_pMP3Session->GetSongInfo(nBPM,nLength,&pFileTitle))
//// 		{
//// 			dlg->SetTimeScaleValue((int)(SecsPerMinute / nBPM));
//// 		}
//// 		else
//// 		{
//// 			MessageBox(strFilename + language_loadstring(IDS_MUSIC_READINFOERROR));
//// 			return;
//// 		}
//// 		UBXStreamTime hmsTime;
//// 		m_pMP3Session->GetStreamTime(&hmsTime);
//// 		CString strFileNameWithExt =_T("NULL");
//// // 		TCHAR *szDrive = NULL;
//// // 		TCHAR *szDir = NULL;
//// // 		TCHAR *szFileName = NULL;
//// // 		TCHAR *szFileExt = NULL;
//// // 		_tsplitpath(strFilename,szDrive,szDir,szFileName,szFileExt);
//// // 		strFileNameWithExt.Format(_T("%s.%s"),szFileName,szFileExt);
//// 		int nPos = strFilename.ReverseFind(_T('\\'));
//// 		if (nPos >= 0)
//// 		{
//// 			strFileNameWithExt = strFilename.Right(strFilename.GetLength() - nPos - 1);
//// 		}
////
//// 		UBXMusicInfo MusicInfo;
//// 		MusicInfo.nBPM = nBPM;
//// 		MusicInfo.szFileName = strFileNameWithExt;
//// 		MusicInfo.szFilePath = strFilename;
//// 		MusicInfo.nLength = sizeof(MusicInfo); //便于日后统计结构体信息在传输中是否有丢失
//// 		MusicInfo.StreamTime = hmsTime;
//// 		dlg->SetMusicFile(ACTION_LAYER_MUSIC,(void*)&MusicInfo,MusicInfo.nLength,1);
//// 	}
////}


//void CUIMusicLayerControl::OnBnClickedButtonDebugMusic()
//{
////	m_deleteMusic.EnableWindow(FALSE);
//	CUILayerControlManager* dlg = (CUILayerControlManager*)GetParent();
//	//dlg->SetSelectedOption(ACTION_LAYER_MUSIC);

//	if (m_PlayState == PLAYSTATE_PLAYING)
//	{
//		m_PlayState = PLAYSTATE_PAUSING;
//		m_debugMusic.SetTooltip(language_loadstring(IDS_STRING_LAYERCONTROL_DEBUG));
//		dlg->StopLayerDebug();
//		m_debugMusic.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//	}
//	else if(m_PlayState != PLAYSTATE_PLAYING)
//	{
//		if (m_btnAddMusic)
//		{
//			m_btnAddMusic.EnableWindow(FALSE);
//		}
//		m_debugMusic.EnableWindow(FALSE);
//		m_btnStop.EnableWindow(FALSE);
//		m_PlayState = PLAYSTATE_PLAYING;     //暂停
//		dlg->LayerPlaying(ACTION_LAYER_MUSIC);
//		m_debugMusic.EnableWindow(TRUE);
//		m_btnStop.EnableWindow(TRUE);
//		m_debugMusic.SetIcon(CSize(16,16), IDI_ICON_PAUSE);
//		m_debugMusic.SetTooltip(language_loadstring(IDS_STRING_LAYERCONTROL_PAUSE));
//		dlg->m_bMusicPlaying = TRUE;
//	}
//}
//void CUIMusicLayerControl::OnBnClickedButtonDebugStop()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	CUILayerControlManager* dlg = (CUILayerControlManager*)GetParent();
//	//停止需要把光标重新放到起始位置（0点或者是原来设置的开始播放时间）
//	if(m_PlayState != PLAYSTATE_STOP)
//	{
//		m_debugMusic.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//		m_debugMusic.SetTooltip(language_loadstring(IDS_STRING_LAYERCONTROL_DEBUG));
//		m_PlayState = PLAYSTATE_STOP;     //停止播放
//		dlg->LayerStop();
//	}

//	//设置光标位置
//}

//void CUIMusicLayerControl::OnBnClickedButtonAddmusic()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	CUILayerControlManager* pParentDlg = (CUILayerControlManager*)GetParent();
//	if (pParentDlg)
//	{
//		pParentDlg->AddMusic();
//	}
//}
//BOOL CUIMusicLayerControl::OnInitDialog()
//{
//	__super::OnInitDialog();

//// 	m_deleteMusic.SetIcon(CSize(16,16), IDI_ICON_DELETE_LAYER);
//// 	m_deleteMusic.SetTooltip(_T("删除层"));
//// 	m_deleteMusic.SetFlatStyle();

//	m_debugMusic.SetIcon(CSize(16, 16), IDI_ICON_DEBUG);
//	m_debugMusic.SetTooltip(language_loadstring(IDS_STRING_LAYERCONTROL_DEBUG));
//	m_debugMusic.SetFlatStyle();
//	m_btnStop.SetIcon(CSize(16,16),IDI_ICON_STOP);
//	m_btnStop.SetTooltip(language_loadstring(IDS_STRING_LAYERCONTROL_STOP));
//	m_btnStop.SetFlatStyle();
//	m_btnAddMusic.SetIcon(CSize(24,24),IDI_ICON_MUSIC);
//	m_btnAddMusic.SetFlatStyle(TRUE);
//	/*CreateUBXMp3Session(&m_pMP3Session);*/
//	return TRUE;  // return TRUE unless you set the focus to a control
//	// 异常: OCX 属性页应返回 FALSE
//}

//int CUIMusicLayerControl::GetType()
//{
//	return ACTION_LAYER_MUSIC;
//}

//CWnd* CUIMusicLayerControl::GetCWnd()
//{
//	return this;
//}

//bool CUIMusicLayerControl::Connect(CWnd* pParent)
//{
//	if (Create(CUIMusicLayerControl::IDD, pParent) == FALSE)
//		return false;
//	SetWindowPos(NULL, 0, 0, 100, 22, SWP_SHOWWINDOW);

//	return true;
//}
//void CUIMusicLayerControl::ReleaseConnection()
//{
//	if (m_pMP3Session)
//	{
//		m_pMP3Session->Release();
//		m_pMP3Session = NULL;
//	}
//	delete this;
//}
//void CUIMusicLayerControl::EnablePlay(bool bState)
//{
//	if (m_btnStop.GetSafeHwnd())
//	{
//		m_btnStop.EnableWindow(bState);
//	}
//	if (m_debugMusic.GetSafeHwnd())
//	{
//		m_debugMusic.EnableWindow(bState);
//	}
//	if (m_btnAddMusic.GetSafeHwnd())
//	{
//		m_btnAddMusic.EnableWindow(bState);
//	}
//}
//void CUIMusicLayerControl::LayerDebugStop()
//{
//	m_debugMusic.SetIcon(CSize(16,16), IDI_ICON_DEBUG);
//	m_PlayState = PLAYSTATE_STOP;
////	m_deleteMusic.EnableWindow(TRUE);
//	if (m_btnAddMusic)
//	{
//		m_btnAddMusic.EnableWindow(TRUE);
//	}

//}

//void CUIMusicLayerControl::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting

//	CRect   rc;
//	GetClientRect( &rc );// 获取客户区
//	dc.FillSolidRect(&rc, RGB(250, 250, 220));

//	CDialog::OnPaint();
//}

//void CUIMusicLayerControl::OnOK()
//{
//	// TODO: 在此添加专用代码和/或调用基类

//	//__super::OnOK();
//}

//void CUIMusicLayerControl::OnCancel()
//{
//	// TODO: 在此添加专用代码和/或调用基类

//	//__super::OnCancel();
//}


