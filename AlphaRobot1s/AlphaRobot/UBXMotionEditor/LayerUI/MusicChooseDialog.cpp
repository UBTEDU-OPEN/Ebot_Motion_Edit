//#include "stdafx.h"
//#include "MusicChooseDialog.h"
//#include "OptionsMusicDlg.h"
//#include "BPMSettingDlg.h"
//#include "TimeSettingDialog.h"


////#include <vfw.h>
////#include "afxdialogex.h"

//// CAddBlock 对话框

//IMPLEMENT_DYNAMIC(CMusicChooseDialog, CDialog)

//// 	CMusicChooseDialog::CMusicChooseDialog(CWnd* pParent /*=NULL*/)
//// 	: CDialog(CMusicChooseDialog::IDD, pParent)
//CMusicChooseDialog::CMusicChooseDialog(CWnd* pParent, CString DescriptionInfo)
//: CDialogEx(CMusicChooseDialog::IDD, pParent)
//{
//	m_nStartTime = 0;
//	m_nEndTime = 0;
//	m_EditFilePath = _T("");
//	m_pMP3Session = NULL;
//	m_nOriginScale = 0;
//	//m_nOriginBPM = 0;
//	//m_nCurrentBPM = 0;
//	m_nCurrentScale = 10;
//	m_pEvent = NULL;
//	m_bReadFromAes = FALSE;
//}


//CMusicChooseDialog::~CMusicChooseDialog()
//{
//}

//void CMusicChooseDialog::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);

//// 	DDX_Text(pDX, IDC_EDIT_START_TIME, m_StartTime);
//// 	DDX_Text(pDX, IDC_EDIT_END_TIME, m_EndTime);
////  DDX_Text(pDX, IDC_EDIT_MUSIC_ROAD, m_EditFilePath);
//	DDX_Control(pDX,IDC_PROGRESS_LOADING,m_pbmLoading);
//}


//BEGIN_MESSAGE_MAP(CMusicChooseDialog, CDialog)
//	ON_BN_CLICKED(IDOK, &CMusicChooseDialog::OnBnClickedOk)
//	ON_BN_CLICKED(IDC_BUTTON_CHOOSE, &CMusicChooseDialog::OnBnClickedButton1)
////	ON_BN_CLICKED(IDC_BUTTON_MODIFYBPM , &CMusicChooseDialog::OnBnClickedModifyBPM)
////	ON_BN_CLICKED(IDC_BUTTON_MODIFYSCALE,&CMusicChooseDialog::OnBnClickedModifyScale)
//	ON_BN_CLICKED(IDC_BUTTON_DELETE , &CMusicChooseDialog::OnBnClickedDelete)
//    ON_BN_CLICKED(IDC_BUTTON_CANCEL , &CMusicChooseDialog::OnBnClickedCancel)
////	ON_BN_CLICKED(IDC_BUTTON_MUSIC_MODIFY, &CMusicChooseDialog::OnBnClickedModify)
////	ON_BN_CLICKED(IDC_BUTTON_MUSIC_INSERT, &CMusicChooseDialog::OnBnClickedInsert)
//    ON_MESSAGE(WM_ALPHA1S_MOTIONEDITOR_MUSICANALYZE,OnAnalyzeMusicFile)
//	ON_WM_CLOSE()
//	ON_WM_DESTROY()
//END_MESSAGE_MAP()


