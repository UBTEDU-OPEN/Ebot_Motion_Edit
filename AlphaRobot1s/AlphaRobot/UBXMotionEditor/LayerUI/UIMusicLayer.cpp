#include "stable.h"
#include "UIMusicLayer.h"
#include <QRgb>
#include <QFileInfo>
#include <QDir>
#include <QApplication>

//#include "IUILayerManager.h "
/*#include "vld.h"*/
#include "../zxLib/configs.h"
#include "../zxLib/widget/messagebox.h"

CUIMusicLayer::CUIMusicLayer(void)
{
     m_fActEndTime = 0;
     m_fActStartTime = 0;
     m_GroupBlockDefSetColor = qRgb(135,206,205);
     m_GroupBlockSelSetColor = qRgb(100,150,220);
//     pRedPen.CreatePen(PS_SOLID,0,qRgb(255,0,0)); // 使用createPen画图很快
//     pGreenPen.CreatePen(PS_SOLID,1,qRgb(0,255,0));
//     pBluePen.CreatePen(PS_SOLID,0,qRgb(0,0,255));
//     pBlackPen.CreatePen(PS_SOLID,0,qRgb(0,0,0));
     m_pfWaveArray = NULL;
     m_nPixelPerSec = 100;
     h = 100;
     m_fValueMax = 0;
     m_fValueMin = 0;
     m_nBPMValue = 0;
//     m_pMusicChooseDlg = NULL;
}


CUIMusicLayer::~CUIMusicLayer(void)
{
}

//void CUIMusicLayer::OnDraw(CDC* pDC)
//{
//// 	CUILayerCommonImpl::OnDraw(pDC);
//// 	DrawMusicGroup(pDC);
//    /*return;*/
//    DrawBase(pDC);
//    DrawMusicGroup(pDC);
//    DrawWave(pDC);
//    //DrawBPMLine(pDC);
//    //////////////////////////////////////////////////////////////////////////
//}
//void CUIMusicLayer::OnLButtonDown(int nCol, QPoint point)
//{
//    ClearSelectGroupBlock();

//    SetSelectGroupBlock(0);
//    ILayerUI *pMotionLayer = NULL;
//    if(m_pLayerManager)
//    {
//        pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
//    }
//    if(pMotionLayer)
//    {
//        pMotionLayer->SetSelectGroupByOthers(nCol);
//        //pMotionLayer->AddGroupFromMenu();
//        //SetSelectGroupBlock()
//    }
// 	ILayerUI *pMotionLayer = NULL;
// 	if(m_pLayerManager)
// 	{
// 		pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
// 	}
// 	if(pMotionLayer)
// 	{
// 		pMotionLayer->OnLButtonDownDebug(nCol);
// 	}
//}
// void CUIMusicLayer::OnLButtonDownDebug()
// {
//
// }
// void CUIMusicLayer::OnLButtonDbClickDebug()
// {
//
// }
//void CUIMusicLayer::OnLButtonDown(int nCol)
//{
//    ClearSelectGroupBlock();

//    SetSelectGroupBlock(0);

//    int nSelGroupID = GetSelectGroupBlock();
// 	if (m_pLayerManager)
// 	{
// 		m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(), nSelGroupID);
// 	}

//}
//void CUIMusicLayer::OnRButtonDdown(int nCol)
//{
//    //__super::OnRButtonDdown(nCol);
//    OnLButtonDown(nCol,NULL);
//    CMenu menuContext;
//    CMenu *pSubMenu;
//    menuContext.m_hMenu  = ::language_loadmenu(IDR_MENU_MUSICLAYER);
//    pSubMenu = menuContext.GetSubMenu(0);
//    //pSubMenu->

//    ClearSelectGroupBlock();
//    SetSelectGroupBlock(nCol);

//    int nSelGroupID = GetSelectGroupBlock();
//// 	if (m_pLayerManager)
//// 	{
//// 		m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(), nSelGroupID);
//// 	}

//    int nIndex = GetSelectGroupBlock();
//// 	if (nCol<)
//// 	{
//// 	}
//    if (!pSubMenu)
//    {
//        return;
//    }
//    pSubMenu->EnableMenuItem(ID_SAVE_TO_COMMAND_BOX, MF_BYCOMMAND|MF_GRAYED);
//    if (nIndex == -1 || m_IsPlaying)
//    {
//        // 没有块选中,复制，删除，剪切，不可用
//        pSubMenu->EnableMenuItem(ID_MENU_COPY_BLOCK, MF_BYCOMMAND|MF_GRAYED);
//        pSubMenu->EnableMenuItem(ID_MENU_CUT_BLOCK, MF_BYCOMMAND|MF_GRAYED);
//        pSubMenu->EnableMenuItem(ID_MENU_DELETE_BLOCK, MF_BYCOMMAND|MF_GRAYED);
//        pSubMenu->EnableMenuItem(ID_MENU_PLAY, MF_BYCOMMAND|MF_GRAYED);
//        pSubMenu->EnableMenuItem(ID_MENU_INSERT, MF_BYCOMMAND|MF_GRAYED);
//        pSubMenu->EnableMenuItem(ID_MENU_SET_STARTTIME, MF_BYCOMMAND|MF_GRAYED);
//        pSubMenu->EnableMenuItem(ID_MENU_SET_ENDTIME, MF_BYCOMMAND|MF_GRAYED);
//        pSubMenu->EnableMenuItem(ID_MENU_CANCEL_PLAY, MF_BYCOMMAND|MF_GRAYED);
//        pSubMenu->EnableMenuItem(ID_MENU_SET_ALLSONG,MF_BYCOMMAND|MF_GRAYED);
//        //pSubMenu->EnableMenuItem(ID_MENU_INSERT_ACTION,MF_BYCOMMAND|MF_GRAYED);
//        //pSubMenu->EnableMenuItem(ID_MENU_ADD_SONG,MF_BYCOMMAND|MF_GRAYED);
//    }
//    else
//    {
//        pSubMenu->EnableMenuItem(ID_MENU_PASTE_BLOCK, MF_BYCOMMAND|MF_GRAYED);
//    }

//    if (m_IsPlaying)
//    {
//        pSubMenu->EnableMenuItem(ID_MENU_PASTE_BLOCK, MF_BYCOMMAND|MF_GRAYED);
//        pSubMenu->EnableMenuItem(ID_MENU_ADD_SONG,MF_BYCOMMAND|MF_GRAYED);
//        pSubMenu->EnableMenuItem(ID_MENU_INSERT_ACTION,MF_BYCOMMAND|MF_GRAYED);
//    }

