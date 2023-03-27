#include "stable.h"
#include "MusicDataPlay.h"
#include "UIMusicLayer.h"
#include "UITimeLayer.h"

bool CreateMusicDataPlayer(IGroupDataPlay** ppGroupPlay)
{
	CMusicDataPlay* pProxy = new CMusicDataPlay();
	if (pProxy)
	{
		*ppGroupPlay = pProxy;
        return true;
	}

    return false;
}


CMusicDataPlay::CMusicDataPlay(void)
{
    m_bStop = false;
	m_pRobot3dView = NULL;
    m_bSinglePlay = true;
	m_ScaleTime = 500;
	m_Event = NULL;
	m_nCurrentPos = 0;
	m_LayerUI = NULL;
	m_nPlayState = 0;
	m_nActStartTime = 0;
	m_nActEndTime = 0;
	m_nDebugType = 1; //单层仿真（非全局联合仿真）
}


CMusicDataPlay::~CMusicDataPlay(void)
{

}
void CMusicDataPlay::PausePlay(CActionBlockGroupManager* pGroupManager, IUBXRobot3DView* PlayHandle)
{
	m_nPlayState = 1;
	if (m_LayerUI)
	{
		((CUIMusicLayer*)m_LayerUI)->PlayMusic(m_nActStartTime,m_nActEndTime ,m_nPlayState);
		
	}
	
}
void CMusicDataPlay::OnSetPlayTime(int nStartTime , int nEndTime)
{
	m_nActStartTime = nStartTime;
	m_nActEndTime = nEndTime;

}
void CMusicDataPlay::ThreadProcMain()
{
    bool bPlaying = false;  //是否开始播放
	int nActStartTime = m_nActStartTime;
	int nActEndTime = m_nActEndTime;
    while (m_bStop == false)
	{		
		if (m_nPlayState != 2)
		{
            sleep(100);
			continue;
		}
		if (m_LayerUI)
		{
			int nPos = ((CUIMusicLayer*)m_LayerUI)->GetCurrentPos();
			if (nPos == -1)
			{
				break;
			}
			nActStartTime = nPos;
			nPos = nPos / m_ScaleTime;
			if ((nPos >= 0) && m_Event)
			{
				int nCell = ((CUIMusicLayer*)m_LayerUI)->GetScreenCellCount();
				nCell *= ((CUIMusicLayer*)m_LayerUI)->GetCellWidth() + ((CUIMusicLayer*)m_LayerUI)->GetLineThick();
				if (m_Event )
				{
					if (nPos == 0  && m_nActStartTime != 0)
					{
						m_Event->PlayEvent_OnSetPos(m_nActStartTime);
					}
					else
					m_Event->PlayEvent_OnSetPos(nPos);
				}
			}
			if (nPos > 0 )
			{
                bPlaying = true;
			}
			if (bPlaying && nPos == 0)
			{
                bPlaying = false;
				break;
			}
		}
		int nSleepTime = (m_ScaleTime - 100 > 100)?(m_ScaleTime -100):100;
        sleep(nSleepTime);
	}
	if (m_Event)
	{
		m_Event->PlayEvent_SetDebugType(SINGLE_LAYER_DEBUG);
	}
	OnStopPlay();
}

bool CMusicDataPlay::Connect(ILayerUI* pDataManager,int nPlayType, int nScaleTime, IGroupDataPlayEvent* pEvent)
{
	m_LayerUI = pDataManager;
	m_bSinglePlay = nPlayType;
	m_Event = pEvent;
	m_ScaleTime = nScaleTime;
    m_bStop = false;
	m_nPlayState = 0;
	//m_nDebugType = nDebugType;
    return true;
}

void CMusicDataPlay::ReleaseConnection()
{
	StopPlay();
	//OnStopPlay();
	/*WaitForStop();*/
	delete this;
}

//void CMusicDataPlay::OnPlaying(CActionBlockData* pActionBlock)
//{
//// 	char* pData = NULL;
//// 	int nLen = 0;
//// 
//// 	pActionBlock->GetExtBlockData(&pData, nLen);
//// 	int runTime = ceil(pActionBlock->GetEndTime()*m_ScaleTime);
//// 	m_pRobot3dView->SetMotorDatas(pData, nLen);
//// 	m_pRobot3dView->SetAllMotorAngle(pData, nLen, runTime);
//// 
//// 	delete []pData;
//}

 void CMusicDataPlay::OnStopPlay()
 {
	 m_nPlayState = 0;
	 
	 if (m_Event)
	 {
	 	m_Event->PlayEvent_OnDebugStop(ACTION_LAYER_MUSIC);
	 }
	 
 }

void CMusicDataPlay::StartPlay(/*CActionBlockGroupManager* pGroupManager*/int nEndTime, IUBXRobot3DView* PlayHandle)
{
// 	m_pRobot3dView = PlayHandle;
//	m_GroupManager = pGroupManager;
//// 	if (m_nPlayState == 0)
//// 	{
//// 		m_nActStartTime = ((CUIMusicLayer*)m_LayerUI)->GetActStartTime();
//// 		m_nActEndTime = ((CUIMusicLayer*)m_LayerUI)->GetActEndTime();
//// 	}
//	if (m_LayerUI)
//	{
//		m_nPlayState = 2;  // 2 Means Start playing music
//		((CUIMusicLayer*)m_LayerUI)->PlayMusic(m_nActStartTime , m_nActEndTime , m_nPlayState );
		
//	}
//	m_bStop = false;
////	WaitForStop();
////	StartThread();
}
// void CMusicDataPlay::PlayMusic()
// {
// 	if (m_LayerUI)
// 	{
// 		((CUIMusicLayer*)m_LayerUI)->PlayMusic();
// 		m_nPlayState = 2;
// 	}
// 	
// 	StartThread();
// }
void CMusicDataPlay::StopPlay()
{
//	m_bStop = true;
//	m_nPlayState = 0;
//	if (m_LayerUI)
//	{
//		((CUIMusicLayer*)m_LayerUI)->PlayMusic(m_nActStartTime, m_nActEndTime,m_nPlayState);
//	}
//	WaitForStop();
//	m_nDebugType = 1;
}
