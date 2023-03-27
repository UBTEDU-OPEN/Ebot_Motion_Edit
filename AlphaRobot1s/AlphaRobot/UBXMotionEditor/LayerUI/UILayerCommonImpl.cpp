
#include <QRgb>
#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <QDir>
#include <QFile>
//#include <QMenu>

#include "stable.h"
#include "UILayerCommonImpl.h"
#include "UBXModifyLayoutBlockInfo.h"
#include "IUILayerManager.h"
#include "MotionDataPlay.h"
#include "UBXAddBlankBlockDlg.h"
#include "UBXCommandBoxDlg.h"
#include "UILayerManager.h"
#include "../UIDialogBlockDescription.h"
#include "../dialogcommandbox.h"
#include "../zxLib/configs.h"
#include "../zxLib/widget/messagebox.h"

CUILayerCommonImpl::CUILayerCommonImpl(void)
    : m_pBlockDataManager(NULL)
    , m_pOptionHandler(NULL)
    , m_pLayerManager(NULL)
    , m_pGroupDataPlay(NULL)
    , m_IsPlaying(false)
    , m_IsSinglePlaying(false)
    , m_bMouseDown(false)
    , m_treeView(NULL)
    , m_IsDragMouse(false)
    , m_IsHaveCopyData(false)
    , m_nCurCol(-1)
    , m_pMimeData(NULL)
{
    x = 0;
    y = 0;
    //w = 8;
    h = 20;
    m_divisor = 5;
    m_lineColor = qRgb( 128, 128, 128 );
    //m_lineThick = 1;
    m_HScrollOffset = 0;

    m_CellStartIndex = 0;
    //m_CellCount = 300;
    m_positionDragMouse = 0;

    this->m_fontSize = 80;
    this->m_fontFamily = QObject::tr("Arial");
    this->m_fontColor = qRgb( 10, 10, 10 );

    m_GroupBlockDefColor = qRgb(213, 213, 213);
    m_GroupBlockSelColor = qRgb(213, 215, 251);
    //m_GroupBlockSelColor = RGB(51, 153, 255);
    // 	m_GroupBlockSelSetColor = RGB(13,14,210);
    // 	m_GroupBlockDefSetColor = RGB(213,14,210);

    //m_CellCount = 300;
    //m_lineThick = 1;
    //m_nCellWidth = 8;
    //m_scaleValue = 50;

    m_pMimeData = new QMimeData;
}


CUILayerCommonImpl::~CUILayerCommonImpl(void)
{
    while(!m_GroupSelectID.isEmpty())
    {
        m_GroupSelectID.removeFirst();
    }

    SAFE_DELETE(m_pMimeData);
    //	m_GroupSelectID.RemoveAll();
}

bool CUILayerCommonImpl::Connect(QWidget* managerHandle)
{
    m_pBlockDataManager = new CActionBlockDataManager();
    m_pBlockDataManager->Connect();
    m_ManagerHandle = managerHandle;
    CreateOptionHandler();

    return true;
}
/**************************************************************************
* 函数名: ReleaseConnection
* 功能:
* 参数:
* 返回值:
* 时间:b
* 作者: 周志平
* 改动日志  保留 m_pOptionHandler  m_pBlockDataManager   不置空
**************************************************************************/
bool CUILayerCommonImpl::ReleaseConnection()
{
    if (m_pGroupDataPlay)
    {
        m_pGroupDataPlay->ReleaseConnection();
        m_pGroupDataPlay = NULL;
    }

    if (m_pBlockDataManager)
    {
        m_pBlockDataManager->ReleaseConnection();
        delete m_pBlockDataManager;

        m_pBlockDataManager = NULL;
    }

    if (m_pOptionHandler)
    {
        m_pOptionHandler->ReleaseConnection();
        m_pOptionHandler = NULL;
    }

    m_GroupSelectID.clear();

    return true;
}

int CUILayerCommonImpl::GetNewActionBalockDataID()
{
    int BlockID = 1;
    while(1)
    {
        if (m_pBlockDataManager->FindBlockData(BlockID) == NULL)
            break;

        BlockID++;
    }

    return BlockID;
}

bool CUILayerCommonImpl::IsSelectGroupBlock(int nID)
{
    for (int i=0; i<m_GroupSelectID.count(); i++)
    {
        int nFind = m_GroupSelectID.at(i);
        if (nFind == nID)
            return true;
    }

    return false;
}

void CUILayerCommonImpl::ClearSelectGroupBlock()
{
    while(!m_GroupSelectID.isEmpty())
    {
        m_GroupSelectID.removeFirst();
    }
}

void CUILayerCommonImpl::ClearSelectState()
{
    ClearSelectGroupBlock();
}
// int CUILayerCommonImpl::GetSelectGroupAction(int nCol)
// {
// 	SetSelectGroupBlock(nCol);
// 	int nGroupId = GetSelectGroupBlock();
// 	int nTimeSum = 0;
// 	//m_p
// 	return 1;
//
// }

void CUILayerCommonImpl::ILayerOptionHandler_SetSelectGroupBlock(int nCol)
{
    ClearSelectGroupBlock();
    SetSelectGroupBlock(nCol);
}

void CUILayerCommonImpl::SetSelectGroupBlock(int nCol)
{
    if (m_pBlockDataManager == NULL)
        return;

    SetActionBlockDataType::const_iterator iterStart = m_pBlockDataManager->FirstGroupIndex(nCol);
    for(; iterStart != m_pBlockDataManager->EndIndex(); iterStart++)
    {
        CActionBlockGroupManager* pManager = NULL;
        if(iterStart != m_pBlockDataManager->EndIndex())
        {
            pManager = *iterStart;
        }
        if (pManager == NULL)
            continue;

        float fStartTime = pManager->GetStartTime();
        float fEndTime = pManager->GetEndTime();

        if (nCol >= fStartTime && nCol < fEndTime)
        {
            int nId = pManager->GetID();
            bool IsIdExist = false;
            for(int j = 0; j < m_GroupSelectID.count(); j++)
            {
                if (nId == m_GroupSelectID.at(j))
                {
                    IsIdExist = true;
                    break;
                }
            }
            if (!IsIdExist)
            {
                m_GroupSelectID.append(nId);
                m_pBlockDataManager->setGroupIDSelected(nId, true);
            }
            return;
        }
    }
}

int CUILayerCommonImpl::GetSelectGroupBlock()
{
    int nSelectID = -1;

    if (m_GroupSelectID.count())
    {
        nSelectID = m_GroupSelectID.at(0);
    }

    return nSelectID;
}

void CUILayerCommonImpl::MoveAllGourpTime(CActionBlockGroupManager* pManager, float fOffset)
{
    if (pManager == NULL)
        return;

    pManager->SetStartTime(pManager->GetStartTime()+fOffset);
    pManager->SetEndTime((pManager->GetEndTime()+fOffset));
    AdjustAllGroupTime(pManager, fOffset);
}

void CUILayerCommonImpl::AdjustAllGroupTime(CActionBlockGroupManager* pManager, float fAdd)
{
    if (pManager == NULL)
        return;
    bool bNeedUpdateWindow = false;

    // 判断跟下一块数据有没有重叠
    int nIndex = m_pBlockDataManager->GetIndexByID(pManager->GetID());
    if (nIndex == m_pBlockDataManager->GetCount()-1 || nIndex == -1)
    {
        int nCellCount = 0;
        //float fEndTime = pManager->GetEndTime();
        nCellCount = (m_pLayerManager->GetNewMaxCellCount(pManager->GetEndTime())) / (100) + ADD_OFFSET;
        if ((m_pLayerManager->GetNewMaxCellCount(pManager->GetEndTime())) % 100)
        {
            nCellCount++;
        }
        m_pLayerManager->SetCellCount(nCellCount);
        bNeedUpdateWindow = true;
    }
    else
    {
        CActionBlockGroupManager* pNext = m_pBlockDataManager->GetAt(nIndex+1);
        // 需要移动后面所有的块（由于计算误差，后面块的开始时间以前一个快的结束时间计）
        float fMoveBlock = /*pManager->GetEndTime()-pNext->GetStartTime()+*/fAdd;

        for (int i=nIndex+1; i<m_pBlockDataManager->GetCount(); i++)
        {
            CActionBlockGroupManager* pMove = m_pBlockDataManager->GetAt(i);
            if (pMove)
            {
                pMove->SetStartTime(pMove->GetStartTime() + fAdd);
                pMove->SetEndTime(pMove->GetEndTime() + fAdd);
            }

        }


        // 		for (int i=nIndex+1; i<m_pBlockDataManager->GetCount(); i++)
        // 		{
        // 			CActionBlockGroupManager* pMove = m_pBlockDataManager->GetAt(i);
        // 			CActionBlockGroupManager *pMoveAhead = NULL;
        // 			if (i >= 1)
        // 			{
        // 				pMoveAhead = m_pBlockDataManager->GetAt(i - 1);
        // 			}
        // 			if (pMove)
        // 			{
        // 				float fMoveStartTime = pMove->GetStartTime();
        // 				float fMoveTotalTime = 0;
        // 				if(pMoveAhead)
        // 				{
        // 					fMoveStartTime = pMoveAhead->GetEndTime() /*+ fMoveBlock*/;
        // 				}
        // 				//如果前面有动作组，从前面的动作组开始算起，这样做的目的是为了弥补之前的累积误差			                                                                 //时间误差模型下编辑的动作表的误差
        // 				fMoveTotalTime = fMoveStartTime;
        // 				for (int j = 0;j<pMove->GetCount();j++)
        // 				{
        // 					CActionBlockData * pData = pMove->GetAt(j);
        // 					if (pData)
        // 					{
        // 						fMoveTotalTime += pData->GetEndTime();
        // 					}
        // 				}
        // 				pMove->SetStartTime(fMoveStartTime /*+ fMoveBlock*/);
        // 				pMove->SetEndTime(fMoveTotalTime/* + fMoveBlock*/);
        // 			}
        // 		}

        // 调整总的时间
        {
            CActionBlockGroupManager* pLast = m_pBlockDataManager->GetAt(m_pBlockDataManager->GetCount()-1);
            //if (pLast->GetEndTime() >= GetCellCount())
            {
                int nCellCount = (m_pLayerManager->GetNewMaxCellCount(pLast->GetEndTime()))/100+ADD_OFFSET;
                if ((m_pLayerManager->GetNewMaxCellCount(pLast->GetEndTime())) % 100)
                {
                    nCellCount++;
                }
                m_pLayerManager->SetCellCount( nCellCount);
            }
        }

        bNeedUpdateWindow = true;
    }

    if (bNeedUpdateWindow)
    {
        if(m_pLayerManager)
        {
            //            m_pLayerManager->GetWidget()->Invalidate(false);
            //			m_pLayerManager->UpdateScrollBar();
        }
    }
}

