#include <QFile>
#include <QFileInfo>

#include "ui_UILayerManager.h"

#include "stable.h"
#include "UILayerManager.h"
#include "UIMainLayerLayout.h"
//#include "UITimeLayer.h"
#include "LayerUI/UIMotionLayer.h"
// #include "UIEyesLayer.h"
#include "LayerUI/UIMainLayerLayout.h"
#include "LayerUI/MotionDataPlay.h"
#include "LayerUI/UIMusicLayer.h"
#include "LayerUI/IGroupDataPlay.h"
#include "LayerUI/UITimeLayer.h"

//#include "ubxundoredomanager.h"

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


CUILayerManager::CUILayerManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CUILayerManager)
{

    m_CellCount = 10;
    m_lineThick = 1;
    m_nCellWidth = 99;
    m_scaleValue = 10;
    m_nCurCol = 100;
    m_pMusicDataPlay = NULL;
    m_IsDataPlaying = false;
    m_IsMusicPlaying = false;
//	m_pChooseDlg = NULL;
    m_bModified = false;
    m_bMusicModified = false;
    m_nDebugType = SINGLE_LAYER_DEBUG;
    m_pGroupDataPlay = NULL;
    m_pMusicDataPlay = NULL;
    m_ePlayType = ePlayTypeNone;

    connect(this, &CUILayerManager::sigOnMotionPosChanged, this, &CUILayerManager::slotOnMotionPosChanged, Qt::QueuedConnection);

    SetMsgType Msg;
    Msg.clear();
    Msg.insert(WM_MSG_EXECUTE_REDO);
    Msg.insert(WM_MSG_EXECUTE_UNDO);
    Msg.insert(NOTIFY_MSG_PLAY_MOTION);
    Msg.insert(NOTIFY_MSG_PLAY_MUSIC);
    Msg.insert(NOTIFY_MSG_UNITED_PLAY);
    Msg.insert(NOTIFY_MSG_STOP_PLAY);
    this->AddSubscribe(Msg);
    this->EnableRecvNotify(TRUE);
}

CUILayerManager::~CUILayerManager()
{
    if (m_pGroupDataPlay)
    {
        m_pGroupDataPlay->StopPlay();
        m_pGroupDataPlay->ReleaseConnection();
    }

    if (m_pMusicDataPlay)
    {
        m_pMusicDataPlay->StopPlay();
        m_pMusicDataPlay->ReleaseConnection();
    }

    delete ui;
}

void CUILayerManager::OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    zxEvent* pEvent = new zxEvent(nMsgID);
    pEvent->eEventType = zxEventTypeWindow;
    pEvent->lReceiver = (long)this;
    pEvent->dwMsgId = nMsgID;
    pEvent->dwContext = dwContext;
    pEvent->wParam = wParam;
    pEvent->lParam = lParam;
    QCoreApplication::postEvent(this, pEvent);
}

void CUILayerManager::updateTabName()
{
    CUIMainLayerLayout *ui_mainlayer_layout = dynamic_cast<CUIMainLayerLayout*> (m_pEvent);
    if (NULL == ui_mainlayer_layout)
    {
        return;
    }

    if (IUILayerManager_GetMusicModifiedState() || IUILayerManager_GetModifiedState())
    {
        ui_mainlayer_layout->setTabWidgetTabName(this, QString("%1*").arg(m_strFileName));
    }
    else
    {
        ui_mainlayer_layout->setTabWidgetTabName(this, QString("%1").arg(m_strFileName));
    }
}

/************************************
* 名称: updateMotionData
* 功能: 更新动作数据
* 参数: [in] pData: 数据
* 返回: void
* 时间:   2017/01/09
* 作者:   hwx
************************************/
void CUILayerManager::updateMotionData(STMotionData *pData)
{
    if (!pData)
    {
        return;
    }

    CActionBlockDataManager *pDataManager = pData->m_pMotionData;
    if (!pDataManager)
    {
        return;
    }

    char *pMotionData = NULL;
    int nLen = 0;
    pDataManager->GetData(&pMotionData, nLen);
    setMotionLayerData(pMotionData, nLen);

    // 刷新选择线位置
    SetCurrentSelectCol(pData->m_nCurSelectTime);
}

/************************************
* 名称: doProcessSimMsg
* 功能: 处理仿真消息
* 参数: [in] nMsgID: 消息ID
* 参数: [in] eStatus: 播放状态
* 返回: void
* 时间:   2017/03/18
* 作者:   hwx
************************************/
void CUILayerManager::doProcessSimMsg(int nMsgID, emPlayStatus eStatus)
{
    switch (eStatus) {
    case ePlayStatusPause:
    case ePlayStatusPlaying:
    {
        if (nMsgID == NOTIFY_MSG_PLAY_MOTION)
        {
            m_ePlayType = ePlayTypeMotion;
        }
        else if (nMsgID == NOTIFY_MSG_PLAY_MUSIC)
        {
            m_ePlayType = ePlayTypeMusic;
        }
        else if (nMsgID == NOTIFY_MSG_UNITED_PLAY)
        {
            m_ePlayType = ePlayTypeUnited;
        }
        break;
    }
    case ePlayStatusStop:
    case ePlayStatusFinished:
    {
        m_ePlayType = ePlayTypeNone;
    }
    default:
        m_ePlayType = ePlayTypeNone;
        break;
    }
}

