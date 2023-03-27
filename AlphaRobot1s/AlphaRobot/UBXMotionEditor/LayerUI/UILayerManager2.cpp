#include "stable.h"
#include "UILayerManager.h"
#include "UITimeLayer.h"
#include "UIMotionLayer.h"
// #include "UIEyesLayer.h"
#include "UIMainLayerLayout.h"
#include "MotionDataPlay.h"
#include "UIMusicLayer.h"
#include "IGroupDataPlay.h"
bool CreateUILayerManager(IUILayerManager** mManager)
{
    CUILayerManager* pProxy = new CUILayerManager();
    if (pProxy)
    {
        *mManager = pProxy;

        return true;
    }

    return false;
}

//IMPLEMENT_DYNCREATE( CUILayerManager, QWidget)
CUILayerManager::CUILayerManager(void)
    : m_pRobot3dView(NULL)
    , m_pGroupDataPlay(NULL)
    , m_pSocketSession(NULL)
    , m_IsMusicPlaying(false)
{
    m_CellCount = 10;
    m_lineThick = 1;
    m_nCellWidth = 99;
    m_scaleValue = 10;
    m_nCurCol = 100;
    m_pMusicDataPlay = NULL;
    m_IsDataPlaying = false;
//	m_pChooseDlg = NULL;
    m_bModified = false;
    m_nDebugType = SINGLE_LAYER_DEBUG;

}


CUILayerManager::~CUILayerManager(void)
{

}


bool CUILayerManager::Create(QString lpszClassName, QString lpszWindowName, ulong dwStyle,
    const QRect& rect, QWidget* pParentWnd, uint nID/*, CCreateContext* pContext*/)
{
//	lpszClassName = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS,
//		AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(LTGRAY_BRUSH), NULL) ;

//	QWidget::Create( lpszClassName, lpszWindowName, dwStyle /*| WS_VSCROLL | WS_HSCROLL*/ | CS_DBLCLKS, rect, pParentWnd, nID, pContext);
//	if ( NULL == m_pChooseDlg)
//	{
//		m_pChooseDlg = new CMusicChooseDialog();
//		if (m_pChooseDlg->Create(CMusicChooseDialog::IDD ,this))
//		{
//			m_pChooseDlg->ShowWindow(SW_HIDE);
//		}

//	}
//	//ResetHScroll();
    return true;
}

void CUILayerManager::ResetHScroll()
{
// 	CRect rc;
// 	GetClientRect(rc);
//
// 	GetScrollInfo( SB_HORZ, &m_hSi );
// 	m_hSi.cbSize = sizeof( SCROLLINFO );
// 	m_hSi.nMin = 0;
// 	m_hSi.nPage = 1;
// 	m_hSi.nMax = ;
// 	SetScrollInfo( SB_HORZ, &m_hSi );
}

bool CUILayerManager::Connect(QWidget* pWnd, IUILayerManager_Event* pEvent)
{
//    QRect rect;

//	m_pEvent = pEvent;

    return true;
}

void CUILayerManager::ReleaseConnection()
{
//	if (m_pGroupDataPlay)
//	{
//		m_pGroupDataPlay->ReleaseConnection();
//		m_pGroupDataPlay = NULL;
//	}
//	if (m_pMusicDataPlay)
//	{
//		m_pMusicDataPlay->ReleaseConnection();
//		m_pMusicDataPlay = NULL;
//	}
//	if (GetSafeHwnd())
//	{
//		DestroyWindow();
//	}

//	DeleteAllLayer();
//	if (m_pChooseDlg)
//	{
//		m_pChooseDlg->Release();
//		if (m_pChooseDlg->GetSafeHwnd())
//		{
//			m_pChooseDlg->DestroyWindow();
//		}
//		delete m_pChooseDlg;
//		m_pChooseDlg = NULL;
//	}
//	delete this;
}

QWidget* CUILayerManager::GetWidget()
{
    return this;
}

void CUILayerManager::OnDraw(/*CDC* pDC*/)
{

}

ILayerUI* CUILayerManager::GetLastLayer()
{
    return m_listActionLayer.takeLast();
}

void CUILayerManager::SetScrollPos(int nPos)
{
    if (m_pEvent)
    {
        m_pEvent->LayerUIEvent_SetScrollBarPos(nPos);
    }
}
void CUILayerManager::ClearSelectState()
{
    for (int i=0; i<GetLayerCount(); i++)
    {
        ILayerUI* pLayUI = FindLayerByIndex(i);
        if (pLayUI)
        {
            pLayUI->ClearSelectState();
        }
    }
}
void CUILayerManager::ReleaseMusic()
{
//	if (m_pChooseDlg)
//	{
//		m_pChooseDlg->RemoveSong();
//	}
}
void CUILayerManager::AddMusic()
{
    ILayerUI* pLayer = NULL;
    pLayer = FindLayer(ACTION_LAYER_MUSIC);
    ((CUIMusicLayer*)pLayer)->AddMusic(NULL,false);
}
bool CUILayerManager::AddAcionLayer(int nLayerID)
{
    ILayerUI* pLayer = NULL;

    if (nLayerID == ACTION_LAYER_TIME)
    {
        pLayer = new CUITimeLayer();
    }
    else if (nLayerID == ACTION_LAYER_MOTION)
    {
        pLayer = new CUIMotionLayer();
    }
// 	else if (nLayerID == ACTION_LAYER_EYE)
// 	{
// 		pLayer = new CUIEyesLayer();
// 	}
// 	else if (nLayerID == ACTION_LAYER_EAR)
// 	{
// 		pLayer = new CUIEarLayer();
// 	}
    else if (nLayerID == ACTION_LAYER_MUSIC)
    {
        pLayer = new CUIMusicLayer();
    }
    else
    {
        return false;
    }

    if (pLayer == NULL)
        return false;

    pLayer->SetLayerManager(this);
//    pLayer->Connect(GetQWidget());
    if (nLayerID == ACTION_LAYER_MUSIC)
    {
//		pLayer->GetMusicDlgPtr(m_pChooseDlg);
    }

    return AddActionLayer(pLayer);
}