void CUILayerCommonImpl::OnLButtonDBClick(int nCol)
{
    qDebug()<<(QObject::tr("dbCLick\n"))<<endl;
    if (m_IsPlaying || !m_pLayerManager)
        return;
    DialogBlockDescription dlg;
    QFile styleSheet(":/res/qss/motiondefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        dlg.setStyleSheet(styleSheet.readAll());
    }
    //    CAutolockRes res;
    ClearSelectGroupBlock();
    SetSelectGroupBlock(nCol);
    int nGroupID = GetSelectGroupBlock();

    if (nGroupID != -1)
    {
        CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->FindBlockData(nGroupID);

        float fStartTime = ceil(pGroupManager->GetStartTime() * m_pLayerManager->GetTimeScaleValues());
        dlg.SetStartTime(fStartTime);

        float fEndTime = ceil(pGroupManager->GetEndTime() * m_pLayerManager->GetTimeScaleValues());
        dlg.SetEndTime(fEndTime);

        QString strDescription = WCHARToQString(pGroupManager->GetDescription(), MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
        dlg.SetDescription(strDescription);

        if(dlg.exec() == QDialog::Accepted)
        {
            if(!(dlg.m_strBlockDescriptionInfo.isEmpty()))
            {
                float fStartTime = (float)dlg.m_fStartTime / m_pLayerManager->GetTimeScaleValues();
                float fEndTime = (float)dlg.m_fEndTime/ m_pLayerManager->GetTimeScaleValues();
                //设置开始时间
                pGroupManager->SetStartTime(fStartTime);
                //设置结束时间
                pGroupManager->SetEndTime(fEndTime);

                WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
                memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
                QString strDescriptionTmp = dlg.m_strBlockDescriptionInfo;
                int nLen = strDescriptionTmp.length();
                int nDescriptionLen = QStringToWCHAR(wstrDescription, dlg.m_strBlockDescriptionInfo);
                //                wstrDescription[nDescriptionLen/sizeof(WCHAR)-1] ='\0';
                //设置描述
                pGroupManager->SetDescription(wstrDescription);

                g_IsUpdateUIByData = true;
                g_IsModifyGroup = true;

                if (m_pLayerManager)
                {
                    m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(), nGroupID,0);
                    m_pLayerManager->GetWidget()->update();
                    m_pLayerManager->UpdateMP3Widget(GetBlockDataManager(), nGroupID,0);
                    //                    m_pLayerManager->UpdateScrollBar();
                }
                g_IsUpdateUIByData  = false;
                if (m_pLayerManager)
                {
                    m_pLayerManager->IUILayerManager_SetModifiedState(true);
                }
            }
            else
            {
                CMessageBox::warning(NULL, "", QObject::tr("the name of the bolck can't be empty!"));
            }
        }

    }

}

void CUILayerCommonImpl::OnLButtonDown(int nCol, QPoint point, float fStartTime)
{
    m_bMouseDown = true;
    ClearSelectGroupBlock();
    SetSelectGroupBlock(nCol);
    int nSelGroupID = GetSelectGroupBlock();

    if (m_pLayerManager)
    {
        //m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(), nSelGroupID);
        IUBXRobot3DView * pView = m_pLayerManager->Get3DRobotViewInterface();
    }

    //for drag
    //    CActionBlockGroupManager* pGroupManager;
    //    for (int j = 0; j < m_pBlockDataManager->GetCount(); j++)
    //    {
    //        pGroupManager = m_pBlockDataManager->GetAt(j);
    //        int endTime = pGroupManager->GetEndTime() - GetCellStartIndex()*100;
    //        QRect testRect((endTime-2), point.y()-10, endTime, point.y()+10);

    //        //		if (testRect.PtInRect(point))
    //        //		{
    //        //			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
    //        //            m_IsDragMouse = true;
    //        //			m_positionDragMouse = pGroupManager->GetEndTime();
    //        //			break;
    //        //		}
    //    }
}

void CUILayerCommonImpl::OnLButtonUp(QPoint point)
{
    m_bMouseDown = false;
    if (m_IsDragMouse)
    {
        g_IsModifyGroup = true;
        CActionBlockGroupManager* pGroupManager = NULL;
        for (int j = 0; j < m_pBlockDataManager->GetCount(); j++)
        {
            pGroupManager = m_pBlockDataManager->GetAt(j);
            if (m_positionDragMouse == (int)pGroupManager->GetEndTime())
            {
                break;
            }
        }
        if (m_pLayerManager /*&& m_positionDragMouse != pGroupManager->GetEndTime()*/)
        {
            m_pLayerManager->IUILayerManager_SetModifiedState(true);
        }
        int Myoffset = point.x() - (m_positionDragMouse - GetCellStartIndex()*100);
        bool IsAdjustBlock = false;

        float fGroupTimeSum = pGroupManager->GetActionAllTimeSum();
        float *pfOffsetArray = new float[pGroupManager->GetActionCount()];
        for (int i1 = 0;i1<pGroupManager->GetActionCount();i1++)
        {
            pfOffsetArray[i1] = 0;
        }

        for (int k =0; k < pGroupManager->GetActionCount(); k++)
        {
            CActionBlockData* pBlockData = pGroupManager->GetAt(k);
            //float fRunTime = pBlockData->GetRunTime();
            float fRunTime = pBlockData->GetAllTime();//runtime or alltime???
            float fRatio = fRunTime / fGroupTimeSum;
            //TRACE(L"Action%d's fRatio is %f / %f = %f********",k,RunTime,fGroupTimeSum,fRatio);
            pfOffsetArray[k] = (float)Myoffset * fRatio;
            //if ((Myoffset < 0) && (abs(Myoffset)/pGroupManager->GetCount()+1) > RunTime)
            // 不能小于最小时间GRID_MIN_TIMEVALUE
            int nScale = m_pLayerManager->GetTimeScaleValues();
            //TRACE(L"Action%d's NewValue is %f \n",k,(RunTime + pfOffsetArray[k])*nScale);
            if (ceil(( fRunTime + pfOffsetArray[k] ) * nScale) < GRID_MIN_TIMEVALUE)
            {
                //				AfxMessageBox(language_loadstring(IDS_STRING_MOTIONLAYER_ADJUSTRANGETOOBROAD));
                m_IsDragMouse = false;
                m_positionDragMouse = 0;
                if (pfOffsetArray)
                {
                    delete []pfOffsetArray;
                    pfOffsetArray = NULL;
                }
                return;
            }
            else if (ceil(( fRunTime + pfOffsetArray[k] ) * nScale) > GRID_MAX_TIMEVALUE)
            {
                //				AfxMessageBox(language_loadstring(IDS_STRING_MOTIONLAYER_ADJUSTRANGETOOBROAD));
                m_IsDragMouse = false;
                m_positionDragMouse = 0;
                if (pfOffsetArray)
                {
                    delete []pfOffsetArray;
                    pfOffsetArray = NULL;
                }
                return;
            }
        }
        // 		for (int k =0; k < pGroupManager->GetCount(); k++)
        // 		{
        // 			CActionBlockData* BlockData = pGroupManager->GetAt(k);
        // 			float RunTime = BlockData->GetEndTime();
        // 			//if ((Myoffset < 0) && (abs(Myoffset)/pGroupManager->GetCount()+1) > RunTime)
        // 			// 不能小于最小时间GRID_MIN_TIMEVALUE
        // 			int nScale = m_pLayerManager->GetTimeScaleValues();
        // 			if (( RunTime + (float)Myoffset / pGroupManager->GetCount() ) * nScale < GRID_MIN_TIMEVALUE)
        // 			{
        // 				AfxMessageBox(language_loadstring(IDS_STRING_MOTIONLAYER_ADJUSTRANGETOOBROAD));
        // 				m_IsDragMouse = false;
        // 				m_positionDragMouse = 0;
        // 				return;
        // 			}
        // 		}
        float fEndTime = pGroupManager->GetStartTime();
        float AdjustEndTime = 0;
        if (Myoffset != 0)
        {
            int blockCount = pGroupManager->GetActionCount();
            //AdjustEndTime = (float)abs(Myoffset) / blockCount;
            // 			int nTempEndTime = AdjustEndTime * 10;
            // 			//只取一位小数
            // 			AdjustEndTime = (float) nTempEndTime/ 10;
            //四舍五入保留一位小数
            //AdjustEndTime = Round(AdjustEndTime , 1);
            for (int i = 0; i < pGroupManager->GetActionCount(); i++)
            {
                CActionBlockData* AdjustBlockData = pGroupManager->GetAt(i);
                AdjustEndTime = abs(pfOffsetArray[i]);
                //AdjustEndTime = Round(AdjustEndTime,1);
                if (Myoffset > 0)
                {
                    AdjustBlockData->SetRunTime(Round(AdjustBlockData->GetRunTime() + AdjustEndTime,1));
                    AdjustBlockData->SetAllTime(Round(AdjustBlockData->GetAllTime() + AdjustEndTime,1));
                }
                else
                {
                    //AdjustBlockData->SetEndTime(AdjustBlockData->GetEndTime() - AdjustEndTime);
                    AdjustBlockData->SetRunTime(Round(AdjustBlockData->GetRunTime() - AdjustEndTime,1));
                    AdjustBlockData->SetAllTime(Round(AdjustBlockData->GetAllTime() - AdjustEndTime,1));
                }
                //TRACE(L"Action%d's NewValue is %f \n",i,AdjustBlockData->GetEndTime() );
                fEndTime += AdjustBlockData->GetAllTime();
            }
        }
        else
            fEndTime = pGroupManager->GetEndTime();
        float fPrvEndTime = pGroupManager->GetEndTime();
        pGroupManager->SetEndTime(fEndTime/*pGroupManager->GetEndTime() + Myoffset*/);
        // 		if (Myoffset>=0)
        // 		{
        // 			AdjustAllGroupTime(pGroupManager, AdjustEndTime * pGroupManager->GetCount());
        // 		}
        // 		else
        AdjustAllGroupTime(pGroupManager, fEndTime - fPrvEndTime);
        if (m_pLayerManager)
        {
            g_IsUpdateUIByData = true;
            m_pLayerManager->SetActionDataGroupManagerToOptions(m_pBlockDataManager, pGroupManager->GetID(),0);
            m_pLayerManager->UpdateMP3Widget(m_pBlockDataManager, pGroupManager->GetID(),0);
            g_IsUpdateUIByData = false;

            //            m_pLayerManager->GetWidget()->Invalidate(false);
        }

        m_IsDragMouse = false;
        m_positionDragMouse = 0;
        if (pfOffsetArray)
        {
            delete []pfOffsetArray;
            pfOffsetArray = NULL;
        }
    }
}

void CUILayerCommonImpl::OnMouseMove(QPoint point)
{
    if (m_pBlockDataManager == NULL || m_pLayerManager == NULL)
    {
        return;
    }
    int nLineThick = m_pLayerManager->GetLineThick();
    int nCellWidth = m_pLayerManager->GetCellWidth();
    CActionBlockGroupManager* pGroupManager;
    for (int j = 0; j < m_pBlockDataManager->GetCount(); j++)
    {

        pGroupManager = m_pBlockDataManager->GetAt(j);
        if (pGroupManager == NULL)
        {
            continue;
        }
        int endTime = pGroupManager->GetEndTime() - GetCellStartIndex()*(nLineThick + nCellWidth);
        QRect testRect((endTime-2), point.y()-10, endTime, point.y()+10);

        //		if (testRect.PtInRect(point))
        //		{
        //			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
        //			break;
        //		}
    }

    if (m_IsDragMouse)
    {
        //		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
        //		if (m_pLayerManager)
        //		{
        //			m_pLayerManager->SetCurrentSelectCol(GetCellStartIndex()*(nLineThick + nCellWidth) + point.x);
        //		}
    }
}

