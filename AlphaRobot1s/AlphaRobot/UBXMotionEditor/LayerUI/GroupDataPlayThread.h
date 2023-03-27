#pragma once

#include "IGroupDataPlay.h"
#include "UILayerCommonImpl.h"
#include <QThread>

class CActionBlockData;
/************************************************************************/
/* 播放处理类                                                                     */
/************************************************************************/
class CActionBlockProcess
{
public:
    virtual void ProcessPlayActionBlock(CActionBlockData* pActionBlock, int nTotalTime, int TimeScale) = 0;
    virtual void OnStopPlay() = 0;
    virtual void StopPlay() = 0;
};

class CActionBlockData;
class CGroupDataPlayThread : public QThread, public IGroupDataPlay
{
public:
    CGroupDataPlayThread(void);
    ~CGroupDataPlayThread(void);

    virtual void SetProcessHander(CActionBlockProcess* pProcess);

public:
    // IGroupDataPlay接口实现
    virtual bool Connect(CActionBlockGroupManager* pGroupManager, bool bSinglePlay, int nScaleTime, CUILayerCommonImpl* LayerCommon);
    virtual void ReleaseConnection();
    virtual void StartPlay(int nEndTime, void* PlayHandle);
    virtual void StopPlay();
    virtual void OnPlaying(int nOffset);
    virtual void IGroupDataPlay_SetSocketSession(CIUBXSocketSession* pSession);

private:
    bool m_bStop;
    bool m_bSinglePlay;

    CActionBlockGroupManager* m_pGroupManager;
    // 当前运行的总时间
    int	m_nCurrentTime;
    // 是否需要播放下一块的比较时间
    int m_nCurrentCompareTime;
    // 当前块的总时间
    int m_nTotalTime;
    // 当前暂停的时间
    int m_nPauseTime;
    // 当前播放的动作块
    CActionBlockData* m_pCurrentActionBlockPlay;
    // 是否已经完成
    bool m_bIsCompleted;
    // 已经执行过的块
    int m_nPastIndex;
    // 时间粒度
    int m_nCurrentScaleTime;
    CActionBlockProcess* m_pProcess;
    CUILayerCommonImpl* m_LayerCommon;
    int m_nScaleTime;

protected:
    CIUBXSocketSession* m_pSocketSession;
public:
    virtual void ThreadProcMain();
};

