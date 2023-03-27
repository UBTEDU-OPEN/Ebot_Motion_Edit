//// UIMainLayerLayout.cpp : 实现文件
////

//#include "stable.h"
//#include "UBXMotionEditor.h"
//#include "UIMainLayerLayout.h"
////#include "afxdialogex.h"
//#include "ILayerUI.h"
//#include "UBXMotionEditorLayout.h"
//#include "UIMusicLayer.h"

//bool CreateLayerManager(ILayerManager** ppManager)
//{
//    CUIMainLayerLayout* pProxy = new CUIMainLayerLayout();
//    if (pProxy)
//    {
//        *ppManager = pProxy;

//        return true;
//    }

//    return false;
//}
//// CUIMainLayerLayout 对话框

////IMPLEMENT_DYNAMIC(CUIMainLayerLayout, CDialog)

//CUIMainLayerLayout::CUIMainLayerLayout()
//    : CDialog(CUIMainLayerLayout::IDD, pParent)
//    , m_pUILayerManager(NULL)
//    , m_pEvent(NULL)
//    , m_pSocketSession(NULL)
//    , m_StopDebugCount(0)
//{

//}

//CUIMainLayerLayout::~CUIMainLayerLayout()
//{
//}

//void CUIMainLayerLayout::DoDataExchange(CDataExchange* pDX)
//{
//    CDialog::DoDataExchange(pDX);
//}


//bool CUIMainLayerLayout::Connect(/*QWidget* pWnd, ILayerManager_Event* pEvent*/)
//{
////    if (Create(CUIMainLayerLayout::IDD, pWnd))
////    {
////        ShowWindow(SW_SHOW);

////        m_pEvent = pEvent;
////        return true;
////    }

//    return false;
//}

//// 释放资源
//void CUIMainLayerLayout::ReleaseConnection()
//{
////    if (m_pUILayerManager)
////    {
////        m_pUILayerManager->ReleaseConnection();
////        m_pUILayerManager = NULL;
////    }
////// 	if (m_chooseDlg.GetSafeHwnd())
////// 	{
////// 		m_chooseDlg.DestroyWindow();
////// 	}
////    if (GetSafeHwnd())
////    {
////        DestroyWindow();
////    }
	
////    delete this;
//}

//QWidget* CUIMainLayerLayout::GetQWidget()
//{
//    return this;
//}
//void CUIMainLayerLayout::EnableCtrlPlay(int nLayerType , bool bState)
//{
//    if (m_pEvent)
//    {
//        //m_pEvent->ILayerManager_EnablePlay()
//    }
//}
//void CUIMainLayerLayout::EnablePlay(int nLayerType ,bool bState)
//{
//    if (m_pEvent)
//    {
//        m_pEvent->ILayerManager_EnablePlay(nLayerType , bState);
//    }
//}
//bool CUIMainLayerLayout::GetData(char** ppData, int &nLen)
//{
//    if (m_pUILayerManager)
//    {
//        return m_pUILayerManager->GetData(ppData, nLen);
//    }

//    return false;
//}
//void CUIMainLayerLayout::AdjustData()
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->AdjustData();
//    }
//}
//bool CUIMainLayerLayout::SetData(char* pData, int nLen)
//{
//    if (m_pUILayerManager)
//    {
//        return m_pUILayerManager->SetData(pData, nLen);
//    }

//    return false;
//}

//// CUIMainLayerLayout 消息处理程序


//bool CUIMainLayerLayout::OnInitDialog()
//{
//    __super::OnInitDialog();

//    CreateUILayerManager(&m_pUILayerManager);
//    m_pUILayerManager->Connect(this, this);
//// 	m_chooseDlg.Create(CMusicChooseDialog::IDD, this);
//// 	m_chooseDlg.CenterWindow();
//// 	m_chooseDlg.ShowWindow(SW_SHOW);
//    m_dlgTools.m_pLayerManager = m_pUILayerManager;
//    m_dlgTools.Create(CUIMainLayerLayoutToolsDlg::IDD, this);
//    m_dlgTools.SetWindowPos(NULL, 0, 0, 100, 22, SWP_SHOWWINDOW);
//    m_dlgTools.m_pEvent = this;

//    return true;  // return true unless you set the focus to a control
//    // 异常: OCX 属性页应返回 false
//}


//void CUIMainLayerLayout::OnSize(uint nType, int cx, int cy)
//{
//    __super::OnSize(nType, cx, cy);

//    if (m_pUILayerManager == NULL || m_pUILayerManager->GetQWidget()->GetSafeHwnd()==NULL)
//        return;

