//#pragma once
//#include "resource.h"
//#include "afxwin.h"
//#include "AddLayerDlg.h"

//class LayerControlToolsEvent
//{
//public:
//	virtual void LayerControlToolsEvent_GetUnUseLayers(int **pLayerID, int& nLayerCount)=0;
//	virtual QString LayerControlToolsEvent_GetLayerName(int nLayerID) = 0;
//	virtual void LayerControlToolsEvent_AddLayer(int nLayerID) = 0;
//};

//// CUILayerControlToolsDlg �Ի���

//class CUILayerControlToolsDlg : public CDialog
//{
//	DECLARE_DYNAMIC(CUILayerControlToolsDlg)

//public:
//	CUILayerControlToolsDlg(QWidget* pParent = NULL);   // ��׼���캯��
//	virtual ~CUILayerControlToolsDlg();

//// �Ի�������
//	enum { IDD = IDD_DIALOG_LAYER_CONTROL_TOOLS };

//public:
//	LayerControlToolsEvent* m_pEvent;

//private:
//	CXTPButton m_btnAddLayer;
//	CXTPButton m_btnDeleteLayer;

//	CAddLayerDlg m_dlgAddLayer;

//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

//	DECLARE_MESSAGE_MAP()
//public:
//	virtual BOOL OnInitDialog();
//	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnBtnAddLayerClick();
//};
