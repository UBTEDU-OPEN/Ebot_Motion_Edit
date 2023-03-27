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
	// IGroupDataManagerPlay�ӿ�ʵ��
    virtual bool Connectoin(ILayerUI* pDataManager, IGroupDataManagerPlay_Event* pEvent, void* PlayHandle, bool bSingleLayer);
	virtual void ReleaseConnection();
	virtual void StartPlay();
	// ���ڲ��ŵ�ʱ�򲽽�ֵ
	virtual void OnPlaying(int nOffsetTime);
	virtual void StopPlay();

	virtual void IGroupDataManagerPlay_SetSocketSession(CIUBXSocketSession* pSession);

private:
    bool m_bStop;
	// �Ƿ�Ϊֻ����һ�㣬���ڷ��浥��
	bool m_bSinglePlay;

	ILayerUI* m_pLayer;
	IGroupDataManagerPlay_Event* m_pEvent;

	// ��ǰ���ŵ�������
	CActionBlockGroupManager* m_pCurrentPlayGroupManager;
	// ��ǰ���ŵ���ţ����б��е���ţ�
	int m_nCurrentPlayIndex;
	// ��ǰ�Ѿ�ִ�й���ʱ��
	int m_nCurrentConsumeTime;
	// ��Ľ���ʱ��
	int m_nGroupEndTime;
	// �鿪ʼʱ��
	int m_nGroupStartTime;
	// �Ƿ��Ѿ�ִ����
    bool m_bIsCompleted;

	IGroupDataPlay*	m_pGroupDataPlay;

	void* m_PlayHandle;

	CIUBXSocketSession*	 m_pSocketSession;

    bool GetNextGroupPlay();
	void ReSetState();

public:
	virtual void ThreadProcMain();
};