//    if (m_IsSinglePlaying)
//    {
//        pSubMenu->EnableMenuItem(ID_MENU_CANCEL_PLAY, MF_BYCOMMAND|MF_ENABLED);
//    }

//    // 显示MENU
//    QPoint pos;
//    GetCursorPos(&pos);
//    UINT nCommand = CXTPCommandBars::TrackPopupMenu(pSubMenu, TPM_RETURNCMD, pos.x, pos.y, m_pLayerManager->GetQWidget());

//    DoMenuCommand(nCommand, pos, nCol);

//    menuContext.DestroyMenu();
//}
void CUIMusicLayer::ILayerOptionHandler_OnDeleteDataBlock(int nGroupID)
{
    m_fActStartTime = 0;
    m_fActEndTime = 0;
}
void CUIMusicLayer::ILayerOptionHandler_SetBPMValue(int nBPMValue)
{
    m_nBPMValue = nBPMValue;
}

void CUIMusicLayer::OnAddActionBlock(int nGroupManagerID, CActionBlockData* pActionBlockData,QString strBlockName )
{
    // 是否是新添加的GROUP MANAGE
    bool bNotify = false;
    if (pActionBlockData == NULL)
        return;
    CActionBlockGroupManager* pGroupManager = NULL;
    float fStartTime = 0;
    // 如果是－1，则根据当前的时候点，创建一个GROUP DATA
    if (nGroupManagerID == -1)
    {
        pGroupManager = new CActionBlockGroupManager();
        pGroupManager->SetID(m_pBlockDataManager->GetUnUserID());
        WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
        memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
        if (strBlockName.isEmpty())
        {
            QString strDescription = m_pBlockDataManager->GetUnUseDescription();
            /*int nDescriptionLen = */QStringToWCHAR(wstrDescription, strDescription);
            //wstrDescription[nDescriptionLen - 1] ='\0';
        }
        else
        {
            if(strBlockName.length() > MAX_DESCRIPTION_SIZE)
            {
                CMessageBox::warning(NULL,"", QObject::tr("action name too long"));
            }
            /*int nDescriptionLen = */QStringToWCHAR(wstrDescription, strBlockName);
            //wstrDescription[nDescriptionLen - 1] ='\0';
        }
            pGroupManager->SetDescription(wstrDescription);

        char *pdatas = "music";
        int nLenss = strlen(pdatas);
        pGroupManager->SetExtBlockData(nLenss, pdatas);

        CActionBlockGroupManager* pTempGroupManager = NULL;
        if (m_pBlockDataManager->GetCount() > 0)
        {
            pTempGroupManager = m_pBlockDataManager->GetFinalBlockData();
            fStartTime = pTempGroupManager->GetEndTime();
        }

        pGroupManager->SetStartTime(fStartTime);
        pGroupManager->SetEndTime(m_pLayerManager->GetCurrentSelTime());
        m_pBlockDataManager->AddLayerBlock(pGroupManager);
        //SetSelectGroupBlock(m_pLayerManager->GetCurrentSelTime());
        bNotify = true;
    }
    else
    {
        pGroupManager = m_pBlockDataManager->FindBlockData(nGroupManagerID);
    }
    // 重新分配结束时间
    float fLastEndTime = pGroupManager->GetEndTime();
    float fCurrentAllTime = pActionBlockData->GetAllTime();
    pGroupManager->SetEndTime(fLastEndTime + fCurrentAllTime);

    if (pActionBlockData->GetRunTime() == 0)
    {
        pActionBlockData->SetRunTime(fLastEndTime-fStartTime);
        pActionBlockData->SetAllTime(fLastEndTime-fStartTime);
    }
    // 判断ACTION BLOCK的ID，如果为－1，则重新分配ID
    if (pActionBlockData->GetID() == -1)
    {
        pActionBlockData->SetID(pGroupManager->GetUnUserID());
    }
    pGroupManager->AddLayerBlock(pActionBlockData);
    AdjustAllGroupTime(pGroupManager, fCurrentAllTime);
// 	if (bNotify)
// 	{
// 		if (m_pLayerManager)
// 			m_pLayerManager->SetActionDataGroupManagerToOptions(m_pBlockDataManager, pGroupManager->GetID());
// 	}

//    if (m_pLayerManager)
//    {
//        m_pLayerManager->GetQWidget()->Invalidate(false);
//    }
}


void CUIMusicLayer::ILayerOptionHandler_OnAddActionBlock(int nGroupManagerID, CActionBlockData* pActionBlockData, QString strBlockName , bool bIsUpdateMP3Widget)
{
    //CUILayerCommonImpl::ILayerOptionHandler_OnAddActionBlock(nGroupManagerID,pActionBlockData,strBlockName);
    OnAddActionBlock(nGroupManagerID,pActionBlockData,strBlockName);
    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(0);
    if (pGroupManager == NULL)
        return;

    float fStartTime = 0/*pActionBlockData->GetStartTime()*/;
    m_fActStartTime = fStartTime;
    pGroupManager->SetStartTime(fStartTime);

    float fEndTime = pActionBlockData->GetAllTime();
    pGroupManager->SetEndTime(fEndTime);
    m_fActEndTime = fEndTime;
    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(true);
    }
    AdjustAllGroupTime(pGroupManager, fStartTime + fEndTime);
    //AddWave(strBlockName);

}