//    CRect rcTools;
//    m_dlgTools.GetClientRect(rcTools);
//    m_dlgTools.SetWindowPos(NULL, 0, cy-rcTools.Height(), cx, rcTools.Height(), SWP_SHOWWINDOW);

//    m_pUILayerManager->GetQWidget()->SetWindowPos(NULL, 0,0,cx,cy-rcTools.Height(), SWP_SHOWWINDOW);

//    int nCellCount = m_pUILayerManager->GetCellCount();
//    m_dlgTools.ResetHScroll(m_pUILayerManager->GetQWidget(), nCellCount);
//}

//void CUIMainLayerLayout::OnSrcollChange(int nPos)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->SetHScrollOffset(nPos);
//    }
//}

//void CUIMainLayerLayout::LayerUIEvent_SelectChanger(int nCurrentCol, int nScale)
//{
//    if (m_dlgTools.GetSafeHwnd())
//    {
//        m_dlgTools.ShowStatus(nCurrentCol, nScale);
//    }
//}

//void CUIMainLayerLayout::LayerManager_GetUnUseLayers(int **pLayerID, int& nLayerCount)
//{
//    vector<int> unUserLayerID;

//    if (m_pUILayerManager == NULL)
//    {
//        *pLayerID = NULL;
//        nLayerCount = 0;

//        return;
//    }

//    for (int i=ACTION_LAYER_TIME; i<MAX_ACTION_LAYER; i++)
//    {
//        ILayerUI* pLayer = m_pUILayerManager->FindLayer(i);
//        if (pLayer == NULL)
//        {
//            unUserLayerID.push_back(i);
//        }
//    }

//    if (unUserLayerID.size())
//    {
//        *pLayerID = new int[unUserLayerID.size()];
//        nLayerCount = unUserLayerID.size();
//    }
//    else
//    {
//        *pLayerID = NULL;
//        nLayerCount = 0;
//    }

//    int* p = *pLayerID;
//    for (int i=0; i<nLayerCount; i++)
//    {
//        p[i] = unUserLayerID[i];
//    }
//}

//QString CUIMainLayerLayout::LayerManager_GetLayerName(int nLayerID)
//{
//    if(m_pUILayerManager == NULL)
//    {
//        return L"";
//    }

//    return m_pUILayerManager->GetLayerName(nLayerID);
//}
//void CUIMainLayerLayout::LayerManager_ReleaseMusic()
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->ReleaseMusic();
//    }
//}
//void CUIMainLayerLayout::LayerManager_AddMusic()
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->AddMusic();
//    }
//}
//void CUIMainLayerLayout::LayerManager_AddLayer(int nLayerID)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->AddAcionLayer(nLayerID);
//    }
//}

//ILayerUI* CUIMainLayerLayout::LayerManager_GetLayer(int LayerID)
//{
//    if (m_pUILayerManager)
//    {
//        return m_pUILayerManager->FindLayer(LayerID);
//    }

//    return NULL;
//}

//void CUIMainLayerLayout::LayerManager_SetTimeScale(int timeScale)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->SetTimeScaleValues(timeScale);
//        m_dlgTools.ShowStatus(m_pUILayerManager->GetCurrentSelectCol(), timeScale);
//    }
//}

//int CUIMainLayerLayout::LayerManager_GetTimeScaleValue()
//{
//    if (m_pUILayerManager)
//    {
//        return m_pUILayerManager->GetTimeScaleValues();
//    }

//    return -1;
//}

//void CUIMainLayerLayout::LayerManager_LayerPlaying(int layerType, ILayerOptionHandler* optionHandle)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->LayerPlaying(layerType, optionHandle);
//    }
//}
//void CUIMainLayerLayout::LayerManager_SetMusicResource(QString strFilePath)
//{
//    if (m_pUILayerManager)
//    {
//        ILayerUI* pMusicLayer = m_pUILayerManager->FindLayer(ACTION_LAYER_MUSIC);

//        ((CUIMusicLayer*)pMusicLayer)->AddMusic(strFilePath);
//    }
//}
//void CUIMainLayerLayout::LayerManager_LayerStop()
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->LayerStop();
//    }
//}
//void CUIMainLayerLayout::LayerManager_StopLayerDebug()
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->StopLayerDebug();
//    }
//}

//void CUIMainLayerLayout::LayerUIEvent_AddLayerControl(int nLayerType)
//{
//    CUBXMotionEditorLayout* pDlg = (CUBXMotionEditorLayout*)GetParent();
//    pDlg->ILayerControlEvent_AddLayerControl(nLayerType);
//}

