#include "stable.h"
#include "UIMotionLayer.h"
#include "IUILayerManager.h"

CUIMotionLayer::CUIMotionLayer(void):
    m_managerHandle(NULL)
{
    //    m_bLtnDownDebug  = false;
    //m_ActionBlockSelColor = RGB(179,120,120);
    m_ActionBlockSelColor = qRgb(45,150,255);
}


CUIMotionLayer::~CUIMotionLayer(void)
{
}

bool CUIMotionLayer::Connect(QWidget* managerHandle)
{
    //    CreateMotionLayOptionHandler(&m_pOptionHandler);
    //    m_pOptionHandler->Connect(managerHandle, this);
    m_managerHandle = managerHandle;
    CUILayerCommonImpl::Connect(managerHandle);

    return true;
}

bool CUIMotionLayer::ReleaseConnection()
{
    CUILayerCommonImpl::ReleaseConnection();

    return true;
}


bool CUIMotionLayer::GetData(char** ppData, int& nLen)
{
    CUILayerCommonImpl::GetData(ppData, nLen);

    return true;
}
void CUIMotionLayer::SetSelectGroupByOthers(int nCol)
{
    m_bDrawBlockActiion = true;
    ClearSelectGroupBlock();
    SetSelectGroupBlock(nCol);
    int nGroupIndex = 0;
    int nActionIndex = 0;
    int nOffset = 0;
    GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffset);

    int nSelGroupID = GetSelectGroupBlock();
    if (m_pLayerManager)
    {
        g_IsUpdateUIByData = true;
        m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(),  nSelGroupID,nActionIndex);
        m_pLayerManager->UpdateMP3Widget(GetBlockDataManager(), nSelGroupID, nActionIndex);
        g_IsUpdateUIByData = false;
    }

}
void CUIMotionLayer::AddGroupFromMenu()
{
    if (m_pOptionHandler)
    {
        ClearSelectGroupBlock();
        m_pOptionHandler->AddGroupByMenu();
    }
}
void CUIMotionLayer::OnLButtonDownDebug(int nCol)
{
    //	if (m_IsPlaying)
    //	{
    //		return;
    //	}
    //	int nGroupIndex = 0;
    //	int nActionIndex = 0;
    //	int nOffsetTime = 0;
    ////    IUBXRobot3DView *pView = NULL;
    //	if (m_pLayerManager)
    //	{
    //		pView = m_pLayerManager->Get3DRobotViewInterface();
    //	}
    //	GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffsetTime);
    //	if (nGroupIndex == -1 || nActionIndex == -1)
    //	{
    //		return;
    //	}
    //	CActionBlockGroupManager *pManger = NULL;
    //	CActionBlockData *pActionBlock = NULL;
    //	if (m_pBlockDataManager)
    //	{
    //		pManger = m_pBlockDataManager->GetAt(nGroupIndex);
    //	}
    //	if (pManger)
    //	{
    //		pActionBlock = pManger->GetAt(nActionIndex);
    //	}

    //	char* pData = NULL;
    //	int nLen = 0;

    //	if (pActionBlock)
    //	{
    //		pActionBlock->GetExtBlockData(&pData, nLen);
    //	}
    //	int runTime = 200/*pActionBlock->GetEndTime()*GetScaleValue()*/;
    ////	if(pView && pData)
    ////	{
    ////		//pView->SetAllMotorAngle(pData, nLen, runTime );
    ////		pView->SetMotorDatasManual(pData, nLen, runTime);
    ////	}
    //	if (pData)
    //	{
    //		delete []pData;
    //		pData = NULL;
    //	}
}
void CUIMotionLayer::OnDraw(/*CDC* pDC*/)
{
    //	CUILayerCommonImpl::OnDraw(pDC);
    //	if (m_bDrawBlockActiion)
    //	{
    //		DrawBlockAction(pDC);
    //	}
    //	else
    //	{
    //		//除了单击动作组列表不画之外，其他都应该绘制出选中动作块的底色，所有
    //        //绘制动作块的bool值被单击动作组操作置0之后，需要重新设置为true
    //        m_bDrawBlockActiion = true;
    //	}
}
void CUIMotionLayer::ILayerOptionHandler_SetActionBlockState(bool bState)
{
    m_bDrawBlockActiion = bState;
}
void CUIMotionLayer::ILayerOptionHandler_OnSelectActionManageChange(int nGroupID,int nActionIndex)
{
    m_bDrawBlockActiion = true;
    if (m_pLayerManager == NULL || nGroupID <= 0 || nActionIndex < 0)
    {
        return;
    }
    ClearSelectGroupBlock();
    m_GroupSelectID.append(nGroupID);
    CActionBlockGroupManager* pSelectGroup = m_pBlockDataManager->FindBlockData(nGroupID);
    if (pSelectGroup == NULL)
    {
        return;
    }
    float fActionStartTime = pSelectGroup->GetStartTime();

    for (int i = 0 ;i<nActionIndex;i++)
    {
        if (i>= pSelectGroup->GetActionCount())
        {
            break;
        }
        CActionBlockData * pData = pSelectGroup->GetAt(i);
        if (pData )
        {
            fActionStartTime += pData->GetAllTime();
        }
        else
            break;
    }
    UpdateMP3Action(nGroupID, fActionStartTime);

}
void CUIMotionLayer::DrawBlockAction(/*CDC *pDC*/)
{
    //    if (m_pLayerManager == NULL || m_IsDragMouse == true)
    //	{
    //		return;
    //	}
    //	int nCol = 0;
    //	if (m_pLayerManager)
    //	{
    //		nCol = m_pLayerManager->GetCurrentSelectCol() /*- 1*/;
    //	}
    //	if (m_pBlockDataManager == NULL || m_pLayerManager == NULL)
    //		return;
    //#if 1
    //	int nGroupIndex = 0;
    //	int nActionIndex = 0;
    //	int nOffsetTime = 0;

    //	GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffsetTime);

    //	if (nGroupIndex == -1 || nActionIndex == -1)
    //	{
    //		return;
    //	}

    //	int m_lineThick = m_pLayerManager->GetLineThick();
    //	int m_nCellWidth = m_pLayerManager->GetCellWidth();

    //	int xStart, yStart;
    //	GetPosition(xStart, yStart);

    //	CRect rc;
    //	m_ManagerHandle->GetClientRect(rc);
    //	int DisplayEnd = rc.Width()/(m_lineThick + m_nCellWidth) + GetCellStartIndex();

    //	CActionBlockGroupManager *pGroupManager = m_pBlockDataManager->GetAt(nGroupIndex);
    //	float fActionEndTime = pGroupManager->GetStartTime();
    //	if (pGroupManager == NULL /*|| pGroupManager->GetCount() == 1*/)
    //	{
    //		return;
    //	}
    //	CActionBlockData *pBlockData = NULL;

    //	for (int i = 0 ;i<=nActionIndex;i++)
    //	{
    //		 pBlockData = pGroupManager->GetAt(i);
    //		 fActionEndTime  += pBlockData->GetAllTime();
    //	}
    //    pBlockData = pGroupManager->GetAt(nActionIndex);
    //	float fActionStartTime = fActionEndTime - pBlockData->GetAllTime();
    //	CRect rect;
    //	rect.left = fActionStartTime - GetCellStartIndex()*(m_nCellWidth + m_lineThick);
    //	rect.right = fActionEndTime - GetCellStartIndex()*(m_nCellWidth + m_lineThick);
    //	rect.top = yStart + 1;
    //	rect.bottom = rect.top + GetCellHeight() - 1;
    //	CBrush bgBrush(m_ActionBlockSelColor);
    //	pDC->FillRect(rect, &bgBrush);
    //// 	QPen pen( PS_DOT, /*this->m_lineThick*/m_pLayerManager->GetLineThick(), RGB(0,0,0));
    //// 	QPen *pOldPen = pDC->SelectObject( &pen );
    ////
    //// 	pDC->MoveTo(rect.left, rect.top);
    //// 	pDC->LineTo(rect.right, rect.top);
    ////
    //// 	pDC->MoveTo(rect.left, rect.top);
    //// 	pDC->LineTo(rect.left, rect.bottom);
    ////
    //// 	pDC->MoveTo(rect.left, rect.bottom);
    //// 	pDC->LineTo(rect.right, rect.bottom);
    ////
    //// 	pDC->MoveTo(rect.right, rect.top);
    //// 	pDC->LineTo(rect.right, rect.bottom);
    //// 	pDC->SelectObject(&pOldPen);
    //// 	pen.DeleteObject();
    // 	if (/*nActionIndex == 0*/1)
    // 	{
    //		CFont font;
    //		VERIFY( font.CreatePointFont( /*this->m_fontSize*/100, this->m_fontFamily, pDC ) );
    //		CFont* def_font = pDC->SelectObject( &font );

    //		CSize m_size = pDC->GetTextExtent( pGroupManager->GetDescription() ); //计算buf中的字体总宽为多少个像素
    //		pDC->SetBkMode( TRANSPARENT );						  //设置字体背景为透明
    //		CRect fontRect;
    //		fontRect.top = yStart;
    //		fontRect.left = pGroupManager->GetStartTime() - GetCellStartIndex() * (m_nCellWidth + m_lineThick);
    //		fontRect.right = pGroupManager->GetEndTime() - GetCellStartIndex() * (m_nCellWidth + m_lineThick);
    //		fontRect.bottom = rect.bottom;
    //		pDC->SetTextColor( this->m_fontColor );
    //        QString DescriptionInfo = L"  " + (QString)pGroupManager->GetDescription();
    //		pDC->DrawText(DescriptionInfo, &fontRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
    //	}
    //	//pDC->SelectObject(def_font);
    //// 	for (int i=0; i<m_pBlockDataManager->GetCount(); i++)
    //// 	{
    //// 		CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(i);
    //// 		if (pGroupManager == NULL)
    //// 			continue;
    ////
    //// 		int nStartTime = pGroupManager->GetStartTime()/(m_nCellWidth + m_lineThick);
    //// 		int nEndTime = pGroupManager->GetEndTime()/(m_nCellWidth + m_lineThick);
    ////
    //// 		CRect rect;
    //// 		rect.top = yStart;
    //// 		rect.bottom = rect.top+GetCellHeight()-1;
    ////
    //// 		if ((nStartTime <= DisplayEnd) && (nEndTime >= GetCellStartIndex()))
    //// 		{
    //// 			rect.left = pGroupManager->GetStartTime() - GetCellStartIndex()*100;
    ////
    //// 			if (nEndTime <= DisplayEnd)
    //// 			{
    //// 				rect.right = pGroupManager->GetEndTime() - GetCellStartIndex()*100;
    //// 			} else {
    //// 				rect.right = (DisplayEnd+1)*100;
    //// 			}
    //// 		}
    ////
    //// 		if (IsSelectGroupBlock(pGroupManager->GetID()))
    //// 		{
    //// 			CBrush bgBrush(m_GroupBlockSelColor);
    //// 			pDC->FillRect(rect, &bgBrush);
    //// 		} else {
    //// 			CBrush bgBrush(m_GroupBlockDefColor);
    //// 			pDC->FillRect(rect, &bgBrush);
    //// 		}
    ////
    //// 		char* pdata = NULL;
    //// 		int nLens = 0;
    //// 		pGroupManager->GetExtBlockData(&pdata, nLens);
    //// 		delete[] pdata;
    //// 		if (nLens == 12 || nLens == 14)
    //// 		{
    //// 			CBrush bgBrush(RGB(120, 80, 100));
    //// 			pDC->FillRect(rect, &bgBrush);
    //// 		}
    ////
    //// 		// 画层名称
    //// 		CFont font;
    //// 		VERIFY( font.CreatePointFont( /*this->m_fontSize*/100, this->m_fontFamily, pDC ) );
    //// 		CFont* def_font = pDC->SelectObject( &font );
    ////
    //// 		CSize m_size = pDC->GetTextExtent( pGroupManager->GetDescription() ); //计算buf中的字体总宽为多少个像素
    //// 		pDC->SetBkMode( TRANSPARENT );						  //设置字体背景为透明
    //// 		CRect fontRect( rect.TopLeft(), QPoint( rect.right, rect.top + rect.Height()) );
    ////
    //// 		pDC->SetTextColor( this->m_fontColor );
    //// 		QString DescriptionInfo = L"  " + (QString)pGroupManager->GetDescription();
    //// 		pDC->DrawText(DescriptionInfo, &fontRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
    ////
    //// 		// 画实底线
    //// 		QPen pen( PS_SOLID, /*this->m_lineThick*/m_pLayerManager->GetLineThick(), RGB(0, 0, 0));
    //// 		pDC->SelectObject( &pen );
    ////
    //// 		pDC->MoveTo(rect.left, rect.top);
    //// 		pDC->LineTo(rect.right, rect.top);
    ////
    //// 		pDC->MoveTo(rect.left, rect.top);
    //// 		pDC->LineTo(rect.left, rect.bottom);
    ////
    //// 		pDC->MoveTo(rect.left, rect.bottom);
    //// 		pDC->LineTo(rect.right, rect.bottom);
    ////
    //// 		pDC->MoveTo(rect.right, rect.top);
    //// 		pDC->LineTo(rect.right, rect.bottom);
    ////
    //// 		pDC->SelectObject(def_font);
    //// 		font.DeleteObject();
    //// 	}
    //#endif
}
void CUIMotionLayer::OnLButtonDbClickDebug(int nCol)
{
    int nGroupIndex = 0;
    int nActionIndex = 0;
    int nOffsetTime = 0;
    //    IUBXRobot3DView *pView = NULL;
    //	if (m_pLayerManager)
    //	{
    //		pView = m_pLayerManager->Get3DRobotViewInterface();
    //	}
    GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffsetTime);
    CActionBlockGroupManager *pManger = NULL;
    CActionBlockData *pActionBlock = NULL;
    if (nGroupIndex == -1 || nActionIndex == -1)
    {
        return;
    }
    if (m_pBlockDataManager)
    {
        pManger = m_pBlockDataManager->GetAt(nGroupIndex);
    }
    if (pManger)
    {
        pActionBlock = pManger->GetAt(nActionIndex);
    }

    char* pData = NULL;
    int nLen = 0;

    if (pActionBlock)
    {
        pActionBlock->GetExtBlockData(&pData, nLen);
    }
    int runTime = 500/*pActionBlock->GetEndTime()*GetScaleValue()*/;
    //	if(pView && pData)
    //	{
    //		//pView->SetAllMotorAngle(pData, nLen, runTime );
    //		pView->SetMotorDatas(pData, nLen, runTime);
    //	}
    if (pData)
    {
        delete []pData;
        pData = NULL;
    }
}
bool CUIMotionLayer::SetData(char* pData, int nLen)
{
    CUILayerCommonImpl::SetData(pData, nLen);

    return true;
}

