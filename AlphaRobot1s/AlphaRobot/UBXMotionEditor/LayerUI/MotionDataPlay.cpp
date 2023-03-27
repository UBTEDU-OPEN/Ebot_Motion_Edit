#include "stable.h"
#include "MotionDataPlay.h"
#include <QElapsedTimer>
#include "UBXPublic.h"


bool CreateGroupDataPlayer(IGroupDataPlay** ppGroupPlay)
{
    CMotionDataPlay* pProxy = new CMotionDataPlay();
    if (pProxy)
    {
        *ppGroupPlay = pProxy;
        return true;
    }
    
    return false;
}


CMotionDataPlay::CMotionDataPlay(void)
{
    m_bStop = false;
    m_bPause = false;
    m_pRobot3dView = NULL;
    m_nPlayType = false;
    m_ScaleTime = 500;
    m_eStopPlayType = ePlayStatusFinished; // 正常播放结束
    m_Event = NULL;
    m_LayerUI = NULL;
    m_bHighPrecisionSupported = false;
    m_nGroupIndex = 0;
    m_nActionIndex = 0;
    m_nOffsetTime = 0;
    m_pActionData = NULL;
    m_GroupManager = NULL;
    m_nServoCount = getMotorCount();

    m_numAutoChange.setLoopTime(1);
    m_bCanRecNumChangeSig = false;
    connect(&m_numAutoChange, &CNumAutoChange::sigNumChange, this, &CMotionDataPlay::onSigNumChange, Qt::DirectConnection);
}


CMotionDataPlay::~CMotionDataPlay(void)
{
    quit();
    wait(1000);
}
void CMotionDataPlay::SetMotorAngle(char* pData, int nLen, int nID, float fAngle)
{
    int nCount = (nLen-4)/sizeof(MOTOR_DATA);
    MOTOR_DATA* p = (MOTOR_DATA*)(pData+4);
    if (nID >= nCount)
    {
        return;
    }
    p[nID].nAngle = (int)fAngle;
}