void CUIMusicLayer::ILayerOptionHandler_OnAddWave(QString strWavePath )
{
    if (strWavePath.isEmpty())
    {
        return;
    }
    if (!AddWave(strWavePath))
    {
//        MessageBox(NULL, language_loadstring(IDS_WAVEREAD_ERROR), _T("Error"), MB_OK);
    }
}
void CUIMusicLayer::ILayerOptionHandler_OnGroupManagerChange(int nGroupID, float fTimeInc,bool bIsUpdateMp3Widget)
{
    RemoveWave();
    CUILayerCommonImpl::ILayerOptionHandler_OnGroupManagerChange(nGroupID , fTimeInc, bIsUpdateMp3Widget);


}
void CUIMusicLayer::deleteSelectMotionBlock()
{
    for (int i=0; i<m_GroupSelectID.count(); i++)
    {
        int nGroupID = m_GroupSelectID.at(i);
        CActionBlockGroupManager* pGroup = m_pBlockDataManager->FindBlockData(nGroupID);
        int nMoveBlock = 0;
        if (pGroup)
        {
            nMoveBlock = pGroup->GetEndTime()-pGroup->GetStartTime();
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
            MoveAllGourpTime(pGroupToMove, -nMoveBlock);
        }
        else
        {
            // 如果是最后一块数据，则移动总体的时间
            if (GroupIndex > 0){
                CActionBlockGroupManager* pGroupToFront = m_pBlockDataManager->GetAt(GroupIndex-1);
                if (pGroupToFront)
                {
                    AdjustAllGroupTime(pGroupToFront, -nMoveBlock);
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

//            m_pLayerManager->GetQWidget()->Invalidate(false);
        }

        //m_pLayerManager->SetActionDataGroupManagerToOptions(GetBlockDataManager(), -1);
    }

    // 清空所有已经选择的块
    if(!m_GroupSelectID.isEmpty())
            m_GroupSelectID.removeFirst();
//    m_GroupSelectID.RemoveAll();

    if (m_pLayerManager)
    {
        m_pLayerManager->UpdateScrollBar();
    }
    RemoveWave();
//    if (m_ManagerHandle)
//    {
//        m_ManagerHandle->Invalidate(false);
//    }
}
void CUIMusicLayer::SetMusicActionExtData(CActionBlockData *pActionData , const QString &path)
{
    pActionData->SetExtBlockData(path.toUtf8().length(), path.toUtf8().data());
}
void CUIMusicLayer::IMusicChooseEvent_AddMusicLayerBlock(int nGroupManagerID, const QString& musicPath, QString strBlockName )
{
    CActionBlockData* pActionBlock = new CActionBlockData();
    if (pActionBlock == NULL)
        return;
    int nScale = GetScaleValue();
    int nTotalMiliSec = 0/*pInfo->StreamTime.ms*/;
    float fEndTime = (int)nTotalMiliSec/nScale;
    if (nTotalMiliSec % nScale != 0)
    {
        pActionBlock->SetRunTime(fEndTime + 1);
        pActionBlock->SetAllTime(fEndTime + 1);
        //pActionBlock->SetActualEndTime((int)nTotalMiliSec/nScale + 1);
    }
    else
    {
        pActionBlock->SetRunTime(fEndTime);
        pActionBlock->SetAllTime(fEndTime);
        //pActionBlock->SetActualEndTime((int)nTotalMiliSec/nScale);
    }
    pActionBlock->SetID(-1);
    //pActionBlock->SetStartTime(0);

    WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
    memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
    int nDescriptionLen = QStringToWCHAR(wstrDescription, musicPath);
    //wstrDescription[nDescriptionLen-1] ='\0';
    pActionBlock->SetDescription(wstrDescription);

// #ifdef UNICODE
// 	char *p =NULL;
// 	wcharToChar(pInfo->szFilePath,&p);
// 	int nLen = strlen(p) + 1;
// 	pActionBlock->SetExtBlockData(nLen, p);
// 	if (p && nLen)
// 	{
// 		delete []p;
// 		p = NULL;
// 	}
// #else
// 	pActionBlock->SetExtBlockData(strlen(pInfo->szFilePath) + 1,pInfo->szFilePath);
// #endif

    int nCurID = GetSelectGroupBlock();
    //m_pBlockDataManager->GetAt(nCurID);
    if (nCurID != -1)
    {
        deleteSelectMotionBlock();
        nCurID = -1;
    }

    ILayerOptionHandler_OnAddActionBlock(nCurID,pActionBlock, musicPath, true);
//    ILayerOptionHandler_OnAddWave(m_pMusicChooseDlg->m_waveFilePath);
//    ILayerOptionHandler_SetBPMValue(pInfo->nBPM);
    SetMusicActionExtData(pActionBlock, musicPath);
//    if (!pInfo->nBPM)
//    {
//        return;
//    }

    //ILayerUI* pLayerUI = FindLayer(nLayerID);
    if (m_pLayerManager)
    {
        if (m_nBPMValue != 0)
        {
            int nMilisecsPerBeat = (int) SecsPerMinute/m_nBPMValue;
            m_pLayerManager->SetMusicBeatsTime(nMilisecsPerBeat);
        }

        m_pLayerManager->SetOriginPos(0/*pActionBlock->GetStartTime()*/);
    }

}
void CUIMusicLayer::IMusicChooseEvent_SetFileModified(bool bState)
{
    if (m_pLayerManager)
    {
        m_pLayerManager->IUILayerManager_SetModifiedState(bState);
    }
}
void CUIMusicLayer::IMusicChooseEvent_DeleteMusicLayerBlock()
{
    SetSelectGroupBlock(0);
    deleteSelectMotionBlock();
}
void CUIMusicLayer::DoMenuCommand(uint nCommand, QPoint point, int nCol)
{

//    switch(nCommand)
//    {
//    case ID_MENU_SET_STARTTIME:
//        {
//            SetActStartTime(nCol);
//        }
//        break;
//    case ID_MENU_SET_ENDTIME:
//        {
//            SetActEndTime(nCol + 1);
//        }
//        break;
//    case ID_MENU_ADD_SONG:
//        {
//            if (m_IsPlaying)
//                return;
//            if ( m_pMusicChooseDlg && m_pMusicChooseDlg->GetSafeHwnd())
//            {

//                m_pMusicChooseDlg->CenterWindow();
//                m_pMusicChooseDlg->ShowWindow(SW_SHOW);
//            }
//        }
//        break;
//    case ID_MENU_SET_ALLSONG:
//        {
//            CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(0);
//            if (!pGroupManager)
//            {
//                return;
//            }
//            float fEndTime = pGroupManager->GetEndTime();
//            SetActStartTime(0);
//            SetActEndTime(fEndTime);
//        }
//        break;
//    case ID_MENU_DELETE_BLOCK:
//        {
//            SetActStartTime(0);
//            SetActEndTime(0);
//// 			if (m_pLayerManager)
//// 			{
//// 				m_pLayerManager->IUILayerManager_DeleteMusicFile(ACTION_LAYER_MUSIC);
//// 			}
//            DeleteSelectGroup();
//            if (m_pMusicChooseDlg->GetSafeHwnd())
//            {
//                m_pMusicChooseDlg->DeleteMusic();
//            }
//            RemoveWave();
//        }
//        break;
//    case ID_MENU_INSERT_ACTION:
//        {
//            ILayerUI *pMotionLayer = NULL;
//            if(m_pLayerManager)
//            {
//                pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
//            }
//            if(pMotionLayer)
//            {
//                pMotionLayer->SetSelectGroupByOthers(nCol);
//                pMotionLayer->AddGroupFromMenu();
//                //SetSelectGroupBlock()
//            }

//        }
//        break;
//    default:
//        break;
//    }
    //CUILayerCommonImpl::DoMenuCommand(nCommand,point,nCol);
}

bool CUIMusicLayer::Connect(QWidget* managerHandle)
{
    CUILayerCommonImpl::Connect(managerHandle);
    //m_pMusicChooseDlg->Create()

// 	m_pMusicChooseDlg->Create(CMusicChooseDialog::IDD, NULL);
// 	m_pMusicChooseDlg->SetMusicEvent(this);
// 	CRect rect;
// 	m_pMusicChooseDlg->GetWindowRect(&rect);
// 	m_pMusicChooseDlg->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

    //m_waveAnalyze.Connect(_T("F:\\ABC.wav"));
    return true;
}


//从ActionData的扩展数据中获取音乐文件名并添加
bool CUIMusicLayer::AddMusicFromExtData(int nGroupIndex , int nActionIndex)
{
    QString strMusicPath;
    GetActionSongPath(nGroupIndex,nActionIndex,strMusicPath);
    if (!strMusicPath.isEmpty())
    {
        if (strMusicPath.contains(MUSIC_FILE_EXT))
        {
            if(m_pOptionHandler)
            {
                m_pOptionHandler->SetMusicResource(strMusicPath);
                return true;
            }

            return false;
        }
        else
            return false;
    }
    else
        return false;

    return true;
}
//从Music目录下获取第一个音乐文件并添加
bool CUIMusicLayer::AddMusicFromMusicDir()
{
    QString strMusicPath;
    QString strMusicFilePath;

    strMusicPath = CConfigs::getLocalMusicDir();
    QFileInfoList infoList = QDir(strMusicPath).entryInfoList();
    foreach (QFileInfo fileInfo, infoList)
    {
        if(!fileInfo.isFile()) continue;

        QString sExt = '.' + fileInfo.suffix();
        if(0 == sExt.compare(MUSIC_FILE_EXT, Qt::CaseInsensitive))
        {
            strMusicFilePath = QDir::toNativeSeparators(strMusicPath + '/' + fileInfo.fileName());
            break;
        }
    }

    if (strMusicFilePath.isEmpty())
    {
        return false;
    }
    else
    {
        AddMusic(strMusicFilePath);
        return false;
    }

    return true;
}

void CUIMusicLayer::ILayerOptionHandler_OnLButtonDown(int nCol, QPoint point, float fStartTime)
{
    ILayerUI* pMotionLayer = NULL;
    if(m_pLayerManager)
    {
        pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
        if(pMotionLayer)
        {
            pMotionLayer->OnLButtonDown(nCol, point, fStartTime);
        }
    }
}

void CUIMusicLayer::ILayerOptinHandler_OnLButtonDBClick(int nCol)
{
    ILayerUI* pMotionLayer = NULL;
    if(m_pLayerManager)
    {
        pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
        if(pMotionLayer)
        {
            pMotionLayer->OnLButtonDBClick(nCol);
        }
    }
}

void CUIMusicLayer::ILayerOptionHanlder_OnStopPlay()
{
    if(m_pLayerManager)
    {
        m_pLayerManager->LayerStop();
    }
}

void CUIMusicLayer::ILayerOptionHanlder_LayerPlaying()
{
    ILayerUI* pMotionLayer = NULL;
    if(m_pLayerManager)
    {
        pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
        if(pMotionLayer)
        {
            m_pLayerManager->LayerPlaying(pMotionLayer->GetType(), pMotionLayer->GetOptionHandler());
        }
    }
}

void CUIMusicLayer::ILayerOptionHanlder_LayerPause()
{
    if(m_pLayerManager)
    {
        m_pLayerManager->PauseLayerDebug();
    }
}

void CUIMusicLayer::ILayerOptinHandler_FinishMusicPlaying()
{
    if(m_pLayerManager)
    {
        m_pLayerManager->FinishMusicPlaying();
    }
}

void CUIMusicLayer::ILayerOptionHandler_MusicLoaded(const QString& path, const QString& title, int time)
{
    ILayerUI* pMotionLayer = NULL;
    if(m_pLayerManager)
    {
        pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
        if(pMotionLayer)
        {
            pMotionLayer->GetOptionHandler()->MusicLoaded(title, time);
        }
    }
//    IMusicChooseEvent_AddMusicLayerBlock(1, path);
}

void CUIMusicLayer::ILayerOptionHandler_UpdateMusicPosition(quint64 position)
{
    ILayerUI* pMotionLayer = NULL;
    if(m_pLayerManager)
    {
        pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
        if(pMotionLayer)
        {
            pMotionLayer->GetOptionHandler()->UpdateMusicPosition(position);
        }
    }
}

void CUIMusicLayer::ILayerOptionHandler_DeleteMusic()
{
    SetSelectGroupBlock(0);
    deleteSelectMotionBlock();
}

void CUIMusicLayer::ILayerOptionHandler_OnRButtonDown(int nCol, QPoint point)
{
    ILayerUI* pMotionLayer = NULL;
    if(m_pLayerManager)
    {
        pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
        if(pMotionLayer)
        {
            pMotionLayer->OnRButtonDdown(nCol);
        }
    }
}

void CUIMusicLayer::Ilayeroptionhandler_MotionlayerActionCommand(uint nCommand, int nCol)
{
    ILayerUI* pMotionLayer = NULL;
    if(m_pLayerManager)
    {
        pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
        if(pMotionLayer)
        {
            pMotionLayer->DoMenuCommand(nCommand, nCol);
        }
    }
}

void CUIMusicLayer::Ilayeroptionhandler_TreeToView(char *pData, int nLen, int nCol, QString FileName)
{
    ILayerUI* pMotionLayer = NULL;
    if(m_pLayerManager)
    {
        pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
        if(pMotionLayer)
        {
            pMotionLayer->TreeToView(pData, nLen, nCol, FileName);
        }
    }
}

void CUIMusicLayer::ILayerOptionHandler_SetMusicModifiedState(bool bState)
{
    if(m_pLayerManager)
    {
       m_pLayerManager->IUILayerManager_SetMusicModifiedState(bState);
    }
}

bool CUIMusicLayer::ILayerOptionHandler_GetMusicModifiedState()
{
    if(m_pLayerManager)
    {
       return m_pLayerManager->IUILayerManager_GetMusicModifiedState();
    }
    return false;
}

bool CUIMusicLayer::ILayerOptionHandler_hasCopyFrame()
{
    if(!m_pLayerManager)
    {
        return false;
    }

    ILayerUI* pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
    if(!pMotionLayer)
    {
        return false;
    }

    ILayerOptionHandler  *pOptionHandler = pMotionLayer->GetOptionHandler();
    if (!pOptionHandler)
    {
        return false;
    }
    return pOptionHandler->hasCopyFrame();
}

void CUIMusicLayer::GetActionSongPath(int nGroupIndex , int nActionIndex, QString& strSongPath)
{
    if (m_pBlockDataManager == NULL || m_pBlockDataManager->GetCount() <= nGroupIndex)
    {
        return /*NULL*/;
    }
    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(nGroupIndex);
    if (pGroupManager == NULL || pGroupManager->GetActionCount() <= nActionIndex)
    {
        return /*NULL*/;
    }
    CActionBlockData *pActionData = pGroupManager->GetAt(nActionIndex);
    if(pActionData == NULL)
    {
        return /*NULL*/;
    }
    else
    {
        char *pData = NULL;
        int nLen = 0;
        pActionData->GetExtBlockData(&pData,nLen);
        //char* szName = NULL;
        QString str;

        if (pData && nLen)
        {
            str = pData;
            delete []pData;
            pData = NULL;
            /*MultiByteToWideChar()*/
        }
        /*QString strW= QString(str);*/
        strSongPath = QString(str);
        return /*pActionData->GetDescription()*/;
    }
}
WCHAR* CUIMusicLayer::GetActionDescription(int nGroupIndex , int nActionIndex)
{
    if (m_pBlockDataManager == NULL || m_pBlockDataManager->GetCount() <= nGroupIndex)
    {
        return NULL;
    }
    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(nGroupIndex);
    if (pGroupManager == NULL || pGroupManager->GetActionCount() <= nActionIndex)
    {
        return NULL;
    }
    CActionBlockData *pActionData = pGroupManager->GetAt(nActionIndex);
    if(pActionData == NULL)
    {
        return NULL;
    }
    else
    {
        char *pData = NULL;
        int nLen = 0;
        pActionData->GetExtBlockData(&pData,nLen);
        //char* szName = NULL;
        QString str;

        if (pData && nLen)
        {
            str = pData;
            delete []pData;
            pData = NULL;
            /*MultiByteToWideChar()*/
        }
        /*QString strW= QString(str);*/

        return pActionData->GetDescription();
    }
}
void CUIMusicLayer::GetMusicDlgPtr(/*CMusicChooseDialog *pDlg*/)
{
//	m_pMusicChooseDlg = pDlg;
//	if (m_pMusicChooseDlg)
//	{
//		m_pMusicChooseDlg->SetMusicEvent(this);
//	}
}
bool CUIMusicLayer::ReleaseConnection()
{
    CUILayerCommonImpl::ReleaseConnection();
    if (m_pfWaveArray)
    {
        delete []m_pfWaveArray;
        m_pfWaveArray = NULL;
    }
    return true;
}


bool CUIMusicLayer::GetData(char** ppData, int& nLen)
{
    CUILayerCommonImpl::GetData(ppData, nLen);

    return true;
}

bool CUIMusicLayer::SetData(char* pData, int nLen)
{
    CUILayerCommonImpl::SetData(pData, nLen);

    return true;
}

// 取得类型，具体见宏定义
int CUIMusicLayer::GetType()
{
    return ACTION_LAYER_MUSIC;
}

QString CUIMusicLayer::GetLayerName()
{
    QString strName = QObject::tr("");
    return strName /*language_loadstring(IDS_MUSIC_LAYER)*/;
}
void CUIMusicLayer::SetActStartTime(float fActStartTime)
{
    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(0);
    if (!pGroupManager)
    {
        return;
    }
    float fEndTime = pGroupManager->GetEndTime();
    if (fActStartTime <= m_fActEndTime )
    {
        m_fActStartTime = fActStartTime;
    }
    else
    {
        m_fActEndTime = fEndTime;
        m_fActStartTime = fActStartTime;
    }
}
void CUIMusicLayer::SetActEndTime(float fActEndTime)
{
    if (fActEndTime >= m_fActStartTime)
    {
        m_fActEndTime = fActEndTime;
    }
    else
    {
        m_fActStartTime = 0;
        m_fActEndTime = fActEndTime;
    }
    //setcurrentcol
}
void CUIMusicLayer::AddMusic(QString strMusicFile , bool bFromAes)
{
    if(m_pOptionHandler)
    {
        m_pOptionHandler->SetMusicResource(strMusicFile);
    }

//    if (!m_pMusicChooseDlg && !m_pMusicChooseDlg->GetSafeHwnd())
//    {
//        return;
//    }
//    if (bFromAes)
//    {
//        m_pMusicChooseDlg->ReadMusicFromAesFile(strMusicFile);
//    }
//    else
//        m_pMusicChooseDlg->OnBnClickedButton1();
}
void CUIMusicLayer::PlayMusic(int nActStartTime , int nActEndTime , int nPlayState)
{

//    if (m_pMusicChooseDlg && m_pMusicChooseDlg->GetSafeHwnd())
//    {
//        m_pMusicChooseDlg->PlayMusic(nActStartTime , nActEndTime , nPlayState);
//    }
}
int CUIMusicLayer::GetCurrentPos()
{
//    if (m_pMusicChooseDlg && m_pMusicChooseDlg->GetSafeHwnd())
//    {
//        return m_pMusicChooseDlg->GetCurrentPos();
//    }
//    else
//        return -1;




    return 0;
}
float  CUIMusicLayer::GetActStartTime()
{
    return m_fActStartTime;
}
float  CUIMusicLayer::GetActEndTime()
{
    return m_fActEndTime;
}
float  CUIMusicLayer::GetStartTime()
{
    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(0);
    if (!pGroupManager)
    {
        return 0 ;
    }
    int nStartTime = pGroupManager->GetStartTime();
    return nStartTime;
}
float  CUIMusicLayer::GetEndTime()
{
    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(0);
    if (!pGroupManager)
    {
        return 0;
    }
    float fEndTime = pGroupManager->GetEndTime();
    return fEndTime;
}
void CUIMusicLayer::WaveSmooth(double *fArray , int nArrayCount , double fCmpValue , double fWeight)
{
    for (int i = 0 ; i < nArrayCount ; i++)
    {
        double f = *(fArray + i);
        if (f < fCmpValue)
        {
            *(fArray + i) *= fWeight;
        }
    }
}
bool CUIMusicLayer::AddWave(QString strWavePath)
{
//     RemoveWave();
//     if (m_waveAnalyze.Connect(strWavePath))
//     {
//         int nDrawInterval = (int)m_waveAnalyze.m_nSampleRate/m_nPixelPerSec;     //由于一格只有 nPixelePerSec个像素点，所以最多只能在这么多个点上绘制
//         //图形，故在采样点钟每隔nDrawInterval区一个样本绘制
//         long n = m_waveAnalyze.m_nSamples;                      //样本的采样数
//         long nDrawPointsCount = 0;
//         if (m_waveAnalyze.m_nSampleRate)
//         {
//             double fRate = (double)m_nPixelPerSec / m_waveAnalyze.m_nSampleRate;
//             nDrawPointsCount = n * fRate;
//         }
//         else
//         {
//             return false;
//         }
//         int k = 0;
//         long j = 0;
//         double fValueMax = 0;
//         double fValueMin = 0;
//         if (m_pfWaveArray)
//         {
//             delete []m_pfWaveArray;
//             m_pfWaveArray = NULL;
//         }
//         m_pfWaveArray = new double[nDrawPointsCount];
//         for ( j = 0 ; j<nDrawPointsCount;j++)
//         {
//             if (m_waveAnalyze.m_dwChannels == 1)
//             {
//                 m_pfWaveArray[j] = m_waveAnalyze.m_pDm[k];
//             }
//             else
//             {
//                 m_pfWaveArray[j] = 0.5 * (m_waveAnalyze.m_pDl[k] + m_waveAnalyze.m_pDr[k]);
//                 //TRACE(L"Mix is %f , Left is %f , Right is %f\n" ,m_pfWaveArray[j] , m_waveAnalyze.m_pDl[k] ,m_waveAnalyze.m_pDr[k]);
//             }
//             //m_pfWaveArray[j] =  fabs(m_pfWaveArray[j]);
//             k += nDrawInterval;

//             if (m_pfWaveArray[j] > fValueMax)
//             {
//                 fValueMax = m_pfWaveArray[j];      //求出该数组中的绝对值最大值（保证绘图不会超出该范围）
//             }
//             if(m_pfWaveArray[j] < fValueMin)
//             {
//                 fValueMin = m_pfWaveArray[j];
//             }
//         }
//#if 0
//#define  PEAK_DETECTING_WINDOW 1    //波峰检测窗口
//         int nWindowWidth = PEAK_DETECTING_WINDOW;

//         for (int i = 0 ; i < nDrawPointsCount ; i += nWindowWidth * m_nPixelPerSec)
//         {
//             double fSum = 0;
//             double fMaxValue = 0;
//             int j1 = 0;
//             for (j1 = 0 ; j1< nWindowWidth * m_nPixelPerSec;j1++)
//             {
//                 if (i+j1 == nDrawPointsCount)
//                 {
//                    //WaveSmooth(&m_pfWaveArray[i],j1,fMaxValue * 0.5 ,0.3);
//                     break;
//                 }
//                 fSum += m_pfWaveArray[i+j1];
//                 if (m_pfWaveArray[i+j1] > fMaxValue)
//                 {
//                     fMaxValue = m_pfWaveArray[i+j1];      //求出该数组中的绝对值最大值（保证绘图不会超出该范围）
//                 }
//             }
//             double fMeanValue = (double) fSum / (nWindowWidth * m_nPixelPerSec);
//             for (int j = 0 ; j< j1;j++)
//             {
//                 //WaveSmooth(&m_pfWaveArray[i],j1,fMaxValue * 0.5 ,0.3);
//                 if(m_pfWaveArray[i+j] < fMaxValue * 0.5)
//                 {
//                     m_pfWaveArray[i+j] = m_pfWaveArray[i+j] * 0.3;
//                 }
//             }
//         }
//#endif
//         m_fValueMax = fValueMax;
//         m_fValueMin = fValueMin;
//        return true;
//     }
//     //::PostMessageW(m_ManagerHandle->GetSafeHwnd(),WM_PAINT,0,0);
///*	return false;*/
     return false;
}
//void CUIMusicLayer::OnLButtonDBClick(int nCol)
//{
//    return;
//    if (m_IsPlaying)
//        return;
//    if ( m_pMusicChooseDlg && m_pMusicChooseDlg->GetSafeHwnd())
//    {

//        m_pMusicChooseDlg->CenterWindow();
//        m_pMusicChooseDlg->ShowWindow(SW_SHOW);
//    }
//// 	ILayerUI *pMotionLayer = NULL;
//// 	if(m_pLayerManager)
//// 	{
//// 		pMotionLayer = m_pLayerManager->FindLayer(ACTION_LAYER_MOTION);
//// 	}
//// 	if(pMotionLayer)
//// 	{
//// 		pMotionLayer->OnLButtonDownDebug(nCol);
//// 	}
//}
void CUIMusicLayer::DrawMusicGroup(/*CDC* pDC*/)
{
//    if (m_pBlockDataManager == NULL)
//        return;

//    int m_lineThick  = m_pLayerManager->GetLineThick();
//    int m_nCellWidth = m_pLayerManager->GetCellWidth();
//    int nScale = 1000 / (m_lineThick + m_nCellWidth);
//    int nPixelPerGrid = m_nCellWidth + m_lineThick;
////	QWidget* P = m_pLayerManager->GetQWidget();

//    int xStart, yStart;
//    GetPosition(xStart, yStart);

//    CRect rc;
//    m_ManagerHandle->GetClientRect(rc);
//    int DisplayEnd = rc.Width()/*/(m_lineThick + m_nCellWidth)*/ + GetCellStartIndex() * nPixelPerGrid;
//    //DisplayEnd *= nPixelPerGrid;
//    for (int i=0; i<m_pBlockDataManager->GetCount(); i++)
//    {
//        CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(i);
//        if (pGroupManager == NULL)
//            continue;

//        float fStartTime = pGroupManager->GetStartTime() ;
//        float fEndTime = pGroupManager->GetEndTime() ;
//        fStartTime = m_fActStartTime;
//        fEndTime = m_fActEndTime;
//// 		if (m_fActEndTime == m_fActStartTime )
//// 		{
//// 			m_fActStartTime = nStartTime;
//// 			m_fActEndTime = nEndTime;
//// 		}
//// 		else
//// 		{
//// 			nStartTime = m_fActStartTime ;
//// 			nEndTime = m_fActEndTime ;
//// 		}
//// 		if (m_fActEndTime == m_fActStartTime)  //起始时间和截止时间相同的话，会去除图，重新设置起始时间和截止时间为初始值
//// 		{
//// 			m_fActStartTime = pGroupManager->GetStartTime() ;
//// 			m_fActEndTime = pGroupManager->GetEndTime() ;
//// 		}

//        CRect rect;
//        // 		rect.left = xStart+((nStartTime-GetCellStartIndex()) * (m_nCellWidth+m_lineThick));
//        // 		rect.right = rect.left+( (nEndTime-nStartTime+1) * (m_nCellWidth+m_lineThick));
//        rect.top = yStart;
//        rect.bottom = rect.top+GetCellHeight()-1;

//        if ((fStartTime < DisplayEnd  ) &&  (fEndTime >= GetCellStartIndex() *nPixelPerGrid))
//        {
//            rect.left = fStartTime-GetCellStartIndex()*nPixelPerGrid;
//            if(rect.left < 0)
//            {
//                rect.left = 0;
//            }
//            if (fEndTime  <= DisplayEnd )
//            {
//                //rect.right = (nEndTime-GetCellStartIndex())*(m_nCellWidth+m_lineThick);
//                rect.right = fEndTime-GetCellStartIndex()*nPixelPerGrid;
//                //				rect.right = (nEndTime-GetCellStartIndex()+1)*(m_nCellWidth+m_lineThick);
//            } else {
//                rect.right = (DisplayEnd+m_nCellWidth+m_lineThick);
//            }
//        }

//        if (IsSelectGroupBlock(pGroupManager->GetID()))
//        {
//            CBrush bgBrush(m_GroupBlockSelSetColor);
//            pDC->FillRect(rect, &bgBrush);
//        }
//        else
//        {
//            CBrush bgBrush(m_GroupBlockDefSetColor);
//            pDC->FillRect(rect, &bgBrush);
//        }

//        {
//            // 画层名称
//            CFont font;
//            VERIFY( font.CreatePointFont( this->m_fontSize, this->m_fontFamily, pDC ) );
//            CFont* def_font = pDC->SelectObject( &font );

//            CSize m_size = pDC->GetTextExtent( pGroupManager->GetDescription() ); //计算buf中的字体总宽为多少个像素
//            pDC->SetBkMode( TRANSPARENT );						  //设置字体背景为透明
//            CRect fontRect( rect.TopLeft(), QPoint( rect.right, rect.top + rect.Height()) );

//            pDC->SetTextColor( this->m_fontColor );
//            //pDC->DrawText( pGroupManager->GetDescription(), &fontRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

//            // 画实底线
//            CPen pen( PS_SOLID, /*this->m_lineThick*/m_pLayerManager->GetLineThick(), RGB(0, 0, 0));
//            pDC->SelectObject( &pen );

//            pDC->MoveTo(rect.left, rect.top);
//            pDC->LineTo(rect.right, rect.top);

//            pDC->MoveTo(rect.left, rect.top);
//            pDC->LineTo(rect.left, rect.bottom);

//            pDC->MoveTo(rect.left, rect.bottom);
//            pDC->LineTo(rect.right, rect.bottom);

//            pDC->MoveTo(rect.right, rect.top);
//            pDC->LineTo(rect.right, rect.bottom);

//            pDC->SelectObject(def_font);
//            font.DeleteObject();
//        }
//    }

}
void CUIMusicLayer::DrawBPMLine(/*CDC *pDC*/)
{
//    if (!m_pfWaveArray || m_nBPMValue == 0)
//    {
//        return;
//    }
//    int m_nCellWidth = m_pLayerManager->GetCellWidth();
//    int m_lineThick = m_pLayerManager->GetLineThick();
//    int m_CellCount = m_pLayerManager->GetCellCount();
//    int nPixelPerGrid = m_nCellWidth + m_CellCount;


//    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(0);
//    if (!pGroupManager)
//    {
//        return;
//    }
//    float fEndTime = pGroupManager->GetEndTime();
//    float fRealEndTime = ceil(fEndTime * GetScaleValue());

//    //画背景
//    int xStart, yStart;
//    GetPosition(xStart, yStart);
//    int xTmp = xStart;

//    CRect rc;
//    m_ManagerHandle->GetClientRect(rc);
//    int nCount = rc.Width()/(m_lineThick + m_nCellWidth) + GetCellStartIndex();
//    CRect rect;
//    rect = rc;
//    rect.top = CUILayerCommonImpl::h;
//    rect.bottom = rect.top + h;
//    CPen pen( PS_SOLID, /*this->m_lineThick*/2, RGB(250,250,250));
//    pDC->SelectObject(&pen);
//    //pDC->Rectangle(&rect);
//    int nBPMScale = 1000;
//    if (m_nBPMValue)
//    {
//        nBPMScale = (int) SecsPerMinute / m_nBPMValue;
//    }
//    else
//        return;
//    int nPixelPerBeat = nBPMScale / GetScaleValue();
//    int nScreenBeatCount = rc.Width() / nPixelPerBeat;
//    int nXCellStartPixel =  GetCellStartIndex() * nPixelPerGrid;
//    int nBeatCount = nXCellStartPixel / nPixelPerBeat;    //计算之前有多少拍
//    int nX = (nBeatCount + 1) * nPixelPerBeat;
//    nX -= GetCellStartIndex() * nPixelPerGrid;
//    pDC->SelectObject(&pen);
//    for (int i = 0 ;i<nScreenBeatCount;i++ )
//    {
//        int y = CUILayerCommonImpl::h;
//        pDC->MoveTo(nX,y);
//        pDC->LineTo(nX,y+h);
//        nX += nPixelPerBeat;

//    }


//    // 	pDC->MoveTo( xTmp, y);
//    // 	pDC->LineTo( xTmp, y + h);
}
void CUIMusicLayer::ILayerOptionHandler_OnDeleteGroupBlock()
{
    RemoveWave();
    CUILayerCommonImpl::ILayerOptionHandler_OnDeleteGroupBlock();

    //DeleteSelectGroup();

}
void CUIMusicLayer::RemoveWave()
{
//    m_waveAnalyze.DisConnect();
//    if (m_pfWaveArray)
//    {
//        delete []m_pfWaveArray;
//        m_pfWaveArray = NULL;
//    }
}
void CUIMusicLayer::DrawWave(/*CDC* pDC*/)
{

//    if (!m_pfWaveArray || !m_ManagerHandle)
//    {
//        return;
//    }
//    int m_nCellWidth = m_pLayerManager->GetCellWidth();
//    int m_lineThick = m_pLayerManager->GetLineThick();
//    int m_CellCount = m_pLayerManager->GetCellCount();

//    //绘制图形的相关参数
//    int nPixelPerGrid = m_nPixelPerSec;                     //一格中像素点数（一格代表1S钟时间）
//    int nSampleRate = m_waveAnalyze.m_nSampleRate;          //采样频率（1S中采多少个点）
//    int nDrawInterval = (int)nSampleRate/nPixelPerGrid;     //由于一格只有 nPixelPerGrid个像素点，所以最多只能在这么多个点上绘制
//                                                            //图形，故在采样点钟每隔nDrawInterval区一个样本绘制
//    long n = m_waveAnalyze.m_nSamples;                      //样本的采样数
//    long nDrawPointsCount = 0 ;
//    double fRate = (double)m_nPixelPerSec / m_waveAnalyze.m_nSampleRate;
//    nDrawPointsCount = n * fRate;
//    int nMusicLayserHeight= h;
//    double fValueMax = m_fValueMax;
//    DOUBLE fValueMin = m_fValueMin;
//    double fMax = max(fabs(fValueMin),fabs(fValueMax));
//    DOUBLE dx = 1;
//    DOUBLE dy = 0;

//    CRect rc; //取得屏幕环境区
//    m_ManagerHandle->GetClientRect(&rc);
//    //音乐层高度
//    int h0 = CUILayerCommonImpl::h + nMusicLayserHeight * 0.5 /*/ 2*/  ;   //基线的位置
//    if (fValueMin && fValueMax)
//    {
//        dy =  0.5 * nMusicLayserHeight /  ( fMax);
//    }
//    else
//        dy = nMusicLayserHeight / 2 ;


//    int nCount = rc.Width()/(m_lineThick + m_nCellWidth) + GetCellStartIndex();
//    int w = rc.right;
//    //float dx = (float)w / n, dy = (float)nMusicLayserHeight / 2;
//    int i = GetCellStartIndex() * nSampleRate;
//    int x = 0;
//    int y = 0;
//    // MSG message;
//    // draw channels of stereo samples
//    // draw lower zero line

//    pDC->SelectObject(pBluePen);
//    CActionBlockGroupManager* pGroupManager = m_pBlockDataManager->GetAt(0);
//    if (!pGroupManager)
//    {
//        return;
//    }
//    float fEndTime = pGroupManager->GetEndTime();
//    float fRealEndTime = ceil(fEndTime * GetScaleValue());
//    int nStartCell = GetCellStartIndex();
//    int nTemp = (w + GetCellStartIndex() * nPixelPerGrid);
//    if ((w + GetCellStartIndex() * nPixelPerGrid) > fEndTime)
//    {
//        w = fEndTime - GetCellStartIndex() * nPixelPerGrid;
//    }
//    if ((nStartCell * nPixelPerGrid) < fEndTime)
//    {
//        pDC->MoveTo(0, h0);
//        pDC->LineTo(w, h0);
//    }

//    // draw sample data
//    pDC->SelectObject(pRedPen);
//    for(i = GetCellStartIndex() * nPixelPerGrid; i< nPixelPerGrid * (nCount+1); i++)
//    {
//        if(i > nDrawPointsCount)
//        {
//            break;
//        }
//        y = h0 - (int)(m_pfWaveArray[i]* dy);
//        if(x == 0)
//            pDC->MoveTo(x, y);
//        else
//            pDC->LineTo(x, y);
//        x += dx;
//    }

}
void CUIMusicLayer::DrawBase(/*CDC* pDC*/)
{
//    int m_nCellWidth = m_pLayerManager->GetCellWidth();
//    int m_lineThick = m_pLayerManager->GetLineThick();
//    int m_CellCount = m_pLayerManager->GetCellCount();
//    /*h = 40;*/
//    //画背景
//    int wTmp = (m_nCellWidth + m_lineThick) * m_CellCount ;
//    CRect rt( x, y, x + wTmp, y + h );


//    int xStart, yStart;
//    GetPosition(xStart, yStart);
//    int xTmp = xStart;

//    CRect rc;
//    m_ManagerHandle->GetClientRect(rc);
//    int nCount = rc.Width()/(m_lineThick + m_nCellWidth) + GetCellStartIndex();
//    CRect rect;
//    rect = rc;
//    rect.top = CUILayerCommonImpl::h;
//    rect.bottom = rect.top + h;
//    CPen pen( PS_SOLID, /*this->m_lineThick*/m_pLayerManager->GetLineThick(), this->m_lineColor);
//    pDC->SelectObject(&pen);
//    //pDC->Rectangle(&rect);
//    CBrush bgBrush(RGB(254, 254, 254));
//    rt.right = (m_CellCount - GetCellStartIndex())*(m_nCellWidth + m_lineThick);
//    pDC->FillRect( rt, &bgBrush );
//    for( UINT i = GetCellStartIndex(); i <= nCount; i++ )
//    {
//        if (i > m_CellCount - 1)
//        {
//            break;
//        }
//        //CBrush bgBrush(RGB(54, 54, 54));


//        //画竖线
//        //pDC->SelectObject( &pen );
//        // 		pDC->MoveTo( xTmp, y + (int)h/2);
//        // 		pDC->LineTo( xTmp, y + h );

//        // 		pDC->MoveTo(xTmp, y + h-1);
//        // 		pDC->LineTo(xTmp+m_nCellWidth+m_lineThick, y+h-1);

//        // 		if( i % m_divisor == 0 )
//        // 		{
//        // 			RECT rt;
//        // 			rt.left = xTmp + 1;
//        // 			rt.top = y;
//        // 			rt.right = xTmp + m_nCellWidth;
//        // 			rt.bottom = y + h-1;
//        //
//        // 			CBrush brush(RGB(240, 240, 240));
//        // 			pDC->SelectObject(&brush);
//        // 			pDC->FillRect( &rt, &brush );
//        // 		}

//        xTmp += m_lineThick;
//        xTmp += m_nCellWidth;
//    }
//    rect.right = xTmp;
//    nLayerWidth = xTmp;
//    pDC->Rectangle(&rect);
//    // 	pDC->MoveTo( xTmp, y);
//    // 	pDC->LineTo( xTmp, y + h);
}
int  CUIMusicLayer::GetCellHeight()
{
    return this->h;
}
void CUIMusicLayer::CreateOptionHandler()
{
    if (m_pOptionHandler)
    {
        m_pOptionHandler->ReleaseConnection();
    }

    CreateMusicLayOptionHandler(&m_pOptionHandler);
    m_pOptionHandler->Connect(m_ManagerHandle, this);
}
void CUIMusicLayer::wcharToChar (const WCHAR * tchar, char **_char)
{
//    int iLength = 0;
//    //获取字节长度
//    iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
//    //将tchar值赋给_char
//    *_char = new char[iLength ];
//    memset(*_char,0,iLength);
//    WideCharToMultiByte(CP_ACP, 0, tchar, -1, *_char, iLength, NULL, NULL);
}
