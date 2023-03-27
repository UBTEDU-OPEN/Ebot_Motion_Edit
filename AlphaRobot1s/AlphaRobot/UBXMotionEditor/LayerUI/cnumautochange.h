#ifndef CNUMAUTOCHANGE_H
#define CNUMAUTOCHANGE_H


#include <QThread>
#include <QMutex>
#include <QWaitCondition>


class CNumAutoChange : public QThread
{
    Q_OBJECT

public:
    CNumAutoChange();
    ~CNumAutoChange();

public:
    void setMaxMin(int nMax, int nMin, bool bAutoModifyStep = false);

    int stepNum() const;
    void setStepNum(int nStepNum);

    int periodTime() const;
    void setPeriodTime(int nPeriodTimeMs);

    int loopTime() const;
    void setLoopTime(int nLoopTime);

    void start();
    void stop();


public:

signals:
    void sigNumChange(int nOld, int nNew);

public slots:


protected:
    virtual void run();

private:
    int m_nMaxNum;
    int m_nMinNum;
    int m_nStepNum;
    int m_nCurrentNum;
    int m_nPeriodTimeMsecond;
    int m_nLoopTime;
    bool m_bRuning;
};

#endif // CNUMAUTOCHANGE_H