// 取得类型，具体见宏定义
int CUIMotionLayer::GetType()
{
    return ACTION_LAYER_MOTION;
}

QString CUIMotionLayer::GetLayerName()
{
    QString strName = QObject::tr("");
    return strName/*language_loadstring(IDS_MOTION_LAYER)*/;
}
void CUIMotionLayer::GetSelectGroupAction(int nCol,int &nGroupIndex , int &nActionIndex , int &nTimeOffset)
{
    m_pBlockDataManager->getSelectGroupFrameArrayIndex(nCol, nGroupIndex, nActionIndex, nTimeOffset);
}

void CUIMotionLayer::GetSelectGroupAction(int nCol, int &nGroupIndex, int &nActionIndex, int &nTimeOffset, float fStartTime)
{
    m_pBlockDataManager->getSelectGroupFrameArrayIndex(nCol, fStartTime, nGroupIndex, nActionIndex, nTimeOffset);
}
/**************************************************************************
* 函数名:CreateOptionHandler
* 功能:根据Option创建layer
* 参数:
* 返回值:
* 时间:2015/12/30 14:18
* 作者: 周志平
* 改动日志 2015/12/30 14:18 改为如果有Layer，就不创建新的Layer,当前只有一个Layer
**************************************************************************/
void CUIMotionLayer::CreateOptionHandler()
{
    if (m_pOptionHandler)
    {
        m_pOptionHandler->ReleaseConnection();
        delete m_pOptionHandler;
        m_pOptionHandler = NULL;
    }
    CreateMotionLayOptionHandler(&m_pOptionHandler);
    m_pOptionHandler->Connect(m_managerHandle, this);

    if (m_pLayerManager)
    {
        m_pOptionHandler->Set3DRobotViewInterface(m_pLayerManager->Get3DRobotViewInterface());
    }
}
void CUIMotionLayer::OnRButtonDdown(int nCol)
{
    m_bDrawBlockActiion = true;
    ClearSelectGroupBlock();
    SetSelectGroupBlock(nCol);
    int nGroupIndex = 0;
    int nActionIndex = 0;
    int nOffset = 0;
    GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffset);
    int nSelGroupID = GetSelectGroupBlock();
    if (m_pLayerManager)
    {
        g_IsUpdateUIByData = true;
        m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(), nSelGroupID , nActionIndex);
        //m_pLayerManager->UpdateMP3Widget(GetBlockDataManager(), nSelGroupID, nActionIndex);
        m_pLayerManager->UpdateMP3Action(nSelGroupID, nCol);
        g_IsUpdateUIByData = false;
    }