/************************************
* 名称: updateMotionWidget
* 功能: 刷新动作编辑界面
* 参数: [in] nTime: 时间值，并非真实时间，需要x时间粒度
* 返回: void
* 时间:   2017/03/02
* 作者:   hwx
************************************/
void CUILayerManager::updateMotionWidget(int nTime)
{
    // 时间线位置变化后，需要同步更新动作编辑表格
    ILayerUI *pMotionLayer = (CUIMotionLayer*)FindLayer(ACTION_LAYER_MOTION);
    if (!pMotionLayer)
    {
        return;
    }

    CActionBlockDataManager *pActionBlockDataManager = pMotionLayer->GetBlockDataManager();
    if (!pActionBlockDataManager)
    {
        return;
    }

    int nGroupIndex = 0;
    int nFrameIndex = 0;
    int nOffset = 0;
    pActionBlockDataManager->getSelectGroupFrameArrayIndex(nTime, nGroupIndex, nFrameIndex, nOffset);
    CActionBlockGroupManager *pGroup = pActionBlockDataManager->GetAt(nGroupIndex);
    if (!pGroup)
    {
        return;
    }

    g_IsUpdateUIByData = true;
    SetActionDataGroupManagerToOptions(pActionBlockDataManager, pGroup->GetID(), nFrameIndex);
    g_IsUpdateUIByData = false;
}

void CUILayerManager::slotOnMotionPosChanged(int nTime)
{
    //qDebug() << "CUILayerManager::slotOnMotionPosChanged: " << nTime << endl;
    updateMotionWidget(nTime);
}

void CUILayerManager::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CUILayerManager::customEvent(QEvent *event)
{
    zxEvent* pEvent = (zxEvent*)event;
    DWORD nMsgID = pEvent->dwMsgId;
    DWORD nOperationCode = pEvent->dwContext;
    WPARAM wParam = pEvent->wParam;
    LPARAM lParam = pEvent->lParam;

    if ((nMsgID == WM_MSG_EXECUTE_REDO || nMsgID == WM_MSG_EXECUTE_UNDO) &&
            (nOperationCode == eCodeHandleMotionData))
    {
        updateMotionData((STMotionData *)lParam);
    }
    else if (nMsgID == NOTIFY_MSG_PLAY_MOTION || nMsgID == NOTIFY_MSG_PLAY_MUSIC || nMsgID == NOTIFY_MSG_UNITED_PLAY)
    {
        doProcessSimMsg(nMsgID, (emPlayStatus)lParam);
    }
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

bool CUILayerManager::Connect(QWidget* pParent, IUILayerManager_Event* pEvent)
{
//    QRect rect;
    setParent(pParent);
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    ui->setupUi(this);
    m_pEvent = pEvent;
    return true;
}

/**************************************************************************
* 函数名:ReleaseConnection
* 功能:解除链接
* 参数:
* 返回值:
* 时间:2016/01/06 14：00
* 作者: 周志平
* 改动日志  现在解除链接不删除层，改为删除相应层数据
**************************************************************************/
void CUILayerManager::ReleaseConnection()
{
    if (m_pGroupDataPlay)
    {
        m_pGroupDataPlay->ReleaseConnection();
        m_pGroupDataPlay = NULL;
    }
    if (m_pMusicDataPlay)
    {
        m_pMusicDataPlay->ReleaseConnection();
        m_pMusicDataPlay = NULL;
    }

    disconnect();
    DeleteAllLayer();
    delete this;
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
//    return m_listActionLayer.takeLast();
    return m_listActionLayer.last();
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
//    ILayerUI* pLayer = NULL;
//    pLayer = FindLayer(ACTION_LAYER_MUSIC);
//    ((CUIMusicLayer*)pLayer)->AddMusic(NULL,false);
}

/**************************************************************************
* 函数名:AddAcionLayer
* 功能:根据类型增加动作层界面
* 参数:int nLayerID
* 返回值: bool 如果添加成功，返回真，否知返回假
* 时间:2015/12/25 11:25
* 作者: 周志平
* 改动日志  2015/12/25 11:25  把音乐层、时间层屏蔽，并自动connnect 动作层
**************************************************************************/
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
    pLayer->Connect(this);//hels Conenct必须在SetLayerManager后面（Connect中会用到LayerManager）
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

/**************************************************************************
* 函数名:DeleteActionLayer
* 功能:根据ID删除响应层
* 参数: int nTypeID   层ID
* 返回值: bool 当删除成功时，返回真，否则返回假
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
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
/**************************************************************************
* 函数名:FindLayerByIndex
* 功能:根据层索引查找层
* 参数:int nIndex   索引值
* 返回值: ILayerUI* 返回获取的层
* 时间:2015/12/22 10:57
* 作者: 周志平
* 改动日志：2015/12/22 10:57 修改了查找方法
**************************************************************************/
ILayerUI* CUILayerManager::FindLayerByIndex(int nIndex)
{
    ILayerUI* itemFound = NULL;
//    int  pos = m_listActionLayer.indexOf(m_listActionLayer.at(nIndex));
//    if (pos>0)
//    {
//        itemFound = m_listActionLayer.at(pos);
//    }
     itemFound = m_listActionLayer.at(nIndex);
    return itemFound;
}
/**************************************************************************
* 函数名: DeleteAllLayer
* 功能:删除所有动作层
* 参数:
* 返回值:
* 时间:2015/12/25 18:13
* 作者: 周志平
**************************************************************************/
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
        }
    }
    m_listActionLayer.clear();
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

