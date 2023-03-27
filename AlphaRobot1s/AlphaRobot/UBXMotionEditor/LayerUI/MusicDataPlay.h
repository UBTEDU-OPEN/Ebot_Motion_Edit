#pragma once
#include "GroupDataPlayThread.h"
#include "ActionBlockData.h"

class CMusicDataPlay: public QThread, public IGroupDataPlay
{
public:
	CMusicDataPlay(void);
	~CMusicDataPlay(void);

protected:
    bool m_bStop;
    IUBXRobot3DView* m_pRobot3dView;
    bool m_bSinglePlay;
	int  m_nPlayState;
	int  m_nCurrentPos;
	int  m_nActStartTime;
	int  m_nActEndTime;
//	CUILayerCommonImpl* m_LayerCommon;
	int m_ScaleTime;
	CActionBlockGroupManager* m_GroupManager;
	IGroupDataPlayEvent* m_Event;
	ILayerUI* m_LayerUI;
	int m_nDebugType;

private:
	//void OnPlaying(CActionBlockData* pActionBlock);
	void OnStopPlay();
	void SetDebugType(int nDebugType)
	{
		m_nDebugType = nDebugType;
	}
	//void PlayMusic();

public:
	//CGroupDataPlayThread 接口重写
    virtual void StartPlay(int nEndTime, IUBXRobot3DView* PlayHandle);
    virtual void PausePlay(CActionBlockGroupManager* pGroupManager, IUBXRobot3DView* PlayHandle);
	virtual void OnSetPlayTime(int nStartTime , int nEndTime);
public:
	// CActionBlockProcess 接口实现
//	virtual void ProcessPlayActionBlock(CActionBlockData* pActionBlock, int nTotalTime, int TimeScale);
	virtual void StopPlay();
    virtual bool Connect(ILayerUI* pDataManager, int nPlayType, int nScaleTime, IGroupDataPlayEvent* pEvent);
	virtual void ReleaseConnection();
	virtual void ThreadProcMain();

};