//// CAddBlock 消息处理程序
//LRESULT CMusicChooseDialog::OnAnalyzeMusicFile(WPARAM wParam , LPARAM lParam)
//{
//	switch(wParam)
//	{
//	case ANALYZE_STATE_WAVE_CANCELLED:
//		{
//			SetButtonEnableState(TRUE);
//			if(m_pbmLoading.GetSafeHwnd())
//			{
//				m_pbmLoading.SetWindowText(_T("Operation has been cancelled!"));
//				m_pbmLoading.SetMarquee(0,100);
//				m_pbmLoading.ShowWindow(SW_HIDE);
//			}
//			((CButton *)(GetDlgItem(IDC_BUTTON_CANCEL)))->ShowWindow(SW_HIDE);
//			if (m_pMP3Session)
//			{
//				m_pMP3Session->DisConnect();
//				m_pMP3Session->AddSong(m_MusicInfo.szFilePath);
//			}
//		}
//		break;
//	case ANALYZE_STATE_ERROR_ANALYZE:
//		{
//			SetButtonEnableState(TRUE);
//			((CButton *)(GetDlgItem(IDC_BUTTON_CANCEL)))->ShowWindow(SW_HIDE);
//			if(m_pbmLoading.GetSafeHwnd())
//			{
//				m_pbmLoading.SetWindowText(_T("Error in analyzing music file...."));
//				m_pbmLoading.SetMarquee(0,100);
//				m_pbmLoading.ShowWindow(SW_HIDE);
//			}
//			if (m_pMP3Session)
//			{
//				m_pMP3Session->DisConnect();
//				m_pMP3Session->AddSong(m_MusicInfo.szFilePath);
//			}
//			MessageBox(m_musicFilePath + language_loadstring(IDS_MUSIC_FILEERROR));
//			m_musicFilePath = _T("");
//			m_waveFilePath = _T("");
//			m_nOriginBPM  = 0;
//			m_nOriginScale = 0;
//		}
//		break;
//	case ANALYZE_STATE_BPM_ANALYZE:
//		{
//			if(m_pbmLoading.GetSafeHwnd())
//			{
//				m_pbmLoading.ShowWindow(SW_SHOW);
//				m_pbmLoading.SetMarquee(1,100);
//				m_pbmLoading.SetWindowText(language_loadstring(IDS_MUSIC_READING));
//			}
//		}
//		break;
//	case ANALYZE_STATE_BPM_ANALYZED:
//		{
//			int nBPM = lParam;
//			m_nOriginBPM = nBPM;
//		}
//		break;
//	case ANALYZE_STATE_WAVE_SAVE:
//		{
//			if (!m_bReadFromAes)   //如果是从aes文件中读取不能取消
//			{
//				((CButton *)(GetDlgItem(IDC_BUTTON_CANCEL)))->ShowWindow(SW_SHOW);
//				((CButton *)(GetDlgItem(IDC_BUTTON_CANCEL)))->EnableWindow(TRUE);
//			}
//			if(m_pbmLoading.GetSafeHwnd())
//			{
//				m_pbmLoading.ShowWindow(SW_SHOW);
//				m_pbmLoading.SetWindowText(language_loadstring(IDS_MUSIC_LOADING)+ _T(" ") + m_musicFilePath);
//			}
//		}
//		break;
//	case ANALYZE_STATE_BITRATE_EXCEED:
//		{
//			SetButtonEnableState(TRUE);
//			CString strBitRate ;
//			strBitRate.Format(_T("%dkbps,"),lParam);
//			((CButton *)(GetDlgItem(IDC_BUTTON_CANCEL)))->ShowWindow(SW_HIDE);
//			if(m_pbmLoading.GetSafeHwnd())
//			{
//				m_pbmLoading.SetWindowText(_T("Error"));
//				m_pbmLoading.SetMarquee(0,100);
//				m_pbmLoading.ShowWindow(SW_HIDE);
//			}
//			if (m_pMP3Session)
//			{
//				m_pMP3Session->DisConnect();
//				m_pMP3Session->AddSong(m_MusicInfo.szFilePath);
//			}

