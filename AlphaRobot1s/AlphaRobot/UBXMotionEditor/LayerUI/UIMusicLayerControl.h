//#pragma once
//#include "stdafx.h"
//#include "resource.h"
//#include "UILayerControlImpl.h"
//#include "afxwin.h"
//#include <mmsystem.h>

//#pragma   comment(lib,"winmm.lib")
//// CUIMusicLayerControl �Ի���

//class CUIMusicLayerControl : public CDialog, public CUILayerControlImpl
//{
//	DECLARE_DYNAMIC(CUIMusicLayerControl)

//public:
//	CUIMusicLayerControl(CWnd* pParent = NULL);   // ��׼���캯��
//	virtual ~CUIMusicLayerControl();

//// �Ի�������
//	enum { IDD = IDD_UI_MUSIC_LAYER_CONTROL };

//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

//private:
//	PLAYSTATE m_PlayState;
//	int m_MusicLength;
//	IUBXMP3Session *m_pMP3Session;
//	DECLARE_MESSAGE_MAP()
//public:
////	afx_msg void OnBnClickedButtonDeleteMusic();
//	afx_msg void OnBnClickedButtonDebugMusic();
//	afx_msg void OnPaint();
////	CXTPButton m_deleteMusic;
//	CXTPButton m_debugMusic;
//	virtual BOOL OnInitDialog();
//	virtual int GetType();
//	virtual CWnd* GetCWnd();
//	virtual bool Connect(CWnd* pParent);
//	virtual void ReleaseConnection();
//	virtual void LayerDebugStop();
//	virtual void EnablePlay(bool bState);
//	virtual void OnOK();
//	virtual void OnCancel();
//	// ֹͣ����
//	CXTPButton m_btnStop;
//	afx_msg void OnBnClickedButtonDebugStop();
//	afx_msg void OnBnClickedButtonAddmusic();
//	CXTPButton m_btnAddMusic;
//};
