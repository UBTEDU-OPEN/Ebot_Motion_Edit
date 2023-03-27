#pragma once

class CIUBXSocketSession;
class ILayerUI;
class IGroupDataManagerPlay_Event
{
public:
	virtual void LayerDebugStop(int layerType) = 0;

};
/************************************************************************/
/* �����ݲ��Žӿ�                                                                     */
/************************************************************************/
class IGroupDataManagerPlay
{
public: 
    virtual bool Connectoin(ILayerUI* pDataManager, IGroupDataManagerPlay_Event* pEvent, void* PlayHandle, bool bSingleLayer) = 0;
	virtual void ReleaseConnection() = 0;

	// ��ʼ����
	virtual void StartPlay() = 0;
	// ���ڲ��ŵ�ʱ�򲽽�ֵ��ÿ��һ��ʱ�����ȵ���һ��
	virtual void OnPlaying(int nOffsetTime) = 0;
	// ֹͣ����
	virtual void StopPlay() = 0;

	virtual void IGroupDataManagerPlay_SetSocketSession(CIUBXSocketSession* pSession) = 0;
};

bool CreateIGroupDataManagerPlay(IGroupDataManagerPlay** ppManagerPlay);