//			MessageBox(language_loadstring(IDS_MUSIC_BITRATE_VALUE) + strBitRate + language_loadstring(IDS_MUSIC_BITRARE_EXCEED));
//			m_musicFilePath = _T("");
//			m_waveFilePath = _T("");
//			m_nOriginBPM  = 0;
//			m_nOriginScale = 0;
//		}
//		break;
//	case ANALYZE_STATE_WAVE_SAVED:
//		{
//			SetButtonEnableState(true);
//			ShowWindow(SW_HIDE);
//			if(m_pbmLoading.GetSafeHwnd())
//			{
//				m_pbmLoading.ShowWindow(SW_HIDE);
//				m_pbmLoading.SetWindowText(_T(""));
//				m_pbmLoading.SetMarquee(0,100);
//			}
//			((CButton *)(GetDlgItem(IDC_BUTTON_CANCEL)))->ShowWindow(SW_HIDE);
//			UpdateMusicData();
//			if (m_pMP3Session)
//			{
//				m_pMP3Session->DisConnect();
//				m_pMP3Session->AddSong(m_MusicInfo.szFilePath);
//			}
//			OnCtrlShow();
//			m_nStartTime = 0;
//			m_nEndTime = m_MusicInfo.StreamTime.ms;
//			if (!m_MusicInfo.szFilePath)
//			{
//				OnOK();
//				return FALSE;
//			}
//			if (m_pEvent)
//			{
//				m_pEvent->IMusicChooseEvent_DeleteMusicLayerBlock();
//				m_pEvent->IMusicChooseEvent_AddMusicLayerBlock(1,&m_MusicInfo);
//				if(m_bReadFromAes)
//				{
//					//设置打开文件是文件修改状态为FALSE
//					//m_pEvent->IMusicChooseEvent_SetFileModified(FALSE);
//					m_bReadFromAes = FALSE;
//				}
//			}
//			myDeleteDirectory(GetModuleDir()+L"\\Music");
//			FileCopyTo(m_MusicInfo.szFilePath,TRUE);
//		}
//		break;
//	default:
//		break;
//	}
//	return TRUE;
//}
//void CMusicChooseDialog::ThreadProcMain(void)
//{
//	CString strFilename = m_musicFilePath;
//	int nBPM =0;
//	int nLength = 0;
//	int nBitRate = 0;
//	TCHAR *pFileTitle = NULL;
//	//开始分析BPM
//	::PostMessage(GetSafeHwnd(), WM_ALPHA1S_MOTIONEDITOR_MUSICANALYZE,ANALYZE_STATE_BPM_ANALYZE,0);
//	if(!m_pMP3Session->GetSongInfo(nBPM,nLength,nBitRate,&pFileTitle))
//	{
//		::PostMessage(GetSafeHwnd(), WM_ALPHA1S_MOTIONEDITOR_MUSICANALYZE,ANALYZE_STATE_ERROR_ANALYZE,0);
//		return ;
//	}
//	if (nBitRate > MP3_MAX_BITRATE)
//	{
//		::PostMessage(GetSafeHwnd(), WM_ALPHA1S_MOTIONEDITOR_MUSICANALYZE,ANALYZE_STATE_BITRATE_EXCEED,nBitRate);
//		return;
//	}
//	//else
//	::PostMessage(GetSafeHwnd(), WM_ALPHA1S_MOTIONEDITOR_MUSICANALYZE,ANALYZE_STATE_BPM_ANALYZED,nBPM);
//	if (nBPM)
//	{
//		m_nOriginScale = (int) SecsPerMinute / nBPM;
//	}
//	else
//	{
//		m_nOriginScale = 0;
//	}

//	//开始分析波形
//	::PostMessage(GetSafeHwnd(), WM_ALPHA1S_MOTIONEDITOR_MUSICANALYZE,ANALYZE_STATE_WAVE_SAVE,0);
//	int nResult = SaveWav(m_musicFilePath,m_waveFilePath);
//	if ( nResult == -1)
//	{
//		::PostMessage(GetSafeHwnd(), WM_ALPHA1S_MOTIONEDITOR_MUSICANALYZE,ANALYZE_STATE_ERROR_ANALYZE,0);
//		return;
//	}
//	else if (nResult == 0)
//	{
//		::PostMessage(GetSafeHwnd(), WM_ALPHA1S_MOTIONEDITOR_MUSICANALYZE,ANALYZE_STATE_WAVE_CANCELLED,0);
//		return;
//	}
//	::PostMessage(GetSafeHwnd(), WM_ALPHA1S_MOTIONEDITOR_MUSICANALYZE,ANALYZE_STATE_WAVE_SAVED,0);
//}
//BOOL CMusicChooseDialog::OnInitDialog()
//{
//	CDialogEx::OnInitDialog();
//	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),FALSE);
//	if (m_pbmLoading.GetSafeHwnd())
//	{
//		m_pbmLoading.SetTextColor(RGB(0,0,0));
//		m_pbmLoading.SetTheme(xtpControlThemeOffice2003);
//		m_pbmLoading.ModifyStyle(0, PBS_MARQUEE| ES_CENTER);
//		m_pbmLoading.SetMarquee(0, 100);
//		m_pbmLoading.ShowWindow(SW_HIDE);
//	}
//	((CButton *)(GetDlgItem(IDC_BUTTON_CANCEL)))->ShowWindow(SW_HIDE);
//	CreateUBXMp3Session(&m_pMP3Session);
//	DeleteMusic();
//	return TRUE;  // return TRUE unless you set the focus to a control
//	// 异常: OCX 属性页应返回 FALSE
//}
//void CMusicChooseDialog::PlayMusic(int nStartTime, int nEndTime, int nPlayState)
//{
//	if (!m_pMP3Session)
//	{
//		return;
//	}
//	if (0 == nPlayState)   //停止播放
//	{
//		m_pMP3Session->Stop();
//		m_pMP3Session->RemoveSong();
//	}
//	else if ( 1 == nPlayState)
//	{
//		m_pMP3Session->Pause();
//	}
//	else if (2 == nPlayState)
//	{
//		if (m_pMP3Session->isSongInTheQuene())
//		{
//			m_pMP3Session->Play(ceil(nStartTime * m_nCurrentScale * 1.0) , ceil(nEndTime * m_nCurrentScale * 1.0));
//		}
//		else
//		{
//			if (m_MusicInfo.szFilePath)
//			{
//				m_pMP3Session->AddSong(m_MusicInfo.szFilePath);
//				m_pMP3Session->Play(ceil(nStartTime * m_nCurrentScale * 1.0), ceil(nEndTime* m_nCurrentScale * 1.0));
//			}
//		}
//	}