bool CMotionDataPlay::isPauseOrStop()
{
    if (m_bStop || m_bPause)
    {
        return true;
    }

    return false;
}
CActionBlockData* CMotionDataPlay::CaculateActionData(CActionBlockData *pStartActionData ,
                                                      CActionBlockData *pDstActionData,int nTime)
{
    int nServoCnt = m_nServoCount;
    if (pStartActionData == NULL || pDstActionData == NULL || nTime < 0)
    {
        return NULL;
    }
    int nRunTime = qCeil(pDstActionData->GetRunTime() * m_ScaleTime);
    CActionBlockData *pTempBlockData = NULL;
    pTempBlockData = new CActionBlockData();
    char *pData = NULL;
    int nLen = 0;

    WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
    memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
    int nDescriptionLen = QStringToWCHAR(wstrDescription, "Action");
    //wstrDescription[nDescriptionLen-1] ='\0';
    pTempBlockData->SetDescription(wstrDescription);

    pTempBlockData->SetID(-1);
    pStartActionData->GetExtBlockData(&pData,nLen);
    if (pData && nLen)
    {
        pTempBlockData->SetExtBlockData(nLen,pData);
    }
    float fRatio = (float)nTime / nRunTime;
    if (fRatio > 1)
    {
        fRatio = 1;
    }
    for (int i = 0;i<nServoCnt;i++)
    {
        float fStartAngle = pStartActionData->GetMotorAngle(i + 1);
        float fEndEngle = pDstActionData->GetMotorAngle(i + 1);
        float fCurrentAngle = fStartAngle + fRatio *(fEndEngle - fStartAngle);
        fCurrentAngle += 0.5;  //四舍五入
        SetMotorAngle(pData,nLen,i,fCurrentAngle);
    }
    if (pData && nLen)
    {
        pTempBlockData->SetExtBlockData(nLen,pData);
        delete []pData;
        pData = NULL;
    }
    return pTempBlockData;
}
void CMotionDataPlay::OnSingleDataPLaying(CActionBlockData *pStartActionData , CActionBlockData *pDstActionData)
{
    //  大于5000ms的帧需要拆解发送以适应舵机
    int nServoCnt = m_nServoCount;
    if (pDstActionData && (!isPauseOrStop()) /*&& pStartActionData*/)
    {
        int nRunTime = qCeil(pDstActionData->GetRunTime()*m_ScaleTime);//目标真实运动时间
        int nLoopTimes = 1;
        if (nRunTime % MOTOR_MAX_MOVING_TIME != 0)
            nLoopTimes = nRunTime / MOTOR_MAX_MOVING_TIME + 1;
        else
            nLoopTimes = nRunTime / MOTOR_MAX_MOVING_TIME ;
        int nTimeSum = 0;
        for (int i = 0; i<nLoopTimes; i++)
        {
            if (isPauseOrStop())
            {
                break;
            }
            int nLoopRunTime = 0;
            if (i == nLoopTimes - 1)//最后一帧
            {
                nLoopRunTime = nRunTime - nTimeSum;
            }
            else if ( i== nLoopTimes - 2)//倒数第二帧
            {
                nLoopRunTime = (nRunTime - i * MOTOR_MAX_MOVING_TIME)/2;
            }
            else
            {
                nLoopRunTime = MOTOR_MAX_MOVING_TIME;
            }
            nTimeSum += nLoopRunTime;
            CActionBlockData *pTempBlockData = CaculateActionData(pStartActionData,pDstActionData,nTimeSum);
            pTempBlockData->SetRunTime(nLoopRunTime / m_ScaleTime);
            pTempBlockData->SetAllTime(nLoopRunTime / m_ScaleTime);
            if (i == nLoopTimes - 1)//最后一帧
            {
                pTempBlockData->SetAllTime(pTempBlockData->GetRunTime()
                                           + (pDstActionData->GetAllTime() - pDstActionData->GetRunTime()));//最后一帧设置总时间
            }
            m_oMutex.lock();
            OnPlaying(pTempBlockData);
            m_oWait.wait(&m_oMutex, nLoopRunTime);
            m_oMutex.unlock();
            if (pTempBlockData)
            {
                delete pTempBlockData;
                pTempBlockData = NULL;
            }
        }
    }
}
void CMotionDataPlay::SplitActionData(CActionBlockData* pStartActionData,
                                      CActionBlockData* pEndActionData,LstSplitActionData* pDatalst,float fOffsetTime)
{
    // 分拆大于5000ms的动作帧规则：
    // 动作帧总时间按照5000ms为一份进行划分，直到最后剩下处于5000ms到10000ms的部分按照时间除以2平分
    // 如：单帧时间为17000ms，划分为4帧，时间分别为：5000ms,5000ms,3500ms,3500ms
    //    单帧时间为5100ms，划分为2帧，时间分别为：2550ms,2550ms
    if (pStartActionData == NULL || (ceil(pEndActionData->GetRunTime() * m_ScaleTime)- fOffsetTime) <= MOTOR_MAX_MOVING_TIME)
    {
        if (pEndActionData)
        {
            CActionBlockData *pTempActionData = new CActionBlockData();

            WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
            memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
            int nDescriptionLen = QStringToWCHAR(wstrDescription, "Action");
            //wstrDescription[nDescriptionLen-1] ='\0';
            pTempActionData->SetDescription(wstrDescription);
            pTempActionData->SetID(-1);
            
            float fTempRunTime = pEndActionData->GetRunTime() - fOffsetTime/m_ScaleTime;//当前运动时间
            float fTempAllTime = pEndActionData->GetAllTime() - fOffsetTime/m_ScaleTime;//当前总时间
            if(fTempRunTime < 0)
                fTempRunTime = 0;
            if(fTempAllTime < 0)
                fTempAllTime = 0;
            pTempActionData->SetRunTime(fTempRunTime);
            pTempActionData->SetAllTime(fTempAllTime);
            
            char *pData = NULL;
            int nLen = 0;
            pEndActionData->GetExtBlockData(&pData,nLen);
            if (pData && nLen)
            {
                pTempActionData->SetExtBlockData(nLen,pData);
                pDatalst->append(pTempActionData);
                delete []pData;
                pData = NULL;
            }
        }
        return;
    }
    
    int nServoCnt = m_nServoCount;
    if (pEndActionData && (!isPauseOrStop()) && pStartActionData)
    {
        int nTempRunTime = qCeil(pEndActionData->GetRunTime() * m_ScaleTime) - fOffsetTime;
        
        if(nTempRunTime < 0)
            nTempRunTime = 0;
        
        int nLoopTimes = 1;
        if (nTempRunTime % MOTOR_MAX_MOVING_TIME != 0)
            nLoopTimes = nTempRunTime / MOTOR_MAX_MOVING_TIME + 1;
        else
            nLoopTimes = nTempRunTime / MOTOR_MAX_MOVING_TIME ;
        
        int nRunTimeSum = 0;//运动时间累加和
        for (int i = 0; i<nLoopTimes; i++)
        {
            int nLoopRunTime = 0;
            if (i == nLoopTimes - 1)
            {
                //nLoopRunTime = (nRunTime - (i - 1) * MOTOR_MAX_MOVING_TIME) / 2;
                nLoopRunTime = nTempRunTime - nRunTimeSum;
            }
            else if (i == nLoopTimes - 2)
            {
                nLoopRunTime = (nTempRunTime - i * MOTOR_MAX_MOVING_TIME) / 2;
            }
            else
            {
                nLoopRunTime = MOTOR_MAX_MOVING_TIME;
            }
            nRunTimeSum += nLoopRunTime;
            
            CActionBlockData *pTempBlockData = CaculateActionData(pStartActionData,pEndActionData, nRunTimeSum);
            pTempBlockData->SetRunTime((float)nLoopRunTime / m_ScaleTime);
            pTempBlockData->SetAllTime((float)nLoopRunTime / m_ScaleTime);
            if (i == nLoopTimes - 1)
            {
                pTempBlockData->SetAllTime(pTempBlockData->GetRunTime()
                                           + (pEndActionData->GetAllTime() - pEndActionData->GetRunTime()));//最后一帧设置总时间
            }
            pDatalst->append(pTempBlockData);
        }
    }
}