//    CUILayerCommonImpl::OnRButtonDdown(nCol);
//    if (!m_IsPlaying)
//    {
//        OnLButtonDownDebug(nCol);
    //    }
}

void CUIMotionLayer::OnLButtonDBClick(int nCol)
{
    //CUILayerCommonImpl::OnLButtonDBClick(nCol);
    m_bLtnDownDebug = false;
    ClearSelectGroupBlock();
    SetSelectGroupBlock(nCol);
    if (!m_IsPlaying)
    {
        if (m_IsPlaying)
        {
            return;
        }
        int nGroupIndex = 0;
        int nActionIndex = 0;
        int nOffsetTime = 0;
        GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffsetTime);
        if (nGroupIndex == -1 || nActionIndex == -1)
        {
            return;
        }
        CActionBlockGroupManager *pManger = NULL;
        CActionBlockData *pActionBlock = NULL;
        if (m_pBlockDataManager)
        {
            pManger = m_pBlockDataManager->GetAt(nGroupIndex);
        }
        if (pManger)
        {
            pActionBlock = pManger->GetAt(nActionIndex);
        }
        if (pActionBlock)
        {
            ILayerOptionHandler_OnDataBlockPlaying(pActionBlock);
        }
    }
}

void CUIMotionLayer::PlayEvent_OnSetPosChangeToOptions(int nCol)
{

    int nGroupIndex = 0;
    int nActionIndex = 0;
    int nOffset = 0;
    GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffset);
    ClearSelectGroupBlock();
    SetSelectGroupBlock(nCol);
    int nSelGroupID = GetSelectGroupBlock();

    if (m_pLayerManager)
    {
        g_IsUpdateUIByData = true;
        m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(),  nSelGroupID,nActionIndex);
        m_pLayerManager->UpdateMP3Widget(GetBlockDataManager(), nSelGroupID, nActionIndex);
        g_IsUpdateUIByData = false;
    }
}

