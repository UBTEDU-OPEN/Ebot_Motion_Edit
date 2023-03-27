#pragma once

class CIUBXSocketSession;
class ILayerUI;
class IGroupDataManagerPlay_Event
{
public:
	virtual void LayerDebugStop(int layerType) = 0;

};
/************************************************************************/
/* 层数据播放接口                                                                     */
/************************************************************************/
class IGroupDataManagerPlay
{
public: 
    virtual bool Connectoin(ILayerUI* pDataManager, IGroupDataManagerPlay_Event* pEvent, void* PlayHandle, bool bSingleLayer) = 0;
	virtual void ReleaseConnection() = 0;

	// 开始播放
	virtual void StartPlay() = 0;
	// 正在播放的时候步进值，每过一个时间粒度调用一次
	virtual void OnPlaying(int nOffsetTime) = 0;
	// 停止播放
	virtual void StopPlay() = 0;

	virtual void IGroupDataManagerPlay_SetSocketSession(CIUBXSocketSession* pSession) = 0;
};

bool CreateIGroupDataManagerPlay(IGroupDataManagerPlay** ppManagerPlay);