//}
//void CMusicChooseDialog::OnCtrlShow()
//{
//	CString str;
//	if (m_MusicInfo.szFilePath)
//	{
//		str = m_MusicInfo.szFilePath;
//	}
//	else
//		str = _T("No music file!");
// 	GetDlgItem(IDC_EDIT_MUSICSHOW)->SetWindowText(str);
//	str = TimeStringCustomise(m_MusicInfo.StreamTime.hms.hour,m_MusicInfo.StreamTime.hms.minute,m_MusicInfo.StreamTime.hms.second);
//	//str.Format(_T("%d:%d:%d"),m_MusicInfo.StreamTime.hms.hour,m_MusicInfo.StreamTime.hms.minute,m_MusicInfo.StreamTime.hms.second);
//	GetDlgItem(IDC_EDIT_MUSICLENGTHSHOW)->SetWindowText(str);
//	str.Format(_T("%d"),m_MusicInfo.nBPM);
//	GetDlgItem(IDC_EDIT_ORIGINBPMSHOW)->SetWindowText(str + _T("  bpm"));
//	str.Format(_T("%d"),m_nOriginScale);
//	GetDlgItem(IDC_EDIT_BAETSTIME)->SetWindowText(str + _T("  ms"));
//}
//void CMusicChooseDialog::UpdateMusicData()
//{
//	if (m_musicFilePath.IsEmpty() || m_waveFilePath.IsEmpty())
//	{
//		return;
//	}
//	CString strFileNameWithExt = GetName(m_musicFilePath);
//	TCHAR *szFileName = new TCHAR[strFileNameWithExt.GetLength() + 1];
//	_tcscpy(szFileName, strFileNameWithExt);
//	TCHAR *szFilePath = new TCHAR[m_musicFilePath.GetLength() + 1];
//	_tcscpy(szFilePath , m_musicFilePath);
//	TCHAR *szWavePath = new TCHAR[m_waveFilePath.GetLength() + 1];
//	_tcscpy(szWavePath , m_waveFilePath);
//	if (m_MusicInfo.szFileName)
//	{
//		delete []m_MusicInfo.szFileName;
//		m_MusicInfo.szFileName = NULL;
//	}
//	if (m_MusicInfo.szFilePath)
//	{
//		delete []m_MusicInfo.szFilePath;
//		m_MusicInfo.szFilePath = NULL;
//	}
//	if (m_MusicInfo.szWavePath)
//	{
//		delete []m_MusicInfo.szWavePath;
//		m_MusicInfo.szWavePath = NULL;
//	}
//	m_MusicInfo.szFileName = szFileName;
//	m_MusicInfo.szFilePath = szFilePath;
//	m_MusicInfo.szWavePath = szWavePath;
//	m_MusicInfo.StreamTime = m_hmsTime;
//	m_MusicInfo.nBPM = m_nOriginBPM;

//}
//bool CMusicChooseDialog::AddMusicResource(CString strFilePath)
//{
//	CString strFilename = strFilePath;
//	if(m_pMP3Session == NULL)
//	{
//		return false;
//	}
//	if (!strFilename.IsEmpty())
//	{
//		if (m_pMP3Session->isSongInTheQuene() )
//		{
//			//此处可能需要清除音乐层的绘图，并删除之前的音乐层
//			if (!m_pMP3Session->AddSong(strFilename.GetBuffer(0)))
//			{
//				MessageBox(strFilename + language_loadstring(IDS_MUSIC_FILEERROR));
//				return false;
//			}
//		}
//		else
//		{
//			if (!m_pMP3Session->Connect(strFilename.GetBuffer(0)))
//			{
//				MessageBox(strFilename + language_loadstring(IDS_MUSIC_FILEERROR));
//				return false;
//			}
//		}
//		m_musicFilePath = strFilename;
//		m_pMP3Session->GetStreamTime(&m_hmsTime);
//		m_nStartTime = 0;
//		m_nEndTime = m_hmsTime.ms;
//		CenterWindow();
//		ShowWindow(SW_SHOW);
//		SetButtonEnableState(FALSE);
//		this->EnableWindow(TRUE);
//		WaitForStop();
//		StartThread();
//		return true;
//	}
//	else
//		return false;
//}