void CUIMotionLayer::DoMenuCommand(uint nCommand, int nCol)
{
    CUILayerCommonImpl::DoMenuCommand(nCommand, nCol);
    switch(nCommand)
    {
    case eMenuMotionFrameCopy:
    {
        if (m_pOptionHandler)
        {
            m_pOptionHandler->copyMotionFrame();
        }
        break;
    }
    case eMenuMotionFramePaste:
    {
        if (m_pOptionHandler)
        {
            m_pOptionHandler->pasteMotionFrame();
        }
        break;
    }
    case eMenuMotionFramePasteInNewGroup:
    {
        pasteActionFrameToNewGroup();
        break;
    }
    case eMenuMotionFrameDelete:
    {
        if (m_pOptionHandler)
        {
            m_pOptionHandler->deleteMotionFrame();
        }
        break;
    }
    case eMenuMotionFrameModify:
    {
        if (m_pOptionHandler)
        {
            m_pOptionHandler->modifyMotionFrame();
        }
        break;
    }
    case eMenuMotionFrameInsert:
    {
        if (m_pOptionHandler)
        {
            m_pOptionHandler->insertMotionFrame();
        }
        break;
    }
    case eMenuMotionFrameSplitInsert:
    {
        if (m_pOptionHandler)
        {
            m_pOptionHandler->splitInsertMotionFrame();
        }
        break;
    }
    case eMenuMotionFrameMarkFrame:
    {
        break;
    }
    case eMenuMotionFrameMarkGroup:
    {
        break;
    }
    case eMenuMotionFrameAddFrameToPreGroup:
    {
        if (m_pOptionHandler)
        {
            m_pOptionHandler->addFrameToPreMotionBlock();
        }
        break;
    }
    case eMenuMotionFrameCreateNewFrameInNewGroup:
    {
        if (m_pOptionHandler)
        {
            m_pOptionHandler->createFrameInNewBlock();
        }
        break;
    }
    case eMenuMotionBlockCopy:
    {
        onCopyMotionBlock();
        break;
    }
    case eMenuMotionBlockPaste:
    {
        onPasteMotionBlock(nCol);
        break;
    }
    case eMenuMotionBlockCut:
    {
        onCopyMotionBlock();
        deleteSelectMotionBlock();
        break;
    }
    case eMenuMotionBlockInsert:
    {
        onInsertMotionBlock(nCol);
        break;
    }
    case eMenuMotionBlockDelete:
    {
        deleteSelectMotionBlock();
        break;
    }
    case eMenuMotionBlockPlay:
    {
        onPlaySelectMotionBlock();
        break;
    }
    case eMenuMotionBlockCancelPlay:
    {
        onCancelPlayMotionBlock();
        break;
    }
    case eMenuMotionBlockSaveTemplate:
    {
        onSaveMotionBlockToTemplate();
        break;
    }
    case eMenuMotionBlockModifyName:
    {
        CUILayerCommonImpl::OnLButtonDBClick(nCol);
        break;
    }    
    case eMenuMotionBlockMergeGroup:
    {
        m_pOptionHandler->mergeGroup();
        break;
    }

    default:
        break;
    }

    if (!m_IsPlaying)
    {
        OnLButtonDownDebug(nCol);
    }
}