void CUILayerCommonImpl::OnLButtonUpAndShift()
{
    m_bMouseDown = false;
    if (m_GroupSelectID.count() > 1)
    {
        int resultValue = 0/*AfxMessageBox(language_loadstring(IDS_STRING_MOTIONLAYER_MERGELAYER), MB_YESNO, MB_ICONINFORMATION)*/;
        //		if (resultValue == IDCANCEL)
        //		{
        //			ClearSelectGroupBlock();
        //			return;
        //		}
        if (resultValue == 0/*IDYES*/)
        {
            g_IsModifyGroup = true;
            CActionBlockGroupManager* pGroupData = NULL;
            // 第一个合并组
            pGroupData = m_pBlockDataManager->FindBlockData(GetSelectGroupBlock());
            float incTime = 0;
            for (int i = 1; i < m_GroupSelectID.count(); i++)
            {
                int nGroupID = m_GroupSelectID.at(i);
                CActionBlockGroupManager* waitInsertGroup = m_pBlockDataManager->FindBlockData(nGroupID);

                for (int j =0; j < waitInsertGroup->GetActionCount(); j++)
                {
                    CActionBlockData* pActionBlockData = waitInsertGroup->GetAt(j);

                    // 先NEW内存出来，然后复制过去
                    CActionBlockData* pNewBlockData = new CActionBlockData();
                    char* pData = NULL;
                    int nBlockLen = 0;
                    pActionBlockData->GetData(&pData, nBlockLen);
                    if (pData && nBlockLen)
                    {
                        pNewBlockData->SetData(pData, nBlockLen);
                        delete [] pData;
                        pNewBlockData->SetID(pGroupData->GetUnUserID());
                    }
                    pGroupData->AddLayerBlock(pNewBlockData);
                }

                if (i == (m_GroupSelectID.count()-1))
                {
                    incTime = waitInsertGroup->GetEndTime();
                }
                // Modified Flag
                if (m_pLayerManager)
                {
                    m_pLayerManager->IUILayerManager_SetModifiedState(true);
                }
                m_pBlockDataManager->DeleteLayerBlock(nGroupID);
            }

            pGroupData->SetEndTime(incTime);

            if (m_pLayerManager)
            {
                g_IsUpdateUIByData = true;
                m_pLayerManager->SetActionDataGroupManagerToOptions(m_pBlockDataManager, pGroupData->GetID(),0);
                m_pLayerManager->UpdateMP3Widget(m_pBlockDataManager, pGroupData->GetID(),0);
                g_IsUpdateUIByData = false;
                //                m_pLayerManager->GetWidget()->Invalidate(false);
            }
        }
    }
}

void CUILayerCommonImpl::OnLButtonDownAndShift(int nCol)
{
    int selectGroupId = GetSelectGroupBlock();
    if (selectGroupId == -1)
    {
        SetSelectGroupBlock(nCol);
    } else {
        ClearSelectGroupBlock();
        CActionBlockGroupManager* pGroupData = NULL;
        pGroupData = m_pBlockDataManager->FindBlockData(selectGroupId);
        float startTime = pGroupData->GetStartTime();
        float endTime = pGroupData->GetEndTime();

        if (nCol > endTime)
        {
            for (int i=0; i<m_pBlockDataManager->GetCount(); i++)
            {
                CActionBlockGroupManager* pManager = m_pBlockDataManager->GetAt(i);
                if (pManager == NULL)
                    continue;

                float fStartTime = pManager->GetStartTime();
                //int nEndTime = pManager->GetEndTime();

                if (fStartTime >= startTime && fStartTime <= nCol)
                {
                    int nId = pManager->GetID();
                    m_GroupSelectID.append(nId);
                }
            }
        }

        else if (nCol < startTime)
        {
            for (int i=0; i<m_pBlockDataManager->GetCount(); i++)
            {
                CActionBlockGroupManager* pManager = m_pBlockDataManager->GetAt(i);
                if (pManager == NULL)
                    continue;

                //int nStartTime = pManager->GetStartTime();
                float fEndTime = pManager->GetEndTime();

                if (fEndTime >= nCol && fEndTime <= endTime)
                {
                    int nId = pManager->GetID();
                    m_GroupSelectID.append(nId);
                }
            }
        }
        else
        {
            SetSelectGroupBlock(nCol);
        }
    }
}

void CUILayerCommonImpl::onSaveMotionBlockToTemplate()
{
    CActionBlockGroupManager* pGroupData = NULL;
    for (int i=0; i<m_GroupSelectID.count(); i++)
    {
        int nGroupID = m_GroupSelectID.at(i);
        pGroupData = m_pBlockDataManager->FindBlockData(nGroupID);
    }

    if (pGroupData)
    {
        DialogCommandBox dlg;
        QFile styleSheet(":/res/qss/motiondefault.qss");
        if(styleSheet.open(QIODevice::ReadOnly))
        {
            dlg.setStyleSheet(styleSheet.readAll());
        }
        if (dlg.exec() == QDialog::Accepted)
        {
            QString strCommandBoxName = dlg.m_strCommandBoxName;
            if (!strCommandBoxName.isEmpty() && strCommandBoxName != "motion")
            {
                bool IsRename = false;
                QString strCusActionPath = "";
                strCusActionPath = CConfigs::getLocalCusTemplateDir();
                CreateActionDir(strCusActionPath); //如果自定义动作模板库不存在则创建自定义动作模板库目录
                QDir dir(strCusActionPath);
                //设置过滤器(目录，文件或非上级目录)
                dir.setFilter(QDir::Files);
                dir.setSorting(QDir::Name);
                //取得目录中文件列表(包含目录)
                QFileInfoList list = dir.entryInfoList();
                int i=0;
                if(!list.empty())
                {
                    do{
                        QFileInfo fileInfo = list.at(i);
                        QString strFileName = fileInfo.fileName();
                        QString tempName = strCommandBoxName + UBX_TEMPLATE_EXT;
                        if (strFileName.compare(tempName, Qt::CaseInsensitive) == 0)
                        {
                            IsRename = true;
                            break;
                        }

                        i++;
                    }while(i<list.size());
                }

                if (!IsRename)
                {
                    char* pdata = NULL;
                    int nLens = 0;
                    pGroupData->GetData(&pdata, nLens);
                    qDebug()<<"Tree Node: m_treeView = "<<m_treeView<<endl;
                    if (m_treeView)
                    {
                        m_treeView->AddNewTreeNode(pdata, nLens, strCommandBoxName);
                    }
                }
                else
                {
                    CMessageBox::warning(NULL, "", QObject::tr("The specified block name is existed!"));

                }
            }
            else if(strCommandBoxName.isEmpty())
            {
                CMessageBox::warning(NULL, "", QObject::tr("The name of the block cant not be empty!"));
            }
            else
            {
                CMessageBox::warning(NULL, "", QObject::tr("Can't name the new block as motion.abc!"));
            }
        }

    }
}