void CMotionDataPlay::OnAllDataPlaying(CActionBlockData *pStartActionData)
{
    QElapsedTimer timer;
    timer.start();
    quint64 dwStartTime = timer.nsecsElapsed();
    quint64 dwEndTime = timer.nsecsElapsed();
    double dExeTime = 0;
    if (m_nGroupIndex == -1 || m_nActionIndex == -1)
    {
        return;
    }
    
    CActionBlockGroupManager* pCurrentPlayGroupManager = NULL;
    double dStartTime = 0;
    double dOffsetTime = 0;
    
    for (int j = m_nGroupIndex; j < m_LayerUI->GetBlockDataManager()->GetCount(); j++)
    {
        pCurrentPlayGroupManager = m_LayerUI->GetBlockDataManager()->GetAt(j);
        for (int i = m_nActionIndex; i < pCurrentPlayGroupManager->GetActionCount(); i++)
        {
            if (isPauseOrStop())
            {
                return;
            }

            CActionBlockData* pActionBlock = pCurrentPlayGroupManager->GetAt(i);
            if (pActionBlock == NULL)
            {
                qDebug()<<QObject::tr("Error:ActionData is empty！")<<endl;
                break;
            }
            //int runTime = ceil(pActionBlock->GetRunTime()*m_ScaleTime);
            if (j == m_nGroupIndex && i == m_nActionIndex)
            {
                dOffsetTime = pCurrentPlayGroupManager->GetActionStartTimeByIndex(m_nActionIndex);
                dOffsetTime += m_nOffsetTime * 1.0 / m_ScaleTime;//此处有累加误差
                //runTime = runTime - m_nOffsetTime;
            }
            LstSplitActionData lstActionData;
            SplitActionData(pStartActionData, pActionBlock, &lstActionData, m_nOffsetTime);
            pStartActionData = pActionBlock;
            for (int k = 0; k<lstActionData.count(); k++)
            {
                if (isPauseOrStop())
                {
                    return;
                }
                CActionBlockData *pTempData = lstActionData.at(k);
                if (pTempData == NULL)
                {
                    continue;
                }
                // 					TRACE(L"Group%d-Action%d-Dispatch%dAngle List\n",j,i,k);
                // 					for (int ii = 0 ;ii<16;ii++)
                // 					{
                // 						TRACE(L"%d(%d)-",ii+1,pTempData->GetMotorAngle(ii+ 1));
                // 					}
                // 					    TRACE(L"Time is %d\n",(int)pTempData->GetEndTime()* m_ScaleTime);

                float fMinNum = dStartTime;

                dStartTime += qCeil(pTempData->GetAllTime()*m_ScaleTime);


                float fMaxNum = dStartTime;
                int nMaxNum = fMaxNum/m_ScaleTime;
                int nMinNum = fMinNum/m_ScaleTime;
                m_numAutoChange.stop();
                m_numAutoChange.setMaxMin( nMaxNum, nMinNum, true);
                float fRunTime = ceil(pTempData->GetRunTime()*m_ScaleTime);//ms

                int nPeriodTime =(int) fRunTime/((nMaxNum - nMinNum + 1)/m_numAutoChange.stepNum());
                m_numAutoChange.setPeriodTime(nPeriodTime);                
                m_numAutoChange.start();
                m_bCanRecNumChangeSig = true;

                m_oMutex.lock();
                OnPlaying(pTempData/*,m_nOffsetTime*/);

//                    if (m_bHighPrecisionSupported)
//                    {
//                        //						QueryPerformanceCounter(&time_over);
//                        double s = (double)(time_over.QuadPart - time_start.QuadPart)/(double)m_TimeFrequency.QuadPart;
//                        dExeTime = (double)s*1000;
//                    }
//                    else
//                    {
//                        dwEndTime = 10/*GetTickCount()*/;
//                        dExeTime = dwEndTime -dwStartTime;
//                    }
                // 					TRACE(L"fStartTime - dExeTime = %d",(int)fStartTime - (int)dExeTime);
                // 					TRACE(L"\n--------------------------------------------\n");
                dwEndTime = timer.nsecsElapsed();
                dExeTime = (dwEndTime - dwStartTime) / 1000000;//纳秒转毫秒
                if ((dStartTime - dExeTime > 0)  && (!isPauseOrStop()))
                {
                    m_oWait.wait(&m_oMutex, dStartTime - dExeTime);
                    //						WaitForSingleObject(hQuitThreadEvent, dStartTime - dExeTime);
                }
                m_oMutex.unlock();
            }

            m_numAutoChange.stop();
            m_bCanRecNumChangeSig = false;

            if (j == m_nGroupIndex && i == m_nActionIndex)   //记录本动作块之前帧持续的时间，用来确定光标位置
            {
                m_nActionIndex = 0;
                m_nGroupIndex  = 0;
                m_nOffsetTime  = 0;
            }
            //fStartTime += runTime;
            if (m_Event) //&& m_Event->PlayEvent_GetDebugType() == SINGLE_LAYER_DEBUG)
            {
                //qDebug() << "CMotionDataPlay::OnAllDataPlaying::PlayEvent_OnSetPos" << dOffsetTime + dStartTime/m_ScaleTime -1 << endl;
                if ( j == (m_LayerUI->GetBlockDataManager()->GetCount() - 1)
                     && i == (pCurrentPlayGroupManager->GetActionCount() - 1))
                {
                    m_Event->PlayEvent_OnSetPos(/*pCurrentPlayGroupManager->GetStartTime() + fStartTime - 1*/dOffsetTime + dStartTime/m_ScaleTime - TIME_OFFSET_1);
                }
                else
                {
                    m_Event->PlayEvent_OnSetPos(/*pCurrentPlayGroupManager->GetStartTime() + fStartTime - 1*/dOffsetTime + dStartTime/m_ScaleTime + TIME_OFFSET_1);
                }
            }
            for (int i = 0;i<lstActionData.count();i++)
            {
                CActionBlockData *pActionData = lstActionData.at(i);
                if (pActionData)
                {
                    delete pActionData;
                    pActionData = NULL;
                }
            }
            lstActionData.clear();
        }
    }
}
void CMotionDataPlay::OnGroupDataPlaying(CActionBlockData *pStartActionData)
{
    QElapsedTimer timer;
    timer.start();
    quint64 dwStartTime = timer.nsecsElapsed();
    quint64 dwEndTime = timer.nsecsElapsed();

    double dExeTime = 0;
    float fGroupSum = qCeil(m_GroupManager->GetStartTime() * m_ScaleTime);

    for (int i = 0; i < m_GroupManager->GetActionCount(); i++)
    {
        if (isPauseOrStop())
        {
            return;
        }

        CActionBlockData* pActionBlock = m_GroupManager->GetAt(i);
        if (pActionBlock == NULL)
        {
            return;
        }
        LstSplitActionData lstSplitData;
        SplitActionData(pStartActionData,pActionBlock,&lstSplitData,0);
        pStartActionData = pActionBlock;
        m_oMutex.lock();
        for(int j = 0; j<lstSplitData.count(); j++)
        {
            if (isPauseOrStop())
            {
                return;
            }
            CActionBlockData *pTempActionData = lstSplitData.at(j);
            if(pTempActionData == NULL)
            {
                continue;
            }

            float fMinNum = fGroupSum;

            fGroupSum += qCeil(pTempActionData ->GetAllTime()*m_ScaleTime);

            float fMaxNum = fGroupSum;
            int nMaxNum = fMaxNum/m_ScaleTime;
            int nMinNum = fMinNum/m_ScaleTime;
            m_numAutoChange.stop();
            m_numAutoChange.setMaxMin( nMaxNum, nMinNum, true);
            float fRunTime = ceil(pTempActionData->GetRunTime()*m_ScaleTime);//ms
            int nPeriodTime =(int) fRunTime/((nMaxNum - nMinNum + 1)/m_numAutoChange.stepNum());
            m_numAutoChange.setPeriodTime(nPeriodTime);
            m_numAutoChange.start();
            m_bCanRecNumChangeSig = true;


            OnPlaying(pTempActionData);

//                if (m_bHighPrecisionSupported)
//                {
//                    //					QueryPerformanceCounter(&time_over);
//                    double s = (double)(time_over.QuadPart - time_start.QuadPart)/(double)m_TimeFrequency.QuadPart;
//                    dExeTime = (double)s*1000;
//                }
//                else
//                {
//                    dwEndTime = 10/*GetTickCount()*/;
//                    dExeTime = dwStartTime - dwEndTime;
//                }

            dwEndTime = timer.nsecsElapsed();
            dExeTime = (dwEndTime - dwStartTime) / 1000000;//纳秒转毫秒

            if ((fGroupSum - dExeTime - qCeil(m_GroupManager->GetStartTime() * m_ScaleTime) > 0) && (!isPauseOrStop()))
            {
                m_oWait.wait(&m_oMutex, fGroupSum - dExeTime - qCeil(m_GroupManager->GetStartTime() * m_ScaleTime));
            }
        }

        m_numAutoChange.stop();
        m_bCanRecNumChangeSig = false;

        m_oMutex.unlock();
        if (m_Event && m_Event->PlayEvent_GetDebugType() == SINGLE_LAYER_DEBUG)
        {
//             最后一帧动作播放完后，停止在最后一帧时间范围内
            if (i == m_GroupManager->GetActionCount() - 1)
            {
                m_Event->PlayEvent_OnSetPos(fGroupSum / m_ScaleTime - TIME_OFFSET_1);
            }
            else
            {
                // 播放动作过程中，帧动作之间的起始位置作为下一帧动作
                m_Event->PlayEvent_OnSetPos(fGroupSum / m_ScaleTime + TIME_OFFSET_1);
            }
        }
        for (int i = 0;i<lstSplitData.count();i++)
        {
            CActionBlockData *pTempActionData = lstSplitData.at(i);
            if (pTempActionData)
            {
                delete pTempActionData;
                pTempActionData = NULL;
            }
        }
        lstSplitData.clear();
    }
}
void CMotionDataPlay::OnSingleGroupPlaying()
{
    QElapsedTimer timer;
    timer.start();
    quint64 dwStartTime = timer.nsecsElapsed();
    quint64 dwEndTime = timer.nsecsElapsed();
    double dExeTime = 0;
    
    float fGroupSum = ceil(m_GroupManager->GetStartTime() * m_ScaleTime);

    for (int i = 0; i < m_GroupManager->GetActionCount(); i++)
    {
        if (!isPauseOrStop())
        {
            CActionBlockData* pActionBlock = m_GroupManager->GetAt(i);
            
            if (pActionBlock == NULL)
            {
                return;
            }
            
            fGroupSum += ceil(pActionBlock->GetAllTime()*m_ScaleTime);
            
            m_oMutex.lock();
            OnPlaying(pActionBlock);
            
            //			if (m_bHighPrecisionSupported)
            //			{
            //				QueryPerformanceCounter(&time_over);
            //				double s = (double)(time_over.QuadPart - time_start.QuadPart)/(double)m_TimeFrequency.QuadPart;
            //				dExeTime = (double)s*1000;
            //			}
            //			else
            //			{
            //				dwEndTime = GetTickCount();
            //				dExeTime = dwStartTime -dwEndTime;
            //			}
            dwEndTime = timer.nsecsElapsed();
            dExeTime = (dwEndTime - dwStartTime) / 1000000;//纳秒转毫秒
            if ((fGroupSum  - dExeTime > 0)  && (!isPauseOrStop()))
            {
                //				WaitForSingleObject(hQuitThreadEvent , fGroupSum - dExeTime - ceil(m_GroupManager->GetStartTime()*m_ScaleTime));
                m_oWait.wait(&m_oMutex, fGroupSum - dExeTime - qCeil(m_GroupManager->GetStartTime()*m_ScaleTime));
            }
            m_oMutex.unlock();
            if (m_Event && m_Event->PlayEvent_GetDebugType() == SINGLE_LAYER_DEBUG)
            {
                m_Event->PlayEvent_OnSetPos(fGroupSum / m_ScaleTime - TIME_OFFSET_1);
            }
        }
        else
            break;
    }
}
void CMotionDataPlay::OnAllDataPlaying()
{
    QElapsedTimer timer;
    timer.start();
    quint64 dwStartTime = timer.nsecsElapsed();
    quint64 dwEndTime = timer.nsecsElapsed();
    double dExeTime = 0;

    if (m_nGroupIndex == -1 || m_nActionIndex == -1)
    {
        return;
    }
    CActionBlockGroupManager* pCurrentPlayGroupManager = NULL;
    for (int j = m_nGroupIndex; j < m_LayerUI->GetBlockDataManager()->GetCount(); j++)
    {
        float fGroupTimeSum = 0;
        pCurrentPlayGroupManager = m_LayerUI->GetBlockDataManager()->GetAt(j);
        for (int i = m_nActionIndex; i < pCurrentPlayGroupManager->GetActionCount(); i++)
        {
            if (isPauseOrStop())
            {
                return;
            }

            CActionBlockData* pActionBlock = pCurrentPlayGroupManager->GetAt(i);
            if (pActionBlock == NULL)
            {
                continue;
            }
            int runTime = ceil(pActionBlock->GetAllTime()*m_ScaleTime);
            if (j == m_nGroupIndex && i == m_nActionIndex)
            {
                runTime = runTime - m_nOffsetTime;
            }
            //				if (m_bHighPrecisionSupported)                   //是否支持高精度计时器
            //					QueryPerformanceCounter(&time_start);
            //				else
            //					dwStartTime = GetTickCount();
            //执行动作
            //				ResetEvent(hQuitThreadEvent);
            m_oMutex.lock();
            OnPlaying(pActionBlock,m_nOffsetTime); //将第一次的偏移发送，后面置零
            //				if (m_bHighPrecisionSupported)
            //				{
            //					QueryPerformanceCounter(&time_over);
            //					double s = (double)(time_over.QuadPart - time_start.QuadPart)/(double)m_TimeFrequency.QuadPart;
            //					dExeTime = (double)s*1000;
            //				}
            //				else
            //				{
            //					dwEndTime = GetTickCount();
            //					dExeTime = dwEndTime -dwStartTime;
            //				}

            dwEndTime = timer.nsecsElapsed();
            dExeTime = (dwEndTime - dwStartTime) / 1000000;//纳秒转毫秒
            if ((runTime - dExeTime > 0)  && (!isPauseOrStop()))
            {
                //					WaitForSingleObject(hQuitThreadEvent,runTime - dExeTime);
                m_oWait.wait(&m_oMutex, runTime - dExeTime);
            }
            m_oMutex.unlock();
            if (j == m_nGroupIndex && i == m_nActionIndex)   //记录本动作块之前帧持续的时间，用来确定光标位置
            {
                float fActionTimeSum = pCurrentPlayGroupManager->GetActionStartTimeByIndex(i);
                if (fActionTimeSum >= 0)
                {
                    fGroupTimeSum += fActionTimeSum;
                }
                m_nActionIndex = 0;
                m_nGroupIndex  = 0;
                m_nOffsetTime  = 0;
            }
            fGroupTimeSum += pActionBlock->GetAllTime();
            if (m_Event && m_Event->PlayEvent_GetDebugType() == SINGLE_LAYER_DEBUG)
            {
                m_Event->PlayEvent_OnSetPos(pCurrentPlayGroupManager->GetStartTime() + fGroupTimeSum - TIME_OFFSET_1);
                //m_Event->PlayEvent_OnSetPosChangeToOptions(ACTION_LAYER_MOTION,pCurrentPlayGroupManager->GetStartTime() + nGroupTimeSum - 1);
            }
        }
    }
}

