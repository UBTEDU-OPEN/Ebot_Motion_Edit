//#pragma once

//#include "../../UBXLanguage/UBXLChs/resource.h"

//#include "afxdialogex.h"
//#include <windows.h>
//#include <stdio.h>
//#include <mmsystem.h>
//#include <shellapi.h>
//#include <AFXCOM_.H>
//#include "stdafx.h"
//#pragma   comment(lib,"winmm.lib")
//// CMusicChooseDialog �Ի���



//class IMusicChooseEvent
//{
//public:
//	virtual void IMusicChooseEvent_AddMusicLayerBlock(int nGroupManagerID, UBXMusicInfo* pInfo,CString strBlockName = _T("")) = 0;
//    virtual void IMusicChooseEvent_DeleteMusicLayerBlock() = 0;
//	virtual void IMusicChooseEvent_SetFileModified(bool bState) = 0;
//};


//#define WM_ALPHA1S_MOTIONEDITOR_MUSICANALYZE  WM_USER + 147
//#define ANALYZE_STATE_ERROR_ANALYZE   -1      // ����
//#define ANALYZE_STATE_WAVE_CANCELLED   0      // ȡ��
//#define ANALYZE_STATE_BPM_ANALYZE      1      // ����BPM��
//#define ANALYZE_STATE_BPM_ANALYZED     2      // BPM�����ɹ�
//#define ANALYZE_STATE_WAVE_SAVE        3      // ����������
//#define ANALYZE_STATE_WAVE_SAVED       4      // �������
//#define ANALYZE_STATE_BITRATE_EXCEED   5      // ���ֱ����ʹ���

////MP3�ļ����Ƶ���������
//#define MP3_MAX_BITRATE  128

//class CMusicChooseDialog : public CDialogEx , public UBXThread
//{
//	DECLARE_DYNAMIC(CMusicChooseDialog)

//public:
//	//CMusicChooseDialog(CWnd* pParent = NULL);   // ��׼���캯��
//	CMusicChooseDialog(CWnd* pParent = NULL, CString DescriptionInfo = _T(""));
//	virtual ~CMusicChooseDialog();
//	afx_msg void OnBnClickedButton1();
//// �Ի�������
//	enum { IDD = IDD_DIALOG_MUSIC_CHOOSE };

//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

//	DECLARE_MESSAGE_MAP()

//	afx_msg void OnBnClickedOk();
	
//	afx_msg void OnBnClickedModifyBPM();
//	afx_msg void OnBnClickedModifyScale();
//	afx_msg void OnBnClickedModify();
//	afx_msg void OnBnClickedInsert();
//	afx_msg void OnBnClickedDelete();
//	afx_msg void OnBnClickedCancel();
//	afx_msg void OnClose();
//	afx_msg void OnDestroy();
//	afx_msg LRESULT OnAnalyzeMusicFile(WPARAM wParam , LPARAM lParam);
//	virtual BOOL OnInitDialog();
//	virtual void OnOK();
//	virtual void OnCancel();
//    virtual void ThreadProcMain(void);


//public:
//	void SetRawData(char* pData, int nLen);
//	int  GetRawData(char** ppData);
//	void SetButtonEnableState(BOOL buttonState);
//	void Release();
//	void PlayMusic(int nStartTime, int nEndTime, int nPlayState);
//	int  GetCurrentPos();
//	bool AddMusicResource(CString strFilePath);
//	void UpdateMusicData();
//	void OnCtrlShow();
//	void DeleteMusic();
//	int  SaveWav(CString strFilename , CString &strOutFilePath);
//	void FileCopyTo(CString strSearchPath, BOOL cover);
//	CString GetModuleDir();
//	void myDeleteDirectory(CString directory_path);
//	void ReadMusicFromAesFile(CString strFilePath);
//	void RemoveSong();
//	void SetMusicEvent(IMusicChooseEvent *pEvent)
//	{
//		m_pEvent = pEvent;
//	};
//private:
//	CString GetName(CString strName);
//	IUBXMP3Session *m_pMP3Session;
//	UBXMusicInfo m_MusicInfo;
//	IMusicChooseEvent *m_pEvent;
//	bool m_bReadFromAes;
//public:
//	int  m_nStartTime;
//	int  m_nEndTime;
//	int  m_nCurrentScale;
//	int  m_nOriginScale;
//	int  m_nOriginBPM;
//	//���±���Ϊ��ǰ���ڶ�ȡ�����ֵ���Ϣ

//	UBXStreamTime m_hmsTime;
//	CString m_EditFilePath;
//	CString m_musicFilePath;
//	CString m_waveFilePath;

//	CXTPProgressCtrl m_pbmLoading;

//	virtual void PostNcDestroy();
//};