/***********************************************************
 * 函数名： CreateActionDir
 * 功能：  创建动作模板库目录
 * 参数：  QString& strPath（路径名）
 * 返回值： bool 当创建成功时，返回真，否则，返回假
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
bool CUILayerCommonImpl::CreateActionDir(QString& strPath)
{
    bool ret = false;
    QDir dir;
    bool exist = dir.exists(strPath);
    if(!exist)
        ret = dir.mkdir(strPath);
    return ret;
}

float CUILayerCommonImpl::getAllTime(float fStartTime, float fEndTime)
{
    float fAllTime = 0;
    float fGridMaxValue = GRID_MAX_TIMEVALUE / (m_pLayerManager->GetTimeScaleValues());
    float fGridMinValue = GRID_MIN_TIMEVALUE / (m_pLayerManager->GetTimeScaleValues());
    if (fEndTime - fStartTime > fGridMaxValue)
    {
        fAllTime =  fGridMaxValue;
    }
    else if (fEndTime - fStartTime < fGridMinValue)
    {
        fAllTime = fGridMinValue;
    }
    else
    {
        fAllTime = fEndTime - fStartTime;
    }

    return fAllTime;
}

void CUILayerCommonImpl::cloneBeforeChangedMotionData(IOperation *pHandle)
{
    if (!m_pBlockDataManager)
    {
        return;
    }

    pHandle->cloneDataBeforeChanged((LPARAM)m_pBlockDataManager, getCurrentSelTimeCol());
}

void CUILayerCommonImpl::cloneAfterChangedMotionData(IOperation *pHandle)
{
    if (!pHandle || !m_pBlockDataManager)
    {
        return;
    }

    pHandle->cloneDataAfterChanged((LPARAM)m_pBlockDataManager, getCurrentSelTimeCol());
}

CActionBlockGroupManager *CUILayerCommonImpl::getActionBlockGroupMgr(quint64 nTimeCol)
{
    if (!m_pBlockDataManager)
    {
        return NULL;
    }

    CActionBlockGroupManager* pFindGroupManager = NULL;
    for (int j = 0; j < m_pBlockDataManager->GetCount(); j++)
    {
        pFindGroupManager = m_pBlockDataManager->GetAt(j);
        if (pFindGroupManager == NULL)
        {
            continue;
        }

        if ((pFindGroupManager->GetStartTime()) <= nTimeCol && (nTimeCol < pFindGroupManager->GetEndTime()))
        {
            return pFindGroupManager;
        }
    }

    return NULL;
}

CActionBlockGroupManager *CUILayerCommonImpl::getCopyBlockGroupData(float fStartTime)
{
    const  QMimeData* mimeData = m_pMimeData;
    if(mimeData == NULL)
    {
        qDebug()<<QObject::tr("can't find mimeData point")<<endl;
        return NULL;
    }

    //从剪贴板读取Buffer
    QByteArray bArray = mimeData->data("GroupCopyData");
    char * buffer = bArray.data();
    int nLen = bArray.length();
    if (nLen <= 0)
    {
        return NULL;
    }

    int nTypeID = 0;
    memcpy(&nTypeID, buffer, sizeof(int));
    if (nTypeID != GetType())
    {
        return NULL;
    }

    CActionBlockGroupManager* pGroupManager = new CActionBlockGroupManager();
    pGroupManager->SetData(buffer+sizeof(int), nLen-sizeof(int));
    float fGroupLen = pGroupManager->GetEndTime()-pGroupManager->GetStartTime();
    pGroupManager->SetStartTime(fStartTime);
    pGroupManager->SetEndTime(fStartTime + fGroupLen);
    pGroupManager->SetID(m_pBlockDataManager->GetUnUserID());

    return pGroupManager;
}

void CUILayerCommonImpl::getActionFrameData(QMimeData *pMimeData, QList<CActionBlockData *> &lstActionFrames)
{
    if(pMimeData == NULL)
    {
        return;
    }
    QByteArray bArray = pMimeData->data("CopyData");
    char * buffer = bArray.data();
    int nTotalLen = bArray.length();
    if(nTotalLen <= 0)
    {
        return;
    }

    int nTotalRow = 0;
    UBX_MEMCPY_INC(nTotalRow, buffer);

    for (int i = 0; i < nTotalRow; i++)
    {

        int nLen = 0;
        UBX_MEMCPY_INC(nLen, buffer);
        char* pData = new char[nLen];
        memcpy(pData, buffer, nLen);
        buffer += nLen;
        CActionBlockData* pActionBlockData = new CActionBlockData();
        pActionBlockData->SetData(pData, nLen);
        pActionBlockData->SetID(-1);   // 需要重新设置ID
        if(pData)
        {
            delete [] pData;
        }

        lstActionFrames.append(pActionBlockData);
    }

    return;
}

void CUILayerCommonImpl::pasteMotionGroupToPreBlock(CActionBlockGroupManager *pCurActionGroupMgr, CActionBlockGroupManager *pNewActionGroupMgr)
{
    if (!pCurActionGroupMgr || !pNewActionGroupMgr || !m_pBlockDataManager)
    {
        return;
    }

    float fGroupLen = pNewActionGroupMgr->GetEndTime()-pNewActionGroupMgr->GetStartTime();
    m_pBlockDataManager->MoveGroup(pCurActionGroupMgr, fGroupLen); //后移
    m_pBlockDataManager->AddLayerBlock(pNewActionGroupMgr);

    AjustTimeForInsert(pCurActionGroupMgr, fGroupLen);

    ClearSelectGroupBlock();
    SetSelectGroupBlock(pNewActionGroupMgr->GetStartTime()+2);

    g_IsUpdateUIByData = true;
    m_pLayerManager->SetActionDataGroupManagerToOptions(m_pBlockDataManager, pNewActionGroupMgr->GetID(),0);
    m_pLayerManager->UpdateMP3Widget(m_pBlockDataManager, pNewActionGroupMgr->GetID(),0);
    g_IsUpdateUIByData = false;
}

void CUILayerCommonImpl::pasteMotionGroupToEnd(CActionBlockGroupManager *pNewActionGroupMgr)
{
    if (!pNewActionGroupMgr || !m_pBlockDataManager)
    {
        return;
    }

    m_pBlockDataManager->AddLayerBlock(pNewActionGroupMgr);
    AdjustAllGroupTime(pNewActionGroupMgr, pNewActionGroupMgr->GetEndTime()-pNewActionGroupMgr->GetStartTime());

    ClearSelectGroupBlock();
    SetSelectGroupBlock(pNewActionGroupMgr->GetStartTime()+2);

    g_IsUpdateUIByData = true;
    m_pLayerManager->SetActionDataGroupManagerToOptions(m_pBlockDataManager, pNewActionGroupMgr->GetID(),0);
    m_pLayerManager->UpdateMP3Widget(m_pBlockDataManager, pNewActionGroupMgr->GetID(),0);
    g_IsUpdateUIByData = false;
}

void CUILayerCommonImpl::pasteActionFrameToNewGroup()
{
    if (!m_pBlockDataManager || !m_pOptionHandler)
    {
        return;
    }

    QMimeData *pCopyFrameData = m_pOptionHandler->getCopyFrameData();
    if (!pCopyFrameData)
    {
        return;
    }

    // 获取拷贝的动作帧数据
    QList<CActionBlockData *> lstActionFrames;
    getActionFrameData(pCopyFrameData, lstActionFrames);
    if (lstActionFrames.isEmpty())
    {
        return;
    }

    // 拷贝数据，以便进行undo操作
    IOperation *pChangedData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangedData);

    float fStartTime = 0;
    CActionBlockGroupManager* pFinalGroupManager = m_pBlockDataManager->GetFinalBlockData();
    if (pFinalGroupManager)
    {
        fStartTime = pFinalGroupManager->GetEndTime();
    }

    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->createDefaultGroupMgr();
    for (int i = 0; i < lstActionFrames.size(); i++)
    {
        CActionBlockData *pBlockData = lstActionFrames.at(i);
        if (!pBlockData)
        {
            continue;
        }
        pBlockData->SetID(pGroupManager->GetUnUserID());
        pGroupManager->AddLayerBlock(pBlockData);
    }
    pGroupManager->SetStartTime(fStartTime);
    pGroupManager->RecalculateTime(); // 重新计算结束时间

    pasteMotionGroupToEnd(pGroupManager);

    // 拷贝数据，以便进行redo操作
    cloneAfterChangedMotionData(pChangedData);

    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(true);
    }
}

quint64 CUILayerCommonImpl::getCurrentSelTimeCol()
{
    if (!m_pLayerManager)
    {
        return  0;
    }

    return m_pLayerManager->GetCurrentSelectCol();
}

void CUILayerCommonImpl::InsertBlankTime(int nCol)
{
    if (GetSelectGroupBlock() == -1)
        return;

    int nGroupID = GetSelectGroupBlock();
    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->FindBlockData(nGroupID);
    if (pGroupManager == NULL)
        return;

    pGroupManager->SetStartTime(pGroupManager->GetStartTime()+1);
    pGroupManager->SetEndTime(pGroupManager->GetEndTime()+1);
    AdjustAllGroupTime(pGroupManager, 1);
}

/**************************************************************************
* 函数名: onCopyMotionBlock
* 功能: 动作组复制
* 参数:
* 返回值:
* 时间:2015/01/26 17:00
* 作者: 周志平
* 改动日志  修改了剪贴板的实现方法
**************************************************************************/
void CUILayerCommonImpl::onCopyMotionBlock()
{
    if (GetSelectGroupBlock() == -1)
        return;

    int nGroupID = GetSelectGroupBlock();
    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->FindBlockData(nGroupID);
    if (pGroupManager == NULL)
        return;

    char* pData = NULL;
    int nLen = 0;
    pGroupManager->GetData(&pData, nLen);
    if (pData==NULL || nLen == 0)
        return;

    int nLayerID = GetType();

    int nSizeInt = sizeof(int);
    char* buffer = new char[nLen + sizeof(int)];
    memcpy(buffer, &nLayerID, sizeof(int));
    memcpy(buffer + sizeof(int), pData, nLen);

//    QClipboard *clipboard = QApplication::clipboard();
//    clipboard->clear();
//    QByteArray bArray(buffer,nLen + sizeof(int));

//    QMimeData *mimeData = new QMimeData;
//    mimeData->setData("GroupCopyData", bArray);

//    clipboard->setMimeData(mimeData);

//    if(mimeData)
//    {
//        delete mimeData;
//        mimeData = NULL;
//    }

    QByteArray bArray(buffer,nLen + sizeof(int));
    m_pMimeData->setData("GroupCopyData", bArray);

    if(buffer)
    {
        delete [] buffer;
        buffer = NULL;
    }
    if (pData && nLen)
    {
        delete [] pData;
    }
}
/**************************************************************************
* 函数名: onPasteMotionBlock
* 功能: 动作组粘贴
* 参数: int nCol  时间轴
* 返回值:
* 时间:2015/01/26 17:00
* 作者: 周志平
* 改动日志  修改了剪贴板的实现方法
**************************************************************************/
void CUILayerCommonImpl::onPasteMotionBlock(int nCol)
{

//    const QClipboard *clipboard = QApplication::clipboard();
//    const  QMimeData* mimeData = clipboard->mimeData();

    // 拷贝数据，以便进行undo操作
    IOperation *pChangedData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangedData);

    CActionBlockGroupManager* pFindGroupManager = getActionBlockGroupMgr(nCol);
    float fStartTime = 0;
    CActionBlockGroupManager* pGroupManager = NULL;

    ///////////////////////////////////////////////for insert after
    CActionBlockGroupManager * pActionBlockGroupManager = m_pBlockDataManager->getNextActionBlockGroupManager(pFindGroupManager);
    pFindGroupManager = pActionBlockGroupManager;
    ///////////////////////////////////////////////////////////

    if (pFindGroupManager)  // 在当前选中动作组前粘贴动作组
    {
            fStartTime = pFindGroupManager->GetStartTime();
            pGroupManager = getCopyBlockGroupData(fStartTime);
            pasteMotionGroupToPreBlock(pFindGroupManager, pGroupManager);
        //        GlobalUnlock(hData);
    }
    else // 在最后空白处粘贴动作组
    {
        CActionBlockGroupManager* pTempGroupManager = NULL;
        pTempGroupManager = m_pBlockDataManager->GetFinalBlockData();
        if (pTempGroupManager)
        {
            fStartTime = pTempGroupManager->GetEndTime();
        }
        else
        {
            fStartTime = 0;
        }

        pGroupManager = getCopyBlockGroupData(fStartTime);
        pasteMotionGroupToEnd(pGroupManager);
    }

    // 拷贝数据，以便进行redo操作
    cloneAfterChangedMotionData(pChangedData);

    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(true);
    }
    //    if (m_nPasteType == 2)
    //    {
    //        m_nPasteType = 0;  //Reset paste type after cut;
    //    }

}

void CUILayerCommonImpl::deleteSelectMotionBlock(bool bCopyMotion)
{
    // 拷贝数据，以便进行undo操作
    IOperation *pChangedData = NULL;
    if (bCopyMotion)
    {
        pChangedData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
        cloneBeforeChangedMotionData(pChangedData);
    }

    int TempGroupId = -1;
    for (int i=0; i<m_GroupSelectID.count(); i++)
    {
        int nGroupID = m_GroupSelectID.at(i);
        CActionBlockGroupManager* pGroup = m_pBlockDataManager->FindBlockData(nGroupID);
        float fMoveBlock = 0;
        if (pGroup)
        {
            fMoveBlock = pGroup->GetEndTime()-pGroup->GetStartTime();
        }
        int GroupIndex = m_pBlockDataManager->GetIndexByID(nGroupID);
        CActionBlockGroupManager* pGroupToMove = m_pBlockDataManager->GetAt(GroupIndex+1);
        // Modified Flag
        if (m_pLayerManager)
        {
            m_pLayerManager->IUILayerManager_SetModifiedState(true);
        }

        m_pBlockDataManager->DeleteLayerBlock(nGroupID);
        if (pGroupToMove)
        {
            TempGroupId = pGroupToMove->GetID();
            MoveAllGourpTime(pGroupToMove, -fMoveBlock);
        }
        else
        {
            TempGroupId = -1;
            // 如果是最后一块数据，则移动总体的时间
            if (GroupIndex > 0){
                CActionBlockGroupManager* pGroupToFront = m_pBlockDataManager->GetAt(GroupIndex-1);
                if (pGroupToFront)
                {
                    AdjustAllGroupTime(pGroupToFront, -fMoveBlock);
                }
            }
            else if (GroupIndex == 0)
            {
                // 当前层已经没有数据了
                int nCellCount = m_pLayerManager->GetNewMaxCellCount(0)+ADD_OFFSET;
                if ((m_pLayerManager->GetNewMaxCellCount(0)) % 100)
                {
                    nCellCount++;
                }
                m_pLayerManager->SetCellCount(nCellCount);
            }

        }

    }
    // 清空所有已经选择的块
    m_GroupSelectID.clear();
    if (TempGroupId != -1)
        m_GroupSelectID.append(TempGroupId);

    if (m_pLayerManager)
    {
        g_IsUpdateUIByData = true;
        m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(), TempGroupId, 0);
        m_pLayerManager->UpdateMP3Widget(GetBlockDataManager(), TempGroupId, 0);
        g_IsUpdateUIByData = false;
    }

    if (bCopyMotion)
    {
        // 拷贝数据以便redo
        cloneAfterChangedMotionData(pChangedData);
    }
}