bool CUILayerManager::AddActionLayer(ILayerUI* pLayer)
{
    ILayerUI* itemFound = NULL; /*new IActionLayer();*/

    if (pLayer == NULL)
        return false;

    for (int i=0; i<m_listActionLayer.count(); i++)
    {
        itemFound = m_listActionLayer.at(i);
        if (itemFound && itemFound->GetType() == pLayer->GetType())
        {
            return false;
        }
    }

    ILayerUI* pLayerLast = NULL;

    if (m_listActionLayer.count())
    {
        pLayerLast = GetLastLayer();
    }

    // 显示出来
    int x = 0;
    int y = 0;
    if (pLayerLast)
    {
        pLayerLast->GetPosition(x, y);
        y += pLayerLast->GetCellHeight();
    }

    m_pEvent->LayerUIEvent_AddLayerControl(pLayer->GetType());

    pLayer->SetPosition(x, y);

//    Invalidate(false);

    m_listActionLayer.append(pLayer);

    return true;
}

void CUILayerManager::AdjustLayerPosition()
{
    ILayerUI* itemFound = NULL;
    int x = 0;
    int y = 0;
    for (int i=2; i<m_listActionLayer.count(); i++)
    {
        itemFound = m_listActionLayer.at(i);
        itemFound->GetPosition(x, y);
        itemFound->SetPosition(x, i*(itemFound->GetCellHeight()));
    }

//    Invalidate(false);
}

bool CUILayerManager::DeleteActionLayer(int nTypeID)
{
    ILayerUI* itemFound = NULL;

    for (int i=0; i<m_listActionLayer.count(); i++)
    {
        itemFound = m_listActionLayer.at(i);
        if (itemFound && itemFound->GetType() == nTypeID)
        {
            if (m_pEvent)
            {
                m_pEvent->LayerUIEvent_OnDeletOptionHandler(itemFound->GetOptionHandler());
            }
            itemFound->ReleaseConnection();
            delete[] itemFound;
            m_listActionLayer.removeAt(i);
            AdjustLayerPosition();
//			Invalidate(false);
            return true;
        }
    }
    return false;
}

ILayerUI* CUILayerManager::FindLayer(int nTypeID)
{
    ILayerUI* itemFound = NULL;

    for (int i=0; i<m_listActionLayer.count(); i++)
    {
        itemFound = m_listActionLayer.at(i);
        if (itemFound && itemFound->GetType() == nTypeID)
        {
            return itemFound;
        }
    }
    return NULL;
}

ILayerUI* CUILayerManager::FindLayerByIndex(int nIndex)
{
    ILayerUI* itemFound = NULL;
    int  pos = m_listActionLayer.indexOf(m_listActionLayer.at(nIndex));
    if (pos>0)
    {
        itemFound = m_listActionLayer.at(pos);
    }

    return itemFound;
}

void CUILayerManager::DeleteAllLayer()
{
    for (int i=0; i<m_listActionLayer.count(); i++)
    {
        ILayerUI* pcui = m_listActionLayer.at(i);
        if (pcui)
        {
            if (m_pEvent)
            {
                m_pEvent->LayerUIEvent_OnDeletOptionHandler(pcui->GetOptionHandler());
            }
            pcui->ReleaseConnection();
            delete pcui;
            //m_mapActionLayer.RemoveAt(m_mapActionLayer.FindIndex(i));
        }
    }
    while(!m_listActionLayer.isEmpty())
    {
        m_listActionLayer.removeFirst();
    }
//	m_listActionLayer.RemoveAll();
}

int CUILayerManager::GetLayerCount()
{
    return m_listActionLayer.count();
}

// void CUILayerManager::SetMusicSource(QString strMusicFile)
// {
//
// }
void CUILayerManager::AdjustData()
{
    ILayerUI *pMotionLayer = FindLayer(ACTION_LAYER_MOTION);
    ILayerUI *pMusicLayer = FindLayer(ACTION_LAYER_MUSIC);
    if (pMotionLayer)
    {
        pMotionLayer->AdjustData();
    }
    if (pMusicLayer)
    {
        pMusicLayer->AdjustData();
    }

}
bool CUILayerManager::SetData(char* pData, int nLen)
{
    DeleteAllLayer();

    if (pData==NULL || nLen==0)
    {
        AddAcionLayer(ACTION_LAYER_TIME);
        AddAcionLayer(ACTION_LAYER_MUSIC);
        AddAcionLayer(ACTION_LAYER_MOTION);
    }
    else
    {
        ParseRawData(pData, nLen);
    }

    // 设置默认的属性处理对象
    if (m_pEvent)
    {
        ILayerUI* pLayer = FindLayer(ACTION_LAYER_MOTION);
        if (pLayer)
        {
            m_pEvent->LayerUIEvent_SetLayOptionHandler(pLayer->GetOptionHandler());
            m_pEvent->LayerUIEvent_SetActionBlockDataManager(pLayer->GetBlockDataManager(), 0);
        }
    }


    //m_LocationLayerDlg.ResetScrollBar(m_LayoutRange);
    //m_LocationLayerDlg.ResizeAllLayer();

    return true;
}

bool CUILayerManager::GetData(char** ppData, int &nLen)
{
    // 总的数据长度(int) + 总的页数(int) [单页的数据长度(int) + 单页的数据]
    vector<char> DataSave;

    // 保存总的数据长度，先占位，后面再改为实际的长度
    AddVectorData(&DataSave, (char*)&nLen, sizeof(nLen));

    // 保存文件头
    AddVectorData(&DataSave, UBX_FILE_TAG, strlen(UBX_FILE_TAG));
    // 添加版本信息
    int nVersion = UBX_FILE_VERSION;
    AddVectorData(&DataSave, (char*)&nVersion, sizeof(nVersion));

    // 保存总的层数
    int nLayerCount = GetLayerCount();   // 直接调用 layerManager 函数获取,去掉时间层
    AddVectorData(&DataSave, (char*)&nLayerCount, sizeof(nLayerCount));

    for (int i=0; i<nLayerCount; i++)
    {
        char* pRawData = NULL;
        int nRawDataLen = 0;

        ILayerUI* ppLayer = (ILayerUI*)FindLayerByIndex(i);  //

        if (ppLayer == NULL)
            continue;

        // 保存TYPE
        int nType = ppLayer->GetType();
        AddVectorData(&DataSave, (char*)&nType, sizeof(nType));

        ppLayer->GetData(&pRawData, nRawDataLen);
        // 长度信息
        AddVectorData(&DataSave, (char*)&nRawDataLen, sizeof(nRawDataLen));
        if (pRawData && nRawDataLen)
        {
            // 数据
            AddVectorData(&DataSave, pRawData, nRawDataLen);
            delete [] pRawData;
        }
    }

    nLen = DataSave.size();
    ModifyVertorData(DataSave, 0, (char*)&nLen, sizeof(nLen));
    if (nLen)
    {
        *ppData = new char[nLen];
        CopyVertorToPointer(DataSave, *ppData);
    }

    return true;
}