//CString CMusicChooseDialog::GetModuleDir()
//{
//	HMODULE module = GetModuleHandle(0);
//	wchar_t pFileName[MAX_PATH];
//	GetModuleFileName(module, pFileName, MAX_PATH);

//	CString csFullPath(pFileName);
//	int nPos = csFullPath.ReverseFind( _T('\\') );
//	if( nPos < 0 )
//		return CString("");
//	else
//		return csFullPath.Left( nPos );
//}
//void CMusicChooseDialog::myDeleteDirectory(CString directory_path)
//{
//	CFileFind finder;
//	CString path;
//	path.Format(L"%s/*.*",directory_path);
//	BOOL bWorking = finder.FindFile(path);
//	int nDelResult = 0;
//	while(bWorking){
//		bWorking = finder.FindNextFile();
//		if(finder.IsDirectory() && !finder.IsDots()){//处理文件夹
//			myDeleteDirectory(finder.GetFilePath()); //递归删除文件夹
//			RemoveDirectory(finder.GetFilePath());
//		}
//		else{//处理文件
//			nDelResult = DeleteFile(finder.GetFilePath());
//		}
//	}
//	if (nDelResult == 0)
//	{
//		int n  = GetLastError();
//	}
	
//}

//void CMusicChooseDialog::FileCopyTo(CString strSearchPath, BOOL cover)
//{
//	CString strDesPath = GetModuleDir()+L"\\Music";

//	CFileFind filefinder;

//	CString filename;

//	BOOL bfind = filefinder.FindFile(strSearchPath);

//	CString SourcePath, DisPath;

//	while (bfind)

//	{
//		bfind = filefinder.FindNextFile();

//		filename = filefinder.GetFileName();

//		SourcePath = strSearchPath;

//		DisPath = strDesPath + "//" + filename;

//		CopyFile(/*(LPCSTR)*/SourcePath, DisPath, cover);

//	}

//	filefinder.Close();
//}

