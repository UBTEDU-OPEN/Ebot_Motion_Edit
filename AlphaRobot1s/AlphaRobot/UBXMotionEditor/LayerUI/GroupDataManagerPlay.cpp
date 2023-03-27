#include "stable.h"
#include "GroupDataManagerPlay.h"
#include "ILayerUI.h"
#include "ActionBlockDataManager.h"
#include "MotionDataPlay.h"
// #include "EarsLedDataPlay.h"
// #include "EyesLedDataPlay.h"
#include "MusicDataPlay.h"

bool CreateIGroupDataManagerPlay(IGroupDataManagerPlay** ppManagerPlay)
{
	CGroupDataManagerPlay* pProxy = new CGroupDataManagerPlay();
	if (pProxy)
	{
		*ppManagerPlay = pProxy;

		return true;
	}

	return false;
}

CGroupDataManagerPlay::CGroupDataManagerPlay(void)
    : m_bSinglePlay(true)
	, m_pGroupDataPlay(NULL)
{
}


CGroupDataManagerPlay::~CGroupDataManagerPlay(void)
{
}

bool CGroupDataManagerPlay::Connectoin(ILayerUI* pDataManager, IGroupDataManagerPlay_Event* pEvent, void* PlayHandle, bool bSingleLayer)
{
	m_bStop = false;
	
	m_PlayHandle = PlayHandle;
	m_pLayer = pDataManager;
	m_pEvent = pEvent;
	m_bSinglePlay = bSingleLayer;
	m_nCurrentPlayIndex = 0;

	return true;
}

void CGroupDataManagerPlay::ReleaseConnection()
{
	if (m_pGroupDataPlay)
	{
		m_pGroupDataPlay->ReleaseConnection();
		m_pGroupDataPlay = NULL;
	}

	m_pCurrentPlayGroupManager = NULL;
	m_bStop = true;
//    WaitForStop();

	delete this;
}

void CGroupDataManagerPlay::ReSetState()
{
	m_pCurrentPlayGroupManager = NULL;
	m_bStop = true;
	m_nCurrentConsumeTime = 0;
	m_nCurrentPlayIndex = 0;
	m_bIsCompleted = false;
//    WaitForStop();

	m_bStop = false;
}

void CGroupDataManagerPlay::ThreadProcMain()
{
	if (m_bSinglePlay == false)
		return;


// 	if (GetNextGroupPlay())
// 	{
// 		if (m_nCurrentConsumeTime == m_nGroupStartTime)
// 		{
// 			if (m_pGroupDataPlay)
// 			{
// 				m_pGroupDataPlay->StartPlay(m_nGroupEndTime-m_nGroupStartTime, m_PlayHandle);
// 			}
// 		}
// 	}

	while (m_bStop == false)
	{
		if (m_bIsCompleted)
			break;

//		Sleep(m_pLayer->GetScaleValue());
		OnPlaying(1);
	}
}

void CGroupDataManagerPlay::OnPlaying(int nOffsetTime)
{
	m_nCurrentConsumeTime += nOffsetTime;

	if (m_nCurrentConsumeTime  == m_nGroupStartTime)
	{
        if (m_pGroupDataPlay)
        {
//            m_pGroupDataPlay->StartPlay(m_nGroupEndTime-m_nGroupStartTime, m_PlayHandle);
        }
	}

	if (m_nCurrentConsumeTime == m_nGroupEndTime)
	{
		GetNextGroupPlay();
		return;
	}

	if (m_nCurrentConsumeTime >= m_nGroupStartTime &&
		m_nCurrentConsumeTime < m_nGroupEndTime)
	{
		if (m_pCurrentPlayGroupManager)
		{
			if (m_pGroupDataPlay)
			{
//				m_pGroupDataPlay->OnPlaying(nOffsetTime);
			}
		}
	}
}

void CGroupDataManagerPlay::StartPlay()
{
	ReSetState();

	if (GetNextGroupPlay())
	{
		if (m_nCurrentConsumeTime == m_nGroupStartTime)
		{
			if (m_pGroupDataPlay)
			{
//				m_pGroupDataPlay->StartPlay(m_nGroupEndTime-m_nGroupStartTime, m_PlayHandle);
			}
		}
	}

	m_bStop = false;
//	if (m_bSinglePlay)
//		StartThread();
}

bool CGroupDataManagerPlay::GetNextGroupPlay()
{
	if (m_pLayer == NULL ||m_pLayer->GetBlockDataManager()->GetCount() == 0)
	{
		m_bIsCompleted = true;
		m_pEvent->LayerDebugStop(m_pLayer->GetType());
		if (m_pGroupDataPlay)
		{
			m_pGroupDataPlay->StopPlay();
		}
		return false;
	}

	if (m_nCurrentPlayIndex >= m_pLayer->GetBlockDataManager()->GetCount()){
		m_pCurrentPlayGroupManager = NULL;
        m_bIsCompleted = true;
		m_pEvent->LayerDebugStop(m_pLayer->GetType());

		if (m_pGroupDataPlay)
		{
			m_pGroupDataPlay->StopPlay();
		}
		return false;
	}

	m_pCurrentPlayGroupManager = m_pLayer->GetBlockDataManager()->GetAt(m_nCurrentPlayIndex++);
	m_nGroupEndTime = m_pCurrentPlayGroupManager->GetEndTime();
	m_nGroupStartTime = m_pCurrentPlayGroupManager->GetStartTime();

	if (m_pGroupDataPlay != NULL)
	{
		m_pGroupDataPlay->ReleaseConnection();
	}

	if (m_pLayer->GetType() == ACTION_LAYER_MOTION)
	{
//		m_pGroupDataPlay = new CMotionDataPlay();
	}

// 	if (m_pLayer->GetType() == ACTION_LAYER_EAR)
// 	{
// 		m_pGroupDataPlay = new CEarsLedDataPlay();
// 	}
// 
// 	if (m_pLayer->GetType() == ACTION_LAYER_EYE)
// 	{
// 		m_pGroupDataPlay = new CEyesLedDataPlay();
// 	}

	if (m_pLayer->GetType() == ACTION_LAYER_MUSIC)
	{
//		m_pGroupDataPlay = new CMusicDataPlay();
	}

	if (m_pGroupDataPlay)
	{
//		m_pGroupDataPlay->Connect(m_pCurrentPlayGroupManager, false, m_pLayer->GetScaleValue(), NULL);
//		m_pGroupDataPlay->IGroupDataPlay_SetSocketSession(m_pSocketSession);
    }

    return true;
}

void CGroupDataManagerPlay::StopPlay()
{
	if (m_pGroupDataPlay)
	{
		m_pGroupDataPlay->StopPlay();
		m_pEvent->LayerDebugStop(m_pLayer->GetType());		
	}

	ReSetState();
}

void CGroupDataManagerPlay::IGroupDataManagerPlay_SetSocketSession(CIUBXSocketSession* pSession)
{
	m_pSocketSession = pSession;
}