void CMotionDataPlay::onSigNumChange(int nOld, int nNew)
{
    if (m_Event && m_Event->PlayEvent_GetDebugType() == SINGLE_LAYER_DEBUG)
    {
        if(m_bCanRecNumChangeSig)
            m_Event->PlayEvent_OnSetPos(nNew);
    }
}

void CMotionDataPlay::run()
{
    QElapsedTimer timer;
    timer.start();
    quint64 dwStartTime = timer.nsecsElapsed();
    quint64 dwEndTime = timer.nsecsElapsed();
    double dExeTime = 0;

    if (m_pRobot3dView)
    {
        m_pRobot3dView->EnableViewCtrl(false);
        m_pRobot3dView->Set3DModelPlayState(true);
    }

    m_numAutoChange.stop();
    m_bCanRecNumChangeSig = false;

    if (m_nPlayType == PLAYDEBUG_GROUP)
    {
        if (m_Event)
        {
            m_Event->PlayEvent_OnEnableCtrl(ACTION_LAYER_MOTION,false);
        }
        char* pData = NULL;
        int nLen = 0;
        m_pRobot3dView->GetMotorDatas(&pData, nLen);
        if (pData && nLen)
        {
            float fRunTime = 0;
            QString tempDescription;
            
            CActionBlockData* pActionBlockData = new CActionBlockData();
            pActionBlockData->SetID(-1);

            WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
            memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
            int nDescriptionLen = QStringToWCHAR(wstrDescription, "Action");
            //wstrDescription[nDescriptionLen-1] ='\0';
            pActionBlockData->SetDescription(wstrDescription);

            pActionBlockData->SetRunTime(fRunTime/m_ScaleTime);
            pActionBlockData->SetAllTime(fRunTime/m_ScaleTime);
            pActionBlockData->SetExtBlockData(nLen, pData);
            OnGroupDataPlaying(pActionBlockData);
            if (pActionBlockData)
            {
                delete pActionBlockData;
                pActionBlockData = NULL;
            }
        }
        if (m_Event)
        {
            m_Event->PlayEvent_OnEnableCtrl(ACTION_LAYER_MOTION,true);
        }
    } 
    else if(m_nPlayType == PLAYDEBUG_ALL)
    {	
        char* pData = NULL;
        int nLen = 0;
        m_pRobot3dView->GetMotorDatas(&pData, nLen);
        if (pData && nLen)
        {
            float fRunTime = 0;
            QString tempDescription;
            
            CActionBlockData* pActionBlockData = new CActionBlockData();
            pActionBlockData->SetID(-1);

            WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
            memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
            int nDescriptionLen = QStringToWCHAR(wstrDescription, "Action");
            //wstrDescription[nDescriptionLen-1] ='\0';
            pActionBlockData->SetDescription(wstrDescription);


            pActionBlockData->SetRunTime(fRunTime/m_ScaleTime);
            pActionBlockData->SetAllTime(fRunTime/m_ScaleTime);
            pActionBlockData->SetExtBlockData(nLen, pData);
            OnAllDataPlaying(pActionBlockData);
            if (pActionBlockData)
            {
                delete pActionBlockData;
                pActionBlockData = NULL;
            }
        }
        //OnAllDataPlaying();
    }
    else if (m_nPlayType == PLAYDEBUG_BLOCK)
    {
        if (m_Event)
        {
            m_Event->PlayEvent_OnEnableCtrl(ACTION_LAYER_MOTION,false);
        }
        char* pData = NULL;
        int nLen = 0;
        m_pRobot3dView->GetMotorDatas(&pData, nLen);
        if (pData && nLen)
        {
            float fRunTime = 0;
            QString tempDescription;
            
            CActionBlockData* pActionBlockData = new CActionBlockData();
            pActionBlockData->SetID(-1);

            WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
            memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
            int nDescriptionLen = QStringToWCHAR(wstrDescription, "Action");
            //wstrDescription[nDescriptionLen-1] ='\0';
            pActionBlockData->SetDescription(wstrDescription);

            pActionBlockData->SetRunTime(fRunTime/m_ScaleTime);
            pActionBlockData->SetAllTime(fRunTime/m_ScaleTime);
            pActionBlockData->SetExtBlockData(nLen, pData);
            if (pActionBlockData == NULL)
            {
                return;
            }
            OnSingleDataPLaying(pActionBlockData , m_pActionData);
            if (pActionBlockData)
            {
                delete pActionBlockData;
                pActionBlockData = NULL;
            }
        }
        
        if (m_Event)
        {
            m_Event->PlayEvent_OnEnableCtrl(ACTION_LAYER_MOTION,true);
        }
    }
    if (m_Event)
    {
        m_Event->PlayEvent_SetDebugType(SINGLE_LAYER_DEBUG);
    }
    if (m_pRobot3dView)
    {
        m_pRobot3dView->Set3DModelPlayState(false);
        m_pRobot3dView->EnableViewCtrl(true);
    }

    OnStopPlay(m_eStopPlayType);

    dwEndTime = timer.nsecsElapsed();
    //qDebug()<<"Total time is "<< (dwEndTime - dwStartTime) / 1000000;
    //TRACE(L"Total time is %d\n",dwEndTime - dwStartTime);
}

