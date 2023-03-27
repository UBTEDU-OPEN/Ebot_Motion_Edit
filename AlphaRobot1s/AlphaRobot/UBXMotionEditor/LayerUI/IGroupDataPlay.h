#ifndef IGROUPDATA_PLAY_H
#define IGROUPDATA_PLAY_H

/************************************************************************/
/* 组数据播放接口                                                                     */
/************************************************************************/
#define PLAYDEBUG_ALL 0
#define PLAYDEBUG_GROUP 1
#define PLAYDEBUG_BLOCK 2
//class CIUBXSocketSession;
class CActionBlockGroupManager;
class CActionBlockData;
//class CUILayerCommonImpl;
class ILayerUI;
class IUBXRobot3DView;

class IGroupDataPlayEvent
{
public:
        virtual void PlayEvent_OnDebugStop(int nLayerType, int nPlayStatus) = 0;
//	virtual void PlayEvent_SetLayerMenuState(bool menuState) = 0;
        virtual void PlayEvent_OnSetPos(int nCol){}
        virtual void PlayEvent_OnSetPosChangeToOptions(int nLayerType , int nCol){}
        virtual void PlayEvent_OnEnableCtrl(int nLayerType , bool bState){}
        virtual void PlayEvent_SetDebugType(int nDebugTyoe){}
        virtual int  PlayEvent_GetDebugType(){return -1;}
};

class IGroupDataPlay
{
public:
    //nplayType 0完全仿真 1单块仿真（Group） 2单帧仿真
    virtual bool Connect(ILayerUI* pDataManager, int nPlayType, int nScaleTime, IGroupDataPlayEvent* pEvent ) = 0;
    virtual void ReleaseConnection() = 0;
    virtual void StartPlay(CActionBlockGroupManager* pGroupManager, IUBXRobot3DView* PlayHandle) = 0;
    //virtual void StartPlay(int  nEndTime, IUBXRobot3DView* PlayHandle) = 0;

    // 动作层单帧仿真
    virtual void StartBlockPlay(CActionBlockData* pActionData, IUBXRobot3DView* PlayHandle) {}
    virtual void PausePlay() = 0;
    virtual void StopPlay() = 0;
    virtual void OnSetPlayTime(int nStartTime , int nEndTime) {}
    virtual void SetCurrentPos(int nGroupIndex,int nActionIndex,int nOffsetTime){}
};

bool CreateGroupDataPlayer(IGroupDataPlay** ppGroupPlay);
bool CreateMusicDataPlayer(IGroupDataPlay** ppGroupPlay);
#endif