//int CMusicChooseDialog::GetCurrentPos()
//{
//	if (m_pMP3Session && m_pMP3Session->isSongInTheQuene())
//	{
//		return m_pMP3Session->GetSongPos();
//	}
//	else
//		return -1;
//}
//void CMusicChooseDialog::Release()
//{
//	WaitForStop();
//	if (m_pMP3Session)
//	{
//		if (m_pMP3Session->isSongInTheQuene())
//		{
//			m_pMP3Session->Stop();
//		}
//		m_pMP3Session->Release();
//		m_pMP3Session = NULL;
//	}
//	if (m_MusicInfo.szFileName)
//	{
//		delete []m_MusicInfo.szFileName;
//		m_MusicInfo.szFileName = NULL;
//	}
//	if (m_MusicInfo.szFilePath)
//	{
//		delete []m_MusicInfo.szFilePath;
//		m_MusicInfo.szFilePath = NULL;
//	}
//	if (m_MusicInfo.szWavePath)
//	{
//		delete []m_MusicInfo.szWavePath;
//		m_MusicInfo.szWavePath = NULL;
//	}
//	myDeleteDirectory(GetModuleDir()+L"\\wave");
//	myDeleteDirectory(GetModuleDir()+L"\\Music");
//	//DestroyWindow();
//}
//void CMusicChooseDialog::OnBnClickedModifyBPM()
//{
//	return;
//// 	CBPMSettingDlg dlg;
//// 	int nBPM = 0;
//// 	dlg.m_nBpmValue = m_nCurrentBPM;
//// 	UpdateData(FALSE);
//// 	if (IDOK == dlg.DoModal())
//// 	{
//// 		if (dlg.m_nBpmValue == 0)
//// 		{
//// 			MessageBox(language_loadstring(IDS_STRING_MUSIC_INVALIDBPM));
//// 			//dlg.m_nBpmValue = m_nCurrentBPM;
//// 			return;
//// 		}
//// 		m_nCurrentBPM = dlg.m_nBpmValue;
////
//// 		m_nCurrentScale = (int)(SecsPerMinute / (m_nCurrentBPM));
//// 		COptionsMusicDlg *pParentDlg = (COptionsMusicDlg*)GetParent();
//// 		if(pParentDlg)
//// 		{
//// 			pParentDlg->SetScaleValue(m_nCurrentScale);
//// 			//GetDlgItem(idc)
//// 			if (m_MusicInfo.szFilePath)
//// 			{
//// 				UBXMusicInfo MusicInfo;
//// 				MusicInfo.nBPM = m_nCurrentBPM ;
//// 				MusicInfo.szFileName = m_MusicInfo.szFileName;
//// 				MusicInfo.szFilePath = m_MusicInfo.szFilePath;
//// 				MusicInfo.nLength = sizeof(m_MusicInfo);    //便于日后统计结构体信息在传输中是否有丢失
//// 				MusicInfo.StreamTime = m_MusicInfo.StreamTime;
//// 				pParentDlg->AddLayerBlock1S((void*)&MusicInfo,MusicInfo.nLength,1);
//// 			}
////             OnCtrlShow();
//// 		}
//// 	}
//}
//void CMusicChooseDialog::OnBnClickedModifyScale()
//{
//	return;
//// 	CTimeSettingDialog dlg;
//// 	int nBPM = 0;
//// 	dlg.m_TimeScaleValue = m_nCurrentScale;
//// 	UpdateData(FALSE);
//// 	if(IDOK == dlg.DoModal())
//// 	{
//// 		if (dlg.m_TimeScaleValue == 0)
//// 		{
//// 			MessageBox(language_loadstring(IDS_STRING_MUSIC_INVALIDSCALE));
//// 			//dlg.m_nBpmValue = m_nCurrentBPM;
//// 			return;
//// 		}
//// 		m_nCurrentScale = dlg.m_TimeScaleValue;
//// 		m_nCurrentBPM = (int)(SecsPerMinute / (m_nCurrentScale));
//// 		COptionsMusicDlg *pParentDlg = (COptionsMusicDlg*)GetParent();
//// 		if(pParentDlg)
//// 		{
//// 			pParentDlg->SetScaleValue(m_nCurrentScale);
//// 			if (m_MusicInfo.szFilePath)
//// 			{
//// 				UBXMusicInfo MusicInfo;
//// 				MusicInfo.nBPM = m_nCurrentBPM ;
//// 				MusicInfo.szFileName = m_MusicInfo.szFileName;
//// 				MusicInfo.szFilePath = m_MusicInfo.szFilePath;
//// 				MusicInfo.nLength = sizeof(m_MusicInfo);    //便于日后统计结构体信息在传输中是否有丢失
//// 				MusicInfo.StreamTime = m_MusicInfo.StreamTime;
//// 				pParentDlg->AddLayerBlock1S((void*)&MusicInfo,MusicInfo.nLength,1);
//// 			}
////             OnCtrlShow();
//// 		}
//// 	}
//}
//void CMusicChooseDialog::DeleteMusic()
//{
//	if (m_pMP3Session)
//	{
//		m_pMP3Session->DisConnect();
//	}
//	if (m_MusicInfo.szFileName)
//	{
//		delete []m_MusicInfo.szFileName;
//		m_MusicInfo.szFileName = NULL;
//	}
//	if (m_MusicInfo.szFilePath)
//	{
//		delete []m_MusicInfo.szFilePath;
//		m_MusicInfo.szFilePath = NULL;
//	}
//	m_MusicInfo.nBPM = 0;
//	m_MusicInfo.StreamTime.ms = 0;
//	m_MusicInfo.StreamTime.samples = 0;
//	m_MusicInfo.StreamTime.sec = 0;
//	m_MusicInfo.StreamTime.hms.minute = 0;
//	m_MusicInfo.StreamTime.hms.second = 0;
//	m_MusicInfo.StreamTime.hms.hour = 0;
//	m_MusicInfo.StreamTime.hms.millisecond= 0;
//	m_nOriginBPM  = 0;
//	m_nOriginScale = 0;
//	if(m_pEvent)
//	{
//		m_pEvent->IMusicChooseEvent_DeleteMusicLayerBlock();
//	}
//	myDeleteDirectory(GetModuleDir()+L"\\wave");
//	myDeleteDirectory(GetModuleDir()+L"\\Music");
//	//(m_pEvent->IMusicChooseEvent_DeleteMusicLayerBlock();)
//	OnCtrlShow();
//}
//int CMusicChooseDialog::SaveWav(CString strFilename , CString &strOutFilePath)
//{
//	if (m_pMP3Session )
//	{
//		if (m_pMP3Session->isSongInTheQuene())
//		{
//			m_pMP3Session->RemoveSong();
//		}
		