/**************************************************************************
* 函数名: SetData
* 功能:
* 参数:
* 返回值:
* 时间: 2015/12/30 14:26
* 作者: 周志平
* 改动日志    2016/1/6号  设置数据时，暂时不做DeleteAllLayer();改为DeleteLayerData()
**************************************************************************/
bool CUILayerManager::SetData(char* pData, int nLen)
{
    DeleteAllLayer();
    //这里统一添加Layer
    AddAcionLayer(ACTION_LAYER_TIME);
    AddAcionLayer(ACTION_LAYER_MUSIC);
    AddAcionLayer(ACTION_LAYER_MOTION);
    if (pData != NULL && nLen!=0)
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
            g_IsUpdateUIByData = true;
            m_pEvent->LayerUIEvent_SetActionBlockDataManager(pLayer->GetBlockDataManager(), 0);
            UpdateMP3Widget(pLayer->GetBlockDataManager(), 0, 0);
            g_IsUpdateUIByData = false;

            // 刷新编辑动作表格数据
            ILayerOptionHandler* pOptionHandler = pLayer->GetOptionHandler();
            if (pOptionHandler)
            {
                  pLayer->OnLButtonDown(pOptionHandler->GetCurrentSelectCol(), QPoint(0, 0), 0);
            }
        }
    }
    //m_LocationLayerDlg.ResetScrollBar(m_LayoutRange);
    //m_LocationLayerDlg.ResizeAllLayer();

    return true;
}

bool CUILayerManager::setMotionLayerData(char *pData, int nLen)
{
    // 设置默认的属性处理对象
    if (!m_pEvent)
    {
        return false;
    }

    ILayerUI* pMotionLayer = FindLayer(ACTION_LAYER_MOTION);
    if (!pMotionLayer)
    {
        return false;
    }

    pMotionLayer->SetData(pData, nLen);
    m_pEvent->LayerUIEvent_SetLayOptionHandler(pMotionLayer->GetOptionHandler());

    CActionBlockDataManager *pActionBlockDataManager = pMotionLayer->GetBlockDataManager();
    if (!pActionBlockDataManager)
    {
        return false;
    }

    // 获取音乐层当前选择线选中的时间位置，注意已经除以了时间粒度
    int nMusicLayerCurSelectTime = GetCurrentSelectCol();

    int nGroupIndex = 0;
    int nFrameIndex = 0;
    int nOffset = 0;
    // 返回的都是数组下标
    pActionBlockDataManager->getSelectGroupFrameArrayIndex(nMusicLayerCurSelectTime, nGroupIndex, nFrameIndex, nOffset);

    int nGroupId = 0;

    CActionBlockGroupManager *pGroupManager = pActionBlockDataManager->GetAt(nGroupIndex);
    if (pGroupManager)  // 针对选择线落在动作组外的情况，此时即使没有获取到数据，还是需要刷新界面的
    {
        nGroupId = pGroupManager->GetID();
//        CActionBlockData *pBlockData = pGroupManager->GetAt(nFrameIndex);
//        if (pBlockData)
//        {
//            nFrameId = pBlockData->GetID();
//        }
//        return false;
    }

    g_IsModifyGroup = true;
    g_IsUpdateUIByData = true;
    // 注意该函数参数：nGroupId为动作组id，nFrameIndex为动作帧在数组中的下标
    m_pEvent->LayerUIEvent_SetActionBlockDataManager(pActionBlockDataManager, nGroupId, nFrameIndex);
    UpdateMP3Widget(pActionBlockDataManager, nGroupId, nFrameIndex);
    g_IsUpdateUIByData = false;
}


/**************************************************************************
* 函数名:GetData
* 功能:获取动作数据
* 参数:char** ppData    数据
* 参数:int &nLen        数据长度
* 返回值:
* 时间:2015/12/22 10:48
* 作者: 周志平
**************************************************************************/
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
    int nLayerCount =GetLayerCount();   // 直接调用 layerManager 函数获取,去掉时间层
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


