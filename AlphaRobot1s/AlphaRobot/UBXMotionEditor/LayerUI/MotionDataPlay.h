#ifndef _MOTIONDATAPLAY_H_
#define _MOTIONDATAPLAY_H_

#include "IGroupDataPlay.h"
#include "datamodel/ActionBlockData.h"
#include "LayerUI/ILayerUI.h"
#include "datamodel/ActionBlockDataManager.h"
#include "IUBXRobot3DView.h"
#include "cnumautochange.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

//拆分之后的动作帧
typedef QList<CActionBlockData*> LstSplitActionData;
class IUBXRobot3DView;



class CMotionDataPlay: public QThread, public IGroupDataPlay
{
public:
    CMotionDataPlay(void);
    ~CMotionDataPlay(void);
    
protected:
    bool m_bStop;
    bool m_bPause;
    bool m_bHighPrecisionSupported;  //是否支持高精度定时器
    IUBXRobot3DView* m_pRobot3dView;
    int m_nPlayType;
    //	CUILayerCommonImpl* m_LayerCommon;
    int m_ScaleTime;
    CActionBlockGroupManager* m_GroupManager;
    CActionBlockData *m_pActionData;
    IGroupDataPlayEvent* m_Event;
    ILayerUI* m_LayerUI;
    UBXCritSec   m_criSec;
    QMutex m_oMutex;
    QWaitCondition m_oWait;

    emPlayStatus m_eStopPlayType; // 动作结束播放方式，包括暂停播放、停止播放、正常播放结束

    int m_nGroupIndex;             //当前播放的组索引
    int m_nActionIndex;            //当前组中的动作索引
    int m_nOffsetTime;             //相对于当前动作偏移的索引

    int m_nServoCount;             //舵机数量

    CNumAutoChange m_numAutoChange;
    bool m_bCanRecNumChangeSig;

private:
    void OnPlaying(CActionBlockData* pActionBlock , int nOffsetTime = 0);
    void OnStopPlay(emPlayStatus ePlayStatus);
    void OnSingleGroupPlaying();
    void OnAllDataPlaying();
    void OnGroupDataPlaying(CActionBlockData *pStartActionData);
    void OnAllDataPlaying(CActionBlockData *pStartActionData);
    void OnSingleDataPLaying(CActionBlockData *pStartActionData , CActionBlockData *pDstActionData);
    
    //对两个动作ActionData拆分，拆分成若干个单个运行时间不超过5000ms的帧，且保证其角度连续，并依次放在LstSplitActionData列表中
    void SplitActionData(CActionBlockData* pStartActionData,CActionBlockData* pEndActionData,
                         LstSplitActionData* pDatalst,float fOffsetTime);
    //函数作用，当机器人从处于pStartActionData的状态运动到pDstActionData的动作块的时候，
    //计算两者之间持续时间中处于nTime时刻的状态运动块，并返回新的动作块值
    CActionBlockData* CaculateActionData(CActionBlockData *pStartActionData , CActionBlockData *pDstActionData,int nTime);
    void SetMotorAngle(char* pData, int nLen, int nID, float fAngle);

    bool isPauseOrStop();

public:
    //IGroupDataPlay 接口重写
    virtual void StartPlay(CActionBlockGroupManager* pActionData, IUBXRobot3DView* PlayHandle);
    virtual void StartBlockPlay(CActionBlockData* pActionData, IUBXRobot3DView* PlayHandle) ;
    virtual void PausePlay();
    virtual void StopPlay();
    virtual bool Connect(ILayerUI* pDataManager, int nPlayType, int nScaleTime, IGroupDataPlayEvent* pEvent);
    virtual void ReleaseConnection();
    virtual void SetCurrentPos(int nGroupIndex,int nActionIndex,int nOffsetTime);


public slots:
    void onSigNumChange(int nOld, int nNew);

protected:
    virtual void run();

};
#endif  //_MOTIONDATAPLAY_H_