//void CUIMainLayerLayout::LayerUIEvent_SetLayOptionHandler(ILayerOptionHandler* pHandler)
//{
//    if (m_pEvent)
//    {
//        m_pEvent->ILayerManager_SetLayOptionHandler(pHandler);
//    }
//}
//void CUIMainLayerLayout::LayerUIEvent_SetMusicLayerSourceHandler(ILayerOptionHandler* pHandler , int nLayerType
//                                                               , void *pData , int nDataLen , int nDataID)
//{
//    if (m_pEvent)
//    {
//        //m_pEvent->ILayerManager_SetLayOptionHandler(pHandler);
//        m_pEvent->ILayerManager_SetMusicLayerSourceHandler(pHandler,nLayerType,pData , nDataLen ,nDataID);
//    }
//}
//void CUIMainLayerLayout::LayerUIEvent_DeleteMusciFile(ILayerOptionHandler* pHandler)
//{
//    if (m_pEvent)
//    {
//        m_pEvent->ILayerManager_DeleteMusciFile(pHandler);
//    }
//}
//void CUIMainLayerLayout::LayerUIEvent_OnDeletOptionHandler(ILayerOptionHandler* pHandler)
//{
//    if (m_pEvent)
//    {
//        m_pEvent->ILayerManager_OnDeletOptionHandler(pHandler);
//    }
//}
//void CUIMainLayerLayout::LayerUIEvent_SetScaleValue(int nScale)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->SetTimeScaleValues(nScale);
//        m_dlgTools.ShowStatus(m_pUILayerManager->GetCurrentSelectCol(), nScale);
//    }
//}
//void CUIMainLayerLayout::ShowScale(int nScale)
//{
//    m_dlgTools.ShowStatus(m_pUILayerManager->GetCurrentSelectCol(), nScale);
//}
//void CUIMainLayerLayout::LayerUIEvent_SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID)
//{
//    if (m_pEvent)
//    {
//        m_pEvent->ILayerManager_SetActionBlockDataManager(pManager, nCurrentGroupID, nCurrentActionID);
//    }
//}
//void CUIMainLayerLayout::LayerUIEvent_SetScrollBarPos(int nPos)
//{
//    m_dlgTools.SetHScrollPos(m_pUILayerManager->GetQWidget(), m_pUILayerManager->GetCellCount(),nPos);
//}
//void CUIMainLayerLayout::LayerUIEvent_UpdateScrollBar()
//{
//    m_dlgTools.ResetHScroll(m_pUILayerManager->GetQWidget(), m_pUILayerManager->GetCellCount());
//}

//void CUIMainLayerLayout::LayerManager_DeleteLayer(int nLayerID)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->DeleteActionLayer(nLayerID);
//    }
//}

//void CUIMainLayerLayout::LayerManager_Set3DRobotViewInterface(IUBXRobot3DView* pRobotView)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->Set3DRobotViewInterface(pRobotView);
//    }
//}

//void CUIMainLayerLayout::LayerManager_SetTreeView(ITreeNavigation* pView)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->SetTreeView(pView);
//    }
//}

//void CUIMainLayerLayout::LayerManager_TreeToView(char *pData, int nLen, QPoint* nPoint, QString FileName)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->TreeToView(pData, nLen, nPoint, FileName);
//    }
//}

//void CUIMainLayerLayout::LayerDebugStop(int layerType)
//{
//    m_StopDebugCount++;
//    if (m_pEvent)
//    {
//// 		if (m_StopDebugCount == 2)
//// 		{
//        m_dlgTools.StopTotalDebug(layerType);
//        m_StopDebugCount = 0;
//        /*}*/
//        m_pEvent->ILayerManager_LayerDebugStop(layerType);
//    }
//}

//ILayerOptionHandler* CUIMainLayerLayout::LayerUIEvent_GetOptionHandler()
//{
//    return m_pEvent->ILayerManager_GetOptionHandler();
//}

//void CUIMainLayerLayout::LayerManager_SetSocketSession(CIUBXSocketSession* pSession)
//{
//    m_pSocketSession = pSession;
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->IUILayerManager_SetSocketSession(pSession);
//    }
//}

//void CUIMainLayerLayout::LayerManager_UpdateScrollBar()
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->UpdateScrollBar();
//    }
//}


//void CUIMainLayerLayout::OnPaint()
//{
//    CPaintDC dc(this); // device context for painting
	
//    CRect   rc;
//    GetClientRect( &rc );// 获取客户区
//    dc.FillSolidRect(&rc, RGB(0,160,0));

//    CDialog::OnPaint();
//}