void CUILayerManager::SetHScrollPos(int nCol)
{
//    //RETURN_SM
//    PlayEvent_OnSetPos(nCol);
//    return;
////    CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
//    CUIMotionLayer *pMotionLayer = (CUIMotionLayer*)FindLayer(ACTION_LAYER_MOTION);
//    if (pTimeLayer/* && GetSafeHwnd()*/)
//    {
//        pTimeLayer->SetCurrentSelectCol(nCol);
//        if (m_pEvent)
//        {
//            m_pEvent->LayerUIEvent_SelectChanger(nCol, pTimeLayer->GetScaleValue());
//        }
//    }
//    //检查是否需要自动滚页
//    if (/*GetSafeHwnd() &&*/ pMotionLayer)
//    {
//        int nScreenCell = pTimeLayer->GetScreenCellCount();
//        int nScreenCellCount = nScreenCell - pTimeLayer->GetCellStartIndex();
//        if (nScreenCellCount == 0)
//        {
//            return;
//        }
//        int nGroupIndex = 0;
//        int nActionIndex = 0;
//        int nOffsetTime = 0;
//        pMotionLayer->GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffsetTime);
//        if (nGroupIndex == -1 || nActionIndex == -1)
//        {
//            return;
//        }
//        int nCell = (nCol / (m_lineThick + m_nCellWidth));



//        // 如果光标位置显示不在界面上，自动跳到光标位置处
//        if (pMotionLayer->GetCellStartIndex() <= nCell && nCell <= nScreenCell)
//        {

//        }
//        else
//        {
//            SetHScrollOffset(nCell);   //页数
//            SetScrollPos(nCell);
//        }
//        if ((nScreenCell - nCell) / nScreenCellCount > 1)
//        {
//            // 			SetHScrollOffset(nCol / (m_lineThick + m_nCellWidth));
//            // 			/*UpdateScrollBar();*/
//            // 			SetScrollPos(nCell);
//        }
//        if (nCell == nScreenCell)
//        {
//            SetHScrollOffset(nCol / (m_lineThick + m_nCellWidth));
//            /*UpdateScrollBar();*/
//            SetScrollPos(nCell);
//        }
//    }
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

/**************************************************************************
* 函数名: DeleteLayerData
* 功能:
* 参数:
* 返回值:
* 时间:2015/12/30 14:57
* 作者: 周志平
* 改动日志
**************************************************************************/
void CUILayerManager::DeleteLayerData()
{
    for (int i=0; i<m_listActionLayer.count(); i++)
    {
        ILayerUI* pcui = m_listActionLayer.at(i);
        if (pcui)
        {
            pcui->ReleaseConnection();
        }
    }
}

void CUILayerManager::UpdateMP3Widget(CActionBlockDataManager* pDataManager, int nCurrentGroupID,int nCurrentActionID)
{
    ILayerUI* pLayer = FindLayer(ACTION_LAYER_MUSIC);
    if(NULL == pLayer)
        return;
    ILayerOptionHandler* pOptionHandler = pLayer->GetOptionHandler();
    if(NULL != pOptionHandler)
    {
        pOptionHandler->SetActionBlockDataManager(pDataManager, nCurrentGroupID, nCurrentActionID);
    }
}

void CUILayerManager::UpdateMP3Action(int nGroupID, float fTime)
{
    ILayerUI* pLayer = FindLayer(ACTION_LAYER_MUSIC);
    if(NULL == pLayer)
        return;
    ILayerOptionHandler* pOptionHandler = pLayer->GetOptionHandler();
    if(NULL != pOptionHandler)
    {
        pOptionHandler->UpdateMP3Action(nGroupID, fTime);
    }
}

void CUILayerManager::FinishMusicPlaying()
{
    // 联合仿真但是没有加载音乐，此时暂停播放不要回到起始位置
    PlayEvent_OnSetPos(0);
    m_IsMusicPlaying = false;

    if(!m_IsDataPlaying)
    {
        //hels 通知Motion模块刷新按钮
        ILayerUI* pLayer = pLayer = FindLayer(ACTION_LAYER_MOTION);
        if(NULL == pLayer)
            return;
        ILayerOptionHandler* pOptionHandler = pLayer->GetOptionHandler();
        if(NULL != pOptionHandler && !m_IsDataPlaying && !m_IsMusicPlaying)
        {
            pOptionHandler->MotionPlayStop(ePlayStatusFinished);
        }

        /*m_bTotalPlaying =false;*/
        ILayerUI* itemMotion = FindLayer(ACTION_LAYER_MOTION);
        if (itemMotion)
        {
            itemMotion->onCancelPlayMotionBlock();
            itemMotion->GetOptionHandler()->FinishPlayingMusic();
        }
    }
}

/************************************
* 名称: PauseMusicPlaying
* 功能: 暂停播放音乐
* 参数:
* 返回: void
* 时间:   2017/03/17
* 作者:   hwx
************************************/
void CUILayerManager::PauseMusicPlaying()
{
    m_IsMusicPlaying = false;
}

/************************************
* 名称: StartMusicPlaying
* 功能: 开始播放音乐
* 参数:
* 返回: void
* 时间:   2017/03/17
* 作者:   hwx
************************************/
void CUILayerManager::StartMusicPlaying()
{
    m_IsMusicPlaying = true;
}

void CUILayerManager::HasMotionData(bool hasData)
{
    ILayerUI* pLayer = FindLayer(ACTION_LAYER_MUSIC);
    if(NULL == pLayer)
        return;
    ILayerOptionHandler* pOptionHandler = pLayer->GetOptionHandler();
    if(NULL != pOptionHandler)
    {
        pOptionHandler->HasMotionData(hasData);
    }
}

void CUILayerManager::IUILayerManager_SetMusicFile(int nLayerType , void *pData , int nDataLen , int nDataID)
{
    ILayerUI* pLayerUI = FindLayer(nLayerType);

    if (pLayerUI == NULL)
        return;

    if (m_pEvent)
    {
        m_pEvent->LayerUIEvent_SetLayOptionHandler(pLayerUI->GetOptionHandler());
        m_pEvent->LayerUIEvent_SetMusicLayerSourceHandler(pLayerUI->GetOptionHandler(),nLayerType,pData , nDataLen, nDataID);
    }

    QPoint tempPoint(0,0);
    pLayerUI->OnLButtonDown(m_nCurCol, tempPoint, 0);
}
void CUILayerManager::IUILayerManager_SingleBlockPlayingStop(int nLayerType)
{
    StopLayerDebug();
    //PlayEvent_OnDebugStop(nLayerType);
}

emPlayType CUILayerManager::IUILayerManager_GetCurPlayType()
{
    return m_ePlayType;
}

void CUILayerManager::IUILayerManager_SetMusicModifiedState(bool bState)
{
    m_bMusicModified = bState;

    // 文件有修改后需要刷新tab标签显示
    updateTabName();
}

bool CUILayerManager::IUILayerManager_GetMusicModifiedState()
{
    return m_bMusicModified;
}

void CUILayerManager::IUILayerManager_SetTabWidgetTabText(QWidget *pParent, const QString &sFile)
{
    if (sFile.isEmpty())
    {
        return;
    }

    CUIMainLayerLayout *ui_mainlayer_layout = dynamic_cast<CUIMainLayerLayout*> (pParent);
    if (NULL == ui_mainlayer_layout)
    {
        return;
    }

    QFileInfo o_file_info(sFile);
    m_strFileName = o_file_info.fileName();

    ui_mainlayer_layout->setTabWidgetTabName(this, m_strFileName);
    ui_mainlayer_layout->setTabWidgetTabToolTip(this, sFile);

}
void CUILayerManager::IUILayerManager_SingleGroupPlaying(int nLayerType , CActionBlockGroupManager* pGroupManager)
{
    ILayerUI * itemFound= FindLayer(nLayerType);
    if (!itemFound)
    {
        return;
    }

    ILayerOptionHandler *pHandler = itemFound->GetOptionHandler();
    if (!pHandler)
    {
        return;
    }
    if (nLayerType == ACTION_LAYER_MUSIC && pHandler->IsMusicLoaded())
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
        if (m_pGroupDataPlay == NULL)
        {
            CreateGroupDataPlayer(&m_pGroupDataPlay);
        }
        m_pGroupDataPlay->Connect(itemFound, PLAYDEBUG_GROUP, itemFound->GetScaleValue(), this);
        m_pGroupDataPlay->StartPlay(pGroupManager, Get3DRobotViewInterface());
    }
    if (itemFound)
    {
        itemFound->GetOptionHandler()->SetControlState(false);
        itemFound->SetLayerMenuState(true);
    }
    //hels 通知MP3模块
    itemFound= FindLayer(ACTION_LAYER_MUSIC);
    if(itemFound)
    {
        itemFound->GetOptionHandler()->SetControlState(false, ACTION_LAYER_MOTION);
    }
}
void CUILayerManager::IUILayerManager_SingleBlockPlaying(int nLayerType ,CActionBlockData *pActionData)
{
    ILayerUI * itemFound= FindLayer(nLayerType);
    if (!itemFound)
    {
        return;
    }

    ILayerOptionHandler *pHandler = itemFound->GetOptionHandler();
    if (!pHandler)
    {
        return;
    }

    if (nLayerType == ACTION_LAYER_MUSIC)
    {
        if (pHandler->IsMusicLoaded())
        {
            m_IsMusicPlaying = true;
        }

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
        if (m_pGroupDataPlay == NULL)
        {
            CreateGroupDataPlayer(&m_pGroupDataPlay);
        }
        m_pGroupDataPlay->Connect(itemFound, PLAYDEBUG_BLOCK, itemFound->GetScaleValue(), this);
        m_pGroupDataPlay->StartBlockPlay(pActionData, Get3DRobotViewInterface());
    }
    if (itemFound)
    {
        itemFound->GetOptionHandler()->SetControlState(false);
        itemFound->SetLayerMenuState(true);
    }
    //hels 通知MP3模块
    itemFound= FindLayer(ACTION_LAYER_MUSIC);
    if(itemFound)
    {
        itemFound->GetOptionHandler()->SetControlState(false, ACTION_LAYER_MOTION);
    }
}
bool CUILayerManager::IUILayerManager_IsRobotDebuging()
{
    return m_IsDataPlaying;
}