//BEGIN_MESSAGE_MAP(CUILayerManager, QWidget)
//	ON_WM_PAINT()
//	//ON_WM_HSCROLL()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
//	ON_WM_SIZE()
//	ON_WM_LBUTTONDBLCLK()
//	ON_WM_RBUTTONDOWN()
//	ON_WM_MOUSEMOVE()
//	ON_WM_TIMER()
//END_MESSAGE_MAP()


//void CUILayerManager::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting

//	CRect rc;
//	GetClientRect(rc);

//	CDC memDC;
//	memDC.CreateCompatibleDC(&dc);
//	CBitmap bmpCache;

//	bmpCache.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
//	CBitmap* pOldBitmap = memDC.SelectObject(&bmpCache);
//	memDC.FillSolidRect(0, 0, rc.Width(), rc.Height(), RGB(250, 250, 220));

//	for (int i=0; i<m_listActionLayer.GetCount(); i++)
//	{
//		ILayerUI* pcui = m_listActionLayer.GetAt(m_listActionLayer.FindIndex(i));
//		if (pcui && pcui->GetType() != ACTION_LAYER_TIME)
//		{
//			pcui->OnDraw(&memDC);
//		}
//	}

//	// 重绘时间层
//	ILayerUI* pLayer = FindLayer(ACTION_LAYER_TIME);
//	if (pLayer)
//	{
//		pLayer->OnDraw(&memDC);
//	}

//	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);
//	memDC.SelectObject(pOldBitmap);
//	bmpCache.DeleteObject();
//	memDC.DeleteDC();
//}
void CUILayerManager::SetHScrollPos(int nCol)
{
    //RETURN_SM
    PlayEvent_OnSetPos(nCol);
    return;
    CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
    CUIMotionLayer *pMotionLayer = (CUIMotionLayer*)FindLayer(ACTION_LAYER_MOTION);
    if (pTimeLayer/* && GetSafeHwnd()*/)
    {
        pTimeLayer->SetCurrentSelectCol(nCol);
        if (m_pEvent)
        {
            m_pEvent->LayerUIEvent_SelectChanger(nCol, pTimeLayer->GetScaleValue());
        }
    }
    //检查是否需要自动滚页
    if (/*GetSafeHwnd() &&*/ pMotionLayer)
    {
        int nScreenCell = pTimeLayer->GetScreenCellCount();
        int nScreenCellCount = nScreenCell - pTimeLayer->GetCellStartIndex();
        if (nScreenCellCount == 0)
        {
            return;
        }
        int nGroupIndex = 0;
        int nActionIndex = 0;
        int nOffsetTime = 0;
        pMotionLayer->GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffsetTime);
        if (nGroupIndex == -1 || nActionIndex == -1)
        {
            return;
        }
        int nCell = (nCol / (m_lineThick + m_nCellWidth));



        // 如果光标位置显示不在界面上，自动跳到光标位置处
        if (pMotionLayer->GetCellStartIndex() <= nCell && nCell <= nScreenCell)
        {

        }
        else
        {
            SetHScrollOffset(nCell);   //页数
            SetScrollPos(nCell);
        }
        if ((nScreenCell - nCell) / nScreenCellCount > 1)
        {
            // 			SetHScrollOffset(nCol / (m_lineThick + m_nCellWidth));
            // 			/*UpdateScrollBar();*/
            // 			SetScrollPos(nCell);
        }
        if (nCell == nScreenCell)
        {
            SetHScrollOffset(nCol / (m_lineThick + m_nCellWidth));
            /*UpdateScrollBar();*/
            SetScrollPos(nCell);
        }
    }
//	if (GetSafeHwnd())
//	{
//        Invalidate(false);
//	}
}
void CUILayerManager::SetHScrollOffset(uint offset)
{
//	for (int i=0; i<m_listActionLayer.GetCount(); i++)
//	{
//		ILayerUI* pcui = m_listActionLayer.GetAt(m_listActionLayer.FindIndex(i));
//		if (pcui)
//		{
//			pcui->SetHScrollOffset(offset);
//			pcui->SetCellStartIndex(offset);
//		}
//	}

//	if (GetSafeHwnd())
//	{
//        Invalidate(false);
//	}
}

//void CUILayerManager::OnHScroll(uint nSBCode, uint nPos, CScrollBar* pScrollBar)
//{
//	switch(nSBCode)
//	{
//	case SB_LINEDOWN:
//		if( m_hSi.nPos < m_hSi.nMax )
//			m_hSi.nPos++;
//		break;
//	case SB_LINEUP:
//		if( m_hSi.nPos > m_hSi.nMin )
//			m_hSi.nPos--;
//		break;
//	case SB_PAGEDOWN:
//		m_hSi.nPos += m_hSi.nPage;
//		break;
//	case SB_PAGEUP:
//		m_hSi.nPos -= m_hSi.nPage;
//		break;
//	case SB_THUMBTRACK:
//		m_hSi.nPos = nPos;
//		break;
//	case SB_THUMBPOSITION:
//		m_hSi.nPos = nPos;
//		break;
//	}

//	SetHScrollOffset(m_hSi.nPos);
//    SetScrollInfo(SB_HORZ, &m_hSi, false);
//    Invalidate(false);

//	__super::OnHScroll(nSBCode, nPos, pScrollBar);
//}

int CUILayerManager::GetColIndexByPosX(int x)
{
    if (m_listActionLayer.count() == 0)
        return -1;

    // 因为所有层都是同个宽高，所以只取第一层来计算
    ILayerUI* pLayerUI = FindLayerByIndex(0);
    if(pLayerUI == NULL)
        return -1;

    int xStart, yStart;

    // 先获取起始位置
    pLayerUI->GetPosition(xStart, yStart);

    int xTmp = xStart+pLayerUI->GetLineThick();

    int nCurrPosition = pLayerUI->GetCellStartIndex()*100 + x;
    return nCurrPosition;
// 	for (int i=pLayerUI->GetCellStartIndex(); i<=pLayerUI->GetCellCount(); i++)
// 	{
// 		if (x>=xTmp && x<xTmp+pLayerUI->GetCellWidth()+pLayerUI->GetLineThick())
// 			return i;
//
// 		xTmp += (pLayerUI->GetCellWidth()+pLayerUI->GetLineThick());
// 	}
//
// 	return -1;
}

