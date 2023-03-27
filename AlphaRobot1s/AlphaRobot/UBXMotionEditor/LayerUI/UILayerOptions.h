//#pragma once
//#include "resource.h"
//#include "ILayerOption.h"

//// CUILayerOptions �Ի���

//class CActionBlockDataManager;
//class CUILayerOptions : public CDialog, public ILayerOption
//{
//	DECLARE_DYNAMIC(CUILayerOptions)

//public:
//    CUILayerOptions(QWidget* pParent = NULL);   // ��׼���캯��
//	virtual ~CUILayerOptions();

//// �Ի�������
//	enum { IDD = IDD_DIALOG_LAYER_OPTION };

//private:
//	ILayerOptionHandler* m_pOptionHandler;
//	CActionBlockDataManager* m_pGroupManager;

//public:
//	// ILayerOption�ӿ�ʵ��
//    virtual bool Connect(QWidget* pParent);
//	virtual void ReleaseConnection();
//    virtual QWidget* GetQWidget();
//	virtual void SetOptionHandler(ILayerOptionHandler* pHandler);
//	virtual void SetMusicOptionSources(ILayerOptionHandler* pHandler , int nLayerType , void *pData , int nDataLen , int nDataID);
//	virtual void DeleteMusicResource(ILayerOptionHandler* pHandler);
//	virtual ILayerOptionHandler* CUILayerOptions::GetOptionHandler();
//	virtual void SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID);
//	virtual void ResetState();
//	virtual void OnDeletOptionHandler(ILayerOptionHandler* pHandler);

//	virtual void OnOK();
//	virtual void OnCancel();

//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

//	DECLARE_MESSAGE_MAP()
//public:
//    afx_msg void OnSize(uint nType, int cx, int cy);
//};
