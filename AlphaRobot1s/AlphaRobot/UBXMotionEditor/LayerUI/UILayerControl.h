//#pragma once
//#include "resource.h"
//#include "ILayerControl.h"
//#include "UILayerControlToolsDlg.h"
//#include "UILayerControlManager.h"
//// CUILayerControl �Ի���

//class CUILayerControl : public CDialog, public ILayerControl, public LayerControlToolsEvent
//{
//	DECLARE_DYNAMIC(CUILayerControl)

//public:
//	CUILayerControl(QWidget* pParent = NULL);   // ��׼���캯��
//	virtual ~CUILayerControl();

//// �Ի�������
//	enum { IDD = IDD_DIALOG_LAYER_CONTROL };

//private:
//	CUILayerControlToolsDlg	m_dlgTools;
//	ILayerControlEvent* m_pEvent;
//	CUILayerControlManager m_controlManagerDlg;

//public:
//	// ILayerControl �ӿ�ʵ��
//	virtual bool Connect(QWidget* pParent, ILayerControlEvent* pEvent);
//	virtual void ReleaseConnection();
//	virtual QWidget* GetQWidget();

//	// LayerControlToolsEvent�ӿ�ʵ��
//	virtual void LayerControlToolsEvent_GetUnUseLayers(int **pLayerID, int& nLayerCount);
//	virtual QString LayerControlToolsEvent_GetLayerName(int nLayerID);
//	virtual void LayerControlToolsEvent_AddLayer(int nLayerID);

//	virtual void LayerControlToolsEvent_AddLayerControl(int nLayerID);
//	virtual void LayerControlToolsEvent_DeleteLayer(int nLayerID);
//	virtual void LayerControlToolsEvent_SetTimeScaleValue(int timeScale);
//	virtual int  LayerControlToolsEvent_GetTimeScaleValue();
//	virtual void LayerControlToolsEvent_EnablePlay(int nLayerType ,bool bState) ;

	
//	virtual void LayerControl_DeleteAllControl();
//	virtual void LayerControl_LayerPlaying(int layerType);
//	virtual void LayerControl_SetCurrentOption(int layerType);
//	virtual void LayerControl_StopLayerDebug();
//	virtual void LayerControl_LayerStop();            //��ֹͣ������ûس�ʼλ�ã�
//	virtual void LayerControl_LayerDebugStop(int layerType);
//	//virtual voi
//	virtual void LayerControl_SetMusicSource(int nLayerType , void *pData , int nDataLen , int nDataID);

//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

//	DECLARE_MESSAGE_MAP()
//public:
//	virtual BOOL OnInitDialog();
//	afx_msg void OnSize(UINT nType, int cx, int cy);
//	void AddMusic();
//};