// 根据Y坐标获取行号
int CUILayerManager::GetRowIndexByPosY(int y)
{
    if (m_listActionLayer.count() == 0)
        return -1;

    for (int i=0; i<m_listActionLayer.count(); i++)
    {
        ILayerUI* pcui = m_listActionLayer.at(i);
        if (pcui == NULL)
            continue;

        int xStart, yStart;
        // 先获取起始位置
        pcui->GetPosition(xStart, yStart);
        if (y >= yStart && y<yStart+pcui->GetCellHeight())
            return i;
    }

    return -1;
}
void CUILayerManager::IUILayerManager_SetMusicFile(int nLayerType , void *pData , int nDataLen , int nDataID)
{
    ILayerUI* pLayerUI = FindLayer(nLayerType);

    if (pLayerUI == NULL)
        return;

    if (m_pEvent)
    {
        //m_pEvent->LayerUIEvent_SetLayOptionHandler(pLayerUI->GetOptionHandler());
        m_pEvent->LayerUIEvent_SetMusicLayerSourceHandler(pLayerUI->GetOptionHandler(),nLayerType,pData , nDataLen, nDataID);
    }

    QPoint tempPoint(0,0);
    pLayerUI->OnLButtonDown(m_nCurCol, tempPoint);
}
void CUILayerManager::IUILayerManager_SingleBlockPlayingStop(int nLayerType)
{
    StopLayerDebug();
    //PlayEvent_OnDebugStop(nLayerType);
}
void CUILayerManager::IUILayerManager_SingleGroupPlaying(int nLayerType , CActionBlockGroupManager* pGroupManager)
{
    ILayerUI * itemFound= FindLayer(nLayerType);
    if (nLayerType == ACTION_LAYER_MUSIC)
    {
        m_IsMusicPlaying = true;
    }
    else if (nLayerType == ACTION_LAYER_MOTION)
    {
        m_IsDataPlaying = true;
    }
    else
        return;

    if (m_pGroupDataPlay)
    {
        m_pGroupDataPlay->StopPlay();
    }

    if (m_pMusicDataPlay)
    {
        m_pMusicDataPlay->ReleaseConnection();
        m_pMusicDataPlay = NULL;
    }
    m_nDebugType = SINGLE_LAYER_DEBUG;
    if (nLayerType == ACTION_LAYER_MOTION)
    {
//        if (m_pGroupDataPlay == NULL)
//        {
//            CreateGroupDataPlayer(&m_pGroupDataPlay);
//        }
//        m_pGroupDataPlay->Connect(itemFound, true, itemFound->GetScaleValue(), this);

    }
    if (itemFound)
    {
        itemFound->GetOptionHandler()->SetControlState(false);
        itemFound->SetLayerMenuState(true);
    }
}
void CUILayerManager::IUILayerManager_SingleBlockPlaying(int nLayerType ,CActionBlockData *pActionData)
{
    ILayerUI * itemFound= FindLayer(nLayerType);
    if (nLayerType == ACTION_LAYER_MUSIC)
    {
        m_IsMusicPlaying = true;
        if (m_pMusicDataPlay)
        {
            m_pMusicDataPlay->ReleaseConnection();
            m_pMusicDataPlay = NULL;
        }
    }
    else if (nLayerType == ACTION_LAYER_MOTION)
    {
        m_IsDataPlaying = true;
        if (m_pGroupDataPlay)
        {
            m_pGroupDataPlay->StopPlay();
        }
    }
    else
        return;
    m_nDebugType = SINGLE_LAYER_DEBUG;
    if (nLayerType == ACTION_LAYER_MOTION)
    {
//        if (m_pGroupDataPlay == NULL)
//        {
//            CreateGroupDataPlayer(&m_pGroupDataPlay);
//        }
//        m_pGroupDataPlay->Connect(itemFound, PLAYDEBUG_BLOCK, itemFound->GetScaleValue(), this);
//        m_pGroupDataPlay->StartBlockPlay(pActionData, Get3DRobotViewInterface());
    }
    if (itemFound)
    {
        itemFound->GetOptionHandler()->SetControlState(false);
        itemFound->SetLayerMenuState(true);
    }
}
bool CUILayerManager::IUILayerManager_IsRobotDebuging()
{
    return m_IsDataPlaying;
}
bool CUILayerManager::IUILayerManager_GetModifiedState()
{
    return m_bModified;
}
void CUILayerManager::IUILayerManager_SetModifiedState(bool bState)
{
    m_bModified = bState;
}
void CUILayerManager::IUILayerManager_DeleteMusicFile(int nLayerType)
{
    /*int nLayerType*/
    ILayerUI* pLayerUI = FindLayer(nLayerType);

    if (pLayerUI == NULL)
        return;
    if (m_pEvent)
    {
        m_pEvent->LayerUIEvent_DeleteMusciFile(pLayerUI->GetOptionHandler());
    }
}
void CUILayerManager::SetMusicBeatsTime(int nTime)
{
    ILayerUI* pLayerUI = FindLayer(ACTION_LAYER_MOTION);
    ILayerOptionHandler *pOptionHandler =  pLayerUI->GetOptionHandler();
    if(pOptionHandler)
    {
        pOptionHandler->SetMusicBeatsTime(nTime);
    }
}
void CUILayerManager::IUILayerManager_SwitchLayer(int nLayerID)
{
    ILayerUI* pLayerUI = FindLayer(nLayerID);

    if (pLayerUI == NULL)
        return;

    if (m_pEvent)
    {
        m_pEvent->LayerUIEvent_SetLayOptionHandler(pLayerUI->GetOptionHandler());
    }

    QPoint tempPoint(0,0);
    pLayerUI->OnLButtonDown(m_nCurCol, tempPoint);
}

//void CUILayerManager::OnMouseMove(uint nFlags, QPoint point)
//{
//	int nRow = GetRowIndexByPosY(point.y);
//	m_nCurCol = GetColIndexByPosX(point.x);
//	if (nRow == -1 || m_nCurCol == -1)
//		return;

//	if (nRow == 2)
//	{
//		ILayerUI* pLayer = FindLayerByIndex(2);
//		if (pLayer)
//		{
//			pLayer->OnMouseMove(point);
//		}

//        Invalidate(false);
//	}

//	__super::OnMouseMove(nFlags, point);
//}