void CUILayerCommonImpl::DoMenuCommand(uint nCommand, int nCol)
{
    ClearSelectGroupBlock();
    SetSelectGroupBlock(nCol);
}
void CUILayerCommonImpl::onInsertMotionBlock(int nCol)
{
    if (m_pLayerManager == NULL || m_pBlockDataManager == NULL)
    {
        return;
    }
    int nCellWidth = m_pLayerManager->GetCellWidth() + m_pLayerManager->GetLineThick();
    //////////////////////////////////////////////////////////////////////////
    //首先记住插入之前的组Index（也是插入动作帧之后的组Index）
    ClearSelectGroupBlock();
    SetSelectGroupBlock(nCol);
    int nGroupIndex = 0;
    int nActionIndex = 0;
    int nOffset = 0;
    GetSelectGroupAction(nCol,nGroupIndex,nActionIndex,nOffset);
    if (nGroupIndex == -1 || nActionIndex == -1)
    {
        return;
    }

    // 拷贝数据，以便进行undo操作
    IOperation *pChangedData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangedData);

    //////////////////////////////////////////////////////////////////////////
    if (m_pOptionHandler)
    {
        m_pOptionHandler->InsertGroupByMenu(nCol);
    }
    //插入空白帧后需要重新刷新界面显示（调用SetActionDataGroupManagerToOptions函数刷新）

    //插入之后高亮当前插入组，级nCol位置
    // 	int nGroupPrvIndex = nGroupIndex -1 ;
    // 	if (nGroupPrvIndex == -1)
    // 	{
    // 		return;
    // 	}
    int nPrvCenterPos = m_pBlockDataManager->GetAt(nGroupIndex)->GetStartTime() + 1;
    //- GetCellStartIndex()* nCellWidth ;
    ClearSelectGroupBlock();
    SetSelectGroupBlock(nPrvCenterPos);

    int nSelGroupID = GetSelectGroupBlock();
    if (m_pLayerManager)
    {
        g_IsUpdateUIByData = true;
        m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(), nSelGroupID,0);
        m_pLayerManager->UpdateMP3Widget(GetBlockDataManager(), nSelGroupID, 0);
        g_IsUpdateUIByData = false;
    }

    // 拷贝数据以便进行redo操作
    cloneAfterChangedMotionData(pChangedData);
}
void CUILayerCommonImpl::OnRButtonDdown(int nCol)
{
    //    m_nCurCol = nCol;
}

/**************************************************************************
* 函数名:createLayoutMenuPopMenuActions
* 功能:动作表鼠标右键菜单创建
* 参数:
* 返回值:
* 时间:2016/01/07
* 作者: 周志平
* 改动日志
**************************************************************************/
void CUILayerCommonImpl::createLayoutMenuPopMenuActions()
{

}

void CUILayerCommonImpl::OnDraw(/*CDC* pDC*/)
{
    //	// 先画基本部分
    //	DrawBase(pDC);
    //	/*WM_SIZE*/
    //	DrawgBlockGroup(pDC);

}

bool CUILayerCommonImpl::GetData(char** ppData, int& nLen)
{
    if (m_pBlockDataManager)
    {
        return m_pBlockDataManager->GetData(ppData, nLen);
    }

    return true;
}
void CUILayerCommonImpl::AdjustData()
{
    if (m_pBlockDataManager == NULL)
    {
        return;
    }
    float fEndTime = 0;
    for (int i = 0;i<m_pBlockDataManager->GetCount();i++)
    {
        CActionBlockGroupManager *pGroupManager = m_pBlockDataManager->GetAt(i);
        float fTempTime = pGroupManager->GetActionAllTimeSum();
        pGroupManager->SetStartTime(fEndTime);
        fEndTime += fTempTime;
        pGroupManager->SetEndTime(fEndTime);
    }
}
bool CUILayerCommonImpl::SetData(char* pData, int nLen)
{
    if (m_pBlockDataManager)
    {
        m_pBlockDataManager->ClearBlockDataList();
        m_pBlockDataManager->SetData(pData, nLen);
    }

    return true;
}

// 取得类型，具体见宏定义
int CUILayerCommonImpl::GetType()
{
    return 1;
}

void CUILayerCommonImpl::SetHScrollOffset(int offset)
{
    int m_nCellWidth = m_pLayerManager->GetCellWidth();
    int m_lineThick = m_pLayerManager->GetLineThick();

    m_HScrollOffset = offset*(m_nCellWidth + m_lineThick);
}

void CUILayerCommonImpl::SetPosition(int PositionX, int PositionY)
{
    x = PositionX;
    y = PositionY;
}

void CUILayerCommonImpl::GetPosition(int& x, int& y)
{
    x = this->x;
    y = this->y;
}

void CUILayerCommonImpl::SetScalevalue(int scaleValue)
{
    //m_scaleValue = scaleValue;
    if (m_pLayerManager)
    {
        m_pLayerManager->FreshTimeScaleValues(scaleValue);
        //CUIMainLayerLayout *pMainLayout =
    }
}

int CUILayerCommonImpl::GetScaleValue()
{
    return m_pLayerManager->GetTimeScaleValues();
}

void CUILayerCommonImpl::SetCellStartIndex(uint CellIndex)
{
    m_CellStartIndex = CellIndex;
}

uint CUILayerCommonImpl::GetCellStartIndex()
{
    return m_CellStartIndex;
}
int  CUILayerCommonImpl::GetScreenCellCount()
{
    int m_nCellWidth = m_pLayerManager->GetCellWidth();
    //	int m_lineThick = m_pLayerManager->GetLineThick();
    //	int m_CellCount = m_pLayerManager->GetCellCount();
    //	CRect rc;
    //	if (m_ManagerHandle &&m_ManagerHandle->GetSafeHwnd())
    //	{
    //		m_ManagerHandle->GetClientRect(rc);
    //	}
    //	int nCount = rc.Width()/(m_lineThick + m_nCellWidth) + GetCellStartIndex();
    //	//int nCount = rc.Width()+ GetCellStartIndex();
    int nCount = 0;
    return nCount;
}
int  CUILayerCommonImpl::GetCellCount()
{
    //return m_CellCount;
    return m_pLayerManager->GetCellCount();
}

void CUILayerCommonImpl::SetCellWidth(int width)
{
    //m_nCellWidth = width;
    m_pLayerManager->SetCellWidth(width);
}

void CUILayerCommonImpl::GetLayerWidth(int &LayerWidth)
{
    int m_nCellWidth = m_pLayerManager->GetCellWidth();
    int m_lineThick = m_pLayerManager->GetLineThick();
    int m_CellCount = m_pLayerManager->GetCellCount();

    LayerWidth = (m_nCellWidth + m_lineThick) * m_CellCount;
}

int CUILayerCommonImpl::GetCellWidth()
{
    return m_pLayerManager->GetCellWidth();
}

void CUILayerCommonImpl::SetCellHeight(int height)
{
    h = height;
}

int CUILayerCommonImpl::GetCellHeight()
{
    return h;
}

void CUILayerCommonImpl::SetLineThick(int nThick)
{
    //m_lineThick = nThick;
    m_pLayerManager->SetLineThick(nThick);
}

int CUILayerCommonImpl::GetLineThick()
{
    //return m_lineThick;
    return m_pLayerManager->GetLineThick();
}

void CUILayerCommonImpl::DrawgBlockGroup(/*CDC* pDC*/)
{
    //	if (m_pBlockDataManager == NULL)
    //		return;

    //	int m_lineThick = m_pLayerManager->GetLineThick();
    //	int m_nCellWidth = m_pLayerManager->GetCellWidth();

    //	int xStart, yStart;
    //	GetPosition(xStart, yStart);

    //	CRect rc;
    //	m_ManagerHandle->GetClientRect(rc);
    //	int DisplayEnd = rc.Width()/(m_lineThick + m_nCellWidth) + GetCellStartIndex();

    //	for (int i=0; i<m_pBlockDataManager->GetCount(); i++)
    //	{
    //		CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(i);
    //		if (pGroupManager == NULL)
    //			continue;

    //		int nStartTime = pGroupManager->GetStartTime()/(m_nCellWidth+m_lineThick);
    //		int nEndTime = pGroupManager->GetEndTime()/(m_nCellWidth+m_lineThick);

    //		CRect rect;
    // 		rect.top = yStart;
    // 		rect.bottom = rect.top+GetCellHeight()-1;

    // 		if ((nStartTime <= DisplayEnd) && (nEndTime >= GetCellStartIndex()))
    // 		{
    ////			rect.left = (nStartTime-GetCellStartIndex())*(m_nCellWidth+m_lineThick);
    //			rect.left = pGroupManager->GetStartTime() - GetCellStartIndex()*(m_nCellWidth+m_lineThick);
    //			float s = pGroupManager->GetEndTime();
    //  			if (nEndTime <= DisplayEnd)
    //  			{
    ////  			rect.right = (nEndTime-GetCellStartIndex())*(m_nCellWidth+m_lineThick);
    //				//float fEndTime = pGroupManager->GetEndTime();
    //				rect.right = pGroupManager->GetEndTime() - GetCellStartIndex()*(m_nCellWidth+m_lineThick);
    //  			}
    //			else
    //			{
    ////  			rect.right = (DisplayEnd+1)*(m_nCellWidth+m_lineThick);
    //				rect.right = (DisplayEnd+1)*(m_nCellWidth+m_lineThick);
    //  			}
    //		}

    //		if (IsSelectGroupBlock(pGroupManager->GetID()))
    //		{
    //			CBrush bgBrush(m_GroupBlockSelColor);
    //			pDC->FillRect(rect, &bgBrush);
    //		} else {
    //			CBrush bgBrush(m_GroupBlockDefColor);
    //			pDC->FillRect(rect, &bgBrush);
    //		}

    //		char* pdata = NULL;
    //		int nLens = 0;
    //		pGroupManager->GetExtBlockData(&pdata, nLens);
    //		delete[] pdata;
    //		if (nLens == 12 || nLens == 14)
    //		{
    //			CBrush bgBrush(RGB(120, 80, 100));
    //			pDC->FillRect(rect, &bgBrush);
    //		}

    //		// 画层名称
    //		CFont font;
    //		VERIFY( font.CreatePointFont( /*this->m_fontSize*/100, this->m_fontFamily, pDC ) );
    //		CFont* def_font = pDC->SelectObject( &font );

    //		CSize m_size = pDC->GetTextExtent( pGroupManager->GetDescription() ); //计算buf中的字体总宽为多少个像素
    //		pDC->SetBkMode( TRANSPARENT );						  //设置字体背景为透明
    //        CRect fontRect( rect.TopLeft(), QPoint( rect.right, rect.top + rect.Height()) );

    //		pDC->SetTextColor( this->m_fontColor );
    //        QString DescriptionInfo = L"  " + (QString)pGroupManager->GetDescription();
    //		pDC->DrawText(DescriptionInfo, &fontRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

    //		// 画实底线
    //        QPen pen( PS_SOLID, /*this->m_lineThick*/m_pLayerManager->GetLineThick(), RGB(0, 0, 0));
    //		pDC->SelectObject( &pen );

    //		pDC->MoveTo(rect.left, rect.top);
    //		pDC->LineTo(rect.right, rect.top);

    //		pDC->MoveTo(rect.left, rect.top);
    //		pDC->LineTo(rect.left, rect.bottom);

    //		pDC->MoveTo(rect.left, rect.bottom);
    //		pDC->LineTo(rect.right, rect.bottom);

    //		pDC->MoveTo(rect.right, rect.top);
    //		pDC->LineTo(rect.right, rect.bottom);

    //		pDC->SelectObject(def_font);
    //		font.DeleteObject();
    //	}
}