//		CString strFileNameWithExt =_T("NULL");
//		int nPos = strFilename.ReverseFind(_T('\\'));
//		if (nPos >= 0)
//		{
//			strFileNameWithExt = strFilename.Right(strFilename.GetLength() - nPos - 1);
//		}
//		nPos = strFileNameWithExt.ReverseFind(_T('.'));
//		if (nPos > 0)
//		{
//			strFileNameWithExt = strFileNameWithExt.Left(nPos);
//		}

//		CString temp;
//		CString strExepath;
//		TCHAR temp1[1024];
//		memset(temp1,0,sizeof(TCHAR)*1024);
//		GetFileNameFromExePath(temp,temp1);
//		wstring adbPath(temp1);
//		strExepath = adbPath.c_str();

//		strOutFilePath.Format(_T("%swave\\%s.wav"),strExepath ,strFileNameWithExt);
//		CString strFileDir;
//		strFileDir.Format(_T("%swave"),strExepath);
//		CreateDirectory(strFileDir,NULL);
//		return m_pMP3Session->SaveToWav(strFilename,strOutFilePath.GetBuffer(0));
//	}
//	return -1;
//}
//void CMusicChooseDialog::OnBnClickedCancel()
//{
//	if (m_pMP3Session)
//	{
//		m_pMP3Session->CancelSaveToWav();
//	}
//}
//void CMusicChooseDialog::OnBnClickedDelete()
//{
//	DeleteMusic();
//	//myDeleteDirectory(GetModuleDir()+L"\\Music");
//	//myDeleteDirectory(GetModuleDir() + L"wave");
//}
//void CMusicChooseDialog::OnBnClickedButton1()
//{

//	CString strUBXFileEx = _T("UBX Files(*.mp3)|*.mp3||");
//	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |OFN_ALLOWMULTISELECT|OFN_ENABLESIZING,strUBXFileEx,NULL);
//	dlg.m_ofn.lpstrTitle = _T("Choose a music file");
//	dlg.m_ofn.nMaxFile = 10 * MAX_PATH;
//	dlg.m_ofn.lpstrFile = new TCHAR[dlg.m_ofn.nMaxFile];
//	memset(dlg.m_ofn.lpstrFile , 0 , sizeof(TCHAR)*dlg.m_ofn.nMaxFile);
//	CString strFilename;
//	if (IDOK == dlg.DoModal())
//	{
//		POSITION pos_file;
//		pos_file = dlg.GetStartPosition();
//		while(pos_file != NULL)
//		{
//			strFilename = dlg.GetNextPathName(pos_file);
//		}
//	}
//	delete []dlg.m_ofn.lpstrFile;
	
//	if (AddMusicResource(strFilename))
//	{
//		//m_bReadFromAes = FALSE;
//		//FileCopyTo(strFilename,TRUE);
////		OnCtrlShow();
//	}
//}
//void CMusicChooseDialog::RemoveSong()
//{
//	if (m_pMP3Session)
//	{
//		m_pMP3Session->RemoveSong();
//	}
//}
//void CMusicChooseDialog::ReadMusicFromAesFile(CString strFilePath)
//{
//	m_bReadFromAes = TRUE;
//	AddMusicResource(strFilePath);
//}
//void CMusicChooseDialog::OnBnClickedOk()
//{
//	m_nStartTime = 0;
//	m_nEndTime = m_MusicInfo.StreamTime.ms;
//	if (!m_MusicInfo.szFilePath)
//	{
//		return;
//		OnOK();
//	}
//	if (m_pEvent)
//	{
//		m_pEvent->IMusicChooseEvent_AddMusicLayerBlock(1,&m_MusicInfo);
//	}
//	OnOK();
//}

//void CMusicChooseDialog::OnBnClickedModify()
//{
//	CString startTime;
//	CString endTime;
//	GetDlgItemText(IDC_EDIT_START_TIME, startTime);
//	GetDlgItemText(IDC_EDIT_END_TIME, endTime);
//	m_nStartTime = _wtoi(startTime);
//	m_nEndTime = _wtoi(endTime);
//	COptionsMusicDlg* dlg = (COptionsMusicDlg*)GetParent();
//	dlg->ModifyBlockInfo();
//}

//void CMusicChooseDialog::OnBnClickedInsert()
//{
//	CString startTime;
//	CString endTime;
//	GetDlgItemText(IDC_EDIT_START_TIME, startTime);
//	GetDlgItemText(IDC_EDIT_END_TIME, endTime);
//	m_nStartTime = _wtoi(startTime);
//	m_nEndTime = _wtoi(endTime);
//	COptionsMusicDlg* dlg = (COptionsMusicDlg*)GetParent();
//	dlg->InsertLayerBlock();
//}