//void CUILayerManager::OnLButtonDown(uint nFlags, QPoint point)
//{
//	int nRow = GetRowIndexByPosY(point.y);
//	m_nCurCol = GetColIndexByPosX(point.x);
//	if (nRow == -1 || m_nCurCol == -1)
//		return;
//	//绘图区外面不响应
//	if (m_nCurCol > GetCellCount() * (m_lineThick + m_nCellWidth))
//	{
//		return;
//	}
//	BYTE   KeyBuf[256];
//	memset(KeyBuf,0x00,256);
//	GetKeyboardState(KeyBuf);
//    if ((KeyBuf[VK_SHIFT] >> 7)== true)
//	{
//		ILayerUI* pLayer = FindLayerByIndex(2);
//		if (pLayer)
//		{
//			pLayer->OnLButtonDownAndShift(m_nCurCol);
//		}
//	} else {
//		ClearSelectState();
//		if (nRow != 2)
//		{
//			ILayerUI* pMotionLayer = FindLayerByIndex(2);
//			//m_pEvent->LayerUIEvent_SetActionBlockDataManager(pMotionLayer->GetBlockDataManager(), -1);
//		}

//		ILayerUI* pLayer = FindLayerByIndex(nRow);
//		if (pLayer)
//		{
//			if (pLayer->GetType() != ACTION_LAYER_TIME)
//			{
//				if (m_pEvent)
//				{
//					//m_pEvent->LayerUIEvent_SetLayOptionHandler(pLayer->GetOptionHandler());
//				}
//				if (pLayer)
//				{
//					pLayer->OnLButtonDown(m_nCurCol, point);
//				}
//				if (pLayer && pLayer->GetType() == ACTION_LAYER_MOTION)
//				{
//					KillTimer(0);
//					SetTimer(0,500,NULL);
//				}
//			}
//		}

//		// 重置选中的列
//		CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
//		if (pTimeLayer)
//		{
//			pTimeLayer->SetCurrentSelectCol(m_nCurCol);

//			if (m_pEvent)
//			{
//				m_pEvent->LayerUIEvent_SelectChanger(m_nCurCol, pTimeLayer->GetScaleValue());
//			}
//		}
//	}

//    Invalidate(false);
//	__super::OnLButtonDown(nFlags, point);
//}


//void CUILayerManager::OnLButtonUp(uint nFlags, QPoint point)
//{
//	BYTE   KeyBuf[256];
//	memset(KeyBuf,0x00,256);
//	GetKeyboardState(KeyBuf);
//    if ((KeyBuf[VK_SHIFT] >> 7)== true)
//	{
//		ILayerUI* pLayer = FindLayerByIndex(2);
//		if (pLayer)
//		{
//			pLayer->OnLButtonUpAndShift();
//		}
//	} else {
//		ILayerUI* pLayer = FindLayerByIndex(2);
//		if (pLayer)
//		{
//			pLayer->OnLButtonUp(point);
//		}
//	}

//	__super::OnLButtonUp(nFlags, point);
//}

bool CUILayerManager::ParseRawData(char* pData, int nLen)
{
    // 保存总的数据长度，先占位，后面再改为实际的长度
    int nTotalLen = 0;
    UBX_MEMCPY_INC(nTotalLen, pData);
    if (nTotalLen != nLen)
        return false;

    // 获取文件头
    char strTag[10] = "";
    memcpy(strTag, pData, 9);
    pData += 9;

    if (strcmp(strTag, UBX_FILE_TAG) != 0)
        return false;

    // 版本
    int nVersion = 0;
    UBX_MEMCPY_INC(nVersion, pData);
    if (nVersion != UBX_FILE_VERSION)
        return false;

    // 保存总的层数
    int nLayerCount = 0;
    UBX_MEMCPY_INC(nLayerCount, pData);

    for (int i=0; i<nLayerCount; i++)
    {
        char* pRawData = NULL;
        int nRawDataLen = 0;

        // 获取TYPE
        int nType = 0;
        UBX_MEMCPY_INC(nType, pData);

        ILayerUI* pLayer = NULL;

        AddAcionLayer(nType);
        pLayer = FindLayer(nType);

        // Raw DATA
        UBX_MEMCPY_INC(nRawDataLen, pData);
        if (nRawDataLen)
        {
            pRawData = new char[nRawDataLen];
            memcpy(pRawData, pData, nRawDataLen);
            pData += nRawDataLen;
            pLayer->SetData(pRawData, nRawDataLen);
            delete [] pRawData;
        }
        //AddActionLayer(pLayer);
    }
    //添加音乐文件
    ILayerUI *pMusicLayer = FindLayer(ACTION_LAYER_MUSIC);
    bool bRet = false;
    if (pMusicLayer)
    {
        bRet = pMusicLayer->AddMusicFromExtData(0,0);
    }
    if (!bRet && pMusicLayer)
    {
        pMusicLayer->AddMusicFromMusicDir();
    }
//	this->PostMessage(WM_SIZE);
    return true;
}

QString CUILayerManager::GetModuleDir()
{
//	HMODULE module = GetModuleHandle(0);
//	wchar_t pFileName[MAX_PATH];
//	GetModuleFileName(module, pFileName, MAX_PATH);

//    QString csFullPath(pFileName);
//    int nPos = csFullPath.ReverseFind( QObject::tr('\\') );
//	if( nPos < 0 )
//        return QString("");
//	else
//		return csFullPath.Left( nPos );
    QString strPath = QObject::tr("");
    return strPath;
}
//void CUILayerManager::OnSize(uint nType, int cx, int cy)
//{
//	__super::OnSize(nType, cx, cy);

//	ResetHScroll();
//	// TODO: 在此处添加消息处理程序代码
//}

QString CUILayerManager::GetLayerName(int nLayerID)
{
//	ILayerUI* pLayer = NULL;
//    QString strLayerName = L"";

//	if (nLayerID == ACTION_LAYER_TIME)
//	{
//		return L"";
//	}
//	else if (nLayerID == ACTION_LAYER_MOTION)
//	{
//		strLayerName = language_loadstring(IDS_MOTION_LAYER);
//	}
//	else if (nLayerID == ACTION_LAYER_EAR)
//	{
//		strLayerName = language_loadstring(IDS_EAR_LAYER);
//	}
//	else if (nLayerID == ACTION_LAYER_EYE)
//	{
//		strLayerName = language_loadstring(IDS_EYES_LAYER);
//	}
//	else if (nLayerID == ACTION_LAYER_MUSIC)
//	{
//		strLayerName = language_loadstring(IDS_MUSIC_LAYER);
//	}

    QString strLayerName = QObject::tr("");
    return strLayerName;
}

