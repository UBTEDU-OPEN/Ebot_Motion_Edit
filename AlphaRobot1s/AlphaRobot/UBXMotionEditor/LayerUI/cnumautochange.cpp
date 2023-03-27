#include "cnumautochange.h"

#include <QDebug>

static int nRunTime = 0;

CNumAutoChange::CNumAutoChange()
{
    m_nMaxNum = 100;
    m_nMinNum = 0;
    m_nStepNum = 1;
    m_nCurrentNum = m_nMinNum;
    m_nPeriodTimeMsecond = 100;
    m_nLoopTime = -1;

    m_bRuning = false;
}

CNumAutoChange::~CNumAutoChange()
{
    m_nLoopTime = 1;
    exit();
}

void CNumAutoChange::setMaxMin(int nMax, int nMin, bool bAutoModifyStep)
{
    if(nMax > 0 &&
            nMax > nMin &&
            nMin > -1)
    {
        m_nMaxNum = nMax;
        m_nMinNum = nMin;
        m_nCurrentNum = m_nMinNum;

        if(bAutoModifyStep)
        {
            int nTempStep = (m_nMaxNum - m_nMinNum + 1)/50;
            if(nTempStep > 50)
            {
                nTempStep = 50;
            }
            if(nTempStep > 0)
            {
                m_nStepNum = nTempStep;
            }
            else
            {
                m_nStepNum = 1;
            }
        }
    }
}

int CNumAutoChange::stepNum() const
{
    return m_nStepNum;
}

void CNumAutoChange::setStepNum(int nStepNum)
{
    m_nStepNum = nStepNum;
}

int CNumAutoChange::periodTime() const
{
    return m_nPeriodTimeMsecond;
}

void CNumAutoChange::setPeriodTime(int nPeriodTimeMs)
{
    if(nPeriodTimeMs > 0)
    {
        m_nPeriodTimeMsecond = nPeriodTimeMs;
    }
}

int CNumAutoChange::loopTime() const
{
    return m_nLoopTime;
}

void CNumAutoChange::setLoopTime(int nLoopTime)
{
    m_nLoopTime = nLoopTime;    
}

void CNumAutoChange::start()
{
    m_bRuning = true;
    nRunTime = 0;

    QThread::start();
}

void CNumAutoChange::stop()
{
    m_bRuning = false;
}

void CNumAutoChange::run()
{    
    m_nCurrentNum = m_nMinNum;

    if(m_nLoopTime > 0)
    {
        nRunTime = 0;
    }
    static int nPreOldNum = m_nCurrentNum;

    while ( m_bRuning
            && ( (m_nLoopTime < 0)
            || (nRunTime < m_nLoopTime)))
    {
        if(m_nCurrentNum >= m_nMaxNum)
        {
            if(nPreOldNum >= m_nMaxNum)
            {
                 m_nCurrentNum = m_nMinNum;
            }
            else
            {
                m_nCurrentNum = m_nMaxNum;
            }

            nRunTime++;
        }

        if(m_bRuning)
        {
            emit sigNumChange(nPreOldNum, m_nCurrentNum);
        }

        msleep(m_nPeriodTimeMsecond);

        nPreOldNum = m_nCurrentNum;
        m_nCurrentNum += m_nStepNum;
    }
}