void CUILayerCommonImpl::DrawBase(/* CDC* pDC*/ )
{
    //	int m_nCellWidth = m_pLayerManager->GetCellWidth();
    //	int m_lineThick = m_pLayerManager->GetLineThick();
    //	int m_CellCount = m_pLayerManager->GetCellCount();

    //	//画背景
    //	int wTmp = (m_nCellWidth + m_lineThick) * m_CellCount ;
    //	CRect rt( x, y, x + wTmp, y + h );


    //	int xStart, yStart;
    //	GetPosition(xStart, yStart);
    //	int xTmp = xStart;

    //	CRect rc;
    //	m_ManagerHandle->GetClientRect(rc);
    //	int nCount = rc.Width()/(m_lineThick + m_nCellWidth) + GetCellStartIndex();
    //	CBrush bgBrush(RGB(254, 254, 254));
    //	rt.right = (m_nCellWidth + m_lineThick) * (m_CellCount - GetCellStartIndex());
    //	pDC->FillRect( rt, &bgBrush );
    //    for( uint i = GetCellStartIndex(); i <= nCount; i++ )
    //	{
    //		if (i >= m_CellCount)
    //			break;

    //		//画竖线
    //        QPen pen( PS_SOLID, m_pLayerManager->GetLineThick(), this->m_lineColor);
    //		pDC->SelectObject( &pen );
    //		pDC->MoveTo( xTmp, y);
    //		pDC->LineTo( xTmp, y + h);

    //		pDC->MoveTo(xTmp, y + h-1);
    //		pDC->LineTo(xTmp+m_nCellWidth+m_lineThick, y+h-1);

    //		if( (i+1) % m_divisor == 0 )
    //		{
    //			RECT rt;
    //			rt.left = xTmp + 1;
    //			rt.top = y;
    //			rt.right = xTmp + m_nCellWidth;
    //			rt.bottom = y + h-1;

    //			CBrush brush(RGB(240, 240, 240));
    //			pDC->SelectObject(&brush);
    //			pDC->FillRect( &rt, &brush );
    //		}

    //		xTmp += m_lineThick;
    //		xTmp += m_nCellWidth;
    //	}

    //	pDC->MoveTo( xTmp, y);
    //	pDC->LineTo( xTmp, y + h);
}

void CUILayerCommonImpl::CreateOptionHandler()
{
    m_pOptionHandler = NULL;
}

ILayerOptionHandler* CUILayerCommonImpl::GetOptionHandler()
{
    return m_pOptionHandler;
}

int CUILayerCommonImpl::GetMaxTimeCellCount()
{
    //m_pBlockDataManager->SortBlockDataManager();

    if (m_pBlockDataManager->GetCount() == 0)
        return 0;

    CActionBlockGroupManager* pManager = m_pBlockDataManager->GetAt(m_pBlockDataManager->GetCount()-1);
    if (pManager)
    {
        return pManager->GetEndTime();
    }

    return 0;
}

void CUILayerCommonImpl::SetLayerManager(IUILayerManager* pManager)
{
    m_pLayerManager = pManager;
}

IUILayerManager* CUILayerCommonImpl::GetLayerManager()
{
    return m_pLayerManager;

}

void CUILayerCommonImpl::UpdateMP3Action(int nGroupID, float fStartTime)
{
    if(m_pLayerManager)
    {
        m_pLayerManager->UpdateMP3Action(nGroupID, fStartTime);
    }
}

void CUILayerCommonImpl::ILayerOptionHandler_OnSelectActionManageChange(int nGroupID, int nActionId)
{
    UpdateMP3Action(nGroupID, nActionId);
}

//void CUILayerCommonImpl::Ilayeroptionhandler_MotionlayerActionCommand(uint nCommand, int nCol)
//{
//    switch(nCommand)
//    {
//    case COPY_ACTION_CHOSEN:
//        m_nPasteType = 1;
//        OnCopy();
//        break;
//    case PASTE_ACTION_CHOSEN:
//        OnPaste(nCol);
//        break;
//    case CUT_ACTION_CHOSEN:
//        m_nPasteType = 2;
//        OnCopy();
//        DeleteSelectGroup();
//        break;
//    case DELETE_ACTION_CHOSEN:
//        DeleteSelectGroup();
//        break;
//    case SELECTEDBLOCKPLAY_ACTION_CHOSEN:
//        SelectedBlockPlay();
//        break;
//    case CANCELBLOCKPLAY_ACTION_CHOSEN:
//        CancelBlockPlay();
//        break;
//    case INSERTBLANKTIME_ACTION_CHOSEN:
//        OnMenuInsert(nCol);
//        break;
//    case SAVETOCOMMANDBOX_ACTION_CHOSEN:
//        SaveToCommandBox();
//        break;
//    case MODIFYGROUP_ACTION_CHOSEN:
//        CUILayerCommonImpl::OnLButtonDBClick(nCol);
//        break;
//    default:
//        break;
//    }
//}

void CUILayerCommonImpl::ILayerOptionHandler_OnSelectGroupManageChange(int nGroupID, int nActionId)
{
    if (m_pLayerManager == NULL)
    {
        return;
    }
    ClearSelectGroupBlock();
    m_GroupSelectID.append(nGroupID);
    if (m_pLayerManager)
    {
        CActionBlockGroupManager* pSelectGroup = m_pBlockDataManager->FindBlockData(nGroupID);
        if (pSelectGroup)
        {
            int startTime = pSelectGroup->GetStartTime();
            //const int nCompensationValue = 10;
            //startTime += nCompensationValue;//不补偿可能会选择上一个块
            UpdateMP3Action(nGroupID, startTime);
        }
    }
}

void CUILayerCommonImpl::ILayerOptionHandler_OnDeleteGroupBlock(bool bCopyMotion)
{
    deleteSelectMotionBlock(bCopyMotion);
}

void CUILayerCommonImpl::ILayerOptionHandler_OnActionBlockDataChange(int nGroupID, int nActionBlockID, float fTimeInc)
{
    //Modified Flag
    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->FindBlockData(nGroupID);
    if (pGroupManager == NULL)
        return;

    CActionBlockData* pActionBlock = pGroupManager->FindBlockData(nActionBlockID);
    if (pActionBlock == NULL)
        return;

    pGroupManager->SetEndTime(pGroupManager->GetEndTime() + fTimeInc);
    //     float fEndTime = pGroupManager->GetCurrentActionTimeSum();
    //     pGroupManager->SetEndTime(fEndTime + fTimeInc);
    AdjustAllGroupTime(pGroupManager, fTimeInc);
    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(true);
        //m_pLayerManager->UpdateMP3Widget(m_pBlockDataManager, nGroupID, nActionBlockID);
    }