bool CUILayerManager::IUILayerManager_IsSimulating()
{
    return (m_IsMusicPlaying || m_IsDataPlaying);
}
bool CUILayerManager::IUILayerManager_GetModifiedState()
{
    return m_bModified;
}
void CUILayerManager::IUILayerManager_SetModifiedState(bool bState)
{
    m_bModified = bState;

    // 文件有修改后需要刷新tab标签显示
    updateTabName();
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
    pLayerUI->OnLButtonDown(m_nCurCol, tempPoint, 0);
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
//    int nRow = GetRowIndexByPosY(point.y);
//    m_nCurCol = GetColIndexByPosX(point.x);
//    if (nRow == -1 || m_nCurCol == -1)
//        return;
//    //绘图区外面不响应
//    if (m_nCurCol > GetCellCount() * (m_lineThick + m_nCellWidth))
//    {
//        return;
//    }
//    BYTE   KeyBuf[256];
//    memset(KeyBuf,0x00,256);
//    GetKeyboardState(KeyBuf);
//    if ((KeyBuf[VK_SHIFT] >> 7)== true)
//    {
//        ILayerUI* pLayer = FindLayerByIndex(2);
//        if (pLayer)
//        {
//            pLayer->OnLButtonDownAndShift(m_nCurCol);
//        }
//    } else {
//        ClearSelectState();
//        if (nRow != 2)
//        {
//            ILayerUI* pMotionLayer = FindLayerByIndex(2);
//            //m_pEvent->LayerUIEvent_SetActionBlockDataManager(pMotionLayer->GetBlockDataManager(), -1);
//        }

//        ILayerUI* pLayer = FindLayerByIndex(nRow);
//        if (pLayer)
//        {
//            if (pLayer->GetType() != ACTION_LAYER_TIME)
//            {
//                if (m_pEvent)
//                {
//                    //m_pEvent->LayerUIEvent_SetLayOptionHandler(pLayer->GetOptionHandler());
//                }
//                if (pLayer)
//                {
//                    pLayer->OnLButtonDown(m_nCurCol, point);
//                }
//                if (pLayer && pLayer->GetType() == ACTION_LAYER_MOTION)
//                {
//                    KillTimer(0);
//                    SetTimer(0,500,NULL);
//                }
//            }
//        }

//        // 重置选中的列
//        CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
//        if (pTimeLayer)
//        {
//            pTimeLayer->SetCurrentSelectCol(m_nCurCol);

//            if (m_pEvent)
//            {
//                m_pEvent->LayerUIEvent_SelectChanger(m_nCurCol, pTimeLayer->GetScaleValue());
//            }
//        }
//    }

//    Invalidate(false);
//    __super::OnLButtonDown(nFlags, point);
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

/**************************************************************************
* 函数名:ParseRawData
* 功能:解析数据
* 参数:char* pData 数据
* 参数:int nLen 数据长度
* 返回值: bool 解析成功，返回真，否则返回假
* 时间:2015/12/22 14:10
* 作者: 周志平
* 改动日志: 2015/12/22 14:10 增加无效指针pLayer判断
* 改动日志：2015/12/30 14:51  注释 AddAcionLayer(nType) 因1S各个Layer没有重叠,connect已经加了，不需要删除Layer后再添加

**************************************************************************/
bool CUILayerManager::ParseRawData(char* pData, int nLen)
{
    char *pTravelData = pData;

    // 保存总的数据长度，先占位，后面再改为实际的长度
    int nTotalLen = 0;
    UBX_MEMCPY_INC(nTotalLen, pTravelData);
    if (nTotalLen != nLen)
        return false;

    // 获取文件头
    char strTag[10] = "";
    memcpy(strTag, pTravelData, 9);
    pTravelData += 9;

    if (strcmp(strTag, UBX_FILE_TAG) != 0)
        return false;

    // 版本
    int nVersion = 0;
    UBX_MEMCPY_INC(nVersion, pTravelData);
    if (nVersion != UBX_FILE_VERSION)
        return false;

    // 保存总的层数
    int nLayerCount = 0;
    UBX_MEMCPY_INC(nLayerCount, pTravelData);
    for (int i=0; i<nLayerCount; i++)
    {
        char* pRawData = NULL;
        int nRawDataLen = 0;

        // 获取TYPE
        int nType = 0;
        UBX_MEMCPY_INC(nType, pTravelData);

        ILayerUI* pLayer = NULL;
//        AddAcionLayer(nType);     //由于1.0版本时间层直接调用到了音乐层，这里一层层单独添加会有问题
        pLayer = FindLayer(nType);
        // Raw DATA
        UBX_MEMCPY_INC(nRawDataLen, pTravelData);
        if (nRawDataLen)
        {
            pRawData = new char[nRawDataLen];
            memcpy(pRawData, pTravelData, nRawDataLen);
            pTravelData += nRawDataLen;
            if(pLayer != NULL)
            {
                 pLayer->SetData(pRawData, nRawDataLen);
            }
            delete [] pRawData;
        }
    }
    //添加音乐文件
    ILayerUI *pMusicLayer = FindLayer(ACTION_LAYER_MUSIC);
    bool bRet = false;
//    if (pMusicLayer)
//    {
//        bRet = pMusicLayer->AddMusicFromExtData(0,0);
//    }
    if (/*!bRet && */pMusicLayer)
    {
        pMusicLayer->AddMusicFromMusicDir();
    }
////	this->PostMessage(WM_SIZE);
    return true;
}

QString CUILayerManager::GetModuleDir()
{
//	HMODULE module = GetModuleHandle(0);
//	WCHAR pFileName[MAX_PATH];
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
    //这一段不能开启，因为时间层没有当前时间线的信息，只有mp3proj模块才有
//    CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
//    if (pTimeLayer)
//    {
//        return pTimeLayer->GetCurrentSelTime();
//    }
    //hels 原windows下 GetCurrentSelTime与GetCurrentCol返回的值是一样的。这里直接返回Col即可
    return GetCurrentSelectCol();
}

int CUILayerManager::SetCurrentSelectCol(int nCol)
{
//    CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
//    if (pTimeLayer)
//    {
//        pTimeLayer->SetCurrentSelectCol(nCol);
//    }

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
    ILayerUI* pLayer = FindLayer(ACTION_LAYER_MOTION);
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
    ILayerUI* pLayer = FindLayer(ACTION_LAYER_MUSIC);
    if(NULL == pLayer)
        return;
    ILayerOptionHandler* pOptionHandler = pLayer->GetOptionHandler();
    if(NULL != pOptionHandler)
    {
        pOptionHandler->SetTimeScale(m_scaleValue);
    }
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
    ILayerUI* pLayer = FindLayer(ACTION_LAYER_MUSIC);
    if(NULL == pLayer)
        return;
    ILayerOptionHandler* pOptionHandler = pLayer->GetOptionHandler();
    if(NULL != pOptionHandler)
    {
        pOptionHandler->SetCellCount(nCount);
    }
}

void CUILayerManager::SetCellWidth(int nWidth)
{
    m_nCellWidth = nWidth;
}

void CUILayerManager::LayerPlaying(int layerType, ILayerOptionHandler* optionHandle)
{
    ILayerUI * itemFound= FindLayer(layerType);
    if (!itemFound)
    {
        return;
    }

    ILayerOptionHandler *pHandler = itemFound->GetOptionHandler();
    if (!pHandler)
    {
        return;
    }

    if (layerType == ACTION_LAYER_MUSIC && pHandler->IsMusicLoaded())
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
        if (m_pGroupDataPlay == NULL)
        {
            CreateGroupDataPlayer(&m_pGroupDataPlay);
        }
        m_pGroupDataPlay->Connect(itemFound, PLAYDEBUG_ALL, itemFound->GetScaleValue(), this);
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
//        CreateMusicDataPlayer(&m_pMusicDataPlay);
//        m_pMusicDataPlay->Connect(itemFound, false, itemFound->GetScaleValue(), this);
//        int nCursorPos = GetCurrentSelTime();
//        int nStartTime = ((CUIMusicLayer*)itemFound)->GetActStartTime();
//        int nEndTime = ((CUIMusicLayer*)itemFound)->GetActEndTime();
//        if (nCursorPos < nStartTime || nCursorPos >nEndTime)
//        {
//            nCursorPos = nStartTime;
//        }
//        m_pMusicDataPlay->OnSetPlayTime(nCursorPos , nEndTime);
//        m_pMusicDataPlay->StartPlay(NULL, m_pRobot3dView);
    }

    itemFound->GetOptionHandler()->SetControlState(false);
    itemFound->SetLayerMenuState(true);
    //hels 通知MP3模块
    itemFound= FindLayer(ACTION_LAYER_MUSIC);
    if(itemFound)
    {
        itemFound->GetOptionHandler()->SetControlState(false, ACTION_LAYER_MOTION);
    }
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

void CUILayerManager::PauseLayerDebug()
{
    if (m_pGroupDataPlay && m_IsDataPlaying)
    {
        m_pGroupDataPlay->PausePlay();
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
    ILayerUI* pMusicLayer = FindLayer(ACTION_LAYER_MUSIC);
    if(NULL == pMusicLayer)
        return;
    ILayerOptionHandler* pOptionHandler = pMusicLayer->GetOptionHandler();
    if(NULL != pOptionHandler)
    {
        pOptionHandler->UpdateMP3Action(nCol);
    }

    emit sigOnMotionPosChanged(nCol);

//    CUITimeLayer* pTimeLayer = (CUITimeLayer*)FindLayer(ACTION_LAYER_TIME);
//    CUIMusicLayer *pMusicLayer = (CUIMusicLayer*)FindLayer(ACTION_LAYER_MUSIC);
//    if (pTimeLayer/* && GetSafeHwnd()*/)
//    {
//        pTimeLayer->SetCurrentSelectCol(nCol);
//        if (m_pEvent)
//        {
//            m_pEvent->LayerUIEvent_SelectChanger(nCol, pTimeLayer->GetScaleValue());
//        }
//    }

//    //检查是否需要自动滚页
//    if (/*pMusicLayer &&*//* GetSafeHwnd()*/true)
//    {
//        int nScreenCell = pMusicLayer->GetScreenCellCount();
//        //nScreenCell *= (m_lineThick + m_nCellWidth);
//        int nScreenCellCount = nScreenCell - pMusicLayer->GetCellStartIndex();

//        if (nScreenCellCount == 0)
//        {
//            return;
//        }
//        int nCell = (nCol / (m_lineThick + m_nCellWidth));
//        // 如果光标位置显示不在界面上，自动跳到光标位置处
//        if (pMusicLayer->GetCellStartIndex() <= nCell  && nCell <= nScreenCell)
//        {

//        }
//        else
//        {
//            SetHScrollOffset(nCell);   //页数
//            SetScrollPos(nCell);
//        }
//        if ((nScreenCell - nCell) / nScreenCellCount > 1)
//        {
//// 			SetHScrollOffset(nCol / (m_lineThick + m_nCellWidth));
//// 			/*UpdateScrollBar();*/
//// 			SetScrollPos(nCell);
//        }
//        if (nCell == nScreenCell)
//        {
//            SetHScrollOffset(nCol / (m_lineThick + m_nCellWidth));
//            /*UpdateScrollBar();*/
//            SetScrollPos(nCell);
//        }
//    }
////    if (GetSafeHwnd())
////    {
////        Invalidate(false);
////    }


}
//void CUILayerManager::PlayEvent_On
void CUILayerManager::PlayEvent_OnDebugStop(int nLayerType, int nPlayStatus)
{
// 	m_IsSinglePlaying = false;
// 	m_pOptionHandler->SetControlState(true);

    // 联合仿真暂停，需要同时暂停音乐和动作
    if (nPlayStatus == ePlayStatusPause)
    {
        m_IsMusicPlaying = false;
        m_IsDataPlaying = false;
    }
    else
    {
        if (nLayerType == ACTION_LAYER_MUSIC)
        {
            m_IsMusicPlaying = false;
        }
        else if ( nLayerType == ACTION_LAYER_MOTION)
        {
            m_IsDataPlaying = false;
        }
    }

//    if (m_pEvent)
//    {
//        m_pEvent->LayerDebugStop(nLayerType);
//    }

    ILayerUI* itemFound = FindLayer(nLayerType);
    if (itemFound)
    {
        itemFound->GetOptionHandler()->SetControlState(true);
        itemFound->SetLayerMenuState(false);
    }
    //hels 通知mp3模块刷新按钮
    ILayerUI* pLayer = FindLayer(ACTION_LAYER_MUSIC);
    if(NULL == pLayer)
        return;
    ILayerOptionHandler* pOptionHandler = pLayer->GetOptionHandler();
    if(NULL != pOptionHandler)
    {
        pOptionHandler->MotionPlayStop(nPlayStatus);
    }

    //hels 通知Motion模块刷新按钮
    pLayer = FindLayer(ACTION_LAYER_MOTION);
    if(NULL == pLayer)
        return;
    pOptionHandler = pLayer->GetOptionHandler();
    if(NULL != pOptionHandler && !m_IsDataPlaying && !m_IsMusicPlaying)
    {
        pOptionHandler->MotionPlayStop(nPlayStatus);
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
    ILayerUI* pLayer = FindLayer(ACTION_LAYER_MUSIC);
    if(NULL == pLayer)
        return 100;
    ILayerOptionHandler* pOptionHandler = pLayer->GetOptionHandler();
    if(NULL != pOptionHandler)
    {
        int ret  = pOptionHandler->GetCurrentSelectCol();
        return ret;
    }
    return 100;
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

    m_IsDataPlaying= true;
    ILayerUI* itemMotion = FindLayer(ACTION_LAYER_MOTION);
    //hels 通知mp3模块刷新按钮
    ILayerUI* layerMusic = FindLayer(ACTION_LAYER_MUSIC);
    if(NULL == layerMusic)
        return;
    ILayerOptionHandler* pOptionHandler = layerMusic->GetOptionHandler();
    if(NULL != pOptionHandler)
    {
        if(pOptionHandler->IsMusicLoaded())
        {
            m_IsMusicPlaying = true;
            m_nDebugType = ALL_LAYER_DEBUG;
        }
    }

    if (itemMotion)
    {
        if(m_pGroupDataPlay == NULL)
        {
            CreateGroupDataPlayer(&m_pGroupDataPlay);
        }
        m_pGroupDataPlay->Connect(itemMotion, PLAYDEBUG_ALL, itemMotion->GetScaleValue(), this );
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
        //CreateMusicDataPlayer(&m_pMusicDataPlay);
        //m_pMusicDataPlay->Connect(itemMusic, false, itemMusic->GetScaleValue(), this );
        int nCursorPos = GetCurrentSelTime();
        int nEndTime = ((CUIMusicLayer*)itemMusic)->GetEndTime();
        //m_pMusicDataPlay->OnSetPlayTime(nCursorPos , nEndTime);
        //m_pMusicDataPlay->StartPlay(NULL, m_pRobot3dView);
        itemMusic->GetOptionHandler()->PlayMusic(nCursorPos, nEndTime, ePlayStatusPlaying);//2表示播放
        itemMusic->GetOptionHandler()->SetControlState(false, MAX_ACTION_LAYER);
        itemMusic->SetLayerMenuState(true);
    }
}
void CUILayerManager::PauseTotalLayerDebug()
{
    if (m_pGroupDataPlay)//&& m_IsDataPlaying)
    {
        m_pGroupDataPlay->PausePlay();
        m_IsDataPlaying = false;
    }

    if (m_pMusicDataPlay)// && m_IsMusicPlaying)
    {
        m_pMusicDataPlay->PausePlay();
        m_pMusicDataPlay->ReleaseConnection();
        m_pMusicDataPlay = NULL;
        m_IsMusicPlaying = false;
    }
//    /*m_bTotalPlaying =false;*/
//    ILayerUI* itemMotion = FindLayer(ACTION_LAYER_MOTION);
//    if (itemMotion)
//    {
//        itemMotion->onCancelPlayMotionBlock();
//    }

    ILayerUI* itemMusic = FindLayer(ACTION_LAYER_MUSIC);
    if (itemMusic)
    {
        //itemMusic->onCancelPlayMotionBlock();
        itemMusic->GetOptionHandler()->PlayMusic(0, 0, ePlayStatusPause);//1表示暂停
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
        itemMotion->onCancelPlayMotionBlock();
        itemMotion->GetOptionHandler()->FinishPlayingMusic();
    }

    ILayerUI* itemMusic = FindLayer(ACTION_LAYER_MUSIC);
    if (itemMusic)
    {
        itemMusic->onCancelPlayMotionBlock();
        itemMusic->GetOptionHandler()->PlayMusic(0, 0, ePlayStatusStop);//0表示停止
    }
    PlayEvent_OnSetPos(0);
}