int CUILayerManager::GetCurrentSelTime()
{
    CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
    if (pTimeLayer)
    {
        return pTimeLayer->GetCurrentSelTime();
    }

    return 0;
}

int CUILayerManager::SetCurrentSelectCol(int nCol)
{
    CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
    if (pTimeLayer)
    {
        pTimeLayer->SetCurrentSelectCol(nCol);
    }

    return 0;
}
// void CUILayerManager::SetActionToOptions(int nCurrentGroupID , int nCurrentActionID)
// {
// 	if(m_pEvent)
// 	{
// 	    m_pEvent;
// 	}
// }
void CUILayerManager::SetActionDataGroupManagerToOptions(CActionBlockDataManager* pGroupManager, int nCurrentGroupID,int nCurrentActionID)
{
    if (m_pEvent)
    {
        m_pEvent->LayerUIEvent_SetActionBlockDataManager(pGroupManager, nCurrentGroupID,nCurrentActionID);
    }
}

void CUILayerManager::UpdateScrollBar()
{
    if (m_pEvent)
    {
        m_pEvent->LayerUIEvent_UpdateScrollBar();
    }
}

int CUILayerManager::GetNewMaxCellCount(int nMax)
{
    int nNeedTime = 0;
    // 跳过时间层
    for (int i=ACTION_LAYER_MOTION; i<MAX_ACTION_LAYER; i++)
    {
        ILayerUI* pLayer = FindLayer(i);
        if (pLayer)
        {
            if (pLayer->GetMaxTimeCellCount()>nNeedTime)
            {
                nNeedTime = pLayer->GetMaxTimeCellCount();
            }
        }
    }

    return nMax < nNeedTime ? nNeedTime : nMax;
}


//void CUILayerManager::OnLButtonDblClk(uint nFlags, QPoint point)
//{
//	// 获取所在行
//	int nRow = GetRowIndexByPosY(point.y);
//	// 获取所在列
//	int nCol = GetColIndexByPosX(point.x);

//	ILayerUI* pLayer;
//	if (nRow != -1)
//	{
//		pLayer = FindLayerByIndex(nRow);
//		if (pLayer == NULL)
//			return;
//	}

//	if (nCol == -1 && nRow != -1)
//	{
//		pLayer->AddBlankBlockInTail();
//	}

//	if (nRow == -1 || nCol == -1)
//		return;

//	// 时间层不处理
//	if (pLayer->GetType() != ACTION_LAYER_TIME)
//	{
//		pLayer->OnLButtonDBClick(nCol);
//        Invalidate(false);
//	}

//	__super::OnLButtonDblClk(nFlags, point);
//}

void CUILayerManager::Set3DRobotViewInterface(IUBXRobot3DView* pRobotView)
{
    m_pRobot3dView = pRobotView;
}

void CUILayerManager::SetTreeView(ILibTreeNavigation* pView)
{
    ILayerUI* pLayer = FindLayerByIndex(2);
    if (pLayer)
    {
        pLayer->SetTreeView(pView);
    }
}

void CUILayerManager::TreeToView(char *pData, int nLen, QPoint* nPoint, QString FileName)
{
//	ILayerUI* pLayer = FindLayerByIndex(2);
//	if (pLayer)
//	{
//		ScreenToClient(nPoint);
//		int nCol = GetColIndexByPosX(nPoint->x);
//		pLayer->TreeToView(pData, nLen, nCol, FileName);
//	}
}

//void CUILayerManager::OnRButtonDown(uint nFlags, QPoint point)
//{
//	// 获取所在行
//	int nRow = GetRowIndexByPosY(point.y);
//	// 获取所在列
//	int nCol = GetColIndexByPosX(point.x);

//	if (nRow == -1 || nCol == -1)
//		return;
//	//绘图区外面不响应
//	if (m_nCurCol > GetCellCount() * (m_lineThick + m_nCellWidth))
//	{
//		return;
//	}
//	ILayerUI* pLayer = FindLayerByIndex(nRow);

//	if (pLayer == NULL)
//		return;

//	// 清除选中的状态
//	ClearSelectState();
//	if (m_pEvent)
//	{
//		//m_pEvent->LayerUIEvent_SetLayOptionHandler(pLayer->GetOptionHandler());
//	}

//	// 重置选中的列
//	CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
//	if (pTimeLayer)
//	{
//		pTimeLayer->SetCurrentSelectCol(nCol);

//		if (m_pEvent)
//		{
//			m_pEvent->LayerUIEvent_SelectChanger(nCol, pTimeLayer->GetScaleValue());
//		}
//	}

//	// 时间层不处理
//	if (pLayer->GetType() != ACTION_LAYER_TIME)
//	{

//		pLayer->OnRButtonDdown(nCol);
//        Invalidate(false);
//	}

//	__super::OnRButtonDown(nFlags, point);
//}

void CUILayerManager::SetTimeScaleValues(int timeScale)
{
    m_scaleValue = timeScale;

}
void CUILayerManager::FreshTimeScaleValues(int timeSacle)
{
    if (m_pEvent)
    {
        m_pEvent->LayerUIEvent_SetScaleValue(timeSacle);
    }
}
int CUILayerManager::GetTimeScaleValues()
{
    return m_scaleValue;
}

int CUILayerManager::GetLineThick()
{
    return m_lineThick;
}

void CUILayerManager::SetLineThick(int nThick)
{
    m_lineThick = nThick;
}

int CUILayerManager::GetCellWidth()
{
    return m_nCellWidth;
}

int CUILayerManager::GetCellCount()
{
    return m_CellCount;
}

void CUILayerManager::SetCellCount(int nCount)
{
    m_CellCount = nCount;
}

void CUILayerManager::SetCellWidth(int nWidth)
{
    m_nCellWidth = nWidth;
}