//    int startTime = pGroupManager->GetActionStartTimeByIndex(pGroupManager->indexAt(nActionBlockID));

    // 设置选中的动作帧
    int startTime = getCurrentSelTimeCol();
    UpdateMP3Action(nGroupID, startTime);
}
void CUILayerCommonImpl::ILayerOptionHandler_SetModifiedState(bool bState)
{
    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(bState);
    }
}
int CUILayerCommonImpl::ILayerOptionHandler_GetScaleValue()
{
    return GetScaleValue();
}
void CUILayerCommonImpl::ILayerOptionHandler_SetScaleValue(int nScale)
{
    if (nScale == 0)
    {
        return;
    }
    SetScalevalue(nScale);
    // 	if (m_pEvent)
    // 	{
    // 		m_pEvent->LayerUIEvent_SetScaleValue(nScale);
    // 	}
}
void CUILayerCommonImpl::ILayerOptionHandler_OnGroupManagerChange(int nGroupID, float fTimeInc, bool bIsUpdateMp3Widget)
{
    CActionBlockGroupManager* pManager = m_pBlockDataManager->FindBlockData(nGroupID);
    if (pManager == NULL)
        return;

    char* pdatas = NULL;
    int nLenss = 0;
    pManager->GetExtBlockData(&pdatas, nLenss);
    delete [] pdatas;

//////////////Comment by huagnwuxian 2016.07.28 /////////////
//// 不管是不是插入拆分方式，删除帧后都需要调整时间
//    if (nLenss == 6 || nLenss == 12)
//    {
        pManager->RecalculateTime();
//    }

    if (m_pLayerManager)
    {
        AdjustAllGroupTime(pManager, fTimeInc);
        int nCurSelTimeCol = getCurrentSelTimeCol();
        int nGroupIndex = 0;
        int nFrameIndex = 0;
        int nOffset = 0;
        m_pBlockDataManager->getSelectGroupFrameArrayIndex(nCurSelTimeCol, nGroupIndex, nFrameIndex, nOffset);

        // 删除动作组最后一帧动作时，会出现小于0，此时设置最后一帧选中
        if (nFrameIndex < 0)
        {
            nFrameIndex = pManager->GetActionCount() - 1;
        }

        if(bIsUpdateMp3Widget)
        {
            m_pLayerManager->UpdateMP3Widget(GetBlockDataManager(), nGroupID, nFrameIndex);
        }
        m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(), nGroupID, nFrameIndex);
    }
    // Modified Flag
    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(true);
    }
}
/**************************************************************************
* 函数名:ILayerOptionHandler_OnAddActionBlock
* 功能:添加动作块函数
* 参数:int nGroupManagerID     当前组ID
* 参数:CActionBlockData* pActionBlockData    动作块数据
* 参数:QString strBlockName                  动作块名字
* 返回值:
* 时间:2015/12/25 14:38
* 作者: 周志平
* 改动日志：2015/12/25 14:38这里暂时取消了当前时间的限制，所有Group 的开始时间皆为0，结束时间皆为100
**************************************************************************/
void CUILayerCommonImpl::ILayerOptionHandler_OnAddActionBlock(int nGroupManagerID,CActionBlockData* pActionBlockData,QString strBlockName,bool bIsUpdateMP3Widget)
{
    // 是否是新添加的GROUP MANAGE
    bool bNotify = false;
    if (pActionBlockData == NULL)
        return;
    if(m_pBlockDataManager == NULL)
        return;

    CActionBlockGroupManager* pGroupManager = NULL;
    float fStartTime = 0;
    // 如果是－1，则根据当前的时候点，创建一个GROUP DATA
    if (nGroupManagerID == -1)
    {
        CActionBlockGroupManager* pTempGroupManager = NULL;
        if (m_pBlockDataManager->GetCount()>0)
        {
            pTempGroupManager = m_pBlockDataManager->GetFinalBlockData();
            fStartTime = pTempGroupManager->GetEndTime();
            if (m_pLayerManager->GetCurrentSelTime() < fStartTime)
            {
                delete pActionBlockData;
                pActionBlockData =NULL;
                return;
            }
        }
        float fEndTime = fStartTime + getAllTime(fStartTime, m_pLayerManager->GetCurrentSelectCol());

        pGroupManager = new CActionBlockGroupManager();
        pGroupManager->SetID(m_pBlockDataManager->GetUnUserID());

        WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
        memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
        int nDescriptionLen = 0;
        if (strBlockName.isEmpty())
        {
            QString strDescription = m_pBlockDataManager->GetUnUseDescription();
            nDescriptionLen = QStringToWCHAR(wstrDescription, strDescription);
            //wstrDescription[nDescriptionLen-1] = '\0';
        }
        else
        {
            nDescriptionLen = QStringToWCHAR(wstrDescription, strBlockName);
            //wstrDescription[nDescriptionLen-1] = '\0';
        }
        pGroupManager->SetDescription(wstrDescription);


        char *pdatas = "motion";
        int nLenss = strlen(pdatas);
        pGroupManager->SetExtBlockData(nLenss, pdatas);
        pGroupManager->SetStartTime(fStartTime);
        pGroupManager->SetEndTime(fEndTime);
        m_pBlockDataManager->AddLayerBlock(pGroupManager);
        pGroupManager->AddLayerBlock(pActionBlockData);
        SetSelectGroupBlock(fStartTime + 1);
        if (pActionBlockData)
        {
            pActionBlockData->SetRunTime(fEndTime - fStartTime);
            pActionBlockData->SetAllTime(fEndTime - fStartTime);
        }
        m_pLayerManager->SetHScrollPos(fEndTime);
        AdjustAllGroupTime(pGroupManager, 0);
        if (pActionBlockData->GetID() == -1)
        {
            pActionBlockData->SetID(pGroupManager->GetUnUserID());
        }
        bNotify = true;

    }
    else
    {
        pGroupManager = m_pBlockDataManager->FindBlockData(nGroupManagerID);
        if(pGroupManager == NULL)
        {
            qDebug()<<QObject::tr("can't find *pGroupManager point")<<endl;
        }
        // 重新分配结束时间
        float fLastEndTime = pGroupManager->GetEndTime();//分组的结束时刻
        float fCurrentAllTime = pActionBlockData->GetAllTime();//当前动作的总时间
        if (fCurrentAllTime == 0)  // 当前指针范围在最后一个动作组后，动作帧LineEdit不使能
        {
            fCurrentAllTime = getAllTime(fLastEndTime, m_pLayerManager->GetCurrentSelectCol());
        }

        pActionBlockData->SetRunTime(fCurrentAllTime);
        pActionBlockData->SetAllTime(fCurrentAllTime);
        pGroupManager->SetEndTime(fLastEndTime + fCurrentAllTime);

        // 判断ACTION BLOCK的ID，如果为－1，则重新分配ID
        if (pActionBlockData->GetID() == -1)
        {
            pActionBlockData->SetID(pGroupManager->GetUnUserID());
        }
        pGroupManager->AddLayerBlock(pActionBlockData);
        AdjustAllGroupTime(pGroupManager, fCurrentAllTime);

    }

    if (bNotify)
    {

        if (m_pLayerManager)
        {
            g_IsUpdateUIByData = true;
            m_pLayerManager->SetActionDataGroupManagerToOptions(m_pBlockDataManager, pGroupManager->GetID(), pGroupManager->GetActionCount() - 1);
            g_IsUpdateUIByData = false;
        }

    }
    // Modified Flag
    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(true);
    }
    if (m_pLayerManager)
    {
        if(bIsUpdateMP3Widget)
        {
            m_pLayerManager->UpdateMP3Widget(m_pBlockDataManager, pGroupManager->GetID(), pGroupManager->GetActionCount() - 1);

            // 更新选择先位置
            m_pLayerManager->UpdateMP3Action(pGroupManager->GetID(), pGroupManager->GetEndTime() - TIME_OFFSET_1);
        }

    }
}

void CUILayerCommonImpl::PlayEvent_OnDebugStop(int nLayerType, int nPlayStatus)
{
    m_IsSinglePlaying = false;
    m_pOptionHandler->SetControlState(true);
    m_IsPlaying = false;
}

void CUILayerCommonImpl::SetLayerMenuState(bool menuState)
{
    m_IsPlaying = menuState;
    m_IsSinglePlaying = menuState;
}

void CUILayerCommonImpl::onPlaySelectMotionBlock()
{
    CActionBlockGroupManager* pCurrentPlayGroupManager = m_pBlockDataManager->FindBlockData(GetSelectGroupBlock());
    int endTime = pCurrentPlayGroupManager->GetEndTime();

    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SingleGroupPlaying(GetType(), pCurrentPlayGroupManager);
    }
    if (m_pOptionHandler)
    {
        m_pOptionHandler->SetControlState(false);
    }
    m_IsPlaying = true;
    m_IsSinglePlaying =true;
}

void CUILayerCommonImpl::onCancelPlayMotionBlock()
{
    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SingleBlockPlayingStop(GetType());
        m_IsPlaying = false;
        m_IsSinglePlaying = false;
        //m_pOptionHandler->SetControlState(true);
    }
}

void CUILayerCommonImpl::ILayerOptionHandler_OnDataBlockPlaying(CActionBlockData *pActionData)
{
    if (pActionData == NULL)
    {
        return;
    }
    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SingleBlockPlaying(GetType(),pActionData);
    }
}
void CUILayerCommonImpl::AddBlankBlockInTail()
{
    //	UBXAddBlankBlockDlg dlg;
    //	if (dlg.DoModal() == IDOK)
    //	{
    //		int CellCount = m_pLayerManager->GetCellCount();
    //		m_pLayerManager->SetCellCount(CellCount + dlg.m_BlankBlockCount);

    //		if(m_pLayerManager)
    //		{
    //            m_pLayerManager->GetWidget()->Invalidate(false);
    //			m_pLayerManager->UpdateScrollBar();
    //		}
    //	}
}

void CUILayerCommonImpl::ILayerOptionHandler_InsertGroupByMenu(CActionBlockData* pActionBlockData, int nCol)
{
    //Modified Flag
    float startTime = 0;
    float endTime = 0;
    CActionBlockGroupManager* pFindGroupManager = NULL;
    for (int j = 0; j < m_pBlockDataManager->GetCount(); j++)
    {
        pFindGroupManager = m_pBlockDataManager->GetAt(j);
        startTime = pFindGroupManager->GetStartTime();
        endTime = startTime + pFindGroupManager->GetActionAllTimeSum();
        pFindGroupManager->SetEndTime(endTime);
        if ((startTime) <= nCol && (nCol < endTime))
        {
            if ((j-1) >=0)
            {
                CActionBlockGroupManager* pNextGroupManager(m_pBlockDataManager->GetAt(j-1));
                startTime = pNextGroupManager->GetStartTime();
                endTime = startTime + pNextGroupManager->GetActionAllTimeSum();
            } else {
                startTime = 0;
                endTime = 0;
            }
            break;
        }
    }

    CActionBlockGroupManager* pGroupManager = NULL;
    pGroupManager = new CActionBlockGroupManager();
    pGroupManager->SetID(m_pBlockDataManager->GetUnUserID());

    WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
    memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
    QString strDescription = m_pBlockDataManager->GetUnUseDescription();
    int nDescriptionLen = QStringToWCHAR(wstrDescription, strDescription);
    //wstrDescription[nDescriptionLen - 1] ='\0';
    pGroupManager->SetDescription(wstrDescription);
    pGroupManager->SetStartTime(endTime);
    pGroupManager->SetEndTime(endTime + pActionBlockData->GetAllTime());
    char *pdatas = "motion";
    int nLenss = strlen(pdatas);
    pGroupManager->SetExtBlockData(nLenss, pdatas);

    m_pBlockDataManager->MoveGroup(pFindGroupManager, pActionBlockData->GetAllTime());
    m_pBlockDataManager->AddLayerBlock(pGroupManager);
    pGroupManager->AddLayerBlock(pActionBlockData);
    // Modified Flag
    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(true);
    }
    if (pActionBlockData->GetID() == -1)
    {
        pActionBlockData->SetID(pGroupManager->GetUnUserID());
    }
    SetSelectGroupBlock(pGroupManager->GetStartTime());
    AjustTimeForInsert(pFindGroupManager, pGroupManager->GetEndTime()-pGroupManager->GetStartTime());
}

void CUILayerCommonImpl::ILayerOptionHanlder_UnitedSimulation()
{
    if(m_pLayerManager)
    {
        m_pLayerManager->TotalLayerPlaying();
    }
}

void CUILayerCommonImpl::ILayerOptionHanlder_PauseUnitedSimulation()
{
    if(m_pLayerManager)
    {
        m_pLayerManager->PauseTotalLayerDebug();
    }
}

void CUILayerCommonImpl::ILayerOptionHanlder_StopUnitedSimulation()
{
    if(m_pLayerManager)
    {
        m_pLayerManager->StopTotalLayerDebug();
    }
}

void CUILayerCommonImpl::ILayerOptionHandler_HasMotionData(bool hasData)
{
    if(m_pLayerManager)
    {
        m_pLayerManager->HasMotionData(hasData);
    }
}

bool CUILayerCommonImpl::ILayerOptionHandler_IsPlaying()
{
    return m_IsPlaying;
}

void CUILayerCommonImpl::ILayerOptionHandler_SetMusicModifiedState(bool bState)
{
    if(m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetMusicModifiedState(bState);
    }
}

bool CUILayerCommonImpl::ILayerOptionHandler_GetMusicModifiedState()
{
    if(m_pLayerManager)
    {
        return m_pLayerManager->IUILayerManager_GetMusicModifiedState();
    }
    else
    {
        return false;
    }
}

IUILayerManager *CUILayerCommonImpl::ILayerOptionHandler_GetLayerManager()
{
    return m_pLayerManager;
}

emPlayType CUILayerCommonImpl::ILayerOptionHandler_GetPlayType()
{
    if (m_pLayerManager)
    {
        return m_pLayerManager->IUILayerManager_GetCurPlayType();
    }
}