bool CMotionDataPlay::Connect(ILayerUI* pDataManager, int nPlayType, int nScaleTime, IGroupDataPlayEvent* pEvent)
{
    //	if (QueryPerformanceFrequency(&m_TimeFrequency))   //支持高精度定时器
    //	{
    //		m_bHighPrecisionSupported = true;
    //	}
    m_LayerUI = pDataManager;
    m_nPlayType = nPlayType;
    m_Event = pEvent;
    m_ScaleTime = nScaleTime;
    m_bStop = false;
    //m_nDebugType = nDebugType;
    //	hQuitThreadEvent = CreateEvent(NULL,true,true,NULL);
    return true;
}

void CMotionDataPlay::ReleaseConnection()
{
    StopPlay();
    delete this;
}

void CMotionDataPlay::OnPlaying(CActionBlockData* pActionBlock,int nOffsetTime)
{
    if (pActionBlock == NULL || m_pRobot3dView == NULL)
    {
        return;
    }
    char* pData = NULL;
    int nLen = 0;
    
    pActionBlock->GetExtBlockData(&pData, nLen);
    float fRunTime = ceil(pActionBlock->GetRunTime()*m_ScaleTime);
    float fAllTime = ceil(pActionBlock->GetAllTime()*m_ScaleTime);
    if ((fRunTime - nOffsetTime) * m_ScaleTime > (GRID_MIN_TIMEVALUE))   //小于最小时间不发送本数据（防止电机过快转动烧坏）
    {
        m_pRobot3dView->SetAllMotorAngle(pData, nLen, fRunTime - nOffsetTime, fAllTime - nOffsetTime);
        m_pRobot3dView->SetMotorDatas(pData, nLen, fRunTime - nOffsetTime);
        // 		for (int i = 0;i<SERVO_COUNT;i++)
        // 		{
        // 			m_pRobot3dView->SetMotorPowerState(i+1,POWERON);
        // 		}
    }
    
    delete []pData;
}
void CMotionDataPlay::SetCurrentPos(int nGroupIndex,int nActionIndex,int nOffsetTime)
{
    m_nGroupIndex = nGroupIndex;
    m_nActionIndex = nActionIndex;
    m_nOffsetTime = nOffsetTime;
}
void CMotionDataPlay::OnStopPlay(emPlayStatus ePlayStatus)
{
    if (m_Event)
    {
        m_Event->PlayEvent_OnDebugStop(ACTION_LAYER_MOTION, ePlayStatus);
    }
}