void CUILayerManager::LayerPlaying(int layerType, ILayerOptionHandler* optionHandle)
{
    ILayerUI* itemFound = FindLayer(layerType);
    if (layerType == ACTION_LAYER_MUSIC)
    {
        m_IsMusicPlaying = true;
    }
    else if (layerType == ACTION_LAYER_MOTION)
    {
        m_IsDataPlaying = true;
    }
    else
        return;

    if (m_pGroupDataPlay)
    {
        m_pGroupDataPlay->StopPlay();
    }

    if (m_pMusicDataPlay)
    {
        m_pMusicDataPlay->ReleaseConnection();
        m_pMusicDataPlay = NULL;
    }
    m_nDebugType = SINGLE_LAYER_DEBUG;
    if (layerType == ACTION_LAYER_MOTION)
    {
//        if (m_pGroupDataPlay == NULL)
//        {
//            CreateGroupDataPlayer(&m_pGroupDataPlay);
//        }
//        m_pGroupDataPlay->Connect(itemFound, false, itemFound->GetScaleValue(), this);
        int nGroupIndex = 0;
        int nActionIndex = 0;
        int nOffset = 0;
        int nCursorPos = GetCurrentSelTime();
        ((CUIMotionLayer*)itemFound)->GetSelectGroupAction(nCursorPos,nGroupIndex,nActionIndex,nOffset);

        m_pGroupDataPlay->SetCurrentPos(nGroupIndex,nActionIndex,nOffset * GetTimeScaleValues()); //
        m_pGroupDataPlay->StartPlay(NULL, m_pRobot3dView);
    }
    if (layerType == ACTION_LAYER_MUSIC)
    {
        CreateMusicDataPlayer(&m_pMusicDataPlay);
        m_pMusicDataPlay->Connect(itemFound, false, itemFound->GetScaleValue(), this);
        int nCursorPos = GetCurrentSelTime();
        int nStartTime = ((CUIMusicLayer*)itemFound)->GetActStartTime();
        int nEndTime = ((CUIMusicLayer*)itemFound)->GetActEndTime();
        if (nCursorPos < nStartTime || nCursorPos >nEndTime)
        {
            nCursorPos = nStartTime;
        }
        m_pMusicDataPlay->OnSetPlayTime(nCursorPos , nEndTime);
        m_pMusicDataPlay->StartPlay(NULL, m_pRobot3dView);
    }

    itemFound->GetOptionHandler()->SetControlState(false);
    itemFound->SetLayerMenuState(true);
}
void CUILayerManager::LayerStop()
{
    if (m_pMusicDataPlay && m_IsMusicPlaying)
    {
        /*m_pMusicDataPlay->StopPlay();*/
        m_pMusicDataPlay->ReleaseConnection();
        m_pMusicDataPlay = NULL;
        m_IsMusicPlaying = false;
    }
    if (m_pGroupDataPlay && m_IsDataPlaying)
    {
        m_pGroupDataPlay->StopPlay();
        m_IsDataPlaying = false;
    }
    PlayEvent_OnSetPos(0);
}
void CUILayerManager::StopLayerDebug()
{
    if (m_pGroupDataPlay && m_IsDataPlaying)
    {
        m_pGroupDataPlay->StopPlay();
        m_IsDataPlaying =false;
    }
    if (m_pMusicDataPlay && m_IsMusicPlaying)
    {
        m_pMusicDataPlay->ReleaseConnection();
        m_pMusicDataPlay = NULL;
        m_IsMusicPlaying = false;
    }
// 	ILayerUI* itemFound = FindLayer(ACTION_LAYER_MOTION);
// 	itemFound->GetOptionHandler()->SetControlState(true);
// 	itemFound->SetLayerMenuState(false);
}
void CUILayerManager::SetOriginPos(int nCol)
{
    PlayEvent_OnSetPos(nCol);
}

void CUILayerManager::PlayEvent_SetDebugType(int nDebugTyoe)
{
    m_nDebugType = nDebugTyoe;
}
int  CUILayerManager::PlayEvent_GetDebugType()
{
    return m_nDebugType;
}
void CUILayerManager::PlayEvent_OnEnableCtrl(int nLayerType ,bool bState)
{
// 	CUIMainLayerLayout *pParentDlg = (CUIMainLayerLayout*)GetParent();
// 	if (pParentDlg)
// 	{
// 		pParentDlg->EnablePlay(nLayerType,bState);
// 	}
    if(m_pEvent)
    {
        m_pEvent->EnablePlay(nLayerType,bState);
    }
}
void CUILayerManager::PlayEvent_OnSetPosChangeToOptions(int nLayerType , int nCol)
{
/*	return;*/
     CUIMotionLayer *pMotionLayer = (CUIMotionLayer*)FindLayer(ACTION_LAYER_MOTION);
     if (pMotionLayer)
     {
         pMotionLayer->PlayEvent_OnSetPosChangeToOptions(nCol);
     }
}
void CUILayerManager::PlayEvent_OnSetPos(int nCol)
{
    CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
    CUIMusicLayer *pMusicLayer = (CUIMusicLayer*)FindLayer(ACTION_LAYER_MUSIC);
    if (pTimeLayer/* && GetSafeHwnd()*/)
    {
        pTimeLayer->SetCurrentSelectCol(nCol);
        if (m_pEvent)
        {
            m_pEvent->LayerUIEvent_SelectChanger(nCol, pTimeLayer->GetScaleValue());
        }
    }

    //检查是否需要自动滚页
    if (/*pMusicLayer &&*//* GetSafeHwnd()*/true)
    {
        int nScreenCell = pMusicLayer->GetScreenCellCount();
        //nScreenCell *= (m_lineThick + m_nCellWidth);
        int nScreenCellCount = nScreenCell - pMusicLayer->GetCellStartIndex();

        if (nScreenCellCount == 0)
        {
            return;
        }
        int nCell = (nCol / (m_lineThick + m_nCellWidth));
        // 如果光标位置显示不在界面上，自动跳到光标位置处
        if (pMusicLayer->GetCellStartIndex() <= nCell  && nCell <= nScreenCell)
        {

        }
        else
        {
            SetHScrollOffset(nCell);   //页数
            SetScrollPos(nCell);
        }
        if ((nScreenCell - nCell) / nScreenCellCount > 1)
        {
// 			SetHScrollOffset(nCol / (m_lineThick + m_nCellWidth));
// 			/*UpdateScrollBar();*/
// 			SetScrollPos(nCell);
        }
        if (nCell == nScreenCell)
        {
            SetHScrollOffset(nCol / (m_lineThick + m_nCellWidth));
            /*UpdateScrollBar();*/
            SetScrollPos(nCell);
        }
    }
//    if (GetSafeHwnd())
//    {
//        Invalidate(false);
//    }


}
//void CUILayerManager::PlayEvent_On
void CUILayerManager::PlayEvent_OnDebugStop(int nLayerType)
{
// 	m_IsSinglePlaying = false;
// 	m_pOptionHandler->SetControlState(true);
    if (nLayerType == ACTION_LAYER_MUSIC)
    {
        m_IsMusicPlaying = false;
    }
    else if ( nLayerType == ACTION_LAYER_MOTION)
    {
        m_IsDataPlaying = false;
    }

    if (m_pEvent)
    {
        m_pEvent->LayerDebugStop(nLayerType);
    }

    ILayerUI* itemFound = FindLayer(nLayerType);
    if (itemFound)
    {
        itemFound->GetOptionHandler()->SetControlState(true);
        itemFound->SetLayerMenuState(false);
    }
}