void CUILayerCommonImpl::SetTreeView(ILibTreeNavigation* pView)
{
    m_treeView = pView;
}

void CUILayerCommonImpl::TreeToView(char *pData, int nLen, int nCol, QString FileName)
{
    if (m_IsPlaying || m_IsSinglePlaying || !m_pBlockDataManager)
    {
        return;
    }

    // 保存拖动前的数据，以进行undo操作
    IOperation *pChangedData = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMotionData);
    cloneBeforeChangedMotionData(pChangedData);

    bool IsInGroup = false;
    float startTime = 0;
    float endTime = 0;
    float fStartTime = 0;
    CActionBlockGroupManager* pFindGroupManager;
    for (int j = 0; j < m_pBlockDataManager->GetCount(); j++)
    {
        pFindGroupManager = m_pBlockDataManager->GetAt(j);
        startTime = pFindGroupManager->GetStartTime();
        endTime = pFindGroupManager->GetEndTime();

        if ((startTime) <= nCol && (nCol < endTime))
        {
            IsInGroup = true;
            endTime = startTime;
            fStartTime = startTime;
            break;
        }
    }

    /////////////////////////for insert after
    CActionBlockGroupManager * pActionBlockGroupManager = m_pBlockDataManager->getNextActionBlockGroupManager(pFindGroupManager);
    if(pActionBlockGroupManager)
    {
        pFindGroupManager = pActionBlockGroupManager;
        startTime = pActionBlockGroupManager->GetStartTime();
        endTime = pActionBlockGroupManager->GetEndTime();
        endTime = startTime;
        fStartTime = startTime;
    }
    else
    {
        IsInGroup = false;
    }
    /////////////////////////////////


    if (IsInGroup)
    {
        CActionBlockGroupManager* pGroupManager = NULL;
        pGroupManager = new CActionBlockGroupManager();
        pGroupManager->SetData(pData, nLen);

        //////////////////
        //旧版本保存为整型数据，新版本情况下保存为浮点数，新版本读旧版本的endtime（float读int内存）数据会全是0,所以需要先设置一下他的EndTime
        // 根据z该组下所有的CActionData进行累加得到（拖拽进来的块默认起始时间为0）
        pGroupManager->SetEndTime(pGroupManager->GetActionAllTimeSum());
        pGroupManager->SetID(m_pBlockDataManager->GetUnUserID());
        WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
        memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
        int nDescriptionLen = QStringToWCHAR(wstrDescription, FileName);
        //wstrDescription[nDescriptionLen - 1] ='\0';
        pGroupManager->SetDescription(wstrDescription);
        pGroupManager->SetStartTime(0);
        float fGroupLen = pGroupManager->GetEndTime()-pGroupManager->GetStartTime();
        pGroupManager->SetStartTime(fStartTime);
        pGroupManager->SetEndTime(fStartTime + fGroupLen);

        m_pBlockDataManager->MoveGroup(pFindGroupManager, fGroupLen);
        m_pBlockDataManager->AddLayerBlock(pGroupManager);

        AjustTimeForInsert(pFindGroupManager, pGroupManager->GetEndTime()-pGroupManager->GetStartTime());
        ClearSelectGroupBlock();
        SetSelectGroupBlock(pGroupManager->GetStartTime()+2);

        if(m_pLayerManager)
        {
            g_IsUpdateUIByData = true;
            int nGroupIndex = 0;
            int nFrameIndex = 0;
            int nTimeOffset = 0;
            m_pBlockDataManager->getSelectGroupFrameArrayIndex(nCol, nGroupIndex, nFrameIndex, nTimeOffset);
            if (nFrameIndex < 0)
            {
                nFrameIndex = 0;
            }
            m_pLayerManager->UpdateMP3Widget(m_pBlockDataManager, pGroupManager->GetID(), nFrameIndex);
            m_pLayerManager->SetActionDataGroupManagerToOptions(m_pBlockDataManager, pGroupManager->GetID(), nFrameIndex);
            g_IsUpdateUIByData = false;
        }

    }
    else
    {
        CActionBlockGroupManager* pLastGroupManager = NULL;
        pLastGroupManager = m_pBlockDataManager->GetFinalBlockData();
        if (pLastGroupManager)
        {
            fStartTime = pLastGroupManager->GetEndTime();
        } else {
            fStartTime = 0;
        }

        CActionBlockGroupManager* pGroupManager = NULL;
        pGroupManager = new CActionBlockGroupManager();
        if(!pGroupManager->SetData(pData, nLen))
        {
            return;
        }

        //////////////////
        //旧版本保存为整型数据，新版本情况下保存为浮点数，新版本读旧版本的endtime（float读int内存）数据会全是0,所以需要先设置一下他的EndTime
        // 根据z该组下所有的CActionData进行累加得到（拖拽进来的块默认起始时间为0）
        /////////////////
        float fEndTime = pGroupManager->GetActionAllTimeSum();
        //TRACE(L"ffEndTime is %0.2f\n",ffEndTime);
        pGroupManager->SetEndTime(fEndTime);

        float endTime = pGroupManager->GetEndTime();
        pGroupManager->SetID(m_pBlockDataManager->GetUnUserID());

        WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
        memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
        int nDescriptionLen = QStringToWCHAR(wstrDescription, FileName);
        //wstrDescription[nDescriptionLen - 1] ='\0';

        pGroupManager->SetDescription(wstrDescription);
        pGroupManager->SetStartTime(0);
        float startTime = pGroupManager->GetStartTime();
        pGroupManager->SetStartTime(fStartTime);
        //TRACE(L"fStartTime  is %0.2f\n",fStartTime);
        //TRACE(L"endTime     is %0.2f\n",endTime);
        //TRACE(L"startTime   is %0.2f\n", startTime);
        //TRACE(L"fStartTime + endTime- startTime is %0.2f\n",fStartTime + endTime- startTime);
        pGroupManager->SetEndTime(fStartTime + endTime- startTime);
        m_pBlockDataManager->AddLayerBlock(pGroupManager);

        AdjustAllGroupTime(pGroupManager, pGroupManager->GetEndTime()-pGroupManager->GetStartTime());
        ClearSelectGroupBlock();
        m_pBlockDataManager->clearAllGroupIDSelect();
        SetSelectGroupBlock(pGroupManager->GetStartTime()+2);
        if(m_pLayerManager)
        {
            g_IsUpdateUIByData = true;
            int nGroupIndex = 0;
            int nFrameIndex = 0;
            int nTimeOffset = 0;
            m_pBlockDataManager->getSelectGroupFrameArrayIndex(nCol, nGroupIndex, nFrameIndex, nTimeOffset);
            if (nFrameIndex < 0)
            {
                nFrameIndex = 0;
            }
            m_pLayerManager->UpdateMP3Widget(m_pBlockDataManager, pGroupManager->GetID(), nFrameIndex);
            m_pLayerManager->SetActionDataGroupManagerToOptions(m_pBlockDataManager, pGroupManager->GetID(), nFrameIndex);
            g_IsUpdateUIByData = false;
        }


    }

    // 保存拖动后的数据，以进行redo操作
    cloneAfterChangedMotionData(pChangedData);

    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(true);
    }
}

void CUILayerCommonImpl::AjustTimeForInsert(CActionBlockGroupManager* pManager, float fAdd)
{
    if (pManager == NULL)
        return;

#define ADD_OFFSET  5

    bool bNeedUpdateWindow = false;

    // 判断跟下一块数据有没有重叠
    int nIndex = m_pBlockDataManager->GetIndexByID(pManager->GetID());
    if (nIndex != m_pBlockDataManager->GetCount()-1 || nIndex != -1)
    {
        // 需要移动后面所有的块
        //        float fMoveBlock = fAdd;
        //        for (int i= nIndex; i < (m_pBlockDataManager->GetCount() - 1); i++)
        //        {
        //            CActionBlockGroupManager* pMove = m_pBlockDataManager->GetAt(i);
        //            pMove->SetStartTime(pMove->GetStartTime() + fMoveBlock);
        //            pMove->SetEndTime(pMove->GetEndTime() + fMoveBlock);
        //        }

        // 		for (int i= nIndex; i < (m_pBlockDataManager->GetCount()-1); i++)
        // //		for (int i= nIndex+1; i < (m_pBlockDataManager->GetCount()-1); i++)
        // 		{
        // 			CActionBlockGroupManager* pMove = m_pBlockDataManager->GetAt(i);
        // 			CActionBlockGroupManager* pMoveBefore = NULL;
        // 			float fStartTime = pMove->GetStartTime();
        // 			if (i >= 1)
        // 			{
        // 				pMoveBefore = m_pBlockDataManager->GetAt(i - 1);
        // 				if (pMoveBefore)
        // 				{
        // 					int nSumTime = pMoveBefore->GetCurrentActionTimeSum();
        // 					fStartTime = pMoveBefore->GetStartTime() + nSumTime;
        // 				}
        // 			}
        // 			if (pMove)
        // 			{
        // 				if (nIndex == i)
        // 				{
        // 					pMove->SetStartTime(nStartTime + nMoveBlock);   // 插入后方第一块右移nMoveBlock个方格
        // 				}
        // 				else
        // 					pMove->SetStartTime(nStartTime /*+ nMoveBlock*/);
        // 				pMove->SetEndTime(/*pMove->GetEndTime()*/pMove->GetStartTime()+ pMove->GetCurrentActionTimeSum() /*+ nMoveBlock*/);
        // 			}
        // 		}

        // 调整总的时间
        CActionBlockGroupManager* pLast = m_pBlockDataManager->GetAt(m_pBlockDataManager->GetCount()-1);
        int nCellCount = m_pLayerManager->GetNewMaxCellCount(pLast->GetEndTime())/100 + ADD_OFFSET;
        if ((m_pLayerManager->GetNewMaxCellCount(pManager->GetEndTime())) % 100)
        {
            nCellCount++;
        }
        m_pLayerManager->SetCellCount(nCellCount);
        bNeedUpdateWindow = true;
    }

    if (bNeedUpdateWindow)
    {
        //		if(m_pLayerManager)
        //		{
        //            m_pLayerManager->GetWidget()->Invalidate(false);
        //			m_pLayerManager->UpdateScrollBar();
        //		}
    }
}

void CUILayerCommonImpl::ILayerOptionHandler_ModifyGroupName(int nCol)
{
    CUILayerCommonImpl::OnLButtonDBClick(nCol);
}

void CUILayerCommonImpl::ILayerOptionHandler_CopyGroup()
{
    // 	if (m_pLayerManager)
    // 	{
    // 		m_pLayerManager->IUILayerManager_SetModifiedState(true);
    // 	}
    onCopyMotionBlock();
    // 	if (m_pLayerManager)
    // 	{
    // 		m_pLayerManager->IUILayerManager_SetModifiedState(true);
    // 	}
}

void CUILayerCommonImpl::ILayerOptionHandler_PasteGroup(int nCol)
{
    onPasteMotionBlock(nCol);
    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(true);
    }
}
