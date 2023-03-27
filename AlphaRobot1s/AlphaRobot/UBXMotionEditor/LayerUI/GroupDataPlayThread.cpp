#include "stable.h"
#include "GroupDataPlayThread.h"
#include "ActionBlockGroupManager.h"
#include "ActionBlockData.h"


CGroupDataPlayThread::CGroupDataPlayThread(void)
: m_pGroupManager(NULL)
, m_pProcess(NULL)
, m_pSocketSession(NULL)
, m_LayerCommon(NULL)
{
}


CGroupDataPlayThread::~CGroupDataPlayThread(void)
{
}

void CGroupDataPlayThread::ThreadProcMain()
{
    // 	if (m_pProcess)
    // 	{
    // 		m_pProcess->ProcessPlayActionBlock(m_pCurrentActionBlockPlay, m_pCurrentActionBlockPlay->GetStartTime()*m_nScaleTime, m_nScaleTime);
    // 	}

    if (m_bSinglePlay == false)
        return;
    
    while (m_bStop == false)
    {
        if (m_bIsCompleted)
            break;

        //		Sleep(m_nScaleTime);
        OnPlaying(1);
    }
}

bool CGroupDataPlayThread::Connect(CActionBlockGroupManager* pGroupManager, bool bSinglePlay, int nScaleTime, CUILayerCommonImpl* LayerCommon)
{
    m_bStop = false;
    m_bSinglePlay = bSinglePlay;
    m_pGroupManager = pGroupManager;
    m_bIsCompleted = false;
    m_nScaleTime = nScaleTime;
    
    m_LayerCommon = LayerCommon;
    
    return true;
}

void CGroupDataPlayThread::ReleaseConnection()
{
    m_bStop = true;
    //    WaitForStop();

    delete this;
}

void CGroupDataPlayThread::StartPlay(int nEndTime, void* PlayHandle)
{
    //	m_nTotalTime = nEndTime;
    
    //	// 如果当前组没有数据
    ////	if (m_pGroupManager->GetCount() == 0)
    ////	{
    ////		m_bIsCompleted = true;
    ////		StopPlay();
    ////		return;
    ////	}
    
    //	m_pCurrentActionBlockPlay = m_pGroupManager->GetAt(0);
    //	m_nPastIndex = 0;
    //	m_nCurrentTime = 0;
    //	m_nCurrentCompareTime = m_pCurrentActionBlockPlay->GetStartTime()+m_pCurrentActionBlockPlay->GetEndTime();
    
    //	if (m_pProcess)
    //	{
    //		m_pProcess->ProcessPlayActionBlock(m_pCurrentActionBlockPlay, m_pCurrentActionBlockPlay->GetStartTime()*m_nScaleTime, m_nScaleTime);
    //	}
    
    //	if (m_bSinglePlay)
    //	{
    //		StartThread();
    //	} else {
    //	}
}

void CGroupDataPlayThread::SetProcessHander(CActionBlockProcess* pProcess)
{
    m_pProcess = pProcess;
}

void CGroupDataPlayThread::StopPlay()
{
    //	m_bStop = TRUE;
    //	if (m_LayerCommon)
    //	{
    //		m_LayerCommon->OnDebugStop();
    //		m_LayerCommon->SetLayerMenuState(FALSE);
    //	}
    
    //	if (m_pProcess)
    //	{
    //		m_pProcess->OnStopPlay();
    //	}
}

void CGroupDataPlayThread::OnPlaying(int nOffset)
{
    //	if (m_nCurrentTime >= m_nCurrentCompareTime)
    //	{
    //		if (m_nPastIndex >= m_pGroupManager->GetCount()-1)
    //		{
    //			// 所有块都已经执行完
    //			m_bIsCompleted = true;
    //			StopPlay();
    //			return;
    //		}
    //		else
    //		{
    //			// 执行下一块数据
    //			m_pCurrentActionBlockPlay = m_pGroupManager->GetAt(++m_nPastIndex);
    //			m_nCurrentTime = 0;
    //			m_nCurrentCompareTime = m_pCurrentActionBlockPlay->GetStartTime()+m_pCurrentActionBlockPlay->GetEndTime();
    //			if (m_pProcess)
    //			{
    //				m_pProcess->ProcessPlayActionBlock(m_pCurrentActionBlockPlay, m_pCurrentActionBlockPlay->GetStartTime()*m_nScaleTime, m_nScaleTime);
    //			}
    //		}
    //	}
    
    //	m_nCurrentTime += nOffset;
}

void CGroupDataPlayThread::IGroupDataPlay_SetSocketSession(CIUBXSocketSession* pSession)
{
    m_pSocketSession = pSession;
}