void CUILayerManager::LayerDebugStop(int layerType)
{
// 	m_IsMusicPlaying = false;
// 	if (m_pEvent)
// 	{
// 		m_pEvent->LayerDebugStop(layerType);
// 	}
//
// 	ILayerUI* itemFound = FindLayer(layerType);
// 	itemFound->GetOptionHandler()->SetControlState(true);
// 	itemFound->SetLayerMenuState(false);
}

ILayerOptionHandler* CUILayerManager::GetOptionHandler()
{
    return m_pEvent->LayerUIEvent_GetOptionHandler();
}

void CUILayerManager::IUILayerManager_SetSocketSession(CIUBXSocketSession* pSession)
{
    m_pSocketSession = pSession;
}

int CUILayerManager::GetCurrentSelectCol()
{
    CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
    if (pTimeLayer)
    {
        return pTimeLayer->GetCurrentSelectCol();
    }

    return 0;
}

void CUILayerManager::TotalLayerPlaying()
{
    if (m_pGroupDataPlay)
    {
        m_pGroupDataPlay->StopPlay();
    }

    if (m_pMusicDataPlay)
    {
        m_pMusicDataPlay->ReleaseConnection();
        m_pMusicDataPlay = NULL;
    }
    m_IsMusicPlaying = true;
    m_IsDataPlaying= true;
    ILayerUI* itemMotion = FindLayer(ACTION_LAYER_MOTION);
    m_nDebugType = ALL_LAYER_DEBUG;
    if (itemMotion)
    {/*
        if(m_pGroupDataPlay == NULL)
        {
            CreateGroupDataPlayer(&m_pGroupDataPlay);
        }
        m_pGroupDataPlay->Connect(itemMotion, false, itemMotion->GetScaleValue(), this );*/
        int nGroupIndex = 0;
        int nActionIndex = 0;
        int nOffset = 0;
        int nCursorPos = GetCurrentSelTime();
        ((CUIMotionLayer*)itemMotion)->GetSelectGroupAction(nCursorPos,nGroupIndex,nActionIndex,nOffset);

        m_pGroupDataPlay->SetCurrentPos(nGroupIndex,nActionIndex,nOffset * GetTimeScaleValues()); //
        m_pGroupDataPlay->StartPlay(NULL, m_pRobot3dView);

        itemMotion->GetOptionHandler()->SetControlState(false);
        itemMotion->SetLayerMenuState(true);
    }

    ILayerUI* itemMusic = FindLayer(ACTION_LAYER_MUSIC);
    if (itemMusic)
    {
//        CreateMusicDataPlayer(&m_pMusicDataPlay);
//        m_pMusicDataPlay->Connect(itemMusic, false, itemMusic->GetScaleValue(), this );
//        int nCursorPos = GetCurrentSelTime();
//        int nEndTime = ((CUIMusicLayer*)itemMusic)->GetEndTime();
//        m_pMusicDataPlay->OnSetPlayTime(nCursorPos , nEndTime);
//        m_pMusicDataPlay->StartPlay(NULL, m_pRobot3dView);
//        itemMusic->GetOptionHandler()->SetControlState(false);
//        itemMusic->SetLayerMenuState(true);
    }
}
void CUILayerManager::PauseTotalLayerDebug()
{
    if (m_pGroupDataPlay && m_IsDataPlaying)
    {
        m_pGroupDataPlay->StopPlay();
        m_IsDataPlaying = false;
    }

    if (m_pMusicDataPlay && m_IsMusicPlaying)
    {
        m_pMusicDataPlay->StopPlay();
        m_pMusicDataPlay->ReleaseConnection();
        m_pMusicDataPlay = NULL;
        m_IsMusicPlaying = false;
    }
    /*m_bTotalPlaying =false;*/
    ILayerUI* itemMotion = FindLayer(ACTION_LAYER_MOTION);
    if (itemMotion)
    {
        itemMotion->CancelBlockPlay();
    }

    ILayerUI* itemMusic = FindLayer(ACTION_LAYER_MUSIC);
    if (itemMusic)
    {
        itemMusic->CancelBlockPlay();
    }
}
void CUILayerManager::StopTotalLayerDebug()
{
    if (m_pGroupDataPlay && m_IsDataPlaying)
    {
        m_pGroupDataPlay->StopPlay();
        m_IsDataPlaying = false;
    }

    if (m_pMusicDataPlay && m_IsMusicPlaying)
    {
        m_pMusicDataPlay->StopPlay();
        m_pMusicDataPlay->ReleaseConnection();
        m_pMusicDataPlay = NULL;
        m_IsMusicPlaying = false;
    }
    /*m_bTotalPlaying =false;*/
    ILayerUI* itemMotion = FindLayer(ACTION_LAYER_MOTION);
    if (itemMotion)
    {
        itemMotion->CancelBlockPlay();
    }

    ILayerUI* itemMusic = FindLayer(ACTION_LAYER_MUSIC);
    if (itemMusic)
    {
        itemMusic->CancelBlockPlay();
    }
    PlayEvent_OnSetPos(0);
}


//void CUILayerManager::OnTimer(uint_PTR nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	ILayerUI *pMotionLayer = FindLayer(ACTION_LAYER_MOTION);
//	if (pMotionLayer)
//	{
//		//int nCol  = GetCurrentSelectCol();
//		pMotionLayer->OnLButtonDownDebug(m_nCurCol);
//	}
//	KillTimer(nIDEvent);
//	__super::OnTimer(nIDEvent);
//}


//bool CUILayerManager::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: 在此添加专用代码和/或调用基类

//	if (pMsg->message == WM_KEYUP)
//	{
//		if (pMsg->wParam == VK_LEFT)
//		{
//			int nCol = GetCurrentSelectCol();
//			SetCurrentSelectCol(nCol + 1);
//		}
//	}
//	return __super::PreTranslateMessage(pMsg);
//}
