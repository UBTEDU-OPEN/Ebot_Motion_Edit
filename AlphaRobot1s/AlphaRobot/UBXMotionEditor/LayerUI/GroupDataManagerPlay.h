#pragma once

#include "IGroupDataManagerPlay.h"
#include "IGroupDataPlay.h"
#include <QThread>

class CActionBlockGroupManager;
class CGroupDataManagerPlay :public QThread, public IGroupDataManagerPlay
{
public:
	CGroupDataManagerPlay(void);
	~CGroupDataManagerPlay(void);

public:
	// IGroupDataManagerPlay接口实现
    virtual bool Connectoin(ILayerUI* pDataManager, IGroupDataManagerPlay_Event* pEvent, void* PlayHandle, bool bSingleLayer);
	virtual void ReleaseConnection();
	virtual void StartPlay();
	// 正在播放的时候步进值
	virtual void OnPlaying(int nOffsetTime);
	virtual void StopPlay();

	virtual void IGroupDataManagerPlay_SetSocketSession(CIUBXSocketSession* pSession);

private:
    bool m_bStop;
	// 是否为只播放一层，用于仿真单层
	bool m_bSinglePlay;

	ILayerUI* m_pLayer;
	IGroupDataManagerPlay_Event* m_pEvent;

	// 当前播放的组数据
	CActionBlockGroupManager* m_pCurrentPlayGroupManager;
	// 当前播放的序号（在列表中的序号）
	int m_nCurrentPlayIndex;
	// 当前已经执行过的时间
	int m_nCurrentConsumeTime;
	// 块的结束时间
	int m_nGroupEndTime;
	// 组开始时间
	int m_nGroupStartTime;
	// 是否已经执行完
    bool m_bIsCompleted;

	IGroupDataPlay*	m_pGroupDataPlay;

	void* m_PlayHandle;

	CIUBXSocketSession*	 m_pSocketSession;

    bool GetNextGroupPlay();
	void ReSetState();

public:
	virtual void ThreadProcMain();
};

