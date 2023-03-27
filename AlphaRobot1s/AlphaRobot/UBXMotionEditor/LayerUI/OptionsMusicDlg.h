//#pragma once
//#include "afxcmn.h"

//#include "resource.h"
//#include "ILayerOptionHandler.h"
//#include "MusicChooseDialog.h"

//// COptionsMusicDlg 对话框
//class CActionBlockGroupManager;
////class CActionBlockDataManager;
//class COptionsMusicDlg : public CDialog, public ILayerOptionHandler
//{
//	DECLARE_DYNAMIC(COptionsMusicDlg)

//public:
//	COptionsMusicDlg(QWidget* pParent = NULL);   // 标准构造函数
//	virtual ~COptionsMusicDlg();

//// 对话框数据
//	enum { IDD = IDD_DIALOG_MUSIC_OPTION };

//public:
//	ILayerOptionHandler_Event* m_pEvent;

//private:

//private:
//	CMusicChooseDialog m_optionDlg;
//	int m_nCurrentSelectItem;
//	int m_nCurrentGroupID;
//	CActionBlockDataManager* m_pGroupManager;
//	int m_CurSelectBlockId;
//	CListCtrl m_musicGroupList;
//	CListCtrl m_musicBlockList;
//	int m_CurSelectBlockItem;
//	CImageList m_imageList;
//	int m_DefaultNameNum;
//	IUBXMP3Session *m_pMP3Session;
///*	bool m_bExistSong;*/

//public:
//	// ILayerOptionHandler接口实现
//	virtual bool Connect(QWidget* pParent, ILayerOptionHandler_Event* pEvent);
//	virtual void ReleaseConnection();
//	virtual QWidget* GetQWidget();
//	virtual void SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID);
//	virtual void SetControlState(bool controlState);
//	virtual void PlayMusic(int nStartTime, int nEndTime, int nPlayState);
//	virtual void SetMusicResource(QString strFilePath);
//	virtual int  GetCurrentPos();
//	virtual void COptionsMusicDlg::InsertGroupByMenu(int nCol);
	
//public:
//	void InsertLayerBlock();
//	void ModifyBlockInfo();
//	void DeleteLayerBlock();
//	void AddLayerBlock();
//	void AddLayerBlock1S(void *pData , int nDataLen , int nDataID);
//	void AddMusicFromGroup(QString strFilePath);
//	void UpdateUIByData();
//	void SetCurrentSelectItem(int nItem);
//	void SetCurSelectBlockItem(int nBlockItem);
//	void SetMusicFile(UBXStreamHMSTime hmsTime , QString strMusicName);
//	void DeleteMusicFile();
//	void UpdateBlockList(CActionBlockGroupManager* pGroup);
//	void UpdateBlockOptionUI(CActionBlockData* pBlockData);
//	void SetScaleValue(int nScale);
//	int  GetScaleValue();
//	int GetCurrenGroupManagerID();
//	QString GetModuleDir();
//	QString GetName(QString strName);
//	void FileCopyTo(QString strSearchPath, bool cover);


//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

//	DECLARE_MESSAGE_MAP()
//public:
//	virtual bool OnInitDialog();
//	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnNMClickListGroupList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMClickListBlockList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMRClickGroupList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMRClickBlockList(NMHDR *pNMHDR, LRESULT *pResult);
//	virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
//	virtual void OnOK();
//	virtual void OnCancel();
//	afx_msg void OnDestroy();
//};