void CUIMotionLayer::OnLButtonDown(int nCol, QPoint point, float fStartTime)
{
    m_bDrawBlockActiion = true;
    CUILayerCommonImpl::OnLButtonDown(nCol,point,fStartTime);
    //m_pLayerManager

    int nGroupIndex = 0;
    int nActionIndex = 0;
    int nOffset = 0;
    GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffset, fStartTime);
    int nSelGroupID = GetSelectGroupBlock();
//	if (nSelGroupID != -1 && m_ManagerHandle && m_pLayerManager)
//	{
//		int nLineWidth = m_pLayerManager->GetLineThick();
//		int nCellWidth = m_pLayerManager->GetCellWidth();
//		CRect rect;
//		m_ManagerHandle->GetClientRect(rect);
//		CActionBlockGroupManager *pGroup = m_pBlockDataManager->FindBlockData(nSelGroupID);
//		if (pGroup)
//		{
//			float fStartTime = pGroup->GetStartTime();
//			float fEndTime = pGroup->GetEndTime();
//			int nMaxPos = GetCellStartIndex() * (nLineWidth + nCellWidth) + rect.right;
//			//当屏幕上的选中的块开始时间大于屏幕上的开始索引，且与屏幕上显示的页面最大像素值差距为(nLineWidth + nCellWidth)像素以内
//			//即开始端离屏幕显示的右边距离很近，同时结束时间大于页面最大像素值，则跳转到下一页面
//			if (fStartTime >= GetCellStartIndex() * (nLineWidth + nCellWidth)
//			&&  fEndTime   >  nMaxPos && (nMaxPos - fStartTime) <  (nLineWidth + nCellWidth))
//			{
//				m_pLayerManager->SetCurrentSelectCol(nMaxPos + 2);
//				m_pLayerManager->SetHScrollPos(nMaxPos +  2);
//			}
//		}
//	}

    if (m_pLayerManager)
    {
        g_IsUpdateUIByData = true;
        m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(),  nSelGroupID,nActionIndex);
        m_pLayerManager->UpdateMP3Action(nSelGroupID, nCol);
        g_IsUpdateUIByData = false;
        //IUBXRobot3DView * pView = m_pLayerManager->Get3DRobotViewInterface();
    }

    //SetTimer(NULL,nCol,1000,TimerProc);
// 	if (!m_IsPlaying)
// 	{
// 		OnLButtonDownDebug(nCol);
// 	}
    //m_bLtnDownDebug = true;

}

//VOID CALLBACK CUIMotionLayer::TimerProc(HWND hWnd,uint nMsg,uint nTimerid,DWORD dwTime)
//{
// 	if (!this->m_IsPlaying)
// 	{
// 		OnLButtonDownDebug(nMsg);
// 	}
//}