//void CMusicChooseDialog::OnOK()
//{
//	// TODO: 在此添加专用代码和/或调用基类

//	__super::OnOK();
//}

//void CMusicChooseDialog::OnCancel()
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	ShowWindow(SW_HIDE);
//}

//void CMusicChooseDialog::SetRawData(char* pData, int nLen)
//{
//	int nTotalLen = 0;
//	UBX_MEMCPY_INC(nTotalLen, pData);
//	if (nTotalLen != nLen)
//		return;

//	int InfoLen = 0;
//	UBX_MEMCPY_INC(InfoLen, pData);
//	if (InfoLen)
//	{
//		char* pathInfo = new char[InfoLen+1];
//		ZeroMemory(pathInfo, InfoLen+1);
//		memcpy(pathInfo, pData, InfoLen);
//		pData += InfoLen;
//		m_EditFilePath = pathInfo;
//		delete [] pathInfo;
//	}
//}

//int CMusicChooseDialog::GetRawData(char** ppData)
//{
//	int nLen = 0;
//	vector<char> DataSave;
//	AddVectorData(&DataSave, (char*)&nLen, sizeof(nLen));

//	USES_CONVERSION;
//	char *pathInfo = T2A(m_EditFilePath);
//	int pathLen = strlen(pathInfo);


//	AddVectorData(&DataSave, (char*)&pathLen, sizeof(pathLen));
//	AddVectorData(&DataSave, pathInfo, pathLen);

//	nLen = DataSave.size();
//	ModifyVertorData(DataSave, 0, (char*)&nLen, sizeof(nLen));
//	if (nLen)
//	{
//		*ppData = new char[nLen];
//        Q_ASSERT(*ppData);
//		CopyVertorToPointer(DataSave, *ppData);
//	}

//	return nLen;
//}

//void CMusicChooseDialog::SetButtonEnableState(BOOL buttonState)
//{
//	if (buttonState)
//	{
//		((CButton *)(GetDlgItem(IDOK)))->EnableWindow(TRUE);
//		((CButton *)(GetDlgItem(IDC_BUTTON_DELETE)))->EnableWindow(TRUE);
//		((CButton *)(GetDlgItem(IDCANCEL)))->EnableWindow(TRUE);
//		((CButton *)(GetDlgItem(IDC_BUTTON_CHOOSE)))->EnableWindow(TRUE);
//		((CButton *)(GetDlgItem(IDC_BUTTON_CANCEL)))->EnableWindow(TRUE);
//		AfxGetMainWnd()->EnableWindow(TRUE);
//		CMenu*   menu   =   this->GetSystemMenu(FALSE);
//		menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);

//	} else {
//		((CButton *)(GetDlgItem(IDOK)))->EnableWindow(FALSE);
//		((CButton *)(GetDlgItem(IDC_BUTTON_DELETE)))->EnableWindow(FALSE);
//		((CButton *)(GetDlgItem(IDCANCEL)))->EnableWindow(FALSE);
//		((CButton *)(GetDlgItem(IDC_BUTTON_CHOOSE)))->EnableWindow(FALSE);
//        ((CButton *)(GetDlgItem(IDC_BUTTON_CANCEL)))->EnableWindow(FALSE);
//		AfxGetMainWnd()->EnableWindow(FALSE);
//		CMenu*   menu   =   this->GetSystemMenu(FALSE);
//		menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
//	}
//}

//CString CMusicChooseDialog::GetName(CString strName)
//{
//	int nPos = strName.Find('\\');
//	CString sSubStr = strName;
//	while (nPos)
//	{
//		sSubStr = sSubStr.Mid(nPos+1,sSubStr.GetLength()-nPos);  //取'\'右边字符串
//		nPos = sSubStr.Find('\\');   //不包含'\'，函数值返回-1
//		if (nPos==-1)
//		{
//			nPos = 0;
//		}
//	}
//	return sSubStr;
//}


//void CMusicChooseDialog::OnClose()
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//// 	if (GetSafeHwnd())
//// 	{
//// 		DestroyWindow();
//// 	}
//// 	Release();
//// 	CDialogEx::OnClose();
//	WaitForStop();
//	ShowWindow(SW_HIDE);
//}


//void CMusicChooseDialog::OnDestroy()
//{
//	Release();

//	CDialogEx::OnDestroy();
//	// TODO: 在此处添加消息处理程序代码
//}


//void CMusicChooseDialog::PostNcDestroy()
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	//delete this;
//	CDialogEx::PostNcDestroy();
//}