void CMotionDataPlay::StartBlockPlay(CActionBlockData* pActionData, IUBXRobot3DView* PlayHandle)
{
    m_pRobot3dView = PlayHandle;
    m_pActionData = pActionData;
    m_bStop = true;
    m_oWait.wakeAll();
    wait();
    m_bStop = false;
    m_bPause = false;
    m_eStopPlayType = ePlayStatusFinished; // 默认为正常播放结束
    start();
}

void CMotionDataPlay::StartPlay(CActionBlockGroupManager* pActionData, IUBXRobot3DView* PlayHandle)
{
    m_pRobot3dView = PlayHandle;
    m_GroupManager = pActionData;
    
    m_bStop = true;
    m_oWait.wakeAll();
    wait();
    m_bStop = false;
    m_bPause = false;
    m_eStopPlayType = ePlayStatusFinished; // 默认为正常播放结束
    start();
}

void CMotionDataPlay::PausePlay()
{
    m_bPause = true;
    m_bStop = false;
    m_eStopPlayType = ePlayStatusPause;
    m_oWait.wakeAll();
    wait();
    if (m_Event)
    {
        m_Event->PlayEvent_SetDebugType(SINGLE_LAYER_DEBUG);
    }
    m_GroupManager = NULL;
    m_pActionData = NULL;
}

void CMotionDataPlay::StopPlay()
{
    m_bStop = true;
    m_bPause = false;
    m_eStopPlayType = ePlayStatusStop;
    m_oWait.wakeAll();
    wait();
    if (m_Event)
    {
        m_Event->PlayEvent_SetDebugType(SINGLE_LAYER_DEBUG);
    }
    m_GroupManager = NULL;
    m_pActionData = NULL;
}
